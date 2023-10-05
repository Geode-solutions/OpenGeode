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

#include <geode/geometry/information.h>

#include <absl/container/flat_hash_map.h>

namespace geode
{
    namespace detail
    {
        local_index_t poistion_to_index( Position position )
        {
            static const absl::flat_hash_map< Position, local_index_t > map = {
                { Position::vertex0, 0 },
                { Position::vertex1, 1 },
                { Position::vertex2, 2 },
                { Position::vertex3, 3 },
                { Position::edge0, 0 },
                { Position::edge1, 1 },
                { Position::edge2, 2 },
                { Position::facet0, 0 },
                { Position::facet1, 1 },
                { Position::facet2, 2 },
                { Position::facet3, 3 },
            };
            const auto index_it = map.find( position );
            if( index_it == map.end() )
            {
                return NO_LID;
            }
            return index_it->second;
        }
    } // namespace detail
} // namespace geode
