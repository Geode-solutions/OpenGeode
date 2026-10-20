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

#include <geode/basic/logger.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/mesh/core/point_set.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/brep_output.hpp>

#include <geode/tests/common.hpp>

namespace
{
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

} // namespace

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_inconsistent_relationships_map();
    test_missing_unique_vertices_attribute();
    test_unknown_component_vertices();
}

OPENGEODE_TEST( "model-corrupted-files" )
