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

#include <geode/geometry/rotation.hpp>

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
        const auto axis_length = axis.length();
        OpenGeodeGeometryException::check_exception(
            axis_length > 0 && std::isfinite( axis_length ), point,
            OpenGeodeBasicException::TYPE::data,
            "[rotate] Given axis is null or not finite" );
        // No angle normalization needed: the rotation matrix is quadratic in
        // the quaternion, so q and -q (angle + 2*PI) give the same rotation.
        const auto base = axis / axis_length * std::sin( 0.5 * angle );
        const geode::Vector< 4 > quaternion( { base.value( 0 ), base.value( 1 ),
            base.value( 2 ), std::cos( 0.5 * angle ) } );

        const auto matrix = rotation_matrix( quaternion );
        const Vector< 4 > quat_point(
            { point.value( 0 ), point.value( 1 ), point.value( 2 ), 1.0 } );
        // The last matrix row is (0, 0, 0, 1): the homogeneous coordinate
        // stays 1 and no division is needed.
        Point3D result;
        for( const auto i : geode::LRange{ 3 } )
        {
            result.set_value( i, matrix[i].dot( quat_point ) );
        }
        return result;
    }
} // namespace geode
