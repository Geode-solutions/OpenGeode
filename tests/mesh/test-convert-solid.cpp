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

#include <geode/tests/common.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/vector.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/light_regular_grid.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/helpers/detail/cut_along_solid_facets.h>
#include <geode/mesh/io/tetrahedral_solid_output.h>

#include <geode/mesh/helpers/convert_solid_mesh.h>

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto mesh_grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *mesh_grid );
    builder->initialize_grid( { { 1, 1.5, 1.1 } }, { 5, 5, 5 }, 5 );
    const auto tet_solid_from_mesh_grid_1 =
        geode::convert_solid_mesh_into_tetrahedral_solid( *mesh_grid );
    const auto tet_solid_from_mesh_grid_2 =
        geode::convert_grid_into_tetrahedral_solid( *mesh_grid );
    const geode::LightRegularGrid3D light_grid{ { { 1, 1.5, 1.1 } },
        { 5, 5, 5 }, { 5, 5, 5 } };
    auto tet_solid_from_light_grid =
        geode::convert_grid_into_tetrahedral_solid( *mesh_grid );
    const geode::index_t nb_vertices = 6 * 6 * 6;
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_1.value()->nb_vertices() == nb_vertices,
        "[Test] Number of vertices in TetrahedralSolid3D from RegularGrid3D as "
        "mesh is not correct." );
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_2->nb_vertices() == nb_vertices,
        "[Test] Number of vertices in TetrahedralSolid3D from RegularGrid3D as "
        "grid is not correct." );
    OPENGEODE_EXCEPTION(
        tet_solid_from_light_grid->nb_vertices() == nb_vertices,
        "[Test] Number of vertices in TetrahedralSolid3D from LightRegularGrid "
        "is not correct" );
    const geode::index_t nb_tetrahedra = 5 * 5 * 5 * 6;
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_1.value()->nb_polyhedra() == nb_tetrahedra,
        "[Test] Number of tetrahedra in TetrahedralSolid3D from RegularGrid3D "
        "as mesh is not correct." );
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_2->nb_polyhedra() == nb_tetrahedra,
        "[Test] Number of tetrahedra in TetrahedralSolid3D from RegularGrid3D "
        "as grid is not correct." );
    OPENGEODE_EXCEPTION(
        tet_solid_from_light_grid->nb_polyhedra() == nb_tetrahedra,
        "[Test] Number of tetrahedra in TetrahedralSolid3D from "
        "LightRegularGrid is not correct" );

    auto tet_builder =
        geode::TetrahedralSolidBuilder3D::create( *tet_solid_from_light_grid );
    geode::detail::CutAlongSolidFacets cutter{ *tet_solid_from_light_grid,
        *tet_builder };
    cutter.cut_solid_along_facets( {} );
}

OPENGEODE_TEST( "convert-solid" )