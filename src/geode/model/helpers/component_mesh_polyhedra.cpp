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

#include <absl/container/inlined_vector.h>

#include <geode/basic/algorithm.h>

#include <geode/mesh/core/detail/vertex_cycle.h>

#include <geode/model/helpers/component_mesh_vertices.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    std::vector< std::pair< geode::uuid, geode::PolyhedronVertices > >
        common_block_vertices(
            absl::Span< const std::vector< geode::ComponentMeshVertex > >
                unique_vertices_cmvs )
    {
        if( unique_vertices_cmvs.empty() || unique_vertices_cmvs[0].empty() )
        {
            return {};
        }
        const auto nb_unique_vertices = unique_vertices_cmvs.size();
        std::vector< std::pair< geode::uuid, geode::PolyhedronVertices > >
            common_block_vertices_list;
        for( const auto& first_cmv : unique_vertices_cmvs[0] )
        {
            const auto& first_cmv_block_id = first_cmv.component_id.id();
            geode::PolyhedronVertices mesh_vertices;
            mesh_vertices.push_back( first_cmv.vertex );
            for( const auto other_cmv_list_id :
                geode::LRange{ 1, nb_unique_vertices } )
            {
                /// If there is a cmv with same id, push_back and continue
                /// If not, break
                const auto& other_cmv_list =
                    unique_vertices_cmvs[other_cmv_list_id];
                const auto it = absl::c_find_if( other_cmv_list,
                    [&first_cmv_block_id](
                        const geode::ComponentMeshVertex& other_cmv ) {
                        return first_cmv_block_id
                               == other_cmv.component_id.id();
                    } );
                if( it == other_cmv_list.end() )
                {
                    break;
                }
                mesh_vertices.push_back( it->vertex );
            }
            if( mesh_vertices.size() == nb_unique_vertices )
            {
                common_block_vertices_list.emplace_back(
                    first_cmv_block_id, mesh_vertices );
            }
        }
        return common_block_vertices_list;
    }
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
        std::vector< std::vector< ComponentMeshVertex > > unique_vertices_cmvs(
            unique_vertices.size() );
        for( const auto v_id : LIndices{ unique_vertices } )
        {
            unique_vertices_cmvs[v_id] = brep.component_mesh_vertices(
                unique_vertices[v_id], Block3D::component_type_static() );
        }
        std::vector< MeshElement > result;
        for( auto& block_vertices :
            common_block_vertices( unique_vertices_cmvs ) )
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