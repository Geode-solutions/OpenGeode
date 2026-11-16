/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <vector>

#include <geode/basic/range.hpp>

#include <geode/geometry/octree.hpp>

namespace geode
{
    template < index_t dimension >
    template < typename Filter, typename Action >
    void Octree< dimension >::traverse( Filter&& filter, Action&& action ) const
    {
        if( nb_cells() == 0 )
        {
            return;
        }
        index_t nb_roots{ 1 };
        for( const auto nb : nb_root_cells() )
        {
            nb_roots *= nb;
        }
        std::vector< index_t > to_visit;
        to_visit.reserve( nb_roots );
        for( const auto root : Range{ nb_roots } )
        {
            to_visit.push_back( nb_roots - 1 - root );
        }
        while( !to_visit.empty() )
        {
            const auto cell = to_visit.back();
            to_visit.pop_back();
            if( !filter( cell, cell_bounding_box( cell ) ) )
            {
                continue;
            }
            const auto first_child = children( cell );
            if( !first_child )
            {
                action( cell );
                continue;
            }
            for( const auto child : Range{ NB_CHILDREN } )
            {
                to_visit.push_back(
                    first_child.value() + NB_CHILDREN - 1 - child );
            }
        }
    }
} // namespace geode
