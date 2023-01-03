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

#include <geode/geometry/private/predicates.h>

namespace geode
{
    namespace detail
    {
        inline Position point_segment_position( const Side& s0, const Side& s1 )
        {
            if( s0 == Side::zero )
            {
                return Position::vertex0;
            }
            if( s1 == Side::zero )
            {
                return Position::vertex1;
            }
            if( s0 != s1 )
            {
                return Position::inside;
            }
            return Position::outside;
        }

        inline Position point_segment_position(
            const Side& s0, const Side& s1, bool points_aligned )
        {
            if( !points_aligned )
            {
                return Position::outside;
            }
            return point_segment_position( s0, s1 );
        }

        inline Position point_triangle_position(
            const Side s0, const Side s1, const Side s2 )
        {
            if( s0 == Side::zero )
            {
                if( s1 == Side::zero )
                {
                    return Position::vertex1;
                }
                if( s2 == Side::zero )
                {
                    return Position::vertex0;
                }
                if( s1 == s2 )
                {
                    return Position::edge0;
                }
                return Position::outside;
            }
            if( s1 == Side::zero )
            {
                if( s2 == Side::zero )
                {
                    return Position::vertex2;
                }
                if( s0 == s2 )
                {
                    return Position::edge1;
                }
                return Position::outside;
            }
            if( s2 == Side::zero )
            {
                if( s0 == s1 )
                {
                    return Position::edge2;
                }
                return Position::outside;
            }
            if( s0 == s1 && s1 == s2 )
            {
                return Position::inside;
            }
            return Position::outside;
        }

        inline Position point_triangle_position(
            const Side s0, const Side s1, const Side s2, bool point_on_plane )
        {
            if( !point_on_plane )
            {
                return Position::outside;
            }
            return point_triangle_position( s0, s1, s2 );
        }

        inline Position point_tetrahedron_position(
            const std::array< GEO::Sign, 4 >& signs )
        {
            if( signs[0] == GEO::ZERO )
            {
                if( signs[1] == GEO::ZERO )
                {
                    if( signs[2] == GEO::ZERO )
                    {
                        return Position::vertex3;
                    }
                    if( signs[3] == GEO::ZERO )
                    {
                        return Position::vertex2;
                    }
                    if( signs[2] == signs[3] )
                    {
                        return Position::edge23;
                    }
                    return Position::outside;
                }
                if( signs[2] == GEO::ZERO )
                {
                    if( signs[3] == GEO::ZERO )
                    {
                        return Position::vertex1;
                    }
                    if( signs[1] == signs[3] )
                    {
                        return Position::edge13;
                    }
                    return Position::outside;
                }
                if( signs[3] == GEO::ZERO )
                {
                    if( signs[1] == signs[2] )
                    {
                        return Position::edge12;
                    }
                    return Position::outside;
                }
                if( signs[1] == signs[2] && signs[1] == signs[3] )
                {
                    return Position::facet0;
                }
                return Position::outside;
            }
            if( signs[1] == GEO::ZERO )
            {
                if( signs[2] == GEO::ZERO )
                {
                    if( signs[3] == GEO::ZERO )
                    {
                        return Position::vertex0;
                    }
                    if( signs[0] == signs[3] )
                    {
                        return Position::edge03;
                    }
                    return Position::outside;
                }
                if( signs[3] == GEO::ZERO )
                {
                    if( signs[0] == signs[2] )
                    {
                        return Position::edge02;
                    }
                    return Position::outside;
                }
                if( signs[0] == signs[2] && signs[0] == signs[3] )
                {
                    return Position::facet1;
                }
                return Position::outside;
            }
            if( signs[2] == GEO::ZERO )
            {
                if( signs[3] == GEO::ZERO )
                {
                    if( signs[0] == signs[1] )
                    {
                        return Position::edge01;
                    }
                    return Position::outside;
                }
                if( signs[0] == signs[1] && signs[0] == signs[3] )
                {
                    return Position::facet2;
                }
                return Position::outside;
            }
            if( signs[3] == GEO::ZERO )
            {
                if( signs[0] == signs[1] && signs[0] == signs[2] )
                {
                    return Position::facet3;
                }
                return Position::outside;
            }
            if( signs[0] == signs[1] && signs[0] == signs[2]
                && signs[0] == signs[3] )
            {
                return Position::inside;
            }
            return Position::outside;
        }
    } // namespace detail
} // namespace geode
