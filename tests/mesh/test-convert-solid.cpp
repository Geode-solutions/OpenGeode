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
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/light_regular_grid.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/helpers/detail/split_along_solid_facets.h>
#include <geode/mesh/io/hybrid_solid_output.h>
#include <geode/mesh/io/tetrahedral_solid_output.h>

#include <geode/mesh/helpers/convert_solid_mesh.h>

void test_tetrahedral_solid()
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
    geode::detail::SplitAlongSolidFacets splitter{ *tet_solid_from_light_grid,
        *tet_builder };
    splitter.split_solid_along_facets( {} );
}

void test_hybrid_solid()
{
    geode::OpenGeodeMeshLibrary::initialize();
    std::vector< geode::Point3D > hex_points{
        { { 0, 0, 0 } },
        { { 1, 0, 0 } },
        { { 0, 1, 0 } },
        { { 1, 1, 0 } },
        { { 0, 0, 1 } },
        { { 1, 0, 1 } },
        { { 0, 1, 1 } },
        { { 1, 1, 1 } },
    };
    auto hex = geode::SolidMesh3D::create();
    auto builder_hex = geode::SolidMeshBuilder3D::create( *hex );
    for( const auto& point : hex_points )
    {
        builder_hex->create_point( point );
    }
    builder_hex->create_polyhedron( { 0, 1, 2, 3, 4, 5, 6, 7 },
        { { 0, 2, 3, 1 }, { 0, 4, 5, 1 }, { 3, 1, 5, 7 }, { 4, 6, 7, 5 },
            { 2, 3, 7, 6 }, { 0, 2, 6, 4 } } );
    auto hybrid_solid_hex = geode::convert_solid_mesh_into_hybrid_solid( *hex );
    geode::save_hybrid_solid(
        *hybrid_solid_hex.value(), "hybrid_solid_hex.og_hso3d" );
    std::vector< geode::Point3D > prism_points{
        { { 0, 0, 0 } },
        { { 1, 0, 0 } },
        { { 0, 1, 0 } },
        { { 0, 0, 1 } },
        { { 1, 0, 1 } },
        { { 0, 1, 1 } },
    };
    auto prism = geode::SolidMesh3D::create();
    auto builder_prism = geode::SolidMeshBuilder3D::create( *prism );
    for( const auto& point : prism_points )
    {
        builder_prism->create_point( point );
    }
    builder_prism->create_polyhedron(
        { 0, 1, 2, 3, 4, 5 }, { { 0, 2, 1 }, { 3, 5, 4 }, { 0, 3, 4, 1 },
                                  { 0, 3, 5, 2 }, { 1, 4, 5, 2 } } );
    auto hybrid_solid_prism =
        geode::convert_solid_mesh_into_hybrid_solid( *prism );
    geode::save_hybrid_solid(
        *hybrid_solid_prism.value(), "hybrid_solid_prism.og_hso3d" );
    std::vector< geode::Point3D > pyramid_points{ { { 0, 0, 0 } },
        { { 1, 0, 0 } }, { { 1, 1, 0 } }, { { 0, 1, 0 } },
        { { 0.5, 0.5, 1 } } };
    auto pyramid = geode::SolidMesh3D::create();
    auto builder_pyramid = geode::SolidMeshBuilder3D::create( *pyramid );
    for( const auto& point : pyramid_points )
    {
        builder_pyramid->create_point( point );
    }
    builder_pyramid->create_polyhedron(
        { 0, 1, 2, 3, 4 }, { { 0, 4, 1 }, { 0, 4, 3 }, { 1, 4, 2 }, { 4, 3, 2 },
                               { 0, 3, 2, 1 } } );
    auto hybrid_solid_pyramid =
        geode::convert_solid_mesh_into_hybrid_solid( *pyramid );
    geode::save_hybrid_solid(
        *hybrid_solid_pyramid.value(), "hybrid_solid_pyramid.og_hso3d" );
}
void test()
{
    test_tetrahedral_solid();
    test_hybrid_solid();
}

OPENGEODE_TEST( "convert-solid" )