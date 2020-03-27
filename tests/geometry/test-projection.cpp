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

#include <geode/geometry/point.h>

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/projection.h>

int main()
{
    try
    {
        // Tests for 2D cases
        const geode::Point2D a{ { 1.0, 5.0 } };
        const geode::Point2D b{ { -1.0, -5.0 } };
        const geode::Segment2D segment2D{ a, b };

        geode::Point2D closest_point;

        closest_point = geode::point_segment_projection( a, segment2D );
        OPENGEODE_EXCEPTION( closest_point == a,
            "[Test] Wrong result for point_segment_projection with query point "
            "a" );

        closest_point = geode::point_segment_projection( b, segment2D );
        OPENGEODE_EXCEPTION( closest_point == b,
            "[Test] Wrong result for point_segment_projection with query point "
            "b" );

        const geode::Point2D q1{ { 0.0, 0.0 } };
        closest_point = geode::point_segment_projection( q1, segment2D );
        OPENGEODE_EXCEPTION( closest_point == q1,
            "[Test] Wrong result for point_segment_projection with query point "
            "q1" );

        const geode::Point2D q2{ { 10.0, 10.0 } };
        closest_point = geode::point_segment_projection( q2, segment2D );
        OPENGEODE_EXCEPTION( closest_point == a,
            "[Test] Wrong result for point_segment_projection with query point "
            "q2" );

        const geode::Point2D q3{ { 5.0, -1.0 } };
        closest_point = geode::point_segment_projection( q3, segment2D );
        const geode::Point2D result_q3{ { 0.0, 0.0 } };
        OPENGEODE_EXCEPTION( closest_point == result_q3,
            "[Test] Wrong result for point_segment_projection with query point "
            "q3" );

        const geode::Point2D q4{ { 5.5, 1.5 } };
        closest_point = geode::point_segment_projection( q4, segment2D );
        const geode::Point2D result_q4{ { 0.5, 2.5 } };
        OPENGEODE_EXCEPTION( closest_point == result_q4,
            "[Test] Wrong result for point_segment_projection with query point "
            "q3" );

        // Tests for 3D cases
        // TODO

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
