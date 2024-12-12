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

// Implementation inspired from https://github.com/sandialabs/verdict

#include <geode/geometry/quality.hpp>

#include <limits>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/vector.hpp>

namespace
{

    namespace Detail
    {
        double constexpr sqrtNewtonRaphson( double x, double curr, double prev )
        {
            return curr == prev ? curr
                                : sqrtNewtonRaphson(
                                      x, 0.5 * ( curr + x / curr ), curr );
        }
    } // namespace Detail

    /*
     * Constexpr version of the square root
     * Return value:
     *	- For a finite and non-negative value of "x", returns an approximation
     *for the square root of "x"
     *   - Otherwise, returns NaN
     */
    double constexpr sqrt( double x )
    {
        return x >= 0 && x < std::numeric_limits< double >::infinity()
                   ? Detail::sqrtNewtonRaphson( x, x, 0 )
                   : std::numeric_limits< double >::quiet_NaN();
    }

    constexpr double SIX_SQRT_2 = 6 * sqrt( 2 );
} // namespace

namespace geode
{
    double tetrahedron_aspect_ratio( const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        const Vector3D edge_ab{ vertices[0], vertices[1] };
        const Vector3D edge_ac{ vertices[0], vertices[2] };
        const Vector3D edge_ad{ vertices[0], vertices[3] };
        const auto absolute_det =
            std::fabs( edge_ab.dot( edge_ac.cross( edge_ad ) ) );
        if( absolute_det < GLOBAL_EPSILON )
        {
            return std::numeric_limits< double >::max();
        }
        const Vector3D edge_bc{ vertices[1], vertices[2] };
        const Vector3D edge_bd{ vertices[1], vertices[3] };
        const Vector3D edge_cd{ vertices[2], vertices[3] };
        const auto edge_ab_l2 = edge_ab.length2();
        const auto edge_bc_l2 = edge_bc.length2();
        const auto edge_ac_l2 = edge_ac.length2();
        const auto edge_ad_l2 = edge_ad.length2();
        const auto edge_bd_l2 = edge_bd.length2();
        const auto edge_cd_l2 = edge_cd.length2();
        const auto longest_edge_length = std::sqrt( std::max( { edge_ab_l2,
            edge_bc_l2, edge_ac_l2, edge_ad_l2, edge_bd_l2, edge_cd_l2 } ) );
        const auto total_area2 = edge_ab.cross( edge_bc ).length()
                                 + edge_ab.cross( edge_ad ).length()
                                 + edge_ac.cross( edge_ad ).length()
                                 + edge_bc.cross( edge_cd ).length();
        const auto constant = std::sqrt( 6 ) / 12.;
        const auto aspect_ratio =
            constant * longest_edge_length * total_area2 / absolute_det;
        return aspect_ratio;
    }

    double tetrahedron_volume_to_edge_ratio( const Tetrahedron& tetra )
    {
        const auto signed_volume = geode::tetrahedron_signed_volume( tetra );
        double sq_len{ 0 };
        const auto& vertices = tetra.vertices();
        for( const auto v0 : geode::LRange{ 3 } )
        {
            const auto& point0 = vertices[v0].get();
            for( const auto v1 : geode::LRange{ v0, 4 } )
            {
                const auto& point1 = vertices[v1].get();
                for( const auto d : geode::LRange{ 3 } )
                {
                    const auto diff = point0.value( d ) - point1.value( d );
                    sq_len += diff * diff;
                }
            }
        }
        const auto l_rms = std::sqrt( sq_len / 6 );
        return 6 * std::sqrt( 2 ) * signed_volume / ( l_rms * l_rms * l_rms );
    }
} // namespace geode
