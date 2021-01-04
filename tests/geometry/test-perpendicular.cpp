/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/geometry/perpendicular.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <geode/tests/common.h>

void test_perpendicular()
{
    geode::Vector2D v{ { 1.578, 1e-10 } };
    const auto perp = geode::perpendicular( v ).normalize();

    const auto dot_product =
        v.value( 0 ) * perp.value( 0 ) + v.value( 1 ) * perp.value( 1 );

    OPENGEODE_EXCEPTION( perp.length() == 1 && dot_product == 0.,
        "[Test] Wrong result for normalized_perpendicular" );
}

void test_dot_perpendicular()
{
    const geode::Vector2D v1{ { 0, 1 } };
    const geode::Vector2D v2{ { 1, 0 } };
    const auto dot_perp = geode::dot_perpendicular( v1, v2 );
    const auto dot = geode::dot_perpendicular( geode::perpendicular( v1 ), v2 );

    OPENGEODE_EXCEPTION( dot_perp == -1 && dot == 0,
        "[Test] Wrong result for dot_perpendicular" );
}

void test()
{
    test_perpendicular();
    test_dot_perpendicular();
}

OPENGEODE_TEST( "perpendicular" )
