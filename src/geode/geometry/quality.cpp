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
#include <geode/geometry/vector.hpp>

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
} // namespace geode
