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

#include <geode/model/helpers/component_mesh_vertices.h>

#include <geode/model/mixin/core/vertex_identifier.h>

namespace
{
    template < typename Compare >
    geode::ComponentMeshVertexPairs component_mesh_vertex_pairs(
        absl::Span< const geode::ComponentMeshVertex > unique_vertices0,
        absl::Span< const geode::ComponentMeshVertex > unique_vertices1,
        const Compare& compare )
    {
        if( unique_vertices0.empty() || unique_vertices1.empty() )
        {
            return {};
        }
        geode::ComponentMeshVertexPairs result;
        result.reserve(
            std::min( unique_vertices0.size(), unique_vertices1.size() ) );
        for( const auto& cmv0 : unique_vertices0 )
        {
            for( const auto& cmv1 : unique_vertices1 )
            {
                if( compare( cmv0, cmv1 ) )
                {
                    OPENGEODE_ASSERT( cmv0.vertex != cmv1.vertex,
                        "[component_mesh_vertex_pairs] Vertices should be "
                        "different" );
                    result[cmv0.component_id].emplace_back(
                        std::array< geode::index_t, 2 >{
                            cmv0.vertex, cmv1.vertex } );
                }
            }
        }
        return result;
    }

    template < typename Compare >
    geode::ComponentMeshVertexTriplets component_mesh_vertex_triplets(
        absl::Span< const geode::ComponentMeshVertex > unique_vertices0,
        absl::Span< const geode::ComponentMeshVertex > unique_vertices1,
        absl::Span< const geode::ComponentMeshVertex > unique_vertices2,
        const Compare& compare )
    {
        if( unique_vertices0.empty() || unique_vertices1.empty()
            || unique_vertices2.empty() )
        {
            return {};
        }
        geode::ComponentMeshVertexTriplets result;
        result.reserve( std::min( unique_vertices0.size(),
            std::min( unique_vertices1.size(), unique_vertices2.size() ) ) );
        for( const auto& cmv0 : unique_vertices0 )
        {
            for( const auto& cmv1 : unique_vertices1 )
            {
                for( const auto& cmv2 : unique_vertices2 )
                {
                    if( compare( cmv0, cmv1, cmv2 ) )
                    {
                        OPENGEODE_ASSERT( cmv0.vertex != cmv1.vertex,
                            "[component_mesh_vertex_pairs] Vertices should be "
                            "different" );
                        OPENGEODE_ASSERT( cmv1.vertex != cmv2.vertex,
                            "[component_mesh_vertex_pairs] Vertices should be "
                            "different" );
                        result[cmv0.component_id].emplace_back(
                            std::array< geode::index_t, 3 >{
                                cmv0.vertex, cmv1.vertex, cmv2.vertex } );
                    }
                }
            }
        }
        return result;
    }
} // namespace

namespace geode
{
    ComponentMeshVertexPairs component_mesh_vertex_pairs(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1 )
    {
        return ::component_mesh_vertex_pairs( unique_vertices0,
            unique_vertices1,
            []( const ComponentMeshVertex& cmv0,
                const ComponentMeshVertex& cmv1 ) {
                return cmv0.component_id == cmv1.component_id;
            } );
    }

    ComponentMeshVertexPairs component_mesh_vertex_pairs(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1,
        const ComponentType& type )
    {
        return ::component_mesh_vertex_pairs( unique_vertices0,
            unique_vertices1,
            [type]( const ComponentMeshVertex& cmv0,
                const ComponentMeshVertex& cmv1 ) {
                return cmv0.component_id.type() == type
                       && cmv0.component_id == cmv1.component_id;
            } );
    }

    ComponentMeshVertexTriplets component_mesh_vertex_triplets(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1,
        absl::Span< const ComponentMeshVertex > unique_vertices2 )
    {
        return ::component_mesh_vertex_triplets( unique_vertices0,
            unique_vertices1, unique_vertices2,
            []( const ComponentMeshVertex& cmv0,
                const ComponentMeshVertex& cmv1,
                const ComponentMeshVertex& cmv2 ) {
                return cmv0.component_id == cmv1.component_id
                       && cmv0.component_id == cmv2.component_id;
            } );
    }

    ComponentMeshVertexTriplets component_mesh_vertex_triplets(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1,
        absl::Span< const ComponentMeshVertex > unique_vertices2,
        const ComponentType& type )
    {
        return ::component_mesh_vertex_triplets( unique_vertices0,
            unique_vertices1, unique_vertices2,
            [type]( const ComponentMeshVertex& cmv0,
                const ComponentMeshVertex& cmv1,
                const ComponentMeshVertex& cmv2 ) {
                return cmv0.component_id.type() == type
                       && cmv0.component_id == cmv1.component_id
                       && cmv0.component_id == cmv2.component_id;
            } );
    }
} // namespace geode