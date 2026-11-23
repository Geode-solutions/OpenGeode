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

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/strings/match.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/detail/file_content.hpp>
#include <geode/basic/detail/input_files.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/geode/geode_brep_input.hpp>
#include <geode/model/representation/io/geode/geode_section_input.hpp>
#include <geode/model/representation/io/section_input.hpp>

#include <geode/tests/common.hpp>

namespace
{
    template < typename Model >
    void check_same_components(
        const Model& model, const Model& reference, std::string_view filename )
    {
        for( const auto& corner : reference.corners() )
        {
            const auto& mesh = model.corner( corner.id() ).mesh();
            geode::OpenGeodeModelException::test(
                mesh.nb_vertices() == corner.mesh().nb_vertices(),
                "Corner mesh differs in ", filename );
        }
        for( const auto& line : reference.lines() )
        {
            const auto& mesh = model.line( line.id() ).mesh();
            geode::OpenGeodeModelException::test(
                mesh.nb_vertices() == line.mesh().nb_vertices()
                    && mesh.nb_edges() == line.mesh().nb_edges(),
                "Line mesh differs in ", filename );
        }
        for( const auto& surface : reference.surfaces() )
        {
            const auto& mesh = model.surface( surface.id() ).mesh();
            geode::OpenGeodeModelException::test(
                mesh.nb_vertices() == surface.mesh().nb_vertices()
                    && mesh.nb_polygons() == surface.mesh().nb_polygons()
                    && mesh.type_name() == surface.mesh().type_name(),
                "Surface mesh differs in ", filename );
        }
        for( const auto uv : geode::Range{ reference.nb_unique_vertices() } )
        {
            auto vertices = model.component_mesh_vertices( uv );
            auto expected = reference.component_mesh_vertices( uv );
            absl::c_sort( vertices );
            absl::c_sort( expected );
            geode::OpenGeodeModelException::test( vertices == expected,
                "Unique vertex ", uv, " differs in ", filename );
        }
        geode::OpenGeodeModelException::test(
            model.nb_corners() == reference.nb_corners()
                && model.nb_lines() == reference.nb_lines()
                && model.nb_surfaces() == reference.nb_surfaces()
                && model.nb_model_boundaries()
                       == reference.nb_model_boundaries(),
            "Components differ in ", filename );
        geode::OpenGeodeModelException::test(
            model.nb_unique_vertices() == reference.nb_unique_vertices(),
            "Unique vertices differ in ", filename );
        const geode::detail::ArchiveInputFiles files{ filename };
        // Without identifier file, the model gets a new identifier
        geode::OpenGeodeModelException::test(
            !files.has( "identifier" ) || model.id() == reference.id(),
            "Identifier differs in ", filename, ": ", model.id().string(),
            " vs ", reference.id().string() );
        geode::OpenGeodeModelException::test(
            model.name() == reference.name(), "Name differs in ", filename );
        for( const auto& surface : reference.surfaces() )
        {
            geode::OpenGeodeModelException::test(
                model.nb_relations( surface.id() )
                    == reference.nb_relations( surface.id() ),
                "Relations differ in ", filename );
        }
    }

    void check_same_brep( const geode::BRep& model,
        const geode::BRep& reference,
        std::string_view filename )
    {
        check_same_components( model, reference, filename );
        geode::OpenGeodeModelException::test(
            model.nb_blocks() == reference.nb_blocks(), "Blocks differ in ",
            filename );
        for( const auto& block : reference.blocks() )
        {
            const auto& mesh = model.block( block.id() ).mesh();
            geode::OpenGeodeModelException::test(
                mesh.nb_vertices() == block.mesh().nb_vertices()
                    && mesh.nb_polyhedra() == block.mesh().nb_polyhedra()
                    && mesh.type_name() == block.mesh().type_name(),
                "Block mesh differs in ", filename );
        }
    }

    /*!
     * Load every native model file from its archive and from its extracted
     * directory (through the directory input files): same models.
     */
    void test_data_files()
    {
        geode::index_t nb_files{ 0 };
        for( const auto& file : std::filesystem::recursive_directory_iterator(
                 std::filesystem::u8path( geode::DATA_PATH ) ) )
        {
            const auto path = file.path().u8string();
            const auto extension = file.path().extension().u8string();
            if( extension != ".og_brep" && extension != ".og_sctn" )
            {
                continue;
            }
            geode::Logger::info( "Checking ", path );
            const geode::UnzipFile unzip{ path, geode::uuid{}.string() };
            unzip.extract_all();
            if( extension == ".og_brep" )
            {
                geode::BRep reference{ geode::BITSERY::constructor };
                geode::detail::load_brep_files( reference,
                    geode::detail::DirectoryInputFiles{ unzip.directory() } );
                const auto brep =
                    geode::load_one( geode::load_brep( path ), path );
                check_same_brep( brep, reference, path );
            }
            else
            {
                geode::Section reference{ geode::BITSERY::constructor };
                geode::detail::load_section_files( reference,
                    geode::detail::DirectoryInputFiles{ unzip.directory() } );
                const auto section =
                    geode::load_one( geode::load_section( path ), path );
                check_same_components( section, reference, path );
            }
            nb_files++;
        }
        geode::OpenGeodeModelException::test(
            nb_files >= 10, "Too few model files found: ", nb_files );
    }

    void set_environment(
        const char* name, const std::optional< std::string >& value )
    {
#ifdef _WIN32
        _putenv_s( name, value ? value->c_str() : "" );
#else
        if( value )
        {
            setenv( name, value->c_str(), 1 );
        }
        else
        {
            unsetenv( name );
        }
#endif
    }

