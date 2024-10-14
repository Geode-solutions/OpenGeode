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

#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/regular_grid_solid_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/helpers/convert_solid_mesh.hpp>
#include <geode/mesh/helpers/detail/split_along_solid_facets.hpp>
#include <geode/mesh/io/hybrid_solid_output.hpp>
#include <geode/mesh/io/tetrahedral_solid_output.hpp>

// NOLINTBEGIN(*-magic-numbers)

void test_tetrahedral_solid()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto mesh_grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *mesh_grid );
    builder->initialize_grid(
        geode::Point3D{ { 1, 1.5, 1.1 } }, { 5, 5, 5 }, 5 );
    const auto tet_solid_from_mesh_grid_1 =
        geode::convert_solid_mesh_into_tetrahedral_solid( *mesh_grid );
    const auto tet_solid_from_mesh_grid_2 =
        geode::convert_grid_into_tetrahedral_solid( *mesh_grid );
    const geode::LightRegularGrid3D light_grid{
        geode::Point3D{ { 1, 1.5, 1.1 } }, { 5, 5, 5 }, { 5, 5, 5 }
    };
    std::vector< geode::index_t > cells_to_densify( 25 );
    absl::c_iota( cells_to_densify, 25 );
    auto tet_solid_from_light_grid =
        geode::convert_grid_into_densified_tetrahedral_solid(
            *mesh_grid, cells_to_densify );
    geode::save_tetrahedral_solid(
        *tet_solid_from_light_grid, "test_densified_tet_from_grid.og_tso3d" );
    const geode::index_t nb_vertices = 6 * 6 * 6;
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_1.value()->nb_vertices() == nb_vertices,
        "[Test] Number of vertices in TetrahedralSolid3D from RegularGrid3D as "
        "mesh is not correct." );
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_2->nb_vertices() == nb_vertices,
        "[Test] Number of vertices in TetrahedralSolid3D from RegularGrid3D as "
        "grid is not correct." );
    const geode::index_t nb_densified_vertices =
        nb_vertices + cells_to_densify.size();
    OPENGEODE_EXCEPTION( tet_solid_from_light_grid->nb_vertices()
                             == nb_vertices + cells_to_densify.size(),
        "[Test] Number of vertices in TetrahedralSolid3D from LightRegularGrid "
        "is not correct: ",
        tet_solid_from_light_grid->nb_vertices(), "instead of " );
    const geode::index_t nb_tetrahedra = 5 * 5 * 5 * 6;
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_1.value()->nb_polyhedra() == nb_tetrahedra,
        "[Test] Number of tetrahedra in TetrahedralSolid3D from RegularGrid3D "
        "as mesh is not correct." );
    OPENGEODE_EXCEPTION(
        tet_solid_from_mesh_grid_2->nb_polyhedra() == nb_tetrahedra,
        "[Test] Number of tetrahedra in TetrahedralSolid3D from RegularGrid3D "
        "as grid is not correct." );
    OPENGEODE_EXCEPTION( tet_solid_from_light_grid->nb_polyhedra()
                             == nb_tetrahedra + 6 * cells_to_densify.size(),
        "[Test] Number of tetrahedra in TetrahedralSolid3D from "
        "LightRegularGrid is not correct" );
    for( const auto polyhedron_id :
        geode::Range{ tet_solid_from_light_grid->nb_polyhedra() } )
    {
        const auto tet_volume =
            tet_solid_from_light_grid->polyhedron_volume( polyhedron_id );
        OPENGEODE_EXCEPTION( tet_volume > 0, "[Test] Polyhedron ",
            polyhedron_id,
            " in TetrahedralSolid3D from RegularGrid3D should have a positive "
            "volume, not ",
            tet_volume );
    }

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
        geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 0, 0 } },
        geode::Point3D{ { 0, 1, 0 } },
        geode::Point3D{ { 1, 1, 0 } },
        geode::Point3D{ { 0, 0, 1 } },
        geode::Point3D{ { 1, 0, 1 } },
        geode::Point3D{ { 0, 1, 1 } },
        geode::Point3D{ { 1, 1, 1 } },
    };
    auto hex = geode::SolidMesh3D::create();
    auto builder_hex = geode::SolidMeshBuilder3D::create( *hex );
    for( const auto& point : hex_points )
    {
        builder_hex->create_point( point );
    }
    builder_hex->create_polyhedron( { 0, 1, 2, 3, 4, 5, 6, 7 },
        { { 0, 1, 3, 2 }, { 0, 4, 5, 1 }, { 3, 1, 5, 7 }, { 4, 6, 7, 5 },
            { 2, 3, 7, 6 }, { 0, 2, 6, 4 } } );
    auto hybrid_solid_hex = geode::convert_solid_mesh_into_hybrid_solid( *hex );
    geode::save_hybrid_solid(
        *hybrid_solid_hex.value(), "hybrid_solid_hex.og_hso3d" );
    std::vector< geode::Point3D > prism_points{
        geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 0, 0 } },
        geode::Point3D{ { 0.5, 1, 0 } },
        geode::Point3D{ { 0, 0, 1 } },
        geode::Point3D{ { 1, 0, 1 } },
        geode::Point3D{ { 0.5, 1, 1 } },
    };
    auto prism = geode::SolidMesh3D::create();
    auto builder_prism = geode::SolidMeshBuilder3D::create( *prism );
    for( const auto& point : prism_points )
    {
        builder_prism->create_point( point );
    }
    builder_prism->create_polyhedron(
        { 0, 1, 2, 3, 4, 5 }, { { 0, 1, 2 }, { 3, 5, 4 }, { 0, 3, 4, 1 },
                                  { 0, 2, 5, 3 }, { 1, 4, 5, 2 } } );
    auto hybrid_solid_prism =
        geode::convert_solid_mesh_into_hybrid_solid( *prism );
    geode::save_hybrid_solid(
        *hybrid_solid_prism.value(), "hybrid_solid_prism.og_hso3d" );
    std::vector< geode::Point3D > pyramid_points{ geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 0, 0 } }, geode::Point3D{ { 1, 1, 0 } },
        geode::Point3D{ { 0, 1, 0 } }, geode::Point3D{ { 0.5, 0.5, 1 } } };
    auto pyramid = geode::SolidMesh3D::create();
    auto builder_pyramid = geode::SolidMeshBuilder3D::create( *pyramid );
    for( const auto& point : pyramid_points )
    {
        builder_pyramid->create_point( point );
    }
    builder_pyramid->create_polyhedron(
        { 0, 1, 2, 3, 4 }, { { 0, 4, 1 }, { 3, 4, 0 }, { 1, 4, 2 }, { 4, 3, 2 },
                               { 0, 1, 2, 3 } } );
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
// NOLINTEND(*-magic-numbers)