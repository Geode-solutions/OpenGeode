/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/match.h>
#include <absl/strings/str_replace.h>

#include <geode/basic/growable.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/logger_client.hpp>
#include <geode/basic/logger_manager.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/regular_grid_solid_builder.hpp>
#include <geode/mesh/builder/regular_grid_surface_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/brep_output.hpp>
#include <geode/model/representation/io/section_input.hpp>
#include <geode/model/representation/io/section_output.hpp>

#include <geode/tests/common.hpp>

namespace
{
    /* Records warnings to check that repairs are reported */
    class WarningRecorder : public geode::LoggerClient
    {
    public:
        static void start()
        {
            static const auto registered = [] {
                geode::LoggerManager::register_client(
                    std::make_unique< WarningRecorder >() );
                return true;
            }();
            geode_unused( registered );
            std::lock_guard< std::mutex > lock{ mutex() };
            warnings().clear();
        }

        static bool has_warning( std::string_view text )
        {
            std::lock_guard< std::mutex > lock{ mutex() };
            for( const auto& warning : warnings() )
            {
                if( absl::StrContains( warning, text ) )
                {
                    return true;
                }
            }
            return false;
        }

        void trace( const std::string& /*message*/ ) override {}
        void debug( const std::string& /*message*/ ) override {}
        void info( const std::string& /*message*/ ) override {}
        void warning( const std::string& message ) override
        {
            std::lock_guard< std::mutex > lock{ mutex() };
            warnings().push_back( message );
        }
        void error( const std::string& /*message*/ ) override {}
        void critical( const std::string& /*message*/ ) override {}

    private:
        static std::mutex& mutex()
        {
            static std::mutex mutex;
            return mutex;
        }

        static std::vector< std::string >& warnings()
        {
            static std::vector< std::string > warnings;
            return warnings;
        }
    };

    void check_warning( std::string_view text )
    {
        geode::OpenGeodeModelException::test(
            WarningRecorder::has_warning( text ),
            "[Test] Missing warning: ", text );
    }

    /* Files of a model archive: file name -> file content */
    using ArchiveFiles = absl::btree_map< std::string, std::string >;

    std::string read_file( const std::filesystem::path& path )
    {
        std::ifstream file{ path, std::ifstream::binary };
        return { std::istreambuf_iterator< char >{ file },
            std::istreambuf_iterator< char >{} };
    }

    ArchiveFiles extract_archive( std::string_view filename )
    {
        const geode::UnzipFile unzip{ filename, geode::uuid{}.string() };
        unzip.extract_all();
        ArchiveFiles files;
        for( const auto& file : std::filesystem::directory_iterator(
                 std::filesystem::u8path( unzip.directory() ) ) )
        {
            files.emplace(
                file.path().filename().u8string(), read_file( file.path() ) );
        }
        return files;
    }

    void write_archive( const ArchiveFiles& files, std::string_view filename )
    {
        geode::ZipFile zip{ filename, geode::uuid{}.string() };
        for( const auto& [name, content] : files )
        {
            const auto path = absl::StrCat( zip.directory(), "/", name );
            {
                std::ofstream file{ std::filesystem::u8path( path ),
                    std::ofstream::binary };
                file << content;
            }
            zip.archive_file( path );
        }
        zip.finalize();
    }

    /* Serialized uuid: Growable version 2 followed by the 16 bytes */
    std::string serialized_uuid( const geode::uuid& id )
    {
        std::string bytes{ static_cast< char >( 2 ) };
        const auto hex = id.string();
        for( std::size_t i = 0; i < hex.size(); i++ )
        {
            if( hex[i] == '-' )
            {
                continue;
            }
            bytes.push_back( static_cast< char >(
                std::stoi( hex.substr( i, 2 ), {}, 16 ) ) );
            i++;
        }
        return bytes;
    }

    std::string serialized_u32( std::uint32_t value )
    {
        std::string bytes;
        for( const auto i : geode::LRange{ 4 } )
        {
            bytes.push_back( static_cast< char >( value >> ( 8 * i ) ) );
        }
        return bytes;
    }

