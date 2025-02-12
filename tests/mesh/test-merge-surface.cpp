/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/tests/common.hpp>

#include <async++.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/nn_search.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/convert_surface_mesh.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

void test_create()
{
    std::vector< geode::Point2D > points{ geode::Point2D{ { 0, 0 } },
        geode::Point2D{ { 0, 1 } }, geode::Point2D{ { 0, 2 } },
        geode::Point2D{ { 1, 0 } }, geode::Point2D{ { 1, 1 } },
        geode::Point2D{ { 1, 2 } } };

    auto mesh0 = geode::SurfaceMesh2D::create();
    auto builder0 = geode::SurfaceMeshBuilder2D::create( *mesh0 );
    builder0->create_point( points[0] );
    builder0->create_point( points[1] );
    builder0->create_point( points[3] );
    builder0->create_point( points[4] );
    builder0->create_point( points[5] );
    builder0->create_polygon( { 0, 1, 3 } );
    builder0->create_polygon( { 0, 3, 2 } );
    builder0->create_polygon( { 1, 4, 3 } );
    builder0->compute_polygon_adjacencies();

    auto mesh1 = geode::SurfaceMesh2D::create();
    auto builder1 = geode::SurfaceMeshBuilder2D::create( *mesh1 );
    builder1->create_point( points[1] );
    builder1->create_point( points[2] );
    builder1->create_point( points[4] );
    builder1->create_point( points[5] );
    builder1->create_point( points[0] );
    builder1->create_point( points[3] );
    builder1->create_polygon( { 0, 3, 1 } );
    builder1->create_polygon( { 0, 2, 3 } );
    builder1->create_polygon( { 2, 0, 4 } );
    builder1->create_polygon( { 2, 4, 5 } );
    builder1->compute_polygon_adjacencies();

    std::vector< std::reference_wrapper< const geode::SurfaceMesh2D > > meshes{
        *mesh0, *mesh1
    };
    const auto merged = geode::merge_surface_meshes< 2 >( meshes );
    OPENGEODE_EXCEPTION(
        merged->nb_vertices() == 6, "[Test] Wrong number of vertices" );
    OPENGEODE_EXCEPTION(
        merged->nb_polygons() == 4, "[Test] Wrong number of polygons" );
    for( const auto p : geode::Range{ merged->nb_polygons() } )
    {
        for( const auto e : geode::LRange{ 3 } )
        {
            const auto adj = merged->polygon_adjacent_edge( { p, e } );
            geode_unused( adj );
        }
    }

    OPENGEODE_EXCEPTION( merged->polygon_adjacent( { 0, 0 } ) = 1,
        "[Test] Wrong adjacency for { 0, 0 }" );
    OPENGEODE_EXCEPTION( merged->polygon_adjacent( { 0, 1 } ) = 2,
        "[Test] Wrong adjacency for { 0, 1 }" );
    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 0, 2 } ),
        "[Test] Wrong adjacency for { 0, 2 }" );

    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 1, 0 } ),
        "[Test] Wrong adjacency for { 1, 0 }" );
    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 1, 1 } ),
        "[Test] Wrong adjacency for { 1, 1 }" );
    OPENGEODE_EXCEPTION( merged->polygon_adjacent( { 1, 2 } ) = 0,
        "[Test] Wrong adjacency for { 1, 2 }" );

    OPENGEODE_EXCEPTION( merged->polygon_adjacent( { 2, 0 } ) = 0,
        "[Test] Wrong adjacency for { 2, 0 }" );
    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 2, 1 } ),
        "[Test] Wrong adjacency for { 2, 1 }" );
    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 2, 2 } ),
        "[Test] Wrong adjacency for { 2, 2 }" );

    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 3, 0 } ),
        "[Test] Wrong adjacency for { 3, 0 }" );
    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 3, 1 } ),
        "[Test] Wrong adjacency for { 3, 1 }" );
    OPENGEODE_EXCEPTION( !merged->polygon_adjacent( { 3, 2 } ),
        "[Test] Wrong adjacency for { 3, 2 }" );
}

void test_import()
{
    auto surface = geode::load_triangulated_surface< 3 >(
        absl::StrCat( geode::DATA_PATH, "surface_degen.og_tsf3d" ) );
    std::vector< std::reference_wrapper< const geode::SurfaceMesh3D > > meshes{
        *surface
    };
    const auto merged = geode::merge_surface_meshes< 3 >( meshes );
    geode::save_triangulated_surface(
        *dynamic_cast< geode::TriangulatedSurface3D* >( merged.get() ),
        "output.og_tsf3d" );

    std::vector< geode::Point3D > points( merged->nb_vertices() );
    for( const auto v : geode::Range{ merged->nb_vertices() } )
    {
        points[v] = merged->point( v );
    }
    geode::NNSearch3D nns( points );
    const auto mappings = nns.colocated_index_mapping( geode::GLOBAL_EPSILON );
    OPENGEODE_EXCEPTION( mappings.nb_colocated_points() == 0,
        "[Test] Should be nomore colocated points" );
    for( const auto p : geode::Indices{ points } )
    {
        OPENGEODE_EXCEPTION(
            mappings.colocated_mapping[p] < mappings.unique_points.size(),
            "[Test] Wrong value of colocated_mapping (bigger than unique "
            "points size)" );
        const auto& colocated_point =
            mappings.unique_points[mappings.colocated_mapping[p]];
        OPENGEODE_EXCEPTION( points[p].inexact_equal( colocated_point ),
            "[Test] Colocated point is not close enough to original point" );
    }
    async::parallel_for(
        async::irange( geode::index_t{ 0 }, mappings.unique_points.size() ),
        [&mappings]( geode::index_t up0 ) {
            for( const auto up1 : geode::Indices{ mappings.unique_points } )
            {
                if( up1 <= up0 )
                {
                    continue;
                }
                OPENGEODE_EXCEPTION( !mappings.unique_points[up0].inexact_equal(
                                         mappings.unique_points[up1] ),
                    "[Test] Colocated points are too close" );
            }
        } );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    test_create();
    test_import();
}

OPENGEODE_TEST( "merge-surface" )