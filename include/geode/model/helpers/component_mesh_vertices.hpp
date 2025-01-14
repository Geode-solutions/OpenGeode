/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#pragma once

#include <absl/container/flat_hash_map.h>
#include <absl/types/span.h>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component_type.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>

namespace geode
{
    struct ComponentMeshVertex;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    using ComponentMeshVertexGenericStorage =
        absl::InlinedVector< index_t, dimension >;
    template < index_t dimension >
    using ComponentMeshVertexGeneric = absl::flat_hash_map< ComponentID,
        std::vector< ComponentMeshVertexGenericStorage< dimension > > >;

    template < index_t dimension >
    [[nodiscard]] ComponentMeshVertexGeneric< dimension >
        component_mesh_vertex_generic(
            absl::Span< const absl::Span< const ComponentMeshVertex > >
                unique_vertices );

    template < index_t dimension >
    [[nodiscard]] ComponentMeshVertexGeneric< dimension >
        component_mesh_vertex_generic(
            absl::Span< const absl::Span< const ComponentMeshVertex > >
                unique_vertices,
            const ComponentType& type );

    using ComponentMeshVertexPairs = ComponentMeshVertexGeneric< 2 >;
    [[nodiscard]] ComponentMeshVertexPairs opengeode_model_api
        component_mesh_vertex_pairs(
            absl::Span< const ComponentMeshVertex > unique_vertices0,
            absl::Span< const ComponentMeshVertex > unique_vertices1 );
    [[nodiscard]] ComponentMeshVertexPairs opengeode_model_api
        component_mesh_vertex_pairs(
            absl::Span< const ComponentMeshVertex > unique_vertices0,
            absl::Span< const ComponentMeshVertex > unique_vertices1,
            const ComponentType& type );

    using ComponentMeshVertexTriplets = ComponentMeshVertexGeneric< 3 >;
    [[nodiscard]] ComponentMeshVertexTriplets opengeode_model_api
        component_mesh_vertex_triplets(
            absl::Span< const ComponentMeshVertex > unique_vertices0,
            absl::Span< const ComponentMeshVertex > unique_vertices1,
            absl::Span< const ComponentMeshVertex > unique_vertices2 );
    [[nodiscard]] ComponentMeshVertexTriplets opengeode_model_api
        component_mesh_vertex_triplets(
            absl::Span< const ComponentMeshVertex > unique_vertices0,
            absl::Span< const ComponentMeshVertex > unique_vertices1,
            absl::Span< const ComponentMeshVertex > unique_vertices2,
            const ComponentType& type );

    template < index_t dimension, typename... UniqueVertices >
    [[nodiscard]] ComponentMeshVertexGeneric< dimension >
        component_mesh_vertex_tuple( UniqueVertices... unique_vertices )
    {
        return component_mesh_vertex_generic(
            to_array< absl::Span< const ComponentMeshVertex > >(
                unique_vertices... ) );
    }
    template < index_t dimension, typename... UniqueVertices >
    [[nodiscard]] ComponentMeshVertexGeneric< dimension >
        component_mesh_vertex_tuple(
            UniqueVertices... unique_vertices, const ComponentType& type )
    {
        return component_mesh_vertex_generic(
            to_array< absl::Span< const ComponentMeshVertex > >(
                unique_vertices... ),
            type );
    }
} // namespace geode