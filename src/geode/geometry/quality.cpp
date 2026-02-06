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

#include <geode/geometry/quality.hpp>

#include <limits>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/square_matrix.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/basic/detail/disable_debug_logger.hpp>

namespace
{
    template < geode::index_t dimension >
    double compute_angle( const geode::Point< dimension >& point,
        const geode::Point< dimension >& point_prev,
        const geode::Point< dimension >& point_next )
    {
        const auto prev =
            geode::Vector< dimension >{ point, point_prev }.normalize();
        const auto next =
            geode::Vector< dimension >{ point, point_next }.normalize();
        const auto dot = std::clamp( prev.dot( next ), -1.0, 1.0 );
        const auto angle = std::acos( dot );
        if( std::isnan( angle ) )
        {
            return 0;
        }
        return angle;
    }

    std::array< geode::local_index_t, 3 > lu_decomposition(
        geode::SquareMatrix3D& matrix )
    {
        std::array< geode::local_index_t, 3 > indices{ 0, 0, 0 };
        double inverse_biggest[3];
        for( const auto i : geode::LRange{ 3 } )
        {
            double biggest{ 0.0 };
            for( const auto j : geode::LRange{ 3 } )
            {
                const auto t = std::fabs( matrix.value( i, j ) );
                if( biggest < t )
                    biggest = t;
            }
            if( biggest == 0.0 )
            {
                return indices;
            }
            inverse_biggest[i] = 1.0 / biggest;
            indices[i] = i;
        }
        geode::local_index_t pivot_id{ 0 };
        for( const auto k : geode::LRange{ 2 } )
        {
            double biggest{ 0.0 };
            for( const auto i : geode::LRange{ k, 3 } )
            {
                const double t = std::fabs( matrix.value( indices[i], k ) )
                                 * inverse_biggest[indices[i]];
                if( biggest < t )
                {
                    biggest = t;
                    pivot_id = i;
                }
            }
            if( biggest == 0.0 )
            {
                return indices;
            }
            if( pivot_id != k )
            {
                const int j = indices[k];
                indices[k] = indices[pivot_id];
                indices[pivot_id] = j;
            }
            const auto pivot = matrix.value( indices[k], k );
            for( const auto i : geode::LRange{ k + 1, 3 } )
            {
                const double m = matrix.value( indices[i], k ) / pivot;
                matrix.set_value( indices[i], k, m );
                if( m != 0.0 )
                {
                    for( const auto j : geode::LRange{ k + 1, 3 } )
                    {
                        matrix.set_value( indices[i], j,
                            matrix.value( indices[i], j )
                                - m * matrix.value( indices[k], j ) );
                    }
                }
            }
        }
        return indices;
    }
    std::array< double, 3 > lu_solving( const geode::SquareMatrix3D& lu_matrix,
        const std::array< double, 3 >& rhs,
        const std::array< geode::local_index_t, 3 >& indices )
    {
        std::array< double, 3 > result{ 0., 0., 0. };
        for( const auto i : geode::LRange{ 3 } )
        {
            double s{ 0.0 };
            for( const auto j : geode::LRange{ 0, i } )
            {
                s += lu_matrix.value( indices[i], j ) * result[j];
            }
            result[i] = rhs[indices[i]] - s;
        }
        for( const auto i :
            geode::LReverseRange{ 3 } ) // for(  int i = 2; i >= 0; --i )
        {
            double s{ 0.0 };
            for( const auto j : geode::LRange{ i + 1, 3 } )
            {
                s += lu_matrix.value( indices[i], j ) * result[j];
            }
            result[i] = ( result[i] - s ) / lu_matrix.value( indices[i], i );
        }

        return result;
    }

} // namespace

namespace geode
{
    // Implementation inspired from https://github.com/sandialabs/verdict
    double tetrahedron_aspect_ratio( const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        const Vector3D edge01{ vertices[0], vertices[1] };
        const Vector3D edge02{ vertices[0], vertices[2] };
        const Vector3D edge03{ vertices[0], vertices[3] };
        const auto absolute_det =
            std::fabs( edge01.dot( edge02.cross( edge03 ) ) );
        if( absolute_det < GLOBAL_EPSILON )
        {
            return std::numeric_limits< double >::max();
        }
        const Vector3D edge12{ vertices[1], vertices[2] };
        const Vector3D edge13{ vertices[1], vertices[3] };
        const Vector3D edge23{ vertices[2], vertices[3] };
        const auto longest_edge_length = std::sqrt(
            std::max( { edge01.length2(), edge12.length2(), edge02.length2(),
                edge03.length2(), edge13.length2(), edge23.length2() } ) );
        const auto total_area2 =
            edge01.cross( edge12 ).length() + edge01.cross( edge03 ).length()
            + edge02.cross( edge03 ).length() + edge12.cross( edge23 ).length();
        const auto constant = std::sqrt( 6 ) / 12.;
        const auto aspect_ratio =
            constant * longest_edge_length * total_area2 / absolute_det;
        return aspect_ratio;
    }

