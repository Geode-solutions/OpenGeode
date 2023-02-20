/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/core/brep.h>

#include <geode/model/helpers/surface_radial_sort.h>

#include <geode/tests/common.h>

void test()
{
    geode::OpenGeodeModel::initialize();
    std::vector< geode::Point3D > points{ { { 0, 0, 0 } }, { { 0, 1, 0 } },
        { { 0, 0, 1 } }, { { -1, 0, -1 } }, { { 1, 0, -1 } } };

    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    builder.create_unique_vertices( points.size() );

    const auto line_id = builder.add_line();
    const auto& line = brep.line( line_id );
    auto line_builder = builder.line_mesh_builder( line_id );
    line_builder->create_point( points[0] );
    builder.set_unique_vertex( { line.component_id(), 0 }, 0 );
    line_builder->create_point( points[1] );
    builder.set_unique_vertex( { line.component_id(), 1 }, 1 );
    line_builder->create_edge( 0, 1 );

    const auto surface0_id = builder.add_surface();
    const auto& surface0 = brep.surface( surface0_id );
    auto surface0_builder = builder.surface_mesh_builder( surface0_id );
    surface0_builder->create_point( points[0] );
    builder.set_unique_vertex( { surface0.component_id(), 0 }, 0 );
    surface0_builder->create_point( points[1] );
    builder.set_unique_vertex( { surface0.component_id(), 1 }, 1 );
    surface0_builder->create_point( points[2] );
    builder.set_unique_vertex( { surface0.component_id(), 2 }, 2 );
    surface0_builder->create_polygon( { 0, 1, 2 } );

    const auto surface1_id = builder.add_surface();
    const auto& surface1 = brep.surface( surface1_id );
    auto surface1_builder = builder.surface_mesh_builder( surface1_id );
    surface1_builder->create_point( points[0] );
    builder.set_unique_vertex( { surface1.component_id(), 0 }, 0 );
    surface1_builder->create_point( points[1] );
    builder.set_unique_vertex( { surface1.component_id(), 1 }, 1 );
    surface1_builder->create_point( points[3] );
    builder.set_unique_vertex( { surface1.component_id(), 2 }, 3 );
    surface1_builder->create_polygon( { 0, 1, 2 } );

    const auto surface2_id = builder.add_surface();
    const auto& surface2 = brep.surface( surface2_id );
    auto surface2_builder = builder.surface_mesh_builder( surface2_id );
    surface2_builder->create_point( points[1] );
    builder.set_unique_vertex( { surface2.component_id(), 0 }, 1 );
    surface2_builder->create_point( points[0] );
    builder.set_unique_vertex( { surface2.component_id(), 1 }, 0 );
    surface2_builder->create_point( points[4] );
    builder.set_unique_vertex( { surface2.component_id(), 2 }, 4 );
    surface2_builder->create_polygon( { 0, 1, 2 } );

    geode::SortedSurfaces answer( 3 );
    geode::index_t count{ 0 };
    answer.surfaces[count++] = { surface0_id, geode::SidedSurface::POSITIVE };
    answer.surfaces[count++] = { surface0_id, geode::SidedSurface::NEGATIVE };
    answer.surfaces[count++] = { surface1_id, geode::SidedSurface::POSITIVE };
    answer.surfaces[count++] = { surface1_id, geode::SidedSurface::NEGATIVE };
    answer.surfaces[count++] = { surface2_id, geode::SidedSurface::NEGATIVE };
    answer.surfaces[count++] = { surface2_id, geode::SidedSurface::POSITIVE };

    const auto sorted = geode::surface_radial_sort( brep, line );

    const auto it = absl::c_find( answer.surfaces, sorted.surfaces.front() );
    OPENGEODE_EXCEPTION(
        it != answer.surfaces.end(), "[Test] Wrong radial sort" );
    auto i = std::distance( answer.surfaces.begin(), it );

    for( const auto& surface : sorted.surfaces )
    {
        OPENGEODE_EXCEPTION(
            surface == answer.surfaces[i], "[Test] Wrong surface" );
        i = ( i + 1 ) % answer.surfaces.size();
    }

    const auto test0 = sorted.next(
        sorted.find( { surface2_id, geode::SidedSurface::POSITIVE } ).front() );
    OPENGEODE_EXCEPTION(
        sorted.surfaces[test0].id == surface0_id, "[Test] Wrong surface" );
    OPENGEODE_EXCEPTION(
        sorted.surfaces[test0].side == geode::SidedSurface::POSITIVE,
        "[Test] Wrong side" );

    const auto test1 = sorted.next(
        sorted.find( { surface0_id, geode::SidedSurface::POSITIVE } ).front() );
    OPENGEODE_EXCEPTION(
        sorted.surfaces[test1].id == surface2_id, "[Test] Wrong surface" );
    OPENGEODE_EXCEPTION(
        sorted.surfaces[test1].side == geode::SidedSurface::POSITIVE,
        "[Test] Wrong side" );

    const auto test2 = sorted.next(
        sorted.find( { surface0_id, geode::SidedSurface::NEGATIVE } ).front() );
    OPENGEODE_EXCEPTION(
        sorted.surfaces[test2].id == surface1_id, "[Test] Wrong surface" );
    OPENGEODE_EXCEPTION(
        sorted.surfaces[test2].side == geode::SidedSurface::POSITIVE,
        "[Test] Wrong side" );

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "surface-radial-sort" )