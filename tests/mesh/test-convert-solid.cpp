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

#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/regular_grid_solid_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
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

void check_adjacencies_consistency(
    const geode::SolidMesh3D& solid, geode::index_t expected_nb_adjacencies )
{
    geode::index_t nb_adjacencies{ 0 };
    for( const auto p : geode::Range{ solid.nb_polyhedra() } )
    {
        for( const auto f : geode::LRange{ solid.nb_polyhedron_facets( p ) } )
        {
            const auto adjacent = solid.polyhedron_adjacent( { p, f } );
            if( !adjacent )
            {
                continue;
            }
            nb_adjacencies++;
            auto vertices = solid.polyhedron_facet_vertices( { p, f } );
            absl::c_sort( vertices );
            bool found{ false };
            for( const auto adj_f : geode::LRange{
                     solid.nb_polyhedron_facets( adjacent.value() ) } )
            {
                auto adj_vertices = solid.polyhedron_facet_vertices(
                    { adjacent.value(), adj_f } );
                absl::c_sort( adj_vertices );
                if( adj_vertices == vertices )
                {
                    geode::OpenGeodeMeshException::test(
                        solid.polyhedron_adjacent( { adjacent.value(), adj_f } )
                            == p,
                        "Adjacency is not symmetric between polyhedra ", p,
                        " and ", adjacent.value() );
                    found = true;
                    break;
                }
            }
            geode::OpenGeodeMeshException::test( found, "Facet ", f,
                " of polyhedron ", p, " is not shared with its adjacent ",
                adjacent.value() );
        }
    }
    geode::OpenGeodeMeshException::test(
        nb_adjacencies == expected_nb_adjacencies,
        "Wrong number of adjacencies: ", nb_adjacencies, " instead of ",
        expected_nb_adjacencies );
}

void check_positive_volumes( const geode::SolidMesh3D& solid )
{
    for( const auto p : geode::Range{ solid.nb_polyhedra() } )
    {
        const auto volume = solid.polyhedron_volume( p );
        geode::OpenGeodeMeshException::test( volume > 0, "Polyhedron ", p,
            " should have a positive volume, not ", volume );
    }
}

void test_grid_to_solids()
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
    const auto hybrid_solid_from_mesh_grid =
        geode::convert_grid_into_hybrid_solid( *mesh_grid );
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
    geode::OpenGeodeMeshException::test(
        tet_solid_from_mesh_grid_1.value()->nb_vertices() == nb_vertices,
        "Number of vertices in TetrahedralSolid3D from RegularGrid3D as "
        "mesh is not correct." );
    geode::OpenGeodeMeshException::test(
        tet_solid_from_mesh_grid_2->nb_vertices() == nb_vertices,
        "Number of vertices in TetrahedralSolid3D from RegularGrid3D as "
        "grid is not correct." );
    geode::OpenGeodeMeshException::test(
        hybrid_solid_from_mesh_grid->nb_vertices() == nb_vertices,
        "Number of vertices in HybridSolid3D from RegularGrid3D as "
        "grid is not correct." );
    const geode::index_t nb_densified_vertices =
        nb_vertices + 3 * cells_to_densify.size() + 2 * 5 * 6 + 2 * 5 * 5;
    geode::OpenGeodeMeshException::test(
        tet_solid_from_light_grid->nb_vertices() == nb_densified_vertices,
        "Number of vertices in TetrahedralSolid3D from LightRegularGrid "
        "is not correct: ",
        tet_solid_from_light_grid->nb_vertices(), " instead of ",
        nb_densified_vertices );
    const geode::index_t nb_tetrahedra = 5 * 5 * 5 * 6;
    geode::OpenGeodeMeshException::test(
        tet_solid_from_mesh_grid_1.value()->nb_polyhedra() == nb_tetrahedra,
        "Number of tetrahedra in TetrahedralSolid3D from RegularGrid3D "
        "as mesh is not correct." );
    geode::OpenGeodeMeshException::test(
        tet_solid_from_mesh_grid_2->nb_polyhedra() == nb_tetrahedra,
        "Number of tetrahedra in TetrahedralSolid3D from RegularGrid3D "
        "as grid is not correct." );
    geode::OpenGeodeMeshException::test(
        tet_solid_from_mesh_grid_2->nb_polyhedra() == nb_tetrahedra,
        "Number of tetrahedra in TetrahedralSolid3D from RegularGrid3D "
        "as grid is not correct." );
    geode::OpenGeodeMeshException::test(
        hybrid_solid_from_mesh_grid->nb_polyhedra() == mesh_grid->nb_cells(),
        "Number of hexahedra in HybridSolid3D from "
        "RegularGrid is not correct" );
    for( const auto polyhedron_id :
        geode::Range{ tet_solid_from_light_grid->nb_polyhedra() } )
    {
        const auto tet_volume =
            tet_solid_from_light_grid->polyhedron_volume( polyhedron_id );
        geode::OpenGeodeMeshException::test( tet_volume > 0, "Polyhedron ",
            polyhedron_id,
            " in TetrahedralSolid3D from RegularGrid3D should have a positive "
            "volume, not ",
            tet_volume );
    }
    const geode::index_t nb_densified_tetras =
        5 * 5 * ( 2 * 6 + 2 * ( 5 * 2 + 4 ) + 6 * 4 );
    geode::OpenGeodeMeshException::test(
        tet_solid_from_light_grid->nb_polyhedra() == nb_densified_tetras,
        "Number of tetrahedra in TetrahedralSolid3D from "
        "LightRegularGrid "
        "is not correct: ",
        tet_solid_from_light_grid->nb_polyhedra(), " instead of ",
        nb_densified_tetras );

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
    check_positive_volumes( *hybrid_solid_hex.value() );
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
    check_positive_volumes( *hybrid_solid_prism.value() );
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
    check_positive_volumes( *hybrid_solid_pyramid.value() );
}

void test_hybrid_solid_adjacencies()
{
    auto solid = geode::SolidMesh3D::create();
    auto builder = geode::SolidMeshBuilder3D::create( *solid );
    for( const auto z : geode::Range{ 2 } )
    {
        for( const auto y : geode::Range{ 2 } )
        {
            for( const auto x : geode::Range{ 3 } )
            {
                builder->create_point( geode::Point3D{
                    { static_cast< double >( x ), static_cast< double >( y ),
                        static_cast< double >( z ) } } );
            }
        }
    }
    const std::vector< std::vector< geode::local_index_t > > hex_facets{
        { 0, 1, 3, 2 }, { 0, 4, 5, 1 }, { 3, 1, 5, 7 }, { 4, 6, 7, 5 },
        { 2, 3, 7, 6 }, { 0, 2, 6, 4 }
    };
    for( const auto offset : geode::Range{ 2 } )
    {
        std::vector< geode::index_t > vertices;
        for( const auto v : geode::Range{ 8 } )
        {
            vertices.push_back(
                ( v & 1 ) + offset + 3 * ( ( v >> 1 ) & 1 ) + 6 * ( v >> 2 ) );
        }
        builder->create_polyhedron( vertices, hex_facets );
    }
    builder->compute_polyhedron_adjacencies();
    const auto hybrid = geode::convert_solid_mesh_into_hybrid_solid( *solid );
    geode::OpenGeodeMeshException::test(
        hybrid.has_value(), "Hexahedra should be converted" );
    check_positive_volumes( *hybrid.value() );
    check_adjacencies_consistency( *hybrid.value(), 2 );

    auto prisms = geode::SolidMesh3D::create();
    auto prisms_builder = geode::SolidMeshBuilder3D::create( *prisms );
    prisms_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    prisms_builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
    prisms_builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
    prisms_builder->create_point( geode::Point3D{ { 1, 1, 0 } } );
    prisms_builder->create_point( geode::Point3D{ { 0, 0, 1 } } );
    prisms_builder->create_point( geode::Point3D{ { 1, 0, 1 } } );
    prisms_builder->create_point( geode::Point3D{ { 0, 1, 1 } } );
    prisms_builder->create_point( geode::Point3D{ { 1, 1, 1 } } );
    const std::vector< std::vector< geode::local_index_t > > prism_facets{
        { 0, 1, 2 }, { 3, 5, 4 }, { 0, 3, 4, 1 }, { 0, 2, 5, 3 }, { 1, 4, 5, 2 }
    };
    prisms_builder->create_polyhedron( { 0, 1, 2, 4, 5, 6 }, prism_facets );
    prisms_builder->create_polyhedron( { 1, 3, 2, 5, 7, 6 }, prism_facets );
    prisms_builder->compute_polyhedron_adjacencies();
    const auto hybrid_prisms =
        geode::convert_solid_mesh_into_hybrid_solid( *prisms );
    geode::OpenGeodeMeshException::test(
        hybrid_prisms.has_value(), "Prisms should be converted" );
    check_positive_volumes( *hybrid_prisms.value() );
    check_adjacencies_consistency( *hybrid_prisms.value(), 2 );

    auto pyramids = geode::SolidMesh3D::create();
    auto pyramids_builder = geode::SolidMeshBuilder3D::create( *pyramids );
    pyramids_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    pyramids_builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
    pyramids_builder->create_point( geode::Point3D{ { 1, 1, 0 } } );
    pyramids_builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
    pyramids_builder->create_point( geode::Point3D{ { 0.5, 0.5, 1 } } );
    pyramids_builder->create_point( geode::Point3D{ { 0.5, -1, 0.5 } } );
    pyramids_builder->create_polyhedron(
        { 0, 1, 2, 3, 4 }, { { 0, 4, 1 }, { 3, 4, 0 }, { 1, 4, 2 }, { 4, 3, 2 },
                               { 0, 1, 2, 3 } } );
    pyramids_builder->create_polyhedron( { 0, 1, 4, 5 },
        { { 0, 1, 2 }, { 1, 3, 2 }, { 0, 2, 3 }, { 0, 3, 1 } } );
    pyramids_builder->compute_polyhedron_adjacencies();
    const auto hybrid_pyramids =
        geode::convert_solid_mesh_into_hybrid_solid( *pyramids );
    geode::OpenGeodeMeshException::test(
        hybrid_pyramids.has_value(), "Pyramid should be converted" );
    check_adjacencies_consistency( *hybrid_pyramids.value(), 2 );
}

void test_tetrahedral_solid_adjacencies()
{
    auto solid = geode::SolidMesh3D::create();
    auto builder = geode::SolidMeshBuilder3D::create( *solid );
    builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
    builder->create_point( geode::Point3D{ { 0, 0, 1 } } );
    builder->create_point( geode::Point3D{ { 1, 1, 1 } } );
    const std::vector< std::vector< geode::local_index_t > > facets{
        { 0, 1, 2 }, { 0, 3, 1 }, { 0, 2, 3 }, { 1, 3, 2 }
    };
    builder->create_polyhedron( { 0, 1, 2, 3 }, facets );
    builder->create_polyhedron( { 1, 4, 2, 3 }, facets );
    builder->compute_polyhedron_adjacencies();
    const auto tet_solid =
        geode::convert_solid_mesh_into_tetrahedral_solid( *solid );
    geode::OpenGeodeMeshException::test(
        tet_solid.has_value(), "Tetrahedra should be converted" );
    check_adjacencies_consistency( *tet_solid.value(), 2 );
}

void test_densify_grid_cells_validation()
{
    auto grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid( geode::Point3D{ { 0, 0, 0 } }, { 3, 3, 3 }, 1 );
    const std::array< geode::index_t, 1 > single{ 13 };
    const auto reference =
        geode::convert_grid_into_densified_tetrahedral_solid( *grid, single );
    const std::array< geode::index_t, 2 > duplicated{ 13, 13 };
    const auto densified = geode::convert_grid_into_densified_tetrahedral_solid(
        *grid, duplicated );
    geode::OpenGeodeMeshException::test(
        densified->nb_vertices() == reference->nb_vertices()
            && densified->nb_polyhedra() == reference->nb_polyhedra(),
        "Duplicated cells to densify should be densified once" );
    const std::array< geode::index_t, 1 > out_of_range{ grid->nb_cells() };
    bool has_thrown{ false };
    try
    {
        const auto result =
            geode::convert_grid_into_densified_tetrahedral_solid(
                *grid, out_of_range );
    }
    catch( const geode::OpenGeodeException& )
    {
        has_thrown = true;
    }
    geode::OpenGeodeMeshException::test(
        has_thrown, "Out of range cell to densify should throw" );
}

void test()
{
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    test_grid_to_solids();
    test_hybrid_solid();
    test_hybrid_solid_adjacencies();
    test_tetrahedral_solid_adjacencies();
    test_densify_grid_cells_validation();
}

OPENGEODE_TEST( "convert-solid" )
// NOLINTEND(*-magic-numbers)