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

#include <geode/basic/logger.h>

#include <geode/geometry/vector.h>

#include <geode/tests/common.h>

void test_length()
{
    const geode::Vector3D p{ { 1, 2, 4 } };
    OPENGEODE_EXCEPTION(
        p.length() == std::sqrt( 1 + 4 + 16 ), "[Test] Wrong vector length" );

    const auto p_normalized = p.normalize();
    OPENGEODE_EXCEPTION(
        p_normalized.length() == 1, "[Test] Wrong vector length" );
}

void test_operations()
{
    const geode::Vector3D unit{ { 0, 2, 0 } };
    const geode::Vector3D p{ { 1, 2, 4 } };
    OPENGEODE_EXCEPTION( p.dot( unit ) == 4, "[Test] Wrong dot product" );

    const auto cross_unit =
        geode::Vector3D{ { 1, 0, 0 } }.cross( geode::Vector3D{ { 0, 1, 0 } } );
    const geode::Vector3D answer_unit{ { 0, 0, 1 } };
    OPENGEODE_EXCEPTION(
        cross_unit == answer_unit, "[Test] Wrong unit cross product" );

    const geode::Vector3D answer{ { -8, 0, 2 } };
    OPENGEODE_EXCEPTION(
        p.cross( unit ) == answer, "[Test] Wrong cross product" );
}

void test()
{
    test_length();
    test_operations();
}

OPENGEODE_TEST( "vector" )