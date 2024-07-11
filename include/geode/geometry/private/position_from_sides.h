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

#include <geode/geometry/private/predicates.h>

namespace geode
{
    namespace detail
    {
        inline POSITION point_segment_position( const SIDE& s0, const SIDE& s1 )
        {
            if( s0 == SIDE::zero )
            {
                return POSITION::vertex0;
            }
            if( s1 == SIDE::zero )
            {
                return POSITION::vertex1;
            }
            if( s0 != s1 )
            {
                return POSITION::inside;
            }
            return POSITION::outside;
        }

        inline POSITION point_segment_position(
            const SIDE& s0, const SIDE& s1, bool points_aligned )
        {
            if( !points_aligned )
            {
                return POSITION::outside;
            }
            return point_segment_position( s0, s1 );
        }

        inline POSITION point_triangle_position(
            const SIDE s0, const SIDE s1, const SIDE s2 )
        {
            if( s0 == SIDE::zero )
            {
                if( s1 == SIDE::zero )
                {
                    return POSITION::vertex1;
                }
                if( s2 == SIDE::zero )
                {
                    return POSITION::vertex0;
                }
                if( s1 == s2 )
                {
                    return POSITION::edge0;
                }
                return POSITION::outside;
            }
            if( s1 == SIDE::zero )
            {
                if( s2 == SIDE::zero )
                {
                    return POSITION::vertex2;
                }
                if( s0 == s2 )
                {
                    return POSITION::edge1;
                }
                return POSITION::outside;
            }
            if( s2 == SIDE::zero )
            {
                if( s0 == s1 )
                {
                    return POSITION::edge2;
                }
                return POSITION::outside;
            }
            if( s0 == s1 && s1 == s2 )
            {
                return POSITION::inside;
            }
            return POSITION::outside;
        }

        inline POSITION point_triangle_position(
            const SIDE s0, const SIDE s1, const SIDE s2, bool point_on_plane )
        {
            if( !point_on_plane )
            {
                return POSITION::outside;
            }
            return point_triangle_position( s0, s1, s2 );
        }

        inline POSITION point_tetrahedron_position(
            const std::array< GEO::SIGN, 4 >& signs )
        {
            if( signs[0] == GEO::zero )
            {
                if( signs[1] == GEO::zero )
                {
                    if( signs[2] == GEO::zero )
                    {
                        return POSITION::vertex3;
                    }
                    if( signs[3] == GEO::zero )
                    {
                        return POSITION::vertex2;
                    }
                    if( signs[2] == signs[3] )
                    {
                        return POSITION::edge23;
                    }
                    return POSITION::outside;
                }
                if( signs[2] == GEO::zero )
                {
                    if( signs[3] == GEO::zero )
                    {
                        return POSITION::vertex1;
                    }
                    if( signs[1] == signs[3] )
                    {
                        return POSITION::edge13;
                    }
                    return POSITION::outside;
                }
                if( signs[3] == GEO::zero )
                {
                    if( signs[1] == signs[2] )
                    {
                        return POSITION::edge12;
                    }
                    return POSITION::outside;
                }
                if( signs[1] == signs[2] && signs[1] == signs[3] )
                {
                    return POSITION::facet0;
                }
                return POSITION::outside;
            }
            if( signs[1] == GEO::zero )
            {
                if( signs[2] == GEO::zero )
                {
                    if( signs[3] == GEO::zero )
                    {
                        return POSITION::vertex0;
                    }
                    if( signs[0] == signs[3] )
                    {
                        return POSITION::edge03;
                    }
                    return POSITION::outside;
                }
                if( signs[3] == GEO::zero )
                {
                    if( signs[0] == signs[2] )
                    {
                        return POSITION::edge02;
                    }
                    return POSITION::outside;
                }
                if( signs[0] == signs[2] && signs[0] == signs[3] )
                {
                    return POSITION::facet1;
                }
                return POSITION::outside;
            }
            if( signs[2] == GEO::zero )
            {
                if( signs[3] == GEO::zero )
                {
                    if( signs[0] == signs[1] )
                    {
                        return POSITION::edge01;
                    }
                    return POSITION::outside;
                }
                if( signs[0] == signs[1] && signs[0] == signs[3] )
                {
                    return POSITION::facet2;
                }
                return POSITION::outside;
            }
            if( signs[3] == GEO::zero )
            {
                if( signs[0] == signs[1] && signs[0] == signs[2] )
                {
                    return POSITION::facet3;
                }
                return POSITION::outside;
            }
            if( signs[0] == signs[1] && signs[0] == signs[2]
                && signs[0] == signs[3] )
            {
                return POSITION::inside;
            }
            return POSITION::outside;
        }
    } // namespace detail
} // namespace geode
