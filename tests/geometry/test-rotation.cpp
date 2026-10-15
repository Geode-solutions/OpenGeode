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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/vector.hpp>

#include <geode/geometry/rotation.hpp>

#include <geode/tests/common.hpp>

void test_rotation_robustness()
{
    const geode::Point3D input{ { 1., 2., 3. } };
    const geode::Vector3D axis{ { 1., 1., 0. } };
    const auto huge = geode::rotate( input, axis, 1e17 );
    geode::OpenGeodeGeometryException::test(
        std::fabs( geode::Vector3D{ huge }.length()
                   - geode::Vector3D{ input }.length() )
                <= geode::GLOBAL_EPSILON
            && std::fabs( geode::Vector3D{ huge }.dot( axis )
                          - geode::Vector3D{ input }.dot( axis ) )
                   <= geode::GLOBAL_EPSILON,
        "Wrong result for huge angle rotation" );
    try
    {
        [[maybe_unused]] const auto infinite = geode::rotate(
            input, axis, std::numeric_limits< double >::infinity() );
        geode::OpenGeodeGeometryException::test(
            false, "Infinite angle rotation should throw" );
    }
    catch( const geode::OpenGeodeGeometryException& )
    {
    }

    const geode::Vector3D small_axis{ { 1e-4, 0., 0. } };
    const geode::Vector3D z_axis{ { 0., 0., 3. } };
    const auto small_result = geode::rotate( z_axis, small_axis, M_PI / 2.0 );
    geode::OpenGeodeGeometryException::test(
        small_result.inexact_equal( geode::Point3D{ { 0., -3., 0. } } ),
        "Wrong result for rotation around a short axis" );

    const auto result = geode::rotate( input, axis, -7 * M_PI / 3.0 );
    const auto reference = geode::rotate( input, axis, 5 * M_PI / 3.0 );
    geode::OpenGeodeGeometryException::test( result.inexact_equal( reference ),
        "Wrong result for negative angle rotation" );
}

void test()
{
    const geode::Vector3D input( { 0., 0., 3. } );
    const geode::Vector3D axis( { 2., 0., 0. } );
    const auto result1 = geode::rotate( input, axis, M_PI / 2.0 );
    const geode::Vector3D answer1( { 0., -3., 0. } );
    geode::OpenGeodeGeometryException::test(
        result1.inexact_equal( answer1 ), "Wrong result for configuration 1" );

    const auto result2 = geode::rotate( input, axis, -M_PI / 2.0 );
    const geode::Vector3D answer2( { 0., 3., 0. } );
    geode::OpenGeodeGeometryException::test(
        result2.inexact_equal( answer2 ), "Wrong result for configuration 2" );

    const auto result3 = geode::rotate( input, axis, 5 * M_PI / 2.0 );
    const geode::Vector3D answer3( { 0., -3., 0. } );
    geode::OpenGeodeGeometryException::test(
        result3.inexact_equal( answer3 ), "Wrong result for configuration 2" );
    test_rotation_robustness();
}

OPENGEODE_TEST( "rotation" )
