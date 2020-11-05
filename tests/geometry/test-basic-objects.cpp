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

#include <geode/tests/common.h>

void test_segment()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment{ a, b };

    auto segment2 = segment;

    OPENGEODE_EXCEPTION(
        segment2.vertices()[0].get() == a && segment2.vertices()[1].get() == b,
        "[Test] Wrong result for segment2" );

    geode::Segment2D segment3{ b, a };
    geode::Segment2D segment4{ b, a };
    segment3 = segment4;
    segment4 = std::move( segment2 );

    OPENGEODE_EXCEPTION(
        segment3.vertices()[0].get() == a && segment3.vertices()[1].get() == b,
        "[Test] Wrong result for segment3" );

    OPENGEODE_EXCEPTION(
        segment4.vertices()[0].get() == a && segment4.vertices()[1].get() == b,
        "[Test] Wrong result for segment4" );
}

void test()
{
    test_segment();
}

OPENGEODE_TEST( "basic-object" )