    void replace_unique( std::string& content,
        const std::string& from,
        const std::string& to,
        std::string_view what )
    {
        const auto position = content.find( from );
        geode::OpenGeodeModelException::test(
            position != std::string::npos, "[Test] ", what, " not found" );
        geode::OpenGeodeModelException::test(
            content.find( from, position + 1 ) == std::string::npos, "[Test] ",
            what, " is not unique" );
        content.replace( position, from.size(), to );
    }

    geode::BRep saved_brep( std::string_view filename )
    {
        auto brep = geode::load_brep(
            absl::StrCat( geode::DATA_PATH, "box_brep.og_brep" ) );
        geode::save_brep( brep, filename );
        return brep;
    }

    /*
     * The relationships file stores a uuid -> graph vertex map duplicating
     * the ids stored on the graph vertices. A corrupted map must not change
     * the loaded relations.
     */
    void test_inconsistent_relationships_map()
    {
        const auto brep = saved_brep( "relationships.og_brep" );
        const auto& id0 = brep.component_with_relation( 0 ).id;
        geode::index_t other{ geode::NO_ID };
        for( const auto v :
            geode::Range{ 1, brep.nb_components_with_relations() } )
        {
            if( brep.nb_relations( brep.component_with_relation( v ).id )
                != brep.nb_relations( id0 ) )
            {
                other = v;
                break;
            }
        }
        geode::OpenGeodeModelException::test(
            other != geode::NO_ID, "[Test] No component to swap with" );
        const auto& id1 = brep.component_with_relation( other ).id;
        auto files = extract_archive( "relationships.og_brep" );
        auto& content = files.at( "relationships" );
        replace_unique( content,
            absl::StrCat( serialized_uuid( id0 ), serialized_u32( 0 ) ),
            absl::StrCat( serialized_uuid( id0 ), serialized_u32( other ) ),
            "First uuid mapping" );
        replace_unique( content,
            absl::StrCat( serialized_uuid( id1 ), serialized_u32( other ) ),
            absl::StrCat( serialized_uuid( id1 ), serialized_u32( 0 ) ),
            "Second uuid mapping" );
        write_archive( files, "corrupted_relationships.og_brep" );

        const auto loaded =
            geode::load_brep( "corrupted_relationships.og_brep" );
        geode::OpenGeodeModelException::test(
            loaded.nb_components_with_relations()
                == brep.nb_components_with_relations(),
            "[Test] Wrong number of components with relations" );
        for( const auto v :
            geode::Range{ brep.nb_components_with_relations() } )
        {
            const auto& id = brep.component_with_relation( v ).id;
            geode::OpenGeodeModelException::test(
                loaded.nb_relations( id ) == brep.nb_relations( id ),
                "[Test] Wrong number of relations for ", id.string() );
            absl::flat_hash_set< geode::uuid > related;
            for( const auto& component : brep.relations( id ) )
            {
                related.insert( component.id );
            }
            for( const auto& component : loaded.relations( id ) )
            {
                geode::OpenGeodeModelException::test(
                    related.contains( component.id ),
                    "[Test] Wrong relation for ", id.string() );
            }
        }
    }

    bool has_mesh_component( const geode::BRep& brep, const geode::uuid& id )
    {
        return brep.has_corner( id ) || brep.has_line( id )
               || brep.has_surface( id ) || brep.has_block( id );
    }

    std::string mesh_file_name(
        const ArchiveFiles& files, std::string_view prefix )
    {
        for( const auto& file : files )
        {
            if( absl::StartsWith( file.first, prefix )
                && file.first.size() > prefix.size() + 36 )
            {
                return file.first;
            }
        }
        throw geode::OpenGeodeModelException{ nullptr,
            geode::OpenGeodeException::TYPE::internal, "[Test] No ", prefix,
            " mesh file" };
    }

    void check_same_unique_vertices(
        const geode::BRep& brep, const geode::BRep& loaded )
    {
        geode::OpenGeodeModelException::test(
            loaded.nb_unique_vertices() == brep.nb_unique_vertices(),
            "[Test] Wrong number of unique vertices" );
        for( const auto unique_vertex :
            geode::Range{ brep.nb_unique_vertices() } )
        {
            const auto& vertices =
                brep.component_mesh_vertices( unique_vertex );
            geode::OpenGeodeModelException::test(
                loaded.component_mesh_vertices( unique_vertex ).size()
                    == vertices.size(),
                "[Test] Wrong number of component vertices for unique vertex ",
                unique_vertex );
            for( const auto& vertex : vertices )
            {
                geode::OpenGeodeModelException::test(
                    loaded.unique_vertex( vertex ) == unique_vertex,
                    "[Test] Wrong unique vertex for component vertex" );
            }
        }
    }

    /*
     * Mesh components have a "unique vertices" attribute linking their
     * vertices to the model unique vertices. When this attribute is missing,
     * it is restored from the unique vertices.
     */
    void test_missing_unique_vertices_attribute()
    {
        const auto brep = saved_brep( "unique_vertices.og_brep" );
        auto files = extract_archive( "unique_vertices.og_brep" );
        replace_unique( files.at( "vertices" ),
            serialized_uuid( brep.unique_vertex_attribute_id() ),
            serialized_uuid( geode::uuid{} ), "Unique vertex attribute id" );
        for( const auto prefix : { "Corner", "Line", "Surface", "Block" } )
        {
            replace_unique( files.at( mesh_file_name( files, prefix ) ),
                "unique vertices", "unique_vertices",
                "Unique vertices attribute name" );
        }
        write_archive( files, "corrupted_unique_vertices.og_brep" );

        const auto loaded =
            geode::load_brep( "corrupted_unique_vertices.og_brep" );
        check_same_unique_vertices( brep, loaded );
    }

    /*
     * Unique vertices referencing a component that does not exist in the
     * model are removed.
     */
    void test_unknown_component_vertices()
    {
        saved_brep( "with_corner.og_brep" );
        auto without_corner = geode::load_brep( "with_corner.og_brep" );
        const auto& corner = *without_corner.corners().begin();
        const auto corner_id = corner.id();
        geode::BRepBuilder{ without_corner }.remove_corner( corner );
        geode::save_brep( without_corner, "without_corner.og_brep" );
        auto files = extract_archive( "without_corner.og_brep" );
        files.at( "vertices" ) =
            extract_archive( "with_corner.og_brep" ).at( "vertices" );
        write_archive( files, "unknown_corner_vertices.og_brep" );

        const auto loaded =
            geode::load_brep( "unknown_corner_vertices.og_brep" );
        geode::OpenGeodeModelException::test( !loaded.has_corner( corner_id ),
            "[Test] Removed corner should not be loaded" );
        for( const auto unique_vertex :
            geode::Range{ loaded.nb_unique_vertices() } )
        {
            for( const auto& vertex :
                loaded.component_mesh_vertices( unique_vertex ) )
            {
                geode::OpenGeodeModelException::test(
                    has_mesh_component( loaded, vertex.component_id.id ),
                    "[Test] Unique vertex ", unique_vertex,
                    " references an unknown component" );
                geode::OpenGeodeModelException::test(
                    loaded.unique_vertex( vertex ) == unique_vertex,
                    "[Test] Wrong unique vertex for component vertex" );
            }
        }
    }

    /*
     * The components file of each component type is always saved. A missing
     * file is reported.
     */
    void test_missing_components_file()
    {
        saved_brep( "missing_components.og_brep" );
        auto files = extract_archive( "missing_components.og_brep" );
        files.erase( "lines" );
        write_archive( files, "missing_components_file.og_brep" );

        WarningRecorder::start();
        const auto loaded =
            geode::load_brep( "missing_components_file.og_brep" );
        check_warning( "Missing file" );
        geode::OpenGeodeModelException::test(
            loaded.nb_lines() == 0, "[Test] No line should be loaded" );
    }

    /*
     * Components are stored in a map: uuid -> component. A component stored
     * under another uuid than its own is stored back under its own uuid.
     */
    void test_wrong_component_key()
    {
        const auto brep = saved_brep( "component_key.og_brep" );
        const auto& corner_id = ( *brep.corners().begin() ).id();
        auto files = extract_archive( "component_key.og_brep" );
        auto& content = files.at( "corners" );
        const auto key = serialized_uuid( corner_id );
        const auto position = content.find( key );
        geode::OpenGeodeModelException::test(
            position != std::string::npos, "[Test] Corner key not found" );
        content.replace(
            position, key.size(), serialized_uuid( geode::uuid{} ) );
        geode::OpenGeodeModelException::test(
            content.find( key ) != std::string::npos,
            "[Test] Corner id not found" );
        write_archive( files, "wrong_component_key.og_brep" );

        WarningRecorder::start();
        const auto loaded = geode::load_brep( "wrong_component_key.og_brep" );
        check_warning( "has id" );
        geode::OpenGeodeModelException::test(
            loaded.nb_corners() == brep.nb_corners(),
            "[Test] Wrong number of corners" );
        geode::OpenGeodeModelException::test( loaded.has_corner( corner_id ),
            "[Test] Corner should be stored under its id" );
        geode::OpenGeodeModelException::test(
            loaded.corner( corner_id ).mesh().nb_vertices()
                == brep.corner( corner_id ).mesh().nb_vertices(),
            "[Test] Corner mesh should be loaded" );
    }

    /*
     * Relations referencing a component that does not exist in the model are
     * removed.
     */
    void test_unknown_component_relations()
    {
        saved_brep( "relations_with_corner.og_brep" );
        auto without_corner =
            geode::load_brep( "relations_with_corner.og_brep" );
        const auto& corner = *without_corner.corners().begin();
        const auto corner_id = corner.id();
        geode::BRepBuilder{ without_corner }.remove_corner( corner );
        geode::save_brep( without_corner, "relations_without_corner.og_brep" );
        auto files = extract_archive( "relations_without_corner.og_brep" );
        files.at( "relationships" ) =
            extract_archive( "relations_with_corner.og_brep" )
                .at( "relationships" );
        write_archive( files, "unknown_corner_relations.og_brep" );

        WarningRecorder::start();
        const auto loaded =
            geode::load_brep( "unknown_corner_relations.og_brep" );
        check_warning( "Removing relations of missing" );
        geode::OpenGeodeModelException::test(
            loaded.nb_relations( corner_id ) == 0,
            "[Test] Removed corner should not have relations" );
        for( const auto vertex :
            geode::Range{ loaded.nb_components_with_relations() } )
        {
            const auto& component = loaded.component_with_relation( vertex );
            geode::OpenGeodeModelException::test( component.id != corner_id,
                "[Test] Removed corner should not be in relationships" );
        }
        for( const auto& line : loaded.lines() )
        {
            for( const auto& boundary : loaded.boundaries( line ) )
            {
                geode::OpenGeodeModelException::test(
                    boundary.id() != corner_id,
                    "[Test] Removed corner should not be a boundary" );
            }
        }
    }

    void replace_all( std::string& content,
        std::string_view from,
        std::string_view to,
        std::string_view what )
    {
        geode::OpenGeodeModelException::test(
            content.find( from ) != std::string::npos, "[Test] ", what,
            " not found" );
        content = absl::StrReplaceAll( content, { { from, to } } );
    }

    /*
     * A component whose mesh file is missing gets an empty mesh.
     */
    void test_missing_mesh_file()
    {
        const auto brep = saved_brep( "missing_mesh.og_brep" );
        auto files = extract_archive( "missing_mesh.og_brep" );
        const auto surface_file = mesh_file_name( files, "Surface" );
        const auto line_file = mesh_file_name( files, "Line" );
        files.erase( surface_file );
        files.erase( line_file );
        write_archive( files, "missing_mesh_file.og_brep" );

        WarningRecorder::start();
        const auto loaded = geode::load_brep( "missing_mesh_file.og_brep" );
        check_warning( "Missing mesh file" );
        geode::OpenGeodeModelException::test(
            loaded.nb_surfaces() == brep.nb_surfaces(),
            "[Test] Wrong number of surfaces" );
        geode::index_t nb_empty_surfaces{ 0 };
        for( const auto& surface : loaded.surfaces() )
        {
            const auto& mesh = surface.mesh();
            geode::OpenGeodeModelException::test(
                surface.mesh_type() == mesh.impl_name(),
                "[Test] Wrong surface mesh type" );
            if( mesh.nb_vertices() == 0 )
            {
                nb_empty_surfaces++;
                continue;
            }
            geode::OpenGeodeModelException::test(
                mesh.nb_polygons()
                    == brep.surface( surface.id() ).mesh().nb_polygons(),
                "[Test] Wrong surface mesh" );
        }
        geode::OpenGeodeModelException::test(
            nb_empty_surfaces == 1, "[Test] One surface should be empty" );
        for( const auto unique_vertex :
            geode::Range{ loaded.nb_unique_vertices() } )
        {
            for( const auto& vertex :
                loaded.component_mesh_vertices( unique_vertex ) )
            {
                geode::OpenGeodeModelException::test(
                    loaded.unique_vertex( vertex ) == unique_vertex,
                    "[Test] Wrong unique vertex for component vertex" );
            }
        }
    }

    /*
     * A component whose mesh implementation is not registered is loaded with
     * the default implementation of the mesh type matching its file.
     */
    void test_unknown_mesh_impl()
    {
        const auto brep = saved_brep( "unknown_impl.og_brep" );
        auto files = extract_archive( "unknown_impl.og_brep" );
        replace_all( files.at( "surfaces" ), "OpenGeodeTriangulatedSurface3D",
            "UnknownImplTriangulatedSurface", "Surface mesh type" );
        write_archive( files, "unknown_mesh_impl.og_brep" );

        WarningRecorder::start();
        const auto loaded = geode::load_brep( "unknown_mesh_impl.og_brep" );
        check_warning( "Unknown mesh implementation" );
        for( const auto& surface : loaded.surfaces() )
        {
            geode::OpenGeodeModelException::test(
                surface.mesh_type().get() == "OpenGeodeTriangulatedSurface3D",
                "[Test] Wrong surface mesh type" );
            geode::OpenGeodeModelException::test(
                surface.mesh().nb_polygons()
                    == brep.surface( surface.id() ).mesh().nb_polygons(),
                "[Test] Wrong surface mesh" );
        }
    }

    /*
     * Surfaces and blocks may have a RegularGrid mesh.
     */
    void test_regular_grid_meshes()
    {
        geode::Section section;
        geode::SectionBuilder section_builder{ section };
        const auto& surface_id = section_builder.add_surface();
        auto surface_grid = geode::RegularGrid2D::create();
        geode::RegularGridBuilder2D::create( *surface_grid )
            ->initialize_grid( geode::Point2D{ { 0., 0. } }, { 3, 4 }, 1 );
        section_builder.update_surface_mesh(
            section.surface( surface_id ), std::move( surface_grid ) );
        geode::save_section( section, "grid.og_sctn" );
        const auto loaded_section = geode::load_section( "grid.og_sctn" );
        const auto& loaded_surface = loaded_section.surface( surface_id );
        geode::OpenGeodeModelException::test(
            geode::MeshFactory::type( loaded_surface.mesh_type() )
                == geode::RegularGrid2D::type_name_static(),
            "[Test] Surface mesh should be a RegularGrid" );
        geode::OpenGeodeModelException::test(
            loaded_surface.mesh().nb_polygons() == 12,
            "[Test] Wrong number of grid surface cells" );

        geode::BRep brep;
        geode::BRepBuilder brep_builder{ brep };
        const auto& block_id = brep_builder.add_block();
        auto block_grid = geode::RegularGrid3D::create();
        geode::RegularGridBuilder3D::create( *block_grid )
            ->initialize_grid(
                geode::Point3D{ { 0., 0., 0. } }, { 2, 3, 4 }, 1 );
        brep_builder.update_block_mesh(
            brep.block( block_id ), std::move( block_grid ) );
        geode::save_brep( brep, "grid.og_brep" );
        const auto loaded_brep = geode::load_brep( "grid.og_brep" );
        const auto& loaded_block = loaded_brep.block( block_id );
        geode::OpenGeodeModelException::test(
            geode::MeshFactory::type( loaded_block.mesh_type() )
                == geode::RegularGrid3D::type_name_static(),
            "[Test] Block mesh should be a RegularGrid" );
        geode::OpenGeodeModelException::test(
            loaded_block.mesh().nb_polyhedra() == 24,
            "[Test] Wrong number of grid block cells" );
    }

    template < typename Saver >
    void check_save_failure( std::string_view what, const Saver& saver )
    {
        try
        {
            saver();
        }
        catch( const geode::OpenGeodeException& exception )
        {
            geode::Logger::info( "Saving ", what,
                " in a missing directory throws as expected: ",
                exception.what() );
            return;
        }
        throw geode::OpenGeodeModelException{ nullptr,
            geode::OpenGeodeException::TYPE::internal, "[Test] Saving ", what,
            " in a missing directory should throw" };
    }

    /*
     * Saving model files in a directory that cannot be written throws.
     */
    void test_save_in_missing_directory()
    {
        const auto brep = geode::load_brep(
            absl::StrCat( geode::DATA_PATH, "box_brep.og_brep" ) );
        const std::string directory{ "missing_directory/missing" };
        check_save_failure( "relationships", [&] {
            brep.save_relationships( directory );
        } );
        check_save_failure( "unique vertices", [&] {
            brep.save_unique_vertices( directory );
        } );
        check_save_failure( "physical properties", [&] {
            brep.save_physical_properties( directory );
        } );
        check_save_failure( "model boundaries", [&] {
            brep.save_model_boundaries( directory );
        } );
    }

    /*
     * Model files are saved and loaded with UTF-8 paths.
     */
    void test_utf8_path()
    {
        const auto brep = geode::load_brep(
            absl::StrCat( geode::DATA_PATH, "box_brep.og_brep" ) );
        const std::string filename{ "mod\xc3\xa8le_\xc3\xa9t\xc3\xa9.og_brep" };
        geode::save_brep( brep, filename );
        const auto loaded = geode::load_brep( filename );
        geode::OpenGeodeModelException::test(
            loaded.nb_surfaces() == brep.nb_surfaces(),
            "[Test] Wrong number of surfaces with UTF-8 path" );
        check_same_unique_vertices( brep, loaded );
    }

    /*
     * Names of old components are bounded by MAX_SERIALIZED_NAME_SIZE.
     */
    void test_too_long_component_name()
    {
        auto files = extract_archive(
            absl::StrCat( geode::DATA_PATH, "layers.og_brep" ) );
        auto& content = files.at( "corners" );
        const std::string name{ "\x07unknown" };
        const auto position = content.find( name );
        geode::OpenGeodeModelException::test(
            position != std::string::npos, "[Test] Component name not found" );
        // bitsery 4-byte size encoding: two high bytes, then a little endian
        // 16-bit word
        constexpr auto TOO_LONG = geode::MAX_SERIALIZED_NAME_SIZE + 1;
        std::string too_long_name{ static_cast< char >(
                                       0xC0u | ( TOO_LONG >> 24 ) ),
            static_cast< char >( TOO_LONG >> 16 ),
            static_cast< char >( TOO_LONG ),
            static_cast< char >( TOO_LONG >> 8 ) };
        too_long_name.append( TOO_LONG, 'a' );
        content.replace( position, name.size(), too_long_name );
        write_archive( files, "too_long_component_name.og_brep" );
        try
        {
            geode_unused(
                geode::load_brep( "too_long_component_name.og_brep" ) );
        }
        catch( const geode::OpenGeodeException& exception )
        {
            geode::Logger::info( "Too long component name throws as expected: ",
                exception.what() );
            return;
        }
        throw geode::OpenGeodeModelException{ nullptr,
            geode::OpenGeodeException::TYPE::internal,
            "[Test] Too long component name should not be loaded" };
    }
} // namespace

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_inconsistent_relationships_map();
    test_missing_unique_vertices_attribute();
    test_unknown_component_vertices();
    test_missing_components_file();
    test_wrong_component_key();
    test_unknown_component_relations();
    test_missing_mesh_file();
    test_unknown_mesh_impl();
    test_regular_grid_meshes();
    test_save_in_missing_directory();
    test_utf8_path();
    test_too_long_component_name();
}

OPENGEODE_TEST( "model-corrupted-files" )
