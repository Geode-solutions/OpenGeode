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

#include <geode/basic/logger.h>

namespace
{

    template < geode::index_t dimension, typename Compare >
    void recursive_compare_unique_vertices( geode::index_t index,
        geode::ComponentMeshVertexGeneric< dimension >& result,
        geode::ComponentMeshVertexGenericStorage< dimension >& current_result,
        const geode::ComponentMeshVertex& previous,
        absl::Span< const absl::Span< const geode::ComponentMeshVertex > >
            unique_vertices,
        const Compare& compare )
    {
        if( index == unique_vertices.size() )
        {
            result[previous.component_id].emplace_back(
                std::move( current_result ) );
            return;
        }
        for( const auto& cmv : unique_vertices[index] )
        {
            if( compare( cmv, previous ) )
            {
                OPENGEODE_ASSERT( cmv.vertex != previous.vertex,
                    "[component_mesh_vertex_pairs] Vertices should be "
                    "different" );
                auto temp_result = current_result;
                temp_result[index] = cmv.vertex;
                recursive_compare_unique_vertices< dimension, Compare >(
                    index + 1, result, temp_result, cmv, unique_vertices,
                    compare );
            }
        }
    }

    template < geode::index_t dimension, typename Compare >
    geode::ComponentMeshVertexGeneric< dimension >
        component_mesh_vertex_generic(
            absl::Span< const absl::Span< const geode::ComponentMeshVertex > >
                unique_vertices,
            const Compare& compare )
    {
        if( unique_vertices.empty() )
        {
            return {};
        }
        for( const auto& span : unique_vertices )
        {
            if( span.empty() )
            {
                return {};
            }
        }
        geode::ComponentMeshVertexGeneric< dimension > result;
        for( const auto& cmv : unique_vertices[0] )
        {
            geode::ComponentMeshVertexGenericStorage< dimension >
                current_result{ cmv.vertex };
            current_result.resize( unique_vertices.size() );
            recursive_compare_unique_vertices< dimension, Compare >(
                1, result, current_result, cmv, unique_vertices, compare );
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
        return ::component_mesh_vertex_generic< 2 >(
            to_array< absl::Span< const ComponentMeshVertex > >(
                unique_vertices0, unique_vertices1 ),
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
        return ::component_mesh_vertex_generic< 2 >(
            to_array< absl::Span< const ComponentMeshVertex > >(
                unique_vertices0, unique_vertices1 ),
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
        return ::component_mesh_vertex_generic< 3 >(
            to_array< absl::Span< const ComponentMeshVertex > >(
                unique_vertices0, unique_vertices1, unique_vertices2 ),
            []( const ComponentMeshVertex& cmv0,
                const ComponentMeshVertex& cmv1 ) {
                return cmv0.component_id == cmv1.component_id;
            } );
    }

    ComponentMeshVertexTriplets component_mesh_vertex_triplets(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1,
        absl::Span< const ComponentMeshVertex > unique_vertices2,
        const ComponentType& type )
    {
        return ::component_mesh_vertex_generic< 3 >(
            to_array< absl::Span< const ComponentMeshVertex > >(
                unique_vertices0, unique_vertices1, unique_vertices2 ),
            [type]( const ComponentMeshVertex& cmv0,
                const ComponentMeshVertex& cmv1 ) {
                return cmv0.component_id.type() == type
                       && cmv0.component_id == cmv1.component_id;
            } );
    }

    template < geode::index_t dimension >
    geode::ComponentMeshVertexGeneric< dimension >
        component_mesh_vertex_generic(
            absl::Span< const absl::Span< const geode::ComponentMeshVertex > >
                unique_vertices )
    {
        return ::component_mesh_vertex_generic< dimension >(
            unique_vertices, []( const ComponentMeshVertex& cmv0,
                                 const ComponentMeshVertex& cmv1 ) {
                return cmv0.component_id == cmv1.component_id;
            } );
    }

    template < geode::index_t dimension >
    geode::ComponentMeshVertexGeneric< dimension >
        component_mesh_vertex_generic(
            absl::Span< const absl::Span< const geode::ComponentMeshVertex > >
                unique_vertices,
            const ComponentType& type )
    {
        return ::component_mesh_vertex_generic< dimension >(
            unique_vertices, [type]( const ComponentMeshVertex& cmv0,
                                 const ComponentMeshVertex& cmv1 ) {
                return cmv0.component_id.type() == type
                       && cmv0.component_id == cmv1.component_id;
            } );
    }

    template ComponentMeshVertexGeneric< 2 >
        opengeode_model_api component_mesh_vertex_generic< 2 >(
            absl::Span< const absl::Span< const ComponentMeshVertex > > );
    template ComponentMeshVertexGeneric< 3 >
        opengeode_model_api component_mesh_vertex_generic< 3 >(
            absl::Span< const absl::Span< const ComponentMeshVertex > > );
    template ComponentMeshVertexGeneric< 4 >
        opengeode_model_api component_mesh_vertex_generic< 4 >(
            absl::Span< const absl::Span< const ComponentMeshVertex > > );

    template ComponentMeshVertexGeneric< 2 >
        opengeode_model_api component_mesh_vertex_generic< 2 >(
            absl::Span< const absl::Span< const ComponentMeshVertex > >,
            const ComponentType& );
    template ComponentMeshVertexGeneric< 3 >
        opengeode_model_api component_mesh_vertex_generic< 3 >(
            absl::Span< const absl::Span< const ComponentMeshVertex > >,
            const ComponentType& );
    template ComponentMeshVertexGeneric< 4 >
        opengeode_model_api component_mesh_vertex_generic< 4 >(
            absl::Span< const absl::Span< const ComponentMeshVertex > >,
            const ComponentType& );
} // namespace geode