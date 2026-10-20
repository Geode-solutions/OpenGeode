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

#include <geode/model/helpers/compute_unique_vertices.hpp>

#include <type_traits>

#include <absl/container/flat_hash_map.h>

#include <geode/geometry/nn_search.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    /*!
     * Calls func( component_id, mesh ) on all the mesh components of the
     * model, always in the same order: corners, lines, surfaces, blocks.
     */
    template < typename Model, typename Func >
    void for_each_component_mesh( const Model& model, Func&& func )
    {
        for( const auto& corner : model.corners() )
        {
            func( corner.component_id(), corner.mesh() );
        }
        for( const auto& line : model.lines() )
        {
            func( line.component_id(), line.mesh() );
        }
        for( const auto& surface : model.surfaces() )
        {
            func( surface.component_id(), surface.mesh() );
        }
        if constexpr( std::is_same_v< Model, geode::BRep > )
        {
            for( const auto& block : model.blocks() )
            {
                func( block.component_id(), block.mesh() );
            }
        }
    }

    template < typename Model >
    class UniqueVerticesComputer
    {
        struct ComponentVertices
        {
            geode::index_t offset;
            geode::index_t nb_vertices;
        };

    public:
        explicit UniqueVerticesComputer( const Model& model ) : model_( model )
        {
            geode::index_t nb_vertices{ 0 };
            for_each_component_mesh(
                model_, [this, &nb_vertices](
                            const geode::ComponentID& id, const auto& mesh ) {
                    components_.emplace( id.id,
                        ComponentVertices{ nb_vertices, mesh.nb_vertices() } );
                    nb_vertices += mesh.nb_vertices();
                } );
            std::vector< geode::Point< Model::dim > > points;
            points.reserve( nb_vertices );
            for_each_component_mesh(
                model_, [&points]( const geode::ComponentID& /*unused*/,
                            const auto& mesh ) {
                    for( const auto v : geode::Range{ mesh.nb_vertices() } )
                    {
                        points.emplace_back( mesh.point( v ) );
                    }
                } );
            const geode::NNSearch< Model::dim > nns{ std::move( points ) };
            auto colocated_info =
                nns.colocated_index_mapping( geode::GLOBAL_EPSILON );
            unique_point_uvs_.assign(
                colocated_info.nb_unique_points(), geode::NO_ID );
            colocated_mapping_ = std::move( colocated_info.colocated_mapping );
        }

        /*!
         * Maps each unique point to an existing unique vertex located at
         * this point. Isolated unique vertices (without any component mesh
         * vertex) are ignored. If several existing unique vertices are
         * located at the same point, they are kept distinct and the first
         * one (lowest index) is used for the vertices without unique vertex
         * located at this point.
         */
        void map_initial_unique_vertices()
        {
            for( const auto uv : geode::Range{ model_.nb_unique_vertices() } )
            {
                const auto& cmvs = model_.component_mesh_vertices( uv );
                if( cmvs.empty() )
                {
                    continue;
                }
                const auto& cmv = cmvs.front();
                const auto it = components_.find( cmv.component_id.id );
                if( it == components_.end()
                    || cmv.vertex >= it->second.nb_vertices )
                {
                    continue;
                }
                auto& unique_point_uv =
                    unique_point_uvs_[colocated_mapping_[it->second.offset
                                                         + cmv.vertex]];
                if( unique_point_uv == geode::NO_ID )
                {
                    unique_point_uv = uv;
                }
            }
        }

        /*!
         * Gives a new unique vertex index to the unique points without
         * existing unique vertex but with vertices to assign, and returns the
         * number of new unique vertices.
         */
        geode::index_t number_new_unique_vertices()
        {
            std::vector< bool > needed( unique_point_uvs_.size(), false );
            for_each_vertex_without_unique_vertex(
                [this, &needed]( const geode::ComponentMeshVertex& /*unused*/,
                    geode::index_t unique_point ) {
                    if( unique_point_uvs_[unique_point] == geode::NO_ID )
                    {
                        needed[unique_point] = true;
                    }
                } );
            const auto nb_initial_unique_vertices = model_.nb_unique_vertices();
            auto new_uv = nb_initial_unique_vertices;
            for( const auto unique_point : geode::Indices{ needed } )
            {
                if( needed[unique_point] )
                {
                    unique_point_uvs_[unique_point] = new_uv++;
                }
            }
            return new_uv - nb_initial_unique_vertices;
        }

        void set_unique_vertices( typename Model::Builder& builder ) const
        {
            for_each_vertex_without_unique_vertex(
                [this, &builder]( const geode::ComponentMeshVertex& cmv,
                    geode::index_t unique_point ) {
                    builder.set_unique_vertex(
                        cmv, unique_point_uvs_[unique_point] );
                } );
        }

    private:
        template < typename Func >
        void for_each_vertex_without_unique_vertex( Func&& func ) const
        {
            geode::index_t global_vertex{ 0 };
            for_each_component_mesh(
                model_, [this, &func, &global_vertex](
                            const geode::ComponentID& id, const auto& mesh ) {
                    geode::ComponentMeshVertex cmv{ id, 0 };
                    for( const auto v : geode::Range{ mesh.nb_vertices() } )
                    {
                        cmv.vertex = v;
                        if( model_.unique_vertex( cmv ) == geode::NO_ID )
                        {
                            func( cmv, colocated_mapping_[global_vertex] );
                        }
                        global_vertex++;
                    }
                } );
        }

    private:
        const Model& model_;
        absl::flat_hash_map< geode::uuid, ComponentVertices > components_;
        std::vector< geode::index_t > colocated_mapping_;
        std::vector< geode::index_t > unique_point_uvs_;
    };
} // namespace

namespace geode
{
    template < typename Model >
    void compute_model_unique_vertices(
        const Model& model, typename Model::Builder& builder )
    {
        UniqueVerticesComputer< Model > computer{ model };
        computer.map_initial_unique_vertices();
        builder.create_unique_vertices( computer.number_new_unique_vertices() );
        computer.set_unique_vertices( builder );
    }

    template void opengeode_model_api compute_model_unique_vertices(
        const BRep&, BRepBuilder& );
    template void opengeode_model_api compute_model_unique_vertices(
        const Section&, SectionBuilder& );
} // namespace geode