    double tetrahedron_volume_to_facet_ratio( const Tetrahedron& tetra )
    {
        const auto signed_volume = tetrahedron_signed_volume( tetra );
        if( signed_volume < 0. )
        {
            return 0.;
        }
        const auto& vertices = tetra.vertices();
        const Vector3D edge01{ vertices[0], vertices[1] };
        const Vector3D edge02{ vertices[0], vertices[2] };
        const Vector3D edge03{ vertices[0], vertices[3] };
        const Vector3D edge12{ vertices[1], vertices[2] };
        const Vector3D edge13{ vertices[1], vertices[3] };
        const Vector3D edge23{ vertices[2], vertices[3] };
        const auto max_length = std::sqrt(
            std::max( { edge01.length2(), edge12.length2(), edge02.length2(),
                edge03.length2(), edge13.length2(), edge23.length2() } ) );
        double tetra_area{ 0 };
        tetra_area += edge01.cross( edge02 ).length();
        tetra_area += edge01.cross( edge03 ).length();
        tetra_area += edge02.cross( edge03 ).length();
        tetra_area += edge12.cross( edge13 ).length();
        tetra_area /= 2.;
        return 6. * std::sqrt( 6 ) * signed_volume
               / ( tetra_area * max_length );
    }

    double tetrahedron_volume_to_edge_ratio( const Tetrahedron& tetra )
    {
        const auto signed_volume = tetrahedron_signed_volume( tetra );
        if( signed_volume < 0. )
        {
            return 0.;
        }
        const auto& vertices = tetra.vertices();
        const Vector3D edge01{ vertices[0], vertices[1] };
        const Vector3D edge02{ vertices[0], vertices[2] };
        const Vector3D edge03{ vertices[0], vertices[3] };
        const Vector3D edge12{ vertices[1], vertices[2] };
        const Vector3D edge13{ vertices[1], vertices[3] };
        const Vector3D edge23{ vertices[2], vertices[3] };
        const auto sq_len = edge01.length2() + edge02.length2()
                            + edge03.length2() + edge12.length2()
                            + edge13.length2() + edge23.length2();
        const auto l_rms = std::sqrt( sq_len / 6 );
        return 6 * std::sqrt( 2 ) * signed_volume / ( l_rms * l_rms * l_rms );
    }

    double tetrahedron_collapse_aspect_ratio( const Tetrahedron& tetra )
    {
        if( tetrahedron_volume( tetra ) < GLOBAL_EPSILON )
        {
            return std::numeric_limits< double >::max();
        }
        const auto& vertices = tetra.vertices();
        const Vector3D edge01{ vertices[0], vertices[1] };
        const Vector3D edge02{ vertices[0], vertices[2] };
        const Vector3D edge03{ vertices[0], vertices[3] };
        const Vector3D edge12{ vertices[1], vertices[2] };
        const Vector3D edge13{ vertices[1], vertices[3] };
        const Vector3D edge23{ vertices[2], vertices[3] };
        const auto longest_edge_length = std::sqrt(
            std::max( { edge01.length2(), edge12.length2(), edge02.length2(),
                edge03.length2(), edge13.length2(), edge23.length2() } ) );

        SquareMatrix3D matrix{ { edge03, edge13, edge23 } };
        const auto row_indices = lu_decomposition( matrix );

        std::array< Vector3D, 4 > N;
        for( const auto j : LRange{ 3 } )
        {
            std::array< double, 3 > b{ 0., 0., 0. };
            b[j] = 1.0; // Positive means the inside direction
            N[j] = Vector3D{ lu_solving( matrix, b, row_indices ) };
        }
        N[3] = ( N[0] + N[1] + N[2] ) * -1.;

        std::array< double, 4 >
            H; // H[i] = inverse of the height of its corresponding face
        for( const auto i : LRange{ 4 } )
        {
            H[i] = N[i].length();
        }

        auto heightinv = H[0];
        for( const auto i : LRange{ 1, 4 } )
        { // Get the biggest H[i] (corresponding to the smallest height)
            if( H[i] > heightinv )
            {
                heightinv = H[i];
            }
        }
        return ( longest_edge_length * heightinv ) / std::sqrt( 3. / 2. );
    }

    template < index_t dimension >
    double triangle_angle_based_quality( const Triangle< dimension >& triangle )
    {
        try
        {
            const auto& vertices = triangle.vertices();
            std::array< double, 3 > sinus;
            for( const auto v : LRange{ 3 } )
            {
                const auto point = vertices[v].get();
                const auto point_prev = vertices[( v + 2 ) % 3].get();
                const auto point_next = vertices[( v + 1 ) % 3].get();
                const auto angle =
                    compute_angle( point, point_prev, point_next );
                DEBUG( angle );
                sinus[v] = std::sin( angle );
                DEBUG( sinus[v] );
            }
            const auto denominator = sinus[0] + sinus[1] + sinus[2];
            if( denominator == 0 )
            {
                return 0;
            }
            const auto quality =
                4 * sinus[0] * sinus[1] * sinus[2] / ( denominator );
            DEBUG( quality );
            return quality;
        }
        catch( ... )
        {
            return 0;
        }
    }

    template opengeode_geometry_api double triangle_angle_based_quality< 2 >(
        const Triangle2D& );
    template opengeode_geometry_api double triangle_angle_based_quality< 3 >(
        const Triangle3D& );
} // namespace geode

#include <geode/basic/detail/enable_debug_logger.hpp>