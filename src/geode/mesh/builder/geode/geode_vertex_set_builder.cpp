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

#include <geode/mesh/builder/geode/geode_vertex_set_builder.hpp>

#include <geode/mesh/builder/mesh_builder_factory.hpp>
#include <geode/mesh/core/geode/geode_vertex_set.hpp>

namespace geode
{
    OpenGeodeVertexSetBuilder::OpenGeodeVertexSetBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : OpenGeodeVertexSetBuilder(
              dynamic_cast< OpenGeodeVertexSet& >( vertex_set ) )
    {
    }

    OpenGeodeVertexSetBuilder::OpenGeodeVertexSetBuilder(
        OpenGeodeVertexSet& mesh )
        : VertexSetBuilder( mesh ), geode_vertex_set_( mesh )
    {
    }

    OpenGeodeVertexSetBuilder::OpenGeodeVertexSetBuilder(
        OpenGeodeVertexSetBuilder&& ) noexcept = default;

    void OpenGeodeVertexSetBuilder::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeVertexSetBuilder::do_create_vertices( index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeVertexSetBuilder::do_delete_vertices(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeVertexSetBuilder::do_permute_vertices(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }
} // namespace geode