    /*!
     * Loading a model does not use the temporary directory: pointing it to a
     * regular file makes any use of it fail.
     */
    void test_no_temporary_directory()
    {
        const auto brep_file =
            absl::StrCat( geode::DATA_PATH, "layers.og_brep" );
        const auto section_file =
            absl::StrCat( geode::DATA_PATH, "fractures.og_sctn" );
        const auto reference_brep =
            geode::load_one( geode::load_brep( brep_file ), brep_file );
        const auto reference_section = geode::load_one(
            geode::load_section( section_file ), section_file );
        const std::vector< std::string > variables{ "TMPDIR", "TMP", "TEMP" };
        std::vector< std::optional< std::string > > old_values;
        for( const auto& variable : variables )
        {
            const auto* value = std::getenv( variable.c_str() );
            old_values.emplace_back(
                value ? std::optional< std::string >{ value } : std::nullopt );
        }
        const auto not_a_directory =
            std::filesystem::absolute( "temporary_directory_file" );
        {
            std::ofstream file{ not_a_directory };
            file << "not a directory";
        }
        const auto empty_directory =
            std::filesystem::absolute( "empty_temporary_directory" );
        std::filesystem::remove_all( empty_directory );
        std::filesystem::create_directories( empty_directory );
        for( const auto& temporary : { not_a_directory, empty_directory } )
        {
            for( const auto& variable : variables )
            {
                set_environment( variable.c_str(), temporary.u8string() );
            }
            const auto brep =
                geode::load_one( geode::load_brep( brep_file ), brep_file );
            check_same_brep( brep, reference_brep, brep_file );
            const auto section = geode::load_one(
                geode::load_section( section_file ), section_file );
            check_same_components( section, reference_section, section_file );
        }
        for( const auto i : geode::Indices{ variables } )
        {
            set_environment( variables[i].c_str(), old_values[i] );
        }
        geode::OpenGeodeModelException::test(
            std::filesystem::is_empty( empty_directory ),
            "Loading a model created files in the temporary directory" );
        std::filesystem::remove( not_a_directory );
        std::filesystem::remove_all( empty_directory );
    }

    std::string read_file( const std::string& filename )
    {
        const auto content = geode::detail::map_file_content( filename );
        geode::OpenGeodeModelException::test(
            content.has_value(), "Cannot read ", filename );
        return { content->span().data(), content->span().size() };
    }

    /*!
     * Flip a byte in the middle of the data of an entry of a stored archive.
     */
    void corrupt_entry(
        const std::string& archive_file, const std::string& name )
    {
        const auto size = [&] {
            const geode::detail::ArchiveInputFiles files{ archive_file };
            return files.content( name ).span().size();
        }();
        auto archive = read_file( archive_file );
        // Find the local header of the entry
        const std::string signature{ "PK\x03\x04" };
        auto header = archive.find( signature );
        const auto read_uint16 = [&archive]( size_t position ) {
            return static_cast< size_t >(
                static_cast< unsigned char >( archive[position] )
                + 256U
                      * static_cast< unsigned char >( archive[position + 1] ) );
        };
        while(
            header != std::string::npos
            && !( read_uint16( header + 26 ) == name.size()
                  && archive.compare( header + 30, name.size(), name ) == 0 ) )
        {
            header = archive.find( signature, header + 1 );
        }
        geode::OpenGeodeModelException::test(
            header != std::string::npos, "Entry ", name, " not found" );
        const auto name_position = header + 30;
        const auto extra_length = read_uint16( header + 28 );
        const auto data = name_position + name.size() + extra_length;
        archive[data + size / 2] =
            static_cast< char >( ~archive[data + size / 2] );
        std::ofstream file{ std::filesystem::u8path( archive_file ),
            std::ios::binary };
        file.write(
            archive.data(), static_cast< std::streamsize >( archive.size() ) );
    }

    void test_corrupted_entries()
    {
        const auto source = absl::StrCat( geode::DATA_PATH, "layers.og_brep" );
        std::string mesh_entry;
        {
            const geode::detail::ArchiveInputFiles files{ source };
            for( const auto& name : files.names() )
            {
                if( absl::StartsWith( name, "Surface" ) )
                {
                    mesh_entry = name;
                    break;
                }
            }
        }
        for( const auto& entry : { mesh_entry, std::string{ "vertices" },
                 std::string{ "surfaces" } } )
        {
            const std::string corrupted{ "corrupted_entry.og_brep" };
            std::filesystem::copy_file( source, corrupted,
                std::filesystem::copy_options::overwrite_existing );
            corrupt_entry( corrupted, entry );
            bool thrown{ false };
            try
            {
                geode_unused( geode::load_brep( corrupted ) );
            }
            catch( const geode::OpenGeodeException& exception )
            {
                thrown = true;
                geode::OpenGeodeModelException::test(
                    exception.type() == geode::OpenGeodeException::TYPE::data
                        && absl::StrContains( exception.what(),
                            absl::StrCat( corrupted, "/", entry ) )
                        && absl::StrContains( exception.what(), "CRC" ),
                    "Corrupted entry ", entry,
                    ": wrong error: ", exception.what() );
            }
            geode::OpenGeodeModelException::test(
                thrown, "Corrupted entry ", entry, " was not detected" );
            std::filesystem::remove( corrupted );
        }
    }
} // namespace

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_data_files();
    test_no_temporary_directory();
    test_corrupted_entries();
}

OPENGEODE_TEST( "model-archive-input" )
