/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/rotation.h>

namespace
{
    std::array< geode::Vector< 4 >, 4 > rotation_matrix(
        const geode::Vector< 4 >& quaternion )
    {
        std::array< geode::Vector< 4 >, 4 > matrix;
        matrix[0].set_value( 0,
            1
                - 2.0
                      * ( quaternion.value( 1 ) * quaternion.value( 1 )
                          + quaternion.value( 2 ) * quaternion.value( 2 ) ) );
        matrix[1].set_value(
            0, 2.0
                   * ( quaternion.value( 0 ) * quaternion.value( 1 )
                       + quaternion.value( 2 ) * quaternion.value( 3 ) ) );
        matrix[2].set_value(
            0, 2.0
                   * ( quaternion.value( 2 ) * quaternion.value( 0 )
                       - quaternion.value( 1 ) * quaternion.value( 3 ) ) );
        matrix[3].set_value( 0, 0.0 );

        matrix[0].set_value(
            1, 2.0
                   * ( quaternion.value( 0 ) * quaternion.value( 1 )
                       - quaternion.value( 2 ) * quaternion.value( 3 ) ) );
        matrix[1].set_value( 1,
            1
                - 2.0
                      * ( quaternion.value( 2 ) * quaternion.value( 2 )
                          + quaternion.value( 0 ) * quaternion.value( 0 ) ) );
        matrix[2].set_value(
            1, 2.0
                   * ( quaternion.value( 1 ) * quaternion.value( 2 )
                       + quaternion.value( 0 ) * quaternion.value( 3 ) ) );
        matrix[3].set_value( 1, 0.0 );

        matrix[0].set_value(
            2, 2.0
                   * ( quaternion.value( 2 ) * quaternion.value( 0 )
                       + quaternion.value( 1 ) * quaternion.value( 3 ) ) );
        matrix[1].set_value(
            2, 2.0
                   * ( quaternion.value( 1 ) * quaternion.value( 2 )
                       - quaternion.value( 0 ) * quaternion.value( 3 ) ) );
        matrix[2].set_value( 2,
            1
                - 2.0
                      * ( quaternion.value( 1 ) * quaternion.value( 1 )
                          + quaternion.value( 0 ) * quaternion.value( 0 ) ) );
        matrix[3].set_value( 2, 0.0 );

        matrix[0].set_value( 3, 0.0 );
        matrix[1].set_value( 3, 0.0 );
        matrix[2].set_value( 3, 0.0 );
        matrix[3].set_value( 3, 1.0 );

        return matrix;
    }
} // namespace

namespace geode
{
    Point3D opengeode_geometry_api rotate(
        const Point3D& point, const Vector3D& axis, double angle )
    {
        OPENGEODE_EXCEPTION( axis.length2() > geode::global_epsilon2,
            "[rotate] Given axis is almost null" );
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
        const Vector< 4 > quat_point(
            { point.value( 0 ), point.value( 1 ), point.value( 2 ), 1.0 } );
        Vector< 4 > result;
        for( const auto i : geode::Range{ 4 } )
        {
            result.set_value( i, matrix[i].dot( quat_point ) );
        }

        OPENGEODE_ASSERT( std::fabs( result.value( 3 ) ) > global_epsilon,
            "[rotate] Error in computation" );
        const auto inverse = 1.0 / result.value( 3 );
        return { { result.value( 0 ) * inverse, result.value( 1 ) * inverse,
            result.value( 2 ) * inverse } };
    }
} // namespace geode
