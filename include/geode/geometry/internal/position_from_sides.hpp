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

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/internal/predicates.hpp>

namespace geode
{
    namespace internal
    {
        [[nodiscard]] inline POSITION point_segment_position(
            const SIDE& s0, const SIDE& s1 )
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

        [[nodiscard]] inline POSITION point_segment_position(
            const SIDE& s0, const SIDE& s1, bool points_aligned )
        {
            if( !points_aligned )
            {
                return POSITION::outside;
            }
            return point_segment_position( s0, s1 );
        }

        [[nodiscard]] inline POSITION point_triangle_position(
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

        [[nodiscard]] inline POSITION point_triangle_position(
            const SIDE s0, const SIDE s1, const SIDE s2, bool point_on_plane )
        {
            if( !point_on_plane )
            {
                return POSITION::outside;
            }
            return point_triangle_position( s0, s1, s2 );
        }

        [[nodiscard]] inline POSITION point_tetrahedron_position(
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

        /*
         * Exact sign of the point relative to the tetrahedron facet. Facets
         * are oriented towards the inside of positive tetrahedra.
         */
        [[nodiscard]] inline GEO::SIGN point_tetrahedron_facet_sign(
            const Point3D& point,
            const Tetrahedron& tetra,
            local_index_t facet )
        {
            const auto& vertices = tetra.vertices();
            const auto& facet_vertices =
                Tetrahedron::tetrahedron_facet_vertex[facet];
            return GEO::PCK::orient_3d( vertices[facet_vertices[0]],
                vertices[facet_vertices[1]], vertices[facet_vertices[2]],
                point );
        }

        /*
         * Exact orientation of the tetrahedron: sign of vertex 0 relative to
         * facet 0, zero for flat tetrahedra.
         */
        [[nodiscard]] inline GEO::SIGN tetrahedron_orientation(
            const Tetrahedron& tetra )
        {
            return point_tetrahedron_facet_sign(
                tetra.vertices()[0], tetra, 0 );
        }

        /*
         * Exact sign of the point relative to the tetrahedron facet, positive
         * towards the inside of the tetrahedron whatever its orientation
         * (given by tetrahedron_orientation).
         */
        [[nodiscard]] inline GEO::SIGN point_tetrahedron_facet_inner_sign(
            const Point3D& point,
            const Tetrahedron& tetra,
            local_index_t facet,
            GEO::SIGN orientation )
        {
            const auto sign =
                point_tetrahedron_facet_sign( point, tetra, facet );
            return orientation == GEO::negative
                       ? static_cast< GEO::SIGN >( -sign )
                       : sign;
        }
    } // namespace internal
} // namespace geode
