/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/builder/geode_vertex_set_builder.h>

#include <geode/mesh/core/geode_vertex_set.h>

namespace geode
{
    OpenGeodeVertexSetBuilder::OpenGeodeVertexSetBuilder(
        VertexSet& vertex_set )
        : VertexSetBuilder( vertex_set ),
          geode_vertex_set_( dynamic_cast< OpenGeodeVertexSet& >( vertex_set ) )
    {
    }

    void OpenGeodeVertexSetBuilder::do_create_vertex() {}

    void OpenGeodeVertexSetBuilder::do_create_vertices( index_t /*unused*/ ) {}

    void OpenGeodeVertexSetBuilder::do_delete_vertices(
        const std::vector< bool >& /*unused*/ )
    {
    }
} // namespace geode
