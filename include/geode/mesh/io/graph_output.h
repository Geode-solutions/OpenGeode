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

#pragma once

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/io/vertex_set_output.h>

namespace geode
{
    class Graph;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a Graph.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] graph Graph to save.
     * @param[in] filename Path to the file where save the Graph.
     */
    void opengeode_mesh_api save_graph(
        const Graph& graph, const std::string& filename );

    class opengeode_mesh_api GraphOutput : public VertexSetOutput
    {
    protected:
        GraphOutput( const Graph& graph, std::string filename );

        const Graph& graph() const
        {
            return graph_;
        }

    private:
        const Graph& graph_;
    };

    using GraphOutputFactory =
        Factory< std::string, GraphOutput, const Graph&, std::string >;
} // namespace geode
