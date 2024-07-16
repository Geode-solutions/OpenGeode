/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

#include <geode/mesh/helpers/convert_surface_mesh.hpp>

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    const auto surface2d = geode::load_triangulated_surface< 2 >(
        absl::StrCat( geode::DATA_PATH, "3patches.og_tsf2d" ) );
    const auto surface3d =
        geode::convert_triangulated_surface2d_into_3d( *surface2d, 2, 0. );
    OPENGEODE_EXCEPTION( surface2d->nb_vertices() == surface3d->nb_vertices(),
        "[Test] Number of vertices in TriangulatedSurface3D is not "
        "correct" );
    OPENGEODE_EXCEPTION( surface2d->nb_polygons() == surface3d->nb_polygons(),
        "[Test] Number of polygons in TriangulatedSurface3D is not "
        "correct" );

    const auto surface_mesh3d =
        geode::convert_surface_mesh2d_into_3d( *surface2d, 2, 0. );
    OPENGEODE_EXCEPTION(
        surface2d->nb_vertices() == surface_mesh3d->nb_vertices(),
        "[Test] Number of vertices in SurfaceMesh3D is not "
        "correct" );
    OPENGEODE_EXCEPTION(
        surface2d->nb_polygons() == surface_mesh3d->nb_polygons(),
        "[Test] Number of polygons in SurfaceMesh3D is not "
        "correct" );

    const auto converted_surface2d =
        geode::convert_triangulated_surface3d_into_2d( *surface3d, 2 );
    OPENGEODE_EXCEPTION(
        surface2d->nb_vertices() == converted_surface2d->nb_vertices(),
        "[Test] Number of vertices in re-converted TriangulatedSurface2D "
        "is not correct" );
    OPENGEODE_EXCEPTION(
        surface2d->nb_polygons() == converted_surface2d->nb_polygons(),
        "[Test] Number of polygons in re-converted TriangulatedSurface2D "
        "is not correct" );

    geode::LightRegularGrid2D grid{ geode::Point2D{ { 1, 2 } }, { 5, 6 },
        { 1, 1 } };
    const auto converted_grid2d =
        geode::convert_grid_into_triangulated_surface( grid );
    OPENGEODE_EXCEPTION( converted_grid2d->nb_polygons() == 5 * 6 * 2,
        "[Test] Number of polygons in TriangulatedSurface2D from grid is not "
        "correct." );
}

OPENGEODE_TEST( "convert-surface" )