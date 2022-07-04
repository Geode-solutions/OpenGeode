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

#include <geode/model/helpers/component_mesh_vertices.h>

#include <geode/model/mixin/core/vertex_identifier.h>

namespace geode
{
    ComponentMeshVertexPairs component_mesh_vertex_pairs(
        absl::Span< const ComponentMeshVertex > unique_vertices0,
        absl::Span< const ComponentMeshVertex > unique_vertices1 )
    {
        if( unique_vertices0.empty() || unique_vertices1.empty() )
        {
            return {};
        }
        ComponentMeshVertexPairs result;
        result.reserve(
            std::min( unique_vertices0.size(), unique_vertices1.size() ) );
        for( const auto& mcv0 : unique_vertices0 )
        {
            for( const auto& mcv1 : unique_vertices1 )
            {
                if( mcv0.component_id == mcv1.component_id )
                {
                    OPENGEODE_ASSERT( mcv0.vertex != mcv1.vertex,
                        "[component_mesh_vertex_pairs] Vertices should be "
                        "different" );
                    result[mcv0.component_id].emplace_back(
                        mcv0.vertex, mcv1.vertex );
                }
            }
        }
        return result;
    }
} // namespace geode