/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <array>

#include <geode/basic/common.h>

namespace geode
{
    namespace detail
    {
        static constexpr std::array< std::array< geode::local_index_t, 2 >, 4 >
            QUADRANGLE_EDGE_VERTICES{ { { 0, 1 }, { 0, 2 }, { 2, 3 },
                { 1, 3 } } };

        static constexpr std::array< std::array< geode::local_index_t, 2 >, 4 >
            QUADRANGLE_EDGES_AROUND_VERTEX{ { { 0, 1 }, { 0, 3 }, { 1, 2 },
                { 2, 3 } } };

        static constexpr std::array< std::array< geode::local_index_t, 3 >, 4 >
            TETRAHEDRON_FACET_VERTICES{ { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 },
                { 0, 1, 2 } } };

        static constexpr std::array< std::array< geode::local_index_t, 2 >, 6 >
            TETRAHEDRON_EDGE_VERTICES{ { { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 2 },
                { 1, 3 }, { 2, 3 } } };

        static constexpr std::
            array< std::array< std::array< geode::local_index_t, 2 >, 4 >, 4 >
                TETRAHEDRON_OPPOSITE_EDGE_VERTICES{ {
                    { { { NO_LID, NO_LID }, { 2, 3 }, { 1, 3 }, { 1, 2 } } },
                    { { { 2, 3 }, { NO_LID, NO_LID }, { 0, 3 }, { 0, 2 } } },
                    { { { 1, 3 }, { 0, 3 }, { NO_LID, NO_LID }, { 0, 1 } } },
                    { { { 1, 2 }, { 0, 2 }, { 0, 1 }, { NO_LID, NO_LID } } },
                } };

        static constexpr std::array< std::array< geode::local_index_t, 4 >, 6 >
            HEXAHEDRON_FACET_VERTICES{ { { 0, 4, 5, 1 }, { 2, 6, 7, 3 },
                { 0, 3, 7, 4 }, { 1, 5, 6, 2 }, { 0, 1, 2, 3 },
                { 4, 7, 6, 5 } } };

        static constexpr std::array< std::array< geode::local_index_t, 2 >, 12 >
            HEXAHEDRON_EDGE_VERTICES{ { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
                { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 },
                { 2, 6 }, { 3, 7 } } };

        static const std::array< std::vector< geode::local_index_t >, 5 >
            PRISM_FACET_VERTICES{ { { 0, 1, 2 }, { 3, 5, 4 }, { 0, 3, 4, 1 },
                { 1, 4, 5, 2 }, { 0, 2, 5, 3 } } };

        static constexpr std::array< std::array< geode::local_index_t, 2 >, 9 >
            PRISM_EDGE_VERTICES{ { { 0, 1 }, { 1, 2 }, { 2, 0 }, { 3, 4 },
                { 4, 5 }, { 5, 3 }, { 0, 3 }, { 1, 4 }, { 2, 5 } } };

        static const std::array< std::vector< geode::local_index_t >, 5 >
            PYRAMID_FACET_VERTICES{ { { 0, 1, 2, 3 }, { 0, 4, 1 }, { 1, 4, 2 },
                { 2, 4, 3 }, { 0, 3, 4 } } };

        static constexpr std::array< std::array< geode::local_index_t, 2 >, 8 >
            PYRAMID_EDGE_VERTICES{ { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
                { 0, 4 }, { 1, 4 }, { 2, 4 }, { 3, 4 } } };
    } // namespace detail
} // namespace geode
