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

#include <geode/tests/common.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mesh_edges.hpp>
#include <geode/model/helpers/compute_unique_vertices.hpp>
#include <geode/model/helpers/detail/split_along_surface_mesh_borders.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    // 3x3 grid of points, the middle column being the line x = 0 (or the line
    // z = 0 for the vertical surface)
    geode::uuid add_grid_surface(
        geode::BRep& brep, geode::BRepBuilder& builder, bool horizontal )
    {
        const auto surface_id = builder.add_surface();
        auto mesh_builder =
            builder.surface_mesh_builder( brep.surface( surface_id ) );
        for( const auto j : geode::Range{ 3 } )
        {
            for( const auto i : geode::Range{ 3 } )
            {
                const auto u = static_cast< double >( i ) - 1.;
                const auto v = static_cast< double >( j ) - 1.;
                if( horizontal )
                {
                    mesh_builder->create_point( geode::Point3D{ { u, v, 0 } } );
                }
                else
                {
                    mesh_builder->create_point( geode::Point3D{ { 0, v, u } } );
                }
            }
        }
        for( const auto j : geode::Range{ 2 } )
        {
            for( const auto i : geode::Range{ 2 } )
            {
                const auto v0 = j * 3 + i;
                mesh_builder->create_polygon( { v0, v0 + 1, v0 + 4 } );
                mesh_builder->create_polygon( { v0, v0 + 4, v0 + 3 } );
            }
        }
        mesh_builder->compute_polygon_adjacencies();
        return surface_id;
    }
} // namespace

void test_crossing_surfaces()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    // Two surfaces crossing along a line internal to both of them
    const auto horizontal_id = add_grid_surface( brep, builder, true );
    const auto vertical_id = add_grid_surface( brep, builder, false );
    const auto line_id = builder.add_line();
    const auto& line = brep.line( line_id );
    auto line_builder = builder.line_mesh_builder( line );
    for( const auto v : geode::Range{ 3 } )
    {
        line_builder->create_point(
            geode::Point3D{ { 0, static_cast< double >( v ) - 1., 0 } } );
    }
    line_builder->create_edge( 0, 1 );
    line_builder->create_edge( 1, 2 );
    for( const auto& surface_id : { horizontal_id, vertical_id } )
    {
        builder.add_line_surface_internal_relationship(
            line, brep.surface( surface_id ) );
    }
    geode::compute_model_unique_vertices( brep, builder );
    const auto nb_unique_vertices = brep.nb_unique_vertices();

    geode::detail::SplitAlongSurfaceMeshBorders< geode::BRep > splitter{ brep,
        builder };
    const auto mapping = splitter.split_all_surfaces();
    geode::OpenGeodeModelException::test(
        mapping.size() == 6, "Wrong number of split vertices" );
    geode::OpenGeodeModelException::test(
        brep.nb_unique_vertices() == nb_unique_vertices,
        "Split should not create unique vertices" );
    for( const auto& [initial, created] : mapping )
    {
        geode::OpenGeodeModelException::test(
            brep.unique_vertex( initial ) == brep.unique_vertex( created ),
            "Split vertices should share the same unique vertex" );
    }
    for( const auto& surface_id : { horizontal_id, vertical_id } )
    {
        const auto& surface = brep.surface( surface_id );
        geode::OpenGeodeModelException::test(
            surface.mesh().nb_vertices() == 12,
            "Wrong number of vertices after split" );
        for( const auto edge_id : geode::Range{ line.mesh().nb_edges() } )
        {
            const auto edges = geode::detail::surface_component_mesh_edges(
                brep, geode::edge_unique_vertices( brep, line, edge_id ),
                surface );
            geode::OpenGeodeModelException::test(
                edges.size() == 2, "Wrong number of surface edges on line" );
            for( const auto& edge : edges )
            {
                geode::OpenGeodeModelException::test(
                    surface.mesh().is_edge_on_border( edge ),
                    "Surface edge on line should be on border after split" );
            }
        }
    }
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_crossing_surfaces();
}

OPENGEODE_TEST( "split-along-surface-mesh-borders" )
