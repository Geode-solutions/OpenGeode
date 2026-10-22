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

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mensurations.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    const std::array< geode::Point3D, 4 > points{ geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 0, 0 } }, geode::Point3D{ { 0, 1, 0 } },
        geode::Point3D{ { 0, 0, 1 } } };

    geode::uuid add_line( geode::BRep& brep,
        geode::BRepBuilder& builder,
        geode::index_t uv0,
        geode::index_t uv1 )
    {
        const auto line_id = builder.add_line();
        const auto& line = brep.line( line_id );
        auto mesh_builder = builder.line_mesh_builder( line );
        mesh_builder->create_point( points[uv0] );
        mesh_builder->create_point( points[uv1] );
        mesh_builder->create_edge( 0, 1 );
        builder.set_unique_vertex( { line.component_id(), 0 }, uv0 );
        builder.set_unique_vertex( { line.component_id(), 1 }, uv1 );
        return line_id;
    }

    struct Tetrahedron
    {
        geode::BRep brep;
        geode::uuid block_id;
    };

    enum struct EXTRA_LINE
    {
        none,
        empty,
        mismatched
    };

    // Tetrahedron block without block mesh, bounded by 4 surfaces.
    // An extra line can be added as the first boundary of two surfaces: an
    // empty line, or a line whose edge is not an edge of all its incident
    // surfaces.
    std::unique_ptr< Tetrahedron > build_tetrahedron( EXTRA_LINE extra_line )
    {
        auto result = std::make_unique< Tetrahedron >();
        auto& brep = result->brep;
        geode::BRepBuilder builder{ brep };
        builder.create_unique_vertices( 4 );
        const std::array< std::array< geode::index_t, 3 >, 4 > faces{ {
            { 0, 2, 1 },
            { 0, 1, 3 },
            { 0, 3, 2 },
            { 1, 2, 3 },
        } };
        std::array< geode::uuid, 4 > surface_ids;
        for( const auto f : geode::LIndices{ faces } )
        {
            surface_ids[f] = builder.add_surface();
            const auto& surface = brep.surface( surface_ids[f] );
            auto mesh_builder = builder.surface_mesh_builder( surface );
            for( const auto v : geode::LRange{ 3 } )
            {
                mesh_builder->create_point( points[faces[f][v]] );
                builder.set_unique_vertex(
                    { surface.component_id(), v }, faces[f][v] );
            }
            mesh_builder->create_polygon( { 0, 1, 2 } );
        }
        if( extra_line != EXTRA_LINE::none )
        {
            geode::uuid line_id;
            if( extra_line == EXTRA_LINE::empty )
            {
                line_id = builder.add_line();
            }
            else
            {
                // Edge 0-2 is in surfaces 0 and 2, not in surface 1
                line_id = add_line( brep, builder, 0, 2 );
            }
            for( const auto f : geode::LRange{ 2 } )
            {
                builder.add_line_surface_boundary_relationship(
                    brep.line( line_id ), brep.surface( surface_ids[f] ) );
            }
        }
        for( const auto v0 : geode::LRange{ 4 } )
        {
            for( const auto v1 : geode::LRange{ v0 + 1, 4 } )
            {
                const auto line_id = add_line( brep, builder, v0, v1 );
                for( const auto f : geode::LIndices{ faces } )
                {
                    if( absl::c_linear_search( faces[f], v0 )
                        && absl::c_linear_search( faces[f], v1 ) )
                    {
                        builder.add_line_surface_boundary_relationship(
                            brep.line( line_id ),
                            brep.surface( surface_ids[f] ) );
                    }
                }
            }
        }
        result->block_id = builder.add_block();
        for( const auto& surface_id : surface_ids )
        {
            builder.add_surface_block_boundary_relationship(
                brep.surface( surface_id ), brep.block( result->block_id ) );
        }
        return result;
    }

    void check_tetrahedron_volume( EXTRA_LINE extra_line )
    {
        const auto tetrahedron = build_tetrahedron( extra_line );
        const auto& brep = tetrahedron->brep;
        const auto volume =
            geode::block_volume( brep, brep.block( tetrahedron->block_id ) );
        geode::OpenGeodeModelException::test(
            std::fabs( volume - 1. / 6. ) < geode::GLOBAL_EPSILON,
            "Wrong tetrahedron volume: ", volume );
    }
} // namespace

void test_block_volume()
{
    check_tetrahedron_volume( EXTRA_LINE::none );
    const auto tetrahedron = build_tetrahedron( EXTRA_LINE::none );
    const auto box = geode::block_bounding_box(
        tetrahedron->brep, tetrahedron->brep.block( tetrahedron->block_id ) );
    geode::OpenGeodeModelException::test(
        box.min() == points[0] && box.max() == geode::Point3D{ { 1, 1, 1 } },
        "Wrong tetrahedron bounding box" );
}

void test_boundary_line_without_edge()
{
    check_tetrahedron_volume( EXTRA_LINE::empty );
}

void test_boundary_line_not_in_all_surfaces()
{
    check_tetrahedron_volume( EXTRA_LINE::mismatched );
}

void test_block_without_boundary()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    const auto& block = brep.block( builder.add_block() );
    geode::OpenGeodeModelException::test(
        geode::block_volume( brep, block ) == 0,
        "Block without mesh and boundary should have a null volume" );
}

void test_block_with_empty_boundary()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    const auto& block = brep.block( builder.add_block() );
    const auto& surface = brep.surface( builder.add_surface() );
    builder.add_surface_block_boundary_relationship( surface, block );
    geode::OpenGeodeModelException::test(
        geode::block_volume( brep, block ) == 0,
        "Block with empty boundary should have a null volume" );
    const auto box = geode::block_bounding_box( brep, block );
    geode::OpenGeodeModelException::test(
        box.min().value( 0 ) > box.max().value( 0 ),
        "Block with empty boundary should have an empty bounding box" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_block_volume();
    test_boundary_line_without_edge();
    test_boundary_line_not_in_all_surfaces();
    test_block_without_boundary();
    test_block_with_empty_boundary();
}

OPENGEODE_TEST( "component-mensurations" )
