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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>

#include <geode/geometry/point.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects.h>

template < geode::index_t nb_coords >
bool check_bary_coords( const std::array< double, nb_coords >& result,
    const std::array< double, nb_coords >& answer )
{
    double sum{ 0 };
    for( const auto i : geode::Range{ nb_coords } )
    {
        if( result[i] != answer[i] )
        {
            geode::Logger::error( "Lambda ", i, " is incorrect." );
            geode::Logger::error(
                "Result = ", result[i], " - Answer = ", answer[i] );
            return false;
        }
        sum += result[i];
    }
    if( std::fabs( sum - 1 ) > geode::global_epsilon )
    {
        geode::Logger::error( "Sum of all lambdas are different of 1" );
        return false;
    }
    return true;
}

void test_segment_barycentric_coordinates()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 2.0, 3.0 } };
    const geode::Segment3D segment{ a, b };

    OPENGEODE_EXCEPTION(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( a, segment ), { 1, 0 } ),
        "[Test] Wrong result for segment_barycentric_coordinates "
        "with query point a" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( b, segment ), { 0, 1 } ),
        "[Test] Wrong result for segment_barycentric_coordinates "
        "with query point b" );

    const geode::Point3D q1{ { 0.25, 0.50, 0.75 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( q1, segment ),
            { 0.75, 0.25 } ),
        "[Test] Wrong result for segment_barycentric_coordinates "
        "with query point q1" );

    const geode::Point3D q2{ { 2.0, 4.0, 6.0 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( q2, segment ), { -1, 2 } ),
        "[Test] Wrong result for segment_barycentric_coordinates "
        "with query point q2" );
}

void test_triangle_barycentric_coordinates_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( a, triangle2D ),
            { 1, 0, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_2D "
        "with query point a" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( b, triangle2D ),
            { 0, 1, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_2D "
        "with query point b" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( c, triangle2D ),
            { 0, 0, 1 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_2D "
        "with query point c" );

    const geode::Point2D q1{ { 0.25, 0.0 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q1, triangle2D ),
            { 0.75, 0.25, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_2D "
        "with query point q1" );

    const geode::Point2D q2{ { 0.5, 0.25 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q2, triangle2D ),
            { 0.5, 0.25, 0.25 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_2D "
        "with query point q2" );

    const geode::Point2D q3{ { 0.0, 1.0 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q3, triangle2D ),
            { 1, -1, 1 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_2D "
        "with query point q3" );
}

void test_triangle_barycentric_coordinates_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 1.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };

    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( a, triangle3D ),
            { 1, 0, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_3D "
        "with query point a" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( b, triangle3D ),
            { 0, 1, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_3D "
        "with query point b" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( c, triangle3D ),
            { 0, 0, 1 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_3D "
        "with query point c" );

    const geode::Point3D q1{ { 1, 0.5, 0.5 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q1, triangle3D ),
            { 0, 0.5, 0.5 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_3D "
        "with query point q1" );

    const geode::Point3D q2{ { 0.5, 0.25, 0.25 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q2, triangle3D ),
            { 0.5, 0.25, 0.25 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_3D "
        "with query point q2" );

    const geode::Point3D q3{ { 2.0, 1.0, 1.0 } };
    OPENGEODE_EXCEPTION(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q3, triangle3D ),
            { -1, 1, 1 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates_3D "
        "with query point q3" );
}

void test_triangle_barycentric_coordinates()
{
    test_triangle_barycentric_coordinates_2d();
    test_triangle_barycentric_coordinates_3d();
}

void test_tetra_barycentric_coordinates()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 0.0, 0.0, 1.0 } };
    const geode::Tetra tetra{ a, b, c, d };

    OPENGEODE_EXCEPTION(
        check_bary_coords< 4 >(
            geode::tetra_barycentric_coordinates( a, tetra ), { 1, 0, 0, 0 } ),
        "[Test] Wrong result for tetra_barycentric_coordinates "
        "with query point a" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 4 >(
            geode::tetra_barycentric_coordinates( b, tetra ), { 0, 1, 0, 0 } ),
        "[Test] Wrong result for tetra_barycentric_coordinates "
        "with query point b" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 4 >(
            geode::tetra_barycentric_coordinates( c, tetra ), { 0, 0, 1, 0 } ),
        "[Test] Wrong result for tetra_barycentric_coordinates "
        "with query point c" );

    OPENGEODE_EXCEPTION(
        check_bary_coords< 4 >(
            geode::tetra_barycentric_coordinates( d, tetra ), { 0, 0, 0, 1 } ),
        "[Test] Wrong result for tetra_barycentric_coordinates "
        "with query point d" );

    const geode::Point3D q1{ { 0.25, 0.25, 0.25 } };
    OPENGEODE_EXCEPTION( check_bary_coords< 4 >(
                             geode::tetra_barycentric_coordinates( q1, tetra ),
                             { 0.25, 0.25, 0.25, 0.25 } ),
        "[Test] Wrong result for tetra_barycentric_coordinates "
        "with query point q1" );

    const geode::Point3D q2{ { 0.25, 1, -1 } };
    OPENGEODE_EXCEPTION( check_bary_coords< 4 >(
                             geode::tetra_barycentric_coordinates( q2, tetra ),
                             { 0.75, 0.25, 1, -1 } ),
        "[Test] Wrong result for tetra_barycentric_coordinates "
        "with query point q2" );
}

int main()
{
    try
    {
        test_segment_barycentric_coordinates();
        test_triangle_barycentric_coordinates();
        test_tetra_barycentric_coordinates();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
