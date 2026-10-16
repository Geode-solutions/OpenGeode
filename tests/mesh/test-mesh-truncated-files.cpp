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

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <string>

#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/image/core/raster_image.hpp>

#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/texture2d.hpp>
#include <geode/mesh/core/texture_manager.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/edged_curve_input.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/hybrid_solid_input.hpp>
#include <geode/mesh/io/light_regular_grid_input.hpp>
#include <geode/mesh/io/light_regular_grid_output.hpp>
#include <geode/mesh/io/point_set_input.hpp>
#include <geode/mesh/io/polygonal_surface_input.hpp>
#include <geode/mesh/io/polyhedral_solid_input.hpp>
#include <geode/mesh/io/regular_grid_input.hpp>
#include <geode/mesh/io/tetrahedral_solid_input.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/tests/common.hpp>

namespace
{
    using Loader = std::function< void( std::string_view ) >;

    std::string read_file( const std::string& filename )
    {
        std::ifstream file{ filename, std::ifstream::binary };
        return { std::istreambuf_iterator< char >{ file },
            std::istreambuf_iterator< char >{} };
    }

    /*
     * Load truncated copies of the given file: each load must either succeed
     * or throw an OpenGeodeException, it must never crash.
     */
    void test_truncated_file(
        const std::string& filename, const Loader& loader )
    {
        geode::Logger::info( "Truncating ", filename );
        const auto content = read_file( filename );
        geode::OpenGeodeMeshException::test(
            !content.empty(), "Cannot read file ", filename );
        const std::filesystem::path folder{ "truncated_files" };
        std::filesystem::create_directories( folder );
        const auto extension =
            std::filesystem::path{ filename }.extension().string();
        const auto truncated_filename =
            ( folder / absl::StrCat( "truncated", extension ) ).string();
        // Every size in the first bytes (headers and first pointers), then a
        // sampling of the rest: keeps the test fast on big files
        constexpr std::size_t NB_EXHAUSTIVE_BYTES{ 256 };
        constexpr std::size_t NB_SAMPLES{ 128 };
        const auto nb_exhaustive =
            std::min( content.size(), NB_EXHAUSTIVE_BYTES );
        const auto step = std::max< std::size_t >(
            ( content.size() - nb_exhaustive ) / NB_SAMPLES, 1 );
        const auto level = geode::Logger::level();
        geode::Logger::set_level( geode::Logger::LEVEL::off );
        geode::index_t nb_throws{ 0 };
        for( std::size_t size = 0; size < content.size();
            size += size < nb_exhaustive ? 1 : step )
        {
            {
                std::ofstream file{ truncated_filename, std::ofstream::binary };
                file.write(
                    content.data(), static_cast< std::streamsize >( size ) );
            }
            try
            {
                loader( truncated_filename );
            }
            catch( const geode::OpenGeodeException& )
            {
                nb_throws++;
            }
        }
        geode::Logger::set_level( level );
        std::filesystem::remove_all( folder );
        geode::Logger::info(
            "Truncated ", filename, ": ", nb_throws, " loads threw" );
        geode::OpenGeodeMeshException::test(
            nb_throws > 0, "Truncated files of ", filename, " should throw" );
    }

    std::string data_file( std::string_view name )
    {
        return absl::StrCat( geode::DATA_PATH, "backward_io/", name );
    }

    std::string save_textured_surface()
    {
        auto surface = geode::TriangulatedSurface2D::create();
        auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
        builder->create_point( geode::Point2D{ { 0, 0 } } );
        builder->create_point( geode::Point2D{ { 1, 0 } } );
        builder->create_point( geode::Point2D{ { 0, 1 } } );
        builder->create_triangle( { 0, 1, 2 } );
        auto manager = surface->texture_manager();
        auto& texture = manager.find_or_create_texture( "texture" );
        texture.set_texture_coordinates( { 0, 1 }, geode::Point2D{ { 1, 0 } } );
        texture.set_image( geode::RasterImage2D{ { 2, 2 } } );
        const auto filename =
            absl::StrCat( "textured_surface.", surface->native_extension() );
        geode::save_triangulated_surface( *surface, filename );
        return filename;
    }

    std::string save_light_grid()
    {
        const geode::LightRegularGrid3D grid{ geode::Point3D{ { 1.5, 0, 1 } },
            { 2, 3, 4 }, { 1, 2, 3 } };
        const auto filename =
            absl::StrCat( "truncated_light_grid.", grid.native_extension() );
        geode::save_light_regular_grid( grid, filename );
        return filename;
    }
} // namespace

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    const Loader edged_curve = []( std::string_view filename ) {
        geode_unused( geode::load_edged_curve< 3 >( filename ) );
    };
    const Loader graph = []( std::string_view filename ) {
        geode_unused( geode::load_graph( filename ) );
    };
    const Loader point_set = []( std::string_view filename ) {
        geode_unused( geode::load_point_set< 3 >( filename ) );
    };
    const Loader polygonal_surface = []( std::string_view filename ) {
        geode_unused( geode::load_polygonal_surface< 3 >( filename ) );
    };
    const Loader triangulated_surface3d = []( std::string_view filename ) {
        geode_unused( geode::load_triangulated_surface< 3 >( filename ) );
    };
    const Loader triangulated_surface2d = []( std::string_view filename ) {
        geode_unused( geode::load_triangulated_surface< 2 >( filename ) );
    };
    const Loader polyhedral_solid = []( std::string_view filename ) {
        geode_unused( geode::load_polyhedral_solid< 3 >( filename ) );
    };
    const Loader tetrahedral_solid = []( std::string_view filename ) {
        geode_unused( geode::load_tetrahedral_solid< 3 >( filename ) );
    };
    const Loader hybrid_solid = []( std::string_view filename ) {
        geode_unused( geode::load_hybrid_solid< 3 >( filename ) );
    };
    const Loader regular_grid3d = []( std::string_view filename ) {
        geode_unused( geode::load_regular_grid< 3 >( filename ) );
    };
    const Loader regular_grid2d = []( std::string_view filename ) {
        geode_unused( geode::load_regular_grid< 2 >( filename ) );
    };
    const Loader light_grid = []( std::string_view filename ) {
        geode_unused( geode::load_light_regular_grid< 3 >( filename ) );
    };

    test_truncated_file( data_file( "v7/test_v7.og_grp" ), graph );
    test_truncated_file(
        data_file( "v7/test_v7.og_psf3d" ), polygonal_surface );
    test_truncated_file( data_file( "v7/test_v7.og_pso3d" ), polyhedral_solid );
    test_truncated_file( data_file( "v12/test_v12.og_edc3d" ), edged_curve );
    test_truncated_file(
        data_file( "v12/test_v12.og_psf3d" ), polygonal_surface );
    test_truncated_file(
        data_file( "v12/test_v12.og_pso3d" ), polyhedral_solid );
    test_truncated_file( data_file( "v12/test_v12.og_rgd3d" ), regular_grid3d );
    test_truncated_file( data_file( "v13/v13.og_rgd2d" ), regular_grid2d );
    test_truncated_file( data_file( "v17/v17.og_edc3d" ), edged_curve );
    test_truncated_file( data_file( "v17/v17.og_grp" ), graph );
    test_truncated_file( data_file( "v17/v17.og_hso3d" ), hybrid_solid );
    test_truncated_file( data_file( "v17/v17.og_psf3d" ), polygonal_surface );
    test_truncated_file( data_file( "v17/v17.og_pso3d" ), polyhedral_solid );
    test_truncated_file( data_file( "v17/v17.og_pts3d" ), point_set );
    test_truncated_file( data_file( "v17/v17.og_rgd3d" ), regular_grid3d );
    test_truncated_file(
        data_file( "v17/v17.og_tsf3d" ), triangulated_surface3d );
    test_truncated_file( data_file( "v17/v17.og_tso3d" ), tetrahedral_solid );
    test_truncated_file( data_file( "v18_5/v18_5.og_edc3d" ), edged_curve );
    test_truncated_file( data_file( "v18_5/v18_5.og_grp" ), graph );
    test_truncated_file(
        data_file( "v18_5/v18_5.og_psf3d" ), polygonal_surface );
    test_truncated_file(
        data_file( "v18_5/v18_5.og_tsf3d" ), triangulated_surface3d );
    test_truncated_file(
        data_file( "v18_5/v18_5.og_tso3d" ), tetrahedral_solid );
    test_truncated_file( save_textured_surface(), triangulated_surface2d );
    test_truncated_file( save_light_grid(), light_grid );
}

OPENGEODE_TEST( "mesh-truncated-files" )
