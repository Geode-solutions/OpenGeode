/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <cmath>

#include <geode/geometry/rotation.h>

namespace
{
    std::array< std::array< double, 4 >, 4 > rotation_matrix(
        const geode::Vector< 4 >& quaternion )
    {
        std::array< std::array< double, 4 >, 4 > matrix;
        matrix[0][0] =
            1
            - 2.0
                  * ( quaternion.value( 1 ) * quaternion.value( 1 )
                      + quaternion.value( 2 ) * quaternion.value( 2 ) );
        matrix[1][0] = 2.0
                       * ( quaternion.value( 0 ) * quaternion.value( 1 )
                           + quaternion.value( 2 ) * quaternion.value( 3 ) );
        matrix[2][0] = 2.0
                       * ( quaternion.value( 2 ) * quaternion.value( 0 )
                           - quaternion.value( 1 ) * quaternion.value( 3 ) );
        matrix[3][0] = 0.0;

        matrix[0][1] = 2.0
                       * ( quaternion.value( 0 ) * quaternion.value( 1 )
                           - quaternion.value( 2 ) * quaternion.value( 3 ) );
        matrix[1][1] =
            1
            - 2.0
                  * ( quaternion.value( 2 ) * quaternion.value( 2 )
                      + quaternion.value( 0 ) * quaternion.value( 0 ) );
        matrix[2][1] = 2.0
                       * ( quaternion.value( 1 ) * quaternion.value( 2 )
                           + quaternion.value( 0 ) * quaternion.value( 3 ) );
        matrix[3][1] = 0.0;

        matrix[0][2] = 2.0
                       * ( quaternion.value( 2 ) * quaternion.value( 0 )
                           + quaternion.value( 1 ) * quaternion.value( 3 ) );
        matrix[1][2] = 2.0
                       * ( quaternion.value( 1 ) * quaternion.value( 2 )
                           - quaternion.value( 0 ) * quaternion.value( 3 ) );
        matrix[2][2] =
            1
            - 2.0
                  * ( quaternion.value( 1 ) * quaternion.value( 1 )
                      + quaternion.value( 0 ) * quaternion.value( 0 ) );
        matrix[3][2] = 0.0;

        matrix[0][3] = 0.0;
        matrix[1][3] = 0.0;
        matrix[2][3] = 0.0;
        matrix[3][3] = 1.0;

        return matrix;
    }
} // namespace

namespace geode
{
    Vector3D opengeode_geometry_api rotate(
        const Vector3D& vector, const Vector3D& axis, double angle )
    {
        if( axis.length2() < geode::global_epsilon2 )
        {
            OPENGEODE_EXCEPTION( axis.length2() < geode::global_epsilon2,
                "[rotate] Given axis is almost null" );
        }
        while( angle < 0 )
        {
            angle += 2 * M_PI;
        }
        while( angle >= 2 * M_PI )
        {
            angle -= 2 * M_PI;
        }

        const auto base = axis.normalize() * std::sin( 0.5 * angle );
        const geode::Vector< 4 > quaternion( { base.value( 0 ), base.value( 1 ),
            base.value( 2 ), std::cos( 0.5 * angle ) } );

        const auto matrix = rotation_matrix( quaternion );
        const Vector< 4 > quat_vector(
            { vector.value( 0 ), vector.value( 1 ), vector.value( 2 ), 1.0 } );
        Vector< 4 > result;
        for( const auto i : geode::Range{ 4 } )
        {
            double coord{ 0. };
            for( const auto j : geode::Range{ 4 } )
            {
                coord += matrix[i][j] * quat_vector.value( j );
            }
            result.set_value( i, coord );
        }

        OPENGEODE_ASSERT( std::fabs( result.value( 3 ) ) > global_epsilon,
            "[rotate] Error in computation" );
        const auto inverse = 1.0 / result.value( 3 );
        return { { result.value( 0 ) * inverse, result.value( 1 ) * inverse,
            result.value( 2 ) * inverse } };
    }
} // namespace geode
