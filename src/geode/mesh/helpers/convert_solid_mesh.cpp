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

#include <geode/mesh/helpers/convert_solid_mesh.h>

#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/hybrid_solid_builder.h>
#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/light_regular_grid.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/helpers/detail/element_identifier.h>
#include <geode/mesh/helpers/detail/solid_merger.h>
#include <geode/mesh/helpers/private/copy.h>

namespace
{
    bool all_polyhedra_are_simplex( const geode::SolidMesh3D& solid )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            if( !geode::detail::solid_polyhedron_is_a_tetrahedron( solid, p ) )
            {
                return false;
            }
        }
        return true;
    }

    void create_tetrahedra_from_grid_cells(
        const geode::TetrahedralSolid3D& tet_solid,
        geode::TetrahedralSolidBuilder3D& builder,
        const geode::Grid3D& grid )
    {
        builder.reserve_tetrahedra( 6 * grid.nb_cells() );
        geode::GenericMapping< geode::index_t > old2new_mapping;
        for( const auto k : geode::Range{ grid.nb_cells_in_direction( 2 ) } )
        {
            for( const auto j :
                geode::Range{ grid.nb_cells_in_direction( 1 ) } )
            {
                for( const auto i :
                    geode::Range{ grid.nb_cells_in_direction( 0 ) } )
                {
                    const auto cell_vertices =
                        grid.cell_vertices( { i, j, k } );
                    const auto cell = grid.cell_index( { i, j, k } );
                    std::array< geode::index_t, 8 > cell_mesh_vertices;
                    for( const auto v : geode::LIndices{ cell_mesh_vertices } )
                    {
                        const auto cell_vertex_id =
                            grid.vertex_index( cell_vertices[v] );
                        cell_mesh_vertices[v] = cell_vertex_id;
                    }
                    const auto tetra0 = builder.create_tetrahedron(
                        { cell_mesh_vertices[0], cell_mesh_vertices[1],
                            cell_mesh_vertices[2], cell_mesh_vertices[5] } );
                    const auto tetra1 = builder.create_tetrahedron(
                        { cell_mesh_vertices[1], cell_mesh_vertices[3],
                            cell_mesh_vertices[2], cell_mesh_vertices[5] } );
                    const auto tetra2 = builder.create_tetrahedron(
                        { cell_mesh_vertices[3], cell_mesh_vertices[7],
                            cell_mesh_vertices[2], cell_mesh_vertices[5] } );
                    const auto tetra3 = builder.create_tetrahedron(
                        { cell_mesh_vertices[5], cell_mesh_vertices[7],
                            cell_mesh_vertices[2], cell_mesh_vertices[6] } );
                    const auto tetra4 = builder.create_tetrahedron(
                        { cell_mesh_vertices[5], cell_mesh_vertices[6],
                            cell_mesh_vertices[2], cell_mesh_vertices[4] } );
                    const auto tetra5 = builder.create_tetrahedron(
                        { cell_mesh_vertices[4], cell_mesh_vertices[0],
                            cell_mesh_vertices[2], cell_mesh_vertices[5] } );
                    builder.set_polyhedron_adjacent( { tetra1, 0 }, tetra2 );
                    builder.set_polyhedron_adjacent( { tetra1, 1 }, tetra0 );
                    builder.set_polyhedron_adjacent( { tetra0, 0 }, tetra1 );
                    builder.set_polyhedron_adjacent( { tetra0, 1 }, tetra5 );
                    builder.set_polyhedron_adjacent( { tetra2, 0 }, tetra3 );
                    builder.set_polyhedron_adjacent( { tetra2, 1 }, tetra1 );
                    builder.set_polyhedron_adjacent( { tetra3, 1 }, tetra4 );
                    builder.set_polyhedron_adjacent( { tetra3, 3 }, tetra2 );
                    builder.set_polyhedron_adjacent( { tetra4, 1 }, tetra5 );
                    builder.set_polyhedron_adjacent( { tetra4, 3 }, tetra3 );
                    builder.set_polyhedron_adjacent( { tetra5, 0 }, tetra0 );
                    builder.set_polyhedron_adjacent( { tetra5, 1 }, tetra4 );
                    old2new_mapping.map( cell, tetra0 );
                    old2new_mapping.map( cell, tetra1 );
                    old2new_mapping.map( cell, tetra2 );
                    old2new_mapping.map( cell, tetra3 );
                    old2new_mapping.map( cell, tetra4 );
                    old2new_mapping.map( cell, tetra5 );
                }
            }
        }
        builder.compute_polyhedron_adjacencies();
        tet_solid.polyhedron_attribute_manager().import(
            grid.cell_attribute_manager(), old2new_mapping );
    }

    std::unique_ptr< geode::TetrahedralSolid3D >
        create_tetrahedral_solid_from_grid( const geode::Grid3D& grid )
    {
        auto tet_solid = geode::TetrahedralSolid3D::create();
        auto builder = geode::TetrahedralSolidBuilder3D::create( *tet_solid );
        builder->create_vertices( grid.nb_grid_vertices() );
        for( const auto v : geode::Range{ grid.nb_grid_vertices() } )
        {
            builder->set_point(
                v, grid.grid_point( grid.vertex_indices( v ) ) );
        }
        geode::detail::copy_attributes( grid.grid_vertex_attribute_manager(),
            tet_solid->vertex_attribute_manager() );
        create_tetrahedra_from_grid_cells( *tet_solid, *builder, grid );
        return tet_solid;
    }

    bool all_polyhedra_are_hybrid( const geode::SolidMesh3D& solid )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            if( geode::detail::solid_polyhedron_is_a_tetrahedron( solid, p ) )
            {
                continue;
            }
            if( geode::detail::solid_polyhedron_is_a_prism( solid, p ) )
            {
                continue;
            }
            if( geode::detail::solid_polyhedron_is_a_pyramid( solid, p ) )
            {
                continue;
            }
            if( geode::detail::solid_polyhedron_is_a_hexaedron( solid, p ) )
            {
                continue;
            }
            return false;
        }
        return true;
    }
} // namespace

namespace geode
{
    absl::optional< std::unique_ptr< TetrahedralSolid3D > >
        convert_solid_mesh_into_tetrahedral_solid( const SolidMesh3D& solid )
    {
        if( solid.type_name() == TetrahedralSolid3D::type_name_static() )
        {
            return dynamic_cast< const TetrahedralSolid3D& >( solid ).clone();
        }
        if( solid.type_name() == RegularGrid3D::type_name_static() )
        {
            absl::optional< std::unique_ptr< TetrahedralSolid3D > > result{
                create_tetrahedral_solid_from_grid(
                    dynamic_cast< const RegularGrid3D& >( solid ) )
            };
            auto builder =
                geode::TetrahedralSolidBuilder3D::create( *result->get() );
            detail::copy_meta_info( solid, *builder );
            return result;
        }
        if( !all_polyhedra_are_simplex( solid ) )
        {
            Logger::info( "[convert_solid_mesh_into_tetrahedral_solid] "
                          "SolidMesh is not made of only tetrahedra." );
            return absl::nullopt;
        }
        absl::optional< std::unique_ptr< TetrahedralSolid3D > > tet_solid{
            TetrahedralSolid3D::create()
        };
        auto builder = TetrahedralSolidBuilder3D::create( *tet_solid->get() );
        detail::copy_meta_info( solid, *builder );
        detail::copy_points( solid, *builder );
        builder->reserve_tetrahedra( solid.nb_polyhedra() );
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            const std::array< index_t, 4 > vertices{ solid.polyhedron_vertex(
                                                         { p, 0 } ),
                solid.polyhedron_vertex( { p, 1 } ),
                solid.polyhedron_vertex( { p, 2 } ),
                solid.polyhedron_vertex( { p, 3 } ) };
            builder->create_tetrahedron( vertices );
        }
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            for( const auto f : LRange{ 4 } )
            {
                if( const auto adjacent =
                        solid.polyhedron_adjacent( { p, f } ) )
                {
                    builder->set_polyhedron_adjacent(
                        { p, f }, adjacent.value() );
                }
            }
        }
        detail::copy_attributes( solid.vertex_attribute_manager(),
            tet_solid->get()->vertex_attribute_manager() );
        detail::copy_attributes( solid.polyhedron_attribute_manager(),
            tet_solid->get()->polyhedron_attribute_manager() );
        return tet_solid;
    }

    std::unique_ptr< TetrahedralSolid3D > convert_grid_into_tetrahedral_solid(
        const Grid3D& grid )
    {
        return create_tetrahedral_solid_from_grid( grid );
    }

    absl::optional< std::unique_ptr< HybridSolid3D > >
        convert_solid_mesh_into_hybrid_solid( const SolidMesh3D& solid )
    {
        if( !all_polyhedra_are_hybrid( solid ) )
        {
            return absl::nullopt;
        }
        absl::optional< std::unique_ptr< HybridSolid3D > > hybrid_solid{
            HybridSolid3D::create()
        };
        auto builder = HybridSolidBuilder3D::create( *hybrid_solid->get() );
        detail::copy_meta_info( solid, *builder );
        detail::copy_points( solid, *builder );
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            const auto vertices = solid.polyhedron_vertices( p );
            if( vertices.size() == 4 )
            {
                builder->create_tetrahedron(
                    { vertices[0], vertices[1], vertices[2], vertices[3] } );
            }
            else if( vertices.size() == 8 )
            {
                builder->create_hexahedron(
                    { vertices[0], vertices[1], vertices[2], vertices[3],
                        vertices[4], vertices[5], vertices[6], vertices[7] } );
            }
            else if( vertices.size() == 5 )
            {
                builder->create_pyramid( { vertices[0], vertices[1],
                    vertices[2], vertices[3], vertices[4] } );
            }
            else if( vertices.size() == 6 )
            {
                builder->create_prism( { vertices[0], vertices[1], vertices[2],
                    vertices[3], vertices[4], vertices[5] } );
            }
        }
        for( const auto p : Range{ solid.nb_polyhedra() } )
        {
            for( const auto f : LRange{ solid.nb_polyhedron_facets( p ) } )
            {
                if( const auto adjacent =
                        solid.polyhedron_adjacent( { p, f } ) )
                {
                    builder->set_polyhedron_adjacent(
                        { p, f }, adjacent.value() );
                }
            }
        }
        detail::copy_attributes( solid.vertex_attribute_manager(),
            hybrid_solid->get()->vertex_attribute_manager() );
        detail::copy_attributes( solid.polyhedron_attribute_manager(),
            hybrid_solid->get()->polyhedron_attribute_manager() );
        return hybrid_solid;
    }

    std::unique_ptr< SolidMesh3D > merge_solid_meshes(
        absl::Span< const std::reference_wrapper< const SolidMesh3D > > solids )
    {
        detail::SolidMeshMerger3D merger{ solids, global_epsilon };
        return merger.merge();
    }
} // namespace geode
