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

#pragma once

#include <absl/container/flat_hash_map.h>
#include <absl/types/span.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component_type.h>

namespace geode
{
    struct ComponentMeshVertex;
} // namespace geode

namespace geode
{
    using ComponentMeshVertexPairs = absl::flat_hash_map< ComponentID,
        std::vector< std::array< index_t, 2 > > >;

    ComponentMeshVertexPairs opengeode_model_api component_mesh_vertex_pairs(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1 );

    ComponentMeshVertexPairs opengeode_model_api component_mesh_vertex_pairs(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1,
        const ComponentType& type );

    using ComponentMeshVertexTriplets = absl::flat_hash_map< ComponentID,
        std::vector< std::array< index_t, 3 > > >;

    ComponentMeshVertexTriplets opengeode_model_api
        component_mesh_vertex_triplets(
            absl::Span< const ComponentMeshVertex > unique_vertices0,
            absl::Span< const ComponentMeshVertex > unique_vertices1,
            absl::Span< const ComponentMeshVertex > unique_vertices2 );

    ComponentMeshVertexTriplets opengeode_model_api
        component_mesh_vertex_triplets(
            absl::Span< const ComponentMeshVertex > unique_vertices0,
            absl::Span< const ComponentMeshVertex > unique_vertices1,
            absl::Span< const ComponentMeshVertex > unique_vertices2,
            const ComponentType& type );
} // namespace geode