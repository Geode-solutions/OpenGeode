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

#pragma once

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/io/vertex_set_input.h>

namespace geode
{
    class Graph;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an Graph.
     * The adequate loader is called depending on the filename extension.
     * @param[in] impl Data structure implementation.
     * @param[in] filename Path to the file to load.
     */
    std::unique_ptr< Graph > opengeode_mesh_api load_graph(
        const MeshImpl& impl, absl::string_view filename );

    /*!
     * API function for loading an Graph.
     * The adequate loader is called depending on the filename extension.
     * Default data structure implémentation is used.
     * @param[in] filename Path to the file to load.
     */
    std::unique_ptr< Graph > opengeode_mesh_api load_graph(
        absl::string_view filename );

    class opengeode_mesh_api GraphInput : public VertexSetInput
    {
    protected:
        GraphInput( Graph& graph, absl::string_view filename );

        Graph& graph()
        {
            return graph_;
        }

    private:
        Graph& graph_;
    };

    using GraphInputFactory =
        Factory< std::string, GraphInput, Graph&, absl::string_view >;
} // namespace geode
