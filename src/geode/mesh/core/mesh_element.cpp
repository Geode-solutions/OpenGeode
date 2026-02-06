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

#include <geode/mesh/core/mesh_element.hpp>

#include <absl/hash/hash.h>

namespace std
{
    size_t hash< geode::MeshElement >::operator()(
        const geode::MeshElement& mesh_element ) const
    {
        return absl::Hash< geode::uuid >()( mesh_element.mesh_id )
               ^ absl::Hash< geode::index_t >()( mesh_element.element_id );
    }

    size_t hash< geode::MeshVertex >::operator()(
        const geode::MeshVertex& mesh_vertex ) const
    {
        return std::hash< geode::MeshElement >()( mesh_vertex );
    }

    size_t hash< geode::MeshEdge >::operator()(
        const geode::MeshEdge& mesh_edge ) const
    {
        return std::hash< geode::MeshElement >()( mesh_edge );
    }

    size_t hash< geode::MeshPolygon >::operator()(
        const geode::MeshPolygon& mesh_polygon ) const
    {
        return std::hash< geode::MeshElement >()( mesh_polygon );
    }
} // namespace std