/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/logger.h>
#include <geode/basic/point.h>

void test_comparison()
{
    geode::Point3D p{ { 2, 1.0, 2.6 } };
    geode::Point3D p2 = p;

    OPENGEODE_EXCEPTION( p == p2, "[Test] Points should be equal" );
    geode::Point2D P{ { 15, 2.6 } };
    geode::Point2D P2{ { 16, 2.6 } };
    OPENGEODE_EXCEPTION( P != P2, "[Test] Points should be different" );

    geode::Point3D p_epsilon{ { 2.0000000001, 1, 2.6 } };
    OPENGEODE_EXCEPTION( p.inexact_equal( p_epsilon, 0.0001 ),
        "[Test] Points should be almost equal" );
}

void test_operators()
{
    geode::Point3D p{ { 2, 1.0, 2.6 } };
    geode::Point3D p2 = p;
    geode::Point3D answer{ { 4, 2, 5.2 } };
    OPENGEODE_EXCEPTION( p + p2 == answer, "[Test] Points should be equal" );
    OPENGEODE_EXCEPTION( p * 2 == answer, "[Test] Points should be equal" );
    OPENGEODE_EXCEPTION(
        p - p2 == geode::Point3D{}, "[Test] Points should be equal" );
    OPENGEODE_EXCEPTION( answer / 2 == p, "[Test] Points should be equal" );
}

int main()
{
    using namespace geode;

    try
    {
        test_comparison();
        test_operators();

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
