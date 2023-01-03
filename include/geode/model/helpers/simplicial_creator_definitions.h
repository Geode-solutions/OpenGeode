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

#include <geode/model/common.h>

namespace geode
{
    struct CornerDefinition
    {
        index_t vertex;
    };

    struct LineDefinition
    {
        /// List of the unique points: two following vertices represent an edge
        std::vector< index_t > vertices;
    };

    struct SurfaceDefinition
    {
        /// List of the unique points, duplicate points along internal Lines
        std::vector< index_t > vertices;
        /// List of vertices: every three vertices = one triangle
        std::vector< index_t > triangles;
        /// List of boundaries: index i correspond to the i-th created Line
        std::vector< index_t > boundaries;
        /// List of internal lines: index i correspond to the i-th created Line
        std::vector< index_t > internals;
        /// List of internal corners: index i correspond to the i-th created
        /// Corner
        std::vector< index_t > internal_corners;
    };

    struct BlockDefinition
    {
        std::vector< index_t > vertices;
        // List of vertices: every four vertices = one tetrahedron
        std::vector< index_t > tetrahedra;
        /// List of boundaries: index i correspond to the i-th created Surface
        std::vector< index_t > boundaries;
        /// List of internal surfaces: index i correspond to the i-th created
        /// Surface
        std::vector< index_t > internals;
        /// List of internal lines: index i correspond to the i-th created Line
        std::vector< index_t > internal_lines;
        /// List of internal corners: index i correspond to the i-th created
        /// Corner
        std::vector< index_t > internal_corners;
    };

    struct BoundaryDefinition
    {
        std::vector< index_t > boundaries;
    };
} // namespace geode