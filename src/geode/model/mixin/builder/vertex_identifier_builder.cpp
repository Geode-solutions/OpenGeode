/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/model/mixin/builder/vertex_identifier_builder.h>

namespace geode
{
    VertexIdentifierBuilder::VertexIdentifierBuilder(
        VertexIdentifier& vertex_identifier )
        : vertex_identifier_( vertex_identifier )
    {
    }

    index_t VertexIdentifierBuilder::create_unique_vertex()
    {
        return vertex_identifier_.create_unique_vertex();
    }

    index_t VertexIdentifierBuilder::create_unique_vertices( index_t nb )
    {
        return vertex_identifier_.create_unique_vertices( nb );
    }

    void VertexIdentifierBuilder::set_unique_vertex(
        MeshComponentVertex component_vertex_id, index_t unique_vertex_id )
    {
        vertex_identifier_.set_unique_vertex(
            component_vertex_id, unique_vertex_id );
    }

    void VertexIdentifierBuilder::unset_unique_vertex(
        const MeshComponentVertex& component_vertex_id,
        index_t unique_vertex_id )
    {
        vertex_identifier_.unset_unique_vertex(
            component_vertex_id, unique_vertex_id );
    }

    void VertexIdentifierBuilder::update_unique_vertices(
        const ComponentID& component_id, const std::vector< index_t >& old2new )
    {
        vertex_identifier_.update_unique_vertices( component_id, old2new );
    }

    void VertexIdentifierBuilder::load_unique_vertices(
        const std::string& directory )
    {
        vertex_identifier_.load_unique_vertices( directory );
    }
} // namespace geode
