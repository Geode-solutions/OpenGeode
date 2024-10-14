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

#include <geode/mesh/helpers/convert_solid_mesh.hpp>

#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/hybrid_solid_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/helpers/detail/element_identifier.hpp>
#include <geode/mesh/helpers/detail/solid_merger.hpp>
#include <geode/mesh/helpers/internal/copy.hpp>

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

    std::array< geode::index_t, 12 > create_tetrahedra_from_pyramid_pattern(
        geode::TetrahedralSolidBuilder3D& builder,
        const geode::Grid3D& grid,
        const geode::Grid3D::CellIndices& cell_indices,
        geode::index_t additional_vertex_id )
    {
        std::array< geode::index_t, 12 > tetras;
        const auto cell_vertices = grid.cell_vertices( cell_indices );
        std::array< geode::index_t, 8 > cell_mesh_vertices;
        for( const auto v : geode::LIndices{ cell_mesh_vertices } )
        {
            const auto cell_vertex_id = grid.vertex_index( cell_vertices[v] );
            cell_mesh_vertices[v] = cell_vertex_id;
        }
        tetras[0] = builder.create_tetrahedron(
            { cell_mesh_vertices[0], cell_mesh_vertices[1],
                additional_vertex_id, cell_mesh_vertices[5] } );
        tetras[1] = builder.create_tetrahedron(
            { cell_mesh_vertices[4], cell_mesh_vertices[0],
                additional_vertex_id, cell_mesh_vertices[5] } );
        tetras[2] = builder.create_tetrahedron(
            { cell_mesh_vertices[0], cell_mesh_vertices[2],
                cell_mesh_vertices[4], additional_vertex_id } );
        tetras[3] = builder.create_tetrahedron(
            { cell_mesh_vertices[2], cell_mesh_vertices[4],
                additional_vertex_id, cell_mesh_vertices[6] } );
        tetras[4] = builder.create_tetrahedron(
            { cell_mesh_vertices[4], cell_mesh_vertices[5],
                additional_vertex_id, cell_mesh_vertices[6] } );
        tetras[5] = builder.create_tetrahedron(
            { cell_mesh_vertices[5], cell_mesh_vertices[7],
                additional_vertex_id, cell_mesh_vertices[6] } );
        tetras[6] = builder.create_tetrahedron(
            { cell_mesh_vertices[0], cell_mesh_vertices[1],
                cell_mesh_vertices[2], additional_vertex_id } );
        tetras[7] = builder.create_tetrahedron(
            { cell_mesh_vertices[1], cell_mesh_vertices[3],
                cell_mesh_vertices[2], additional_vertex_id } );
        tetras[8] = builder.create_tetrahedron(
            { cell_mesh_vertices[1], cell_mesh_vertices[3],
                additional_vertex_id, cell_mesh_vertices[5] } );
        tetras[9] = builder.create_tetrahedron(
            { cell_mesh_vertices[3], cell_mesh_vertices[7],
                additional_vertex_id, cell_mesh_vertices[5] } );
        tetras[10] = builder.create_tetrahedron(
            { cell_mesh_vertices[2], additional_vertex_id,
                cell_mesh_vertices[3], cell_mesh_vertices[7] } );
        tetras[11] = builder.create_tetrahedron(
            { cell_mesh_vertices[2], cell_mesh_vertices[6],
                additional_vertex_id, cell_mesh_vertices[7] } );
        return tetras;
    }

    std::array< geode::index_t, 6 > create_tetrahedra_from_pIpI_pattern(
        geode::TetrahedralSolidBuilder3D& builder,
        const geode::Grid3D& grid,
        const geode::Grid3D::CellIndices& cell_indices )
    {
        std::array< geode::index_t, 6 > tetras;
        const auto cell_vertices = grid.cell_vertices( cell_indices );
        std::array< geode::index_t, 8 > cell_mesh_vertices;
        for( const auto v : geode::LIndices{ cell_mesh_vertices } )
        {
            const auto cell_vertex_id = grid.vertex_index( cell_vertices[v] );
            cell_mesh_vertices[v] = cell_vertex_id;
        }
        tetras[0] = builder.create_tetrahedron(
            { cell_mesh_vertices[0], cell_mesh_vertices[1],
                cell_mesh_vertices[2], cell_mesh_vertices[5] } );
        tetras[1] = builder.create_tetrahedron(
            { cell_mesh_vertices[1], cell_mesh_vertices[3],
                cell_mesh_vertices[2], cell_mesh_vertices[5] } );
        tetras[2] = builder.create_tetrahedron(
            { cell_mesh_vertices[3], cell_mesh_vertices[7],
                cell_mesh_vertices[2], cell_mesh_vertices[5] } );
        tetras[3] = builder.create_tetrahedron(
            { cell_mesh_vertices[5], cell_mesh_vertices[7],
                cell_mesh_vertices[2], cell_mesh_vertices[6] } );
        tetras[4] = builder.create_tetrahedron(
            { cell_mesh_vertices[5], cell_mesh_vertices[6],
                cell_mesh_vertices[2], cell_mesh_vertices[4] } );
        tetras[5] = builder.create_tetrahedron(
            { cell_mesh_vertices[4], cell_mesh_vertices[0],
                cell_mesh_vertices[2], cell_mesh_vertices[5] } );
        return tetras;
    }

    void create_tetrahedra_from_grid_cells(
        const geode::TetrahedralSolid3D& tet_solid,
        geode::TetrahedralSolidBuilder3D& builder,
        const geode::Grid3D& grid,
        absl::Span< const geode::index_t > cells_to_densify )
    {
        builder.reserve_tetrahedra(
            6 * grid.nb_cells() + 6 * cells_to_densify.size() );
        std::vector< bool > to_densify( grid.nb_cells(), false );
        for( const auto& cell_id : cells_to_densify )
        {
            to_densify[cell_id] = true;
        }
        geode::GenericMapping< geode::index_t > old2new_mapping;
        for( const auto k : geode::Range{ grid.nb_cells_in_direction( 2 ) } )
        {
            for( const auto j :
                geode::Range{ grid.nb_cells_in_direction( 1 ) } )
            {
                for( const auto i :
                    geode::Range{ grid.nb_cells_in_direction( 0 ) } )
                {
                    const auto cell_id = grid.cell_index( { i, j, k } );
                    if( to_densify[cell_id] )
                    {
                        continue;
                    }
                    for( const auto tetra_id :
                        create_tetrahedra_from_pIpI_pattern(
                            builder, grid, { i, j, k } ) )
                    {
                        old2new_mapping.map( cell_id, tetra_id );
                    }
                }
            }
        }
        for( const auto cell_index : geode::Indices{ cells_to_densify } )
        {
            const auto cell_indices =
                grid.cell_indices( cells_to_densify[cell_index] );
            const auto additional_vertex_id =
                grid.nb_grid_vertices() + cell_index;
            for( const auto tetra_id : create_tetrahedra_from_pyramid_pattern(
                     builder, grid, cell_indices, additional_vertex_id ) )
            {
                old2new_mapping.map( cells_to_densify[cell_index], tetra_id );
            }
        }
        builder.compute_polyhedron_adjacencies();
        tet_solid.polyhedron_attribute_manager().import(
            grid.cell_attribute_manager(), old2new_mapping );
    }

    void create_vertices_from_grid( const geode::TetrahedralSolid3D& tet_solid,
        geode::TetrahedralSolidBuilder3D& builder,
        const geode::Grid3D& grid,
        absl::Span< const geode::index_t > cells_to_densify )
    {
        builder.create_vertices(
            grid.nb_grid_vertices() + cells_to_densify.size() );
        for( const auto vertex_id : geode::Range{ grid.nb_grid_vertices() } )
        {
            builder.set_point( vertex_id,
                grid.grid_point( grid.vertex_indices( vertex_id ) ) );
        }
        geode::index_t counter{ grid.nb_grid_vertices() };
        for( const auto cell_id : cells_to_densify )
        {
            builder.set_point(
                counter, grid.cell_barycenter( grid.cell_indices( cell_id ) ) );
            counter++;
        }
        geode::internal::copy_attributes( grid.grid_vertex_attribute_manager(),
            tet_solid.vertex_attribute_manager() );
    }

    std::unique_ptr< geode::TetrahedralSolid3D >
        create_tetrahedral_solid_from_grid( const geode::Grid3D& grid,
            absl::Span< const geode::index_t > cells_to_densify )
    {
        auto tet_solid = geode::TetrahedralSolid3D::create();
        auto builder = geode::TetrahedralSolidBuilder3D::create( *tet_solid );
        create_vertices_from_grid(
            *tet_solid, *builder, grid, cells_to_densify );
        create_tetrahedra_from_grid_cells(
            *tet_solid, *builder, grid, cells_to_densify );
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

    std::array< geode::index_t, 8 > order_hexahedron_vertices(
        const geode::index_t hexahedron_id, const geode::SolidMesh3D& solid )
    {
        std::array< geode::index_t, 8 > ordered_vertices;
        const auto first_polyhedron_facet_vertices =
            solid.polyhedron_facet_vertices( { hexahedron_id, 0 } );
        ordered_vertices[0] = first_polyhedron_facet_vertices[0];
        ordered_vertices[1] = first_polyhedron_facet_vertices[1];
        ordered_vertices[2] = first_polyhedron_facet_vertices[2];
        ordered_vertices[3] = first_polyhedron_facet_vertices[3];
        for( const auto f : geode::LRange( 1, 6 ) )
        {
            const auto polyhedron_facet_vertices =
                solid.polyhedron_facet_vertices( { hexahedron_id, f } );
            const auto v_id_0 =
                absl::c_find( polyhedron_facet_vertices, ordered_vertices[0] );
            if( v_id_0 != polyhedron_facet_vertices.end()
                && absl::c_find(
                       polyhedron_facet_vertices, ordered_vertices[1] )
                       != polyhedron_facet_vertices.end() )
            {
                const auto index_v_id_0 =
                    std::distance( polyhedron_facet_vertices.begin(), v_id_0 );
                ordered_vertices[4] =
                    polyhedron_facet_vertices[( index_v_id_0 + 1 ) % 4];
                ordered_vertices[5] =
                    polyhedron_facet_vertices[( index_v_id_0 + 2 ) % 4];
                continue;
            }
            const auto v_id_2 =
                absl::c_find( polyhedron_facet_vertices, ordered_vertices[2] );
            if( v_id_2 != polyhedron_facet_vertices.end()
                && absl::c_find(
                       polyhedron_facet_vertices, ordered_vertices[3] )
                       != polyhedron_facet_vertices.end() )
            {
                const auto index_v_id_2 =
                    std::distance( polyhedron_facet_vertices.begin(), v_id_2 );
                ordered_vertices[6] =
                    polyhedron_facet_vertices[( index_v_id_2 + 1 ) % 4];
                ordered_vertices[7] =
                    polyhedron_facet_vertices[( index_v_id_2 + 2 ) % 4];
                continue;
            }
        }
        return ordered_vertices;
    }

    std::array< geode::index_t, 6 > order_prism_vertices(
        const geode::index_t prism_id, const geode::SolidMesh3D& solid )
    {
        std::array< geode::index_t, 6 > ordered_vertices;
        geode::index_t already_used_facet{ 0 };
        for( const auto f : geode::LRange{ 5 } )
        {
            if( solid.nb_polyhedron_facet_vertices( { prism_id, f } ) == 3 )
            {
                const auto vertices =
                    solid.polyhedron_facet_vertices( { prism_id, f } );
                ordered_vertices[0] = vertices[0];
                ordered_vertices[1] = vertices[2];
                ordered_vertices[2] = vertices[1];
                already_used_facet = f;
                break;
            }
        }
        for( const auto f : geode::LRange{ 5 } )
        {
            if( f == already_used_facet )
            {
                continue;
            }
            const auto facet_vertices =
                solid.polyhedron_facet_vertices( { prism_id, f } );
            const auto v_id =
                absl::c_find( facet_vertices, ordered_vertices[0] );
            if( v_id == facet_vertices.end()
                || absl::c_find( facet_vertices, ordered_vertices[2] )
                       == facet_vertices.end() )
            {
                continue;
            }
            const auto index_v_id =
                std::distance( facet_vertices.begin(), v_id );
            ordered_vertices[3] = facet_vertices[( index_v_id + 1 ) % 4];
            ordered_vertices[5] = facet_vertices[( index_v_id + 2 ) % 4];
            break;
        }
        for( const auto v : geode::LRange{ 6 } )
        {
            const auto solid_vertex =
                solid.polyhedron_vertex( { prism_id, v } );
            if( ordered_vertices[1] == solid_vertex
                || ordered_vertices[2] == solid_vertex
                || ordered_vertices[3] == solid_vertex
                || ordered_vertices[5] == solid_vertex
                || ordered_vertices[0] == solid_vertex )
            {
                continue;
            }
            ordered_vertices[4] = solid_vertex;
            break;
        }
        return ordered_vertices;
    }
} // namespace

std::array< geode::index_t, 5 > order_pyramid_vertices(
    const geode::index_t pyramid_id, const geode::SolidMesh3D& solid )
{
    std::array< geode::index_t, 5 > ordered_vertices;

    for( const auto f : geode::LRange{ 5 } )
    {
        if( solid.nb_polyhedron_facet_vertices( { pyramid_id, f } ) == 4 )
        {
            const auto vertices =
                solid.polyhedron_facet_vertices( { pyramid_id, f } );
            ordered_vertices[0] = vertices[0];
            ordered_vertices[1] = vertices[3];
            ordered_vertices[2] = vertices[2];
            ordered_vertices[3] = vertices[1];
            break;
        }
    }
    for( const auto v : geode::LRange{ 5 } )
    {
        const auto solid_vertex = solid.polyhedron_vertex( { pyramid_id, v } );
        if( ordered_vertices[1] == solid_vertex
            || ordered_vertices[2] == solid_vertex
            || ordered_vertices[3] == solid_vertex
            || ordered_vertices[0] == solid_vertex )
        {
            continue;
        }
        ordered_vertices[4] = solid_vertex;
        break;
    }
    return ordered_vertices;
}

namespace geode
{
    std::optional< std::unique_ptr< TetrahedralSolid3D > >
        convert_solid_mesh_into_tetrahedral_solid( const SolidMesh3D& solid )
    {
        if( solid.type_name() == TetrahedralSolid3D::type_name_static() )
        {
            return dynamic_cast< const TetrahedralSolid3D& >( solid ).clone();
        }
        if( solid.type_name() == RegularGrid3D::type_name_static() )
        {
            std::optional< std::unique_ptr< TetrahedralSolid3D > > result{
                create_tetrahedral_solid_from_grid(
                    dynamic_cast< const RegularGrid3D& >( solid ), {} )
            };
            auto builder =
                geode::TetrahedralSolidBuilder3D::create( *result->get() );
            internal::copy_meta_info( solid, *builder );
            return result;
        }
        if( !all_polyhedra_are_simplex( solid ) )
        {
            Logger::info( "[convert_solid_mesh_into_tetrahedral_solid] "
                          "SolidMesh is not made of only tetrahedra." );
            return std::nullopt;
        }
        std::optional< std::unique_ptr< TetrahedralSolid3D > > tet_solid{
            TetrahedralSolid3D::create()
        };
        auto builder = TetrahedralSolidBuilder3D::create( *tet_solid->get() );
        internal::copy_meta_info( solid, *builder );
        internal::copy_points( solid, *builder );
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
        internal::copy_attributes( solid.vertex_attribute_manager(),
            tet_solid->get()->vertex_attribute_manager() );
        internal::copy_attributes( solid.polyhedron_attribute_manager(),
            tet_solid->get()->polyhedron_attribute_manager() );
        return tet_solid;
    }

    std::unique_ptr< TetrahedralSolid3D > convert_grid_into_tetrahedral_solid(
        const Grid3D& grid )
    {
        return create_tetrahedral_solid_from_grid( grid, {} );
    }

    std::unique_ptr< TetrahedralSolid3D >
        convert_grid_into_densified_tetrahedral_solid( const Grid3D& grid,
            absl::Span< const geode::index_t > cells_to_densify )
    {
        return create_tetrahedral_solid_from_grid( grid, cells_to_densify );
    }

    std::optional< std::unique_ptr< HybridSolid3D > >
        convert_solid_mesh_into_hybrid_solid( const SolidMesh3D& solid )
    {
        if( !all_polyhedra_are_hybrid( solid ) )
        {
            return std::nullopt;
        }
        std::optional< std::unique_ptr< HybridSolid3D > > hybrid_solid{
            HybridSolid3D::create()
        };
        auto builder = HybridSolidBuilder3D::create( *hybrid_solid->get() );
        internal::copy_meta_info( solid, *builder );
        internal::copy_points( solid, *builder );
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
                const auto ordered_vertices =
                    order_hexahedron_vertices( p, solid );
                builder->create_hexahedron( { ordered_vertices } );
            }
            else if( vertices.size() == 5 )
            {
                const auto ordered_vertices =
                    order_pyramid_vertices( p, solid );
                builder->create_pyramid( ordered_vertices );
            }
            else if( vertices.size() == 6 )
            {
                const auto ordered_vertices = order_prism_vertices( p, solid );
                builder->create_prism( { ordered_vertices } );
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
        internal::copy_attributes( solid.vertex_attribute_manager(),
            hybrid_solid->get()->vertex_attribute_manager() );
        internal::copy_attributes( solid.polyhedron_attribute_manager(),
            hybrid_solid->get()->polyhedron_attribute_manager() );
        return hybrid_solid;
    }

    std::unique_ptr< SolidMesh3D > merge_solid_meshes(
        absl::Span< const std::reference_wrapper< const SolidMesh3D > > solids )
    {
        detail::SolidMeshMerger3D merger{ solids, GLOBAL_EPSILON };
        return merger.merge();
    }
} // namespace geode
