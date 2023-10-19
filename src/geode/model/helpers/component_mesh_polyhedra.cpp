/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/model/helpers/component_mesh_polyhedra.h>

#include <functional>
#include <vector>

#include <absl/container/inlined_vector.h>
#include <absl/types/span.h>

#include <geode/basic/algorithm.h>

#include <geode/mesh/core/detail/vertex_cycle.h>

#include <geode/model/helpers/component_mesh_vertices.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    class PolyhedronVerticesPossibilities
    {
    public:
        PolyhedronVerticesPossibilities(
            absl::Span< const std::reference_wrapper<
                const std::vector< geode::ComponentMeshVertex > > >
                unique_vertices_cmvs )
            : unique_vertices_cmvs_{ unique_vertices_cmvs },
              nb_unique_vertices_{ static_cast< geode::local_index_t >(
                  unique_vertices_cmvs_.size() ) }
        {
        }

        std::vector< std::pair< geode::uuid, geode::PolyhedronVertices > >
            compute_possibilities()
        {
            if( unique_vertices_cmvs_.empty() )
            {
                return {};
            }
            for( const auto& cmvs : unique_vertices_cmvs_ )
            {
                if( cmvs.get().empty() )
                {
                    return {};
                }
            }
            common_block_vertices_list_.clear();
            for( const auto& first_cmv : unique_vertices_cmvs_[0].get() )
            {
                if( first_cmv.component_id.type()
                    != geode::Block3D::component_type_static() )
                {
                    continue;
                }
                const auto& mesh_vertices = block_mesh_vertices( first_cmv );
                if( mesh_vertices.empty() )
                {
                    continue;
                }
                fill_polyhedron_vertices_possibilities(
                    first_cmv.component_id.id(), mesh_vertices, 1,
                    { first_cmv.vertex } );
            }
            return std::move( common_block_vertices_list_ );
        }

    private:
        absl::FixedArray< std::vector< geode::index_t > > block_mesh_vertices(
            const geode::ComponentMeshVertex& first_cmv )
        {
            const auto& first_cmv_block_id = first_cmv.component_id.id();
            absl::FixedArray< std::vector< geode::index_t > > mesh_vertices(
                nb_unique_vertices_ );
            mesh_vertices[0].push_back( first_cmv.vertex );
            for( const auto other_cmv_list_id :
                geode::LRange{ 1, nb_unique_vertices_ } )
            {
                for( const auto& other_cmv :
                    unique_vertices_cmvs_[other_cmv_list_id].get() )
                {
                    if( first_cmv_block_id == other_cmv.component_id.id() )
                    {
                        mesh_vertices[other_cmv_list_id].push_back(
                            other_cmv.vertex );
                    }
                }
                if( mesh_vertices[other_cmv_list_id].empty() )
                {
                    return {};
                }
            }
            return mesh_vertices;
        }

        void fill_polyhedron_vertices_possibilities(
            const geode::uuid& current_block,
            absl::Span< const std::vector< geode::index_t > > mesh_vertices,
            geode::local_index_t current_index,
            geode::PolyhedronVertices current_list )
        {
            for( const auto vertex_possibility : mesh_vertices[current_index] )
            {
                auto updated_list = current_list;
                updated_list.push_back( vertex_possibility );
                if( current_index == nb_unique_vertices_ - 1 )
                {
                    common_block_vertices_list_.emplace_back(
                        current_block, std::move( updated_list ) );
                    continue;
                }
                fill_polyhedron_vertices_possibilities( current_block,
                    mesh_vertices, current_index + 1, updated_list );
            }
        }

    private:
        absl::Span< const std::reference_wrapper<
            const std::vector< geode::ComponentMeshVertex > > >
            unique_vertices_cmvs_;
        const geode::local_index_t nb_unique_vertices_;
        std::vector< std::pair< geode::uuid, geode::PolyhedronVertices > >
            common_block_vertices_list_;
    };
} // namespace

namespace geode
{
    PolyhedronVertices polyhedron_unique_vertices(
        const BRep& model, const Block3D& block, index_t polyhedron_id )
    {
        const auto& solid_mesh = block.mesh();
        const auto nb_vertices =
            solid_mesh.nb_polyhedron_vertices( polyhedron_id );
        PolyhedronVertices polyhedron_unique_vertices( nb_vertices );
        for( const auto v : LRange{ nb_vertices } )
        {
            polyhedron_unique_vertices[v] =
                model.unique_vertex( { block.component_id(),
                    solid_mesh.polyhedron_vertex( { polyhedron_id, v } ) } );
        }
        return polyhedron_unique_vertices;
    }

    std::vector< MeshElement > component_mesh_polyhedra(
        const BRep& brep, const PolyhedronVertices& unique_vertices )
    {
        std::vector<
            std::reference_wrapper< const std::vector< ComponentMeshVertex > > >
            unique_vertices_cmvs;
        unique_vertices_cmvs.reserve( unique_vertices.size() );
        for( const auto unique_vertex : unique_vertices )
        {
            unique_vertices_cmvs.emplace_back(
                brep.component_mesh_vertices( unique_vertex ) );
        }
        std::vector< MeshElement > result;
        PolyhedronVerticesPossibilities vertices_pair_computer{
            unique_vertices_cmvs
        };
        for( auto& block_vertices :
            vertices_pair_computer.compute_possibilities() )
        {
            const auto& block = brep.block( block_vertices.first );
            const auto& block_mesh = block.mesh();
            absl::c_sort( block_vertices.second );
            for( const auto& polyhedron_vertex :
                block_mesh.polyhedra_around_vertex( block_vertices.second[0] ) )
            {
                auto polyhedron_vertices = block_mesh.polyhedron_vertices(
                    polyhedron_vertex.polyhedron_id );
                absl::c_sort( polyhedron_vertices );
                if( absl::c_equal(
                        block_vertices.second, polyhedron_vertices ) )
                {
                    result.emplace_back(
                        block.id(), polyhedron_vertex.polyhedron_id );
                }
            }
        }
        return result;
    }
} // namespace geode