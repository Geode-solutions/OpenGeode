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

#include <string>
#include <string_view>
#include <vector>

#include <geode/basic/factory.hpp>
#include <geode/basic/output.hpp>

#include <geode/mesh/common.hpp>

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
    std::vector< std::string > opengeode_mesh_api save_graph(
        const Graph& graph, std::string_view filename );

    class GraphOutput : public Output< Graph >
    {
    protected:
        explicit GraphOutput( std::string_view filename )
            : Output< Graph >{ filename }
        {
        }
    };

    [[nodiscard]] bool opengeode_mesh_api is_graph_saveable(
        const Graph& graph, std::string_view filename );

    using GraphOutputFactory =
        Factory< std::string, GraphOutput, std::string_view >;
} // namespace geode
