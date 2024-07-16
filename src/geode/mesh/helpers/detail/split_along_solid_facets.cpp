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

#include <geode/mesh/helpers/detail/split_along_solid_facets.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/solid_edges_builder.hpp>
#include <geode/mesh/builder/solid_facets_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_facets.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    namespace detail
    {
        class SplitAlongSolidFacets::Impl
        {
            struct SolidInfo
            {
                SolidInfo( index_t nb_vertices )
                    : polyhedron_vertices( nb_vertices ),
                      vertices_to_check( nb_vertices, false )
                {
                }
                absl::FixedArray< PolyhedraAroundVertex > polyhedron_vertices;
                std::vector< bool > vertices_to_check;
            };

        public:
            Impl( const SolidMesh3D& mesh, SolidMeshBuilder3D& builder )
                : solid_( mesh ), builder_( builder )
            {
            }

            MeshesElementsMapping split_solid_along_facets(
                absl::Span< const PolyhedronFacet > facets_list )
            {
                bool facets_enabled = solid_.are_facets_enabled();
                bool edges_enabled = solid_.are_edges_enabled();
                const auto nb_initial_facets =
                    facets_enabled ? solid_.facets().nb_facets() : 0;
                const auto nb_initial_edges =
                    edges_enabled ? solid_.edges().nb_edges() : 0;
                const auto solid_info =
                    remove_adjacencies_along_facets( facets_list );
                MeshesElementsMapping mapping;
                mapping.vertices = duplicate_points( solid_info );
                if( facets_enabled )
                {
                    mapping.polygons = process_solid_facets(
                        nb_initial_facets, mapping.vertices );
                }
                if( edges_enabled )
                {
                    mapping.edges = process_solid_edges(
                        nb_initial_edges, mapping.vertices );
                }
                return mapping;
            }

        private:
            SolidInfo remove_adjacencies_along_facets(
                absl::Span< const PolyhedronFacet > facets_list )
            {
                SolidInfo info{ solid_.nb_vertices() };
                for( const auto& solid_facet : facets_list )
                {
                    if( const auto adj =
                            solid_.polyhedron_adjacent_facet( solid_facet ) )
                    {
                        builder_.unset_polyhedron_adjacent( solid_facet );
                        builder_.unset_polyhedron_adjacent( adj.value() );
                    }
                    for( const auto vertex_id :
                        solid_.polyhedron_facet_vertices( solid_facet ) )
                    {
                        info.vertices_to_check[vertex_id] = true;
                    }
                }
                for( const auto polyhedron_id : Range{ solid_.nb_polyhedra() } )
                {
                    for( const auto local_vertex_id : LRange{
                             solid_.nb_polyhedron_vertices( polyhedron_id ) } )
                    {
                        const PolyhedronVertex vertex{ polyhedron_id,
                            local_vertex_id };
                        const auto vertex_id =
                            solid_.polyhedron_vertex( vertex );
                        if( info.vertices_to_check[vertex_id] )
                        {
                            info.polyhedron_vertices[vertex_id].emplace_back(
                                vertex );
                        }
                    }
                }
                return info;
            }

            ElementsMapping duplicate_points( const SolidInfo& solid_info )
            {
                ElementsMapping vertices_mapping;
                vertices_mapping.reserve( solid_.nb_vertices() );
                for( const auto vertex_id : Range{ solid_.nb_vertices() } )
                {
                    vertices_mapping.map( vertex_id, vertex_id );
                    if( !solid_info.vertices_to_check[vertex_id] )
                    {
                        continue;
                    }
                    /// Polyhedra around through adjacencies
                    auto polyhedra_around =
                        solid_.polyhedra_around_vertex( vertex_id );
                    /// All polyhedra around vertex
                    const auto& all_polyhedra_around =
                        solid_info.polyhedron_vertices[vertex_id];
                    auto nb_polyhedra_around = polyhedra_around.size();
                    OPENGEODE_ASSERT(
                        nb_polyhedra_around <= all_polyhedra_around.size(),
                        "[SplitAlongSolidFacets] Wrong size comparison" );
                    /// Already processed polyhedra
                    PolyhedraAroundVertex processed_polyhedra;
                    while( nb_polyhedra_around != all_polyhedra_around.size() )
                    {
                        for( const auto& polyhedron : polyhedra_around )
                        {
                            processed_polyhedra.emplace_back( polyhedron );
                        }
                        vertices_mapping.map( vertex_id,
                            process_vertex( vertex_id, processed_polyhedra,
                                all_polyhedra_around ) );
                        polyhedra_around =
                            solid_.polyhedra_around_vertex( vertex_id );
                        /// Add new number of polyhedra around to nb processed
                        nb_polyhedra_around += polyhedra_around.size();
                    }
                }
                return vertices_mapping;
            }

            index_t process_vertex( index_t vertex_id,
                const PolyhedraAroundVertex& processed_polyhedra,
                const PolyhedraAroundVertex& all_polyhedra_around )
            {
                const auto new_vertex_id =
                    builder_.create_point( solid_.point( vertex_id ) );
                solid_.vertex_attribute_manager().copy_attribute_value(
                    vertex_id, new_vertex_id );
                /// Replaces vertex for polyhedra around through current
                /// adjacencies -> for these polyhedra, removes facets on which
                /// adjacency was removed
                builder_.replace_vertex( vertex_id, new_vertex_id );
                for( const auto& polyhedron_vertex : all_polyhedra_around )
                {
                    if( absl::c_find( processed_polyhedra, polyhedron_vertex )
                        == processed_polyhedra.end() )
                    {
                        builder_.associate_polyhedron_vertex_to_vertex(
                            polyhedron_vertex, vertex_id );
                        break;
                    }
                }
                OPENGEODE_ASSERT(
                    !solid_.polyhedra_around_vertex( vertex_id ).empty(),
                    "[BRepFromMeshBuilder::split_block_by_surfaces] Lost "
                    "polyhedron around vertex" );
                return new_vertex_id;
            }

            ElementsMapping process_solid_facets( index_t nb_initial_facets,
                const ElementsMapping& vertices_mapping ) const
            {
                ElementsMapping facets_mapping;
                auto facets_builder = builder_.facets_builder();
                const auto& solid_facets = solid_.facets();
                for( const auto facet_id : Range{ solid_facets.nb_facets() } )
                {
                    /// Recreate facets which were set to be deleted during
                    /// duplication process by the replace_vertex method
                    auto facet_vertices =
                        solid_facets.facet_vertices( facet_id );
                    if( solid_facets.is_facet_isolated( facet_id ) )
                    {
                        if( !solid_.polyhedron_facet_from_vertices(
                                facet_vertices ) )
                        {
                            continue;
                        }
                        facets_builder.find_or_create_facet(
                            solid_facets.facet_vertices( facet_id ) );
                    }
                    if( facet_id < nb_initial_facets )
                    {
                        facets_mapping.map( facet_id, facet_id );
                        continue;
                    }
                    for( auto& facet_vertex : facet_vertices )
                    {
                        facet_vertex =
                            vertices_mapping.out2in( facet_vertex )[0];
                    }
                    facets_mapping.map(
                        solid_facets.facet_from_vertices( facet_vertices )
                            .value(),
                        facet_id );
                }
                const auto old2new = facets_builder.delete_isolated_facets();
                return final_facets_mapping( facets_mapping, old2new );
            }

            ElementsMapping final_facets_mapping(
                const ElementsMapping& solid2split_mapping,
                absl::Span< const index_t > clean_mapping ) const
            {
                ElementsMapping facets_mapping;
                for( const auto& solid2split :
                    solid2split_mapping.in2out_map() )
                {
                    for( const auto facet_after_split : solid2split.second )
                    {
                        facets_mapping.map( solid2split.first,
                            clean_mapping[facet_after_split] );
                    }
                }
                return facets_mapping;
            }

            ElementsMapping process_solid_edges( index_t nb_initial_edges,
                const ElementsMapping& vertices_mapping )
            {
                ElementsMapping edges_mapping;
                auto edges_builder = builder_.edges_builder();
                const auto& solid_edges = solid_.edges();
                for( const auto edge_id : Range{ solid_edges.nb_edges() } )
                {
                    /// Recreate edges which were set to be deleted during
                    /// duplication process by the replace_vertex method
                    auto edge_vertices = solid_edges.edge_vertices( edge_id );
                    if( solid_edges.is_edge_isolated( edge_id ) )
                    {
                        if( !solid_.polyhedron_facet_edge_from_vertices(
                                edge_vertices ) )
                        {
                            continue;
                        }
                        edges_builder.find_or_create_edge(
                            solid_edges.edge_vertices( edge_id ) );
                    }
                    if( edge_id < nb_initial_edges )
                    {
                        edges_mapping.map( edge_id, edge_id );
                        continue;
                    }
                    for( auto& edge_vertex : edge_vertices )
                    {
                        edge_vertex = vertices_mapping.out2in( edge_vertex )[0];
                    }
                    edges_mapping.map(
                        solid_edges.edge_from_vertices( edge_vertices ).value(),
                        edge_id );
                }
                const auto old2new = edges_builder.delete_isolated_edges();
                return final_edges_mapping( edges_mapping, old2new );
            }

            ElementsMapping final_edges_mapping(
                const ElementsMapping& solid2split_mapping,
                absl::Span< const index_t > clean_mapping ) const
            {
                ElementsMapping edges_mapping;
                for( const auto& solid2split :
                    solid2split_mapping.in2out_map() )
                {
                    for( const auto edge_after_split : solid2split.second )
                    {
                        edges_mapping.map( solid2split.first,
                            clean_mapping[edge_after_split] );
                    }
                }
                return edges_mapping;
            }

        private:
            const SolidMesh3D& solid_;
            SolidMeshBuilder3D& builder_;
        };

        SplitAlongSolidFacets::SplitAlongSolidFacets(
            const SolidMesh3D& mesh, SolidMeshBuilder3D& builder )
            : impl_{ mesh, builder }
        {
        }

        SplitAlongSolidFacets::~SplitAlongSolidFacets() {}

        MeshesElementsMapping SplitAlongSolidFacets::split_solid_along_facets(
            absl::Span< const PolyhedronFacet > facets_list )
        {
            return impl_->split_solid_along_facets( facets_list );
        }
    } // namespace detail
} // namespace geode