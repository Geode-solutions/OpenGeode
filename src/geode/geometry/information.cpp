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

#include <geode/geometry/information.h>

#include <absl/container/flat_hash_map.h>

namespace geode
{
    namespace detail
    {
        local_index_t position_to_index( POSITION position )
        {
            static const absl::flat_hash_map< POSITION, local_index_t > map = {
                { POSITION::vertex0, 0 },
                { POSITION::vertex1, 1 },
                { POSITION::vertex2, 2 },
                { POSITION::vertex3, 3 },
                { POSITION::edge0, 0 },
                { POSITION::edge1, 1 },
                { POSITION::edge2, 2 },
                { POSITION::facet0, 0 },
                { POSITION::facet1, 1 },
                { POSITION::facet2, 2 },
                { POSITION::facet3, 3 },
                { POSITION::edge01, 0 },
                { POSITION::edge02, 1 },
                { POSITION::edge03, 2 },
                { POSITION::edge12, 3 },
                { POSITION::edge13, 4 },
                { POSITION::edge23, 5 },
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
