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

#include <geode/geometry/vector.h>

#include <geode/geometry/rotation.h>

#include <geode/tests/common.h>

void test()
{
    geode::Vector3D input( { 0., 0., 3. } );
    geode::Vector3D axis( { 2., 0., 0. } );
    const auto result1 = geode::rotate( input, axis, M_PI_2 );
    geode::Vector3D answer1( { 0., -3., 0. } );
    OPENGEODE_EXCEPTION(
        result1.inexact_equal( answer1, geode::global_epsilon ),
        "[Test] Wrong result for configuration 1" );

    const auto result2 = geode::rotate( input, axis, -M_PI_2 );
    geode::Vector3D answer2( { 0., 3., 0. } );
    OPENGEODE_EXCEPTION(
        result2.inexact_equal( answer2, geode::global_epsilon ),
        "[Test] Wrong result for configuration 2" );

    const auto result3 = geode::rotate( input, axis, 5 * M_PI_2 );
    geode::Vector3D answer3( { 0., -3., 0. } );
    OPENGEODE_EXCEPTION(
        result3.inexact_equal( answer3, geode::global_epsilon ),
        "[Test] Wrong result for configuration 2" );
}

OPENGEODE_TEST( "rotation" )
