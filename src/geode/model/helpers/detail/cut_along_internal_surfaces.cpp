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

#include <geode/model/helpers/detail/cut_along_internal_surfaces.h>

#include <async++.h>

#include <absl/memory/memory.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/helpers/detail/cut_along_solid_facets.h>

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

        public:
            Impl( const BRep& model, BRepBuilder& builder )
                : model_( model ), builder_( builder )
            {
            }

            CMVmappings cut()
            {
                using Task =
                    async::task< std::pair< uuid, MeshesElementsMapping > >;
                absl::FixedArray< Task > tasks( model_.nb_blocks() );
                index_t count{ 0 };
                for( const auto& block : model_.blocks() )
                {
                    tasks[count++] = async::spawn( [this, &block] {
                        const auto& mesh = block.mesh();
                        auto builder =
                            builder_.block_mesh_builder( block.id() );
                        const auto facets_list =
                            internal_surface_facets( block );
                        CutAlongSolidFacets block_cutter{ mesh, *builder };
                        return std::make_pair(
                            block.id(), block_cutter.cut_solid_along_facets(
                                            facets_list ) );
                    } );
                }
                CMVmappings mapping;
                async::when_all( tasks )
                    .then( [this, &mapping]( std::vector< Task > all_task ) {
                        for( auto& task : all_task )
                        {
                            const auto result = task.get();
                            auto cmv_mapping = update_unique_vertices(
                                model_.block( result.first ),
                                result.second.vertices );
                            if( !cmv_mapping.empty() )
                            {
                                mapping.insert( mapping.end(),
                                    std::make_move_iterator(
                                        cmv_mapping.begin() ),
                                    std::make_move_iterator(
                                        cmv_mapping.end() ) );
                            }
                        }
                    } )
                    .get();
                return mapping;
            }

            CMVmappings cut_block( const Block3D& block )
            {
                const auto& mesh = block.mesh();
                auto builder = builder_.block_mesh_builder( block.id() );
                const auto facets_list = internal_surface_facets( block );
                CutAlongSolidFacets block_cutter{ mesh, *builder };
                auto mapping =
                    block_cutter.cut_solid_along_facets( facets_list );
                return update_unique_vertices( block, mapping.vertices );
            }

        private:
            CMVmappings update_unique_vertices(
                const Block3D& block, const ElementsMapping& mapping )
            {
                CMVmappings cmv_mapping;
                for( const auto& vertex_mapping : mapping.in2out_map() )
                {
                    ComponentMeshVertex original_cmv{ block.component_id(),
                        vertex_mapping.first };
                    const auto unique_vertex_id =
                        model_.unique_vertex( original_cmv );
                    for( const auto vertex_out : vertex_mapping.second )
                    {
                        if( vertex_out == vertex_mapping.first )
                        {
                            continue;
                        }
                        ComponentMeshVertex cmv_out{ block.component_id(),
                            vertex_out };
                        cmv_mapping.emplace_back( original_cmv, cmv_out );
                        builder_.set_unique_vertex( cmv_out, unique_vertex_id );
                    }
                }
                return cmv_mapping;
            }

            std::vector< PolyhedronFacet > internal_surface_facets(
                const Block3D& block ) const
            {
                std::vector< PolyhedronFacet > result;
                for( const auto& surface : model_.internal_surfaces( block ) )
                {
                    const auto& mesh = surface.mesh();
                    for( const auto p : Range{ mesh.nb_polygons() } )
                    {
                        for( auto& facet :
                            block_mesh_polyhedra_from_surface_polygon(
                                model_, block, surface, p ) )
                        {
                            result.emplace_back( std::move( facet ) );
                        }
                    }
                }
                return result;
            }

        private:
            const BRep& model_;
            BRepBuilder& builder_;
        };

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