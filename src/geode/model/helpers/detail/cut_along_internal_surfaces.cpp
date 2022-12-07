/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/model/helpers/detail/cut_along_internal_surfaces.h>

#include <async++.h>

#include <absl/memory/memory.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/component_mesh_polygons.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    namespace detail
    {
        class CutAlongInternalSurfaces::Impl
        {
            using CMVmapping =
                std::pair< ComponentMeshVertex, ComponentMeshVertex >;
            using CMVmappings = std::vector< CMVmapping >;
            using Task = async::task< CMVmappings >;
            struct BlockInfo
            {
                BlockInfo( index_t nb_vertices )
                    : polyhedron_vertices( nb_vertices )
                {
                }
                absl::FixedArray< PolyhedraAroundVertex > polyhedron_vertices;
                std::vector< index_t > vertices_to_check;
            };

        public:
            Impl( BRep& model )
                : model_( model ),
                  builder_ptr_{ absl::make_unique< BRepBuilder >( model ) },
                  builder_( *builder_ptr_ )
            {
            }

            Impl( const BRep& model, BRepBuilder& builder )
                : model_( model ), builder_( builder )
            {
            }

            CMVmappings cut()
            {
                absl::FixedArray< Task > tasks( model_.nb_blocks() );
                index_t count{ 0 };
                for( const auto& block : model_.blocks() )
                {
                    tasks[count++] = async::spawn( [this, &block] {
                        return split_points( block );
                    } );
                }
                CMVmappings mapping;
                async::when_all( tasks.begin(), tasks.end() )
                    .then( [this, &mapping]( std::vector< Task > all_task ) {
                        for( auto& task : all_task )
                        {
                            auto cmv_mappings = task.get();
                            update_unique_vertices( cmv_mappings );
                            mapping.insert( mapping.end(),
                                std::make_move_iterator( cmv_mappings.begin() ),
                                std::make_move_iterator( cmv_mappings.end() ) );
                        }
                    } )
                    .get();
                return mapping;
            }

            CMVmappings cut_block( const Block3D& block )
            {
                const auto mapping = split_points( block );
                update_unique_vertices( mapping );
                return mapping;
            }

        private:
            void update_unique_vertices( const CMVmappings& mapping )
            {
                for( const auto& cmv_mapping : mapping )
                {
                    const auto unique_vertex_id =
                        model_.unique_vertex( cmv_mapping.first );
                    builder_.set_unique_vertex(
                        cmv_mapping.second, unique_vertex_id );
                }
            }

            CMVmappings split_points( const Block3D& block )
            {
                auto builder = builder_.block_mesh_builder( block.id() );
                remove_adjacencies_along_internal_surfaces( block, *builder );
                return duplicate_points( block, *builder );
            }

            CMVmappings duplicate_points(
                const Block3D& block, SolidMeshBuilder3D& builder )
            {
                const auto& mesh = block.mesh();
                const auto info = compute_block_info( mesh );
                CMVmappings mapping;
                mapping.reserve( info.vertices_to_check.size() );
                for( const auto vertex_id : info.vertices_to_check )
                {
                    auto polyhedra_around =
                        mesh.polyhedra_around_vertex( vertex_id );
                    const auto& polyhedron_vertices =
                        info.polyhedron_vertices[vertex_id];
                    auto nb_polyhedra_around = polyhedra_around.size();
                    OPENGEODE_ASSERT(
                        nb_polyhedra_around <= polyhedron_vertices.size(),
                        "[CutAlongInternalSurfaces] Wrong size comparison" );
                    PolyhedraAroundVertex total_polyhedra;
                    while( nb_polyhedra_around != polyhedron_vertices.size() )
                    {
                        for( auto& polyhedron : polyhedra_around )
                        {
                            total_polyhedra.emplace_back(
                                std::move( polyhedron ) );
                        }
                        mapping.emplace_back(
                            process_component( block, mesh, builder, vertex_id,
                                total_polyhedra, polyhedron_vertices ) );
                        polyhedra_around =
                            mesh.polyhedra_around_vertex( vertex_id );
                        nb_polyhedra_around += polyhedra_around.size();
                    }
                }
                return mapping;
            }

            BlockInfo compute_block_info( const SolidMesh3D& mesh ) const
            {
                BlockInfo info{ mesh.nb_vertices() };
                std::vector< bool > vertex_to_check(
                    mesh.nb_vertices(), false );
                for( const auto p : Range{ mesh.nb_polyhedra() } )
                {
                    for( const auto f :
                        LRange{ mesh.nb_polyhedron_facets( p ) } )
                    {
                        PolyhedronFacet facet{ p, f };
                        if( !mesh.is_polyhedron_facet_on_border( facet ) )
                        {
                            for( const auto vertex_id :
                                mesh.polyhedron_facet_vertices( facet ) )
                            {
                                vertex_to_check[vertex_id] = true;
                            }
                        }
                    }
                    for( const auto v :
                        LRange{ mesh.nb_polyhedron_vertices( p ) } )
                    {
                        PolyhedronVertex vertex{ p, v };
                        const auto vertex_id = mesh.polyhedron_vertex( vertex );
                        info.polyhedron_vertices[vertex_id].emplace_back(
                            std::move( vertex ) );
                    }
                }
                for( const auto v : Indices{ vertex_to_check } )
                {
                    if( vertex_to_check[v] )
                    {
                        info.vertices_to_check.push_back( v );
                    }
                }
                return info;
            }

            void remove_adjacencies_along_internal_surfaces(
                const Block3D& block, SolidMeshBuilder3D& builder )
            {
                std::vector< PolyhedronFacet > facets;
                for( const auto& surface : model_.internal_surfaces( block ) )
                {
                    const auto& mesh = surface.mesh();
                    for( const auto p : Range{ mesh.nb_polygons() } )
                    {
                        for( auto& facet :
                            block_mesh_polyhedra_from_surface_polygon(
                                model_, block, surface, p ) )
                        {
                            facets.emplace_back( std::move( facet ) );
                        }
                    }
                }
                for( const auto& facet : facets )
                {
                    builder.unset_polyhedron_adjacent( facet );
                }
            }

            CMVmapping process_component( const Block3D& block,
                const SolidMesh3D& mesh,
                SolidMeshBuilder3D& builder,
                index_t vertex_id,
                const PolyhedraAroundVertex& total_polyhedra,
                const PolyhedraAroundVertex& polyhedron_vertices )
            {
                const auto new_vertex_id =
                    builder.create_point( mesh.point( vertex_id ) );
                builder.replace_vertex( vertex_id, new_vertex_id );
                for( const auto& polyhedron_vertex : polyhedron_vertices )
                {
                    if( absl::c_find( total_polyhedra, polyhedron_vertex )
                        == total_polyhedra.end() )
                    {
                        builder.associate_polyhedron_vertex_to_vertex(
                            polyhedron_vertex, vertex_id );
                        break;
                    }
                }
                OPENGEODE_ASSERT(
                    !mesh.polyhedra_around_vertex( vertex_id ).empty(),
                    "[BRepFromMeshBuilder::cut_block_by_surfaces] Lost "
                    "polyhedron around vertex" );
                return { ComponentMeshVertex{ block.component_id(), vertex_id },
                    ComponentMeshVertex{
                        block.component_id(), new_vertex_id } };
            }

        private:
            const BRep& model_;
            std::unique_ptr< BRepBuilder > builder_ptr_;
            BRepBuilder& builder_;
        };

        CutAlongInternalSurfaces::CutAlongInternalSurfaces( BRep& model )
            : impl_{ model }
        {
        }

        CutAlongInternalSurfaces::CutAlongInternalSurfaces(
            const BRep& model, BRepBuilder& builder )
            : impl_{ model, builder }
        {
        }

        CutAlongInternalSurfaces::~CutAlongInternalSurfaces() {}

        std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
            CutAlongInternalSurfaces::cut_all_blocks()
        {
            return impl_->cut();
        }

        std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
            CutAlongInternalSurfaces::cut_block( const Block3D& block )
        {
            return impl_->cut_block( block );
        }
    } // namespace detail
} // namespace geode