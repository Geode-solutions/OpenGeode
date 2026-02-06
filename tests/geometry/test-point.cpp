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

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

void test_comparison()
{
    const geode::Point3D p{ { 2, 1.0, 2.6 } };
    const geode::Point3D p2 = p;

    OPENGEODE_EXCEPTION( p == p2, "[Test] Points should be equal" );
    const geode::Point2D P{ { 15, 2.6 } };
    const geode::Point2D P2{ { 16, 2.6 } };
    OPENGEODE_EXCEPTION( P != P2, "[Test] Points should be different" );

    const geode::Point3D p_epsilon{ { 2.0000000001, 1, 2.6 } };
    OPENGEODE_EXCEPTION(
        p.inexact_equal( p_epsilon ), "[Test] Points should be almost equal" );
}

void test_operators()
{
    const geode::Point3D p{ { 2, 1.0, 2.6 } };
    const geode::Point3D p2 = p;
    const geode::Point3D answer{ { 4, 2, 5.2 } };
    OPENGEODE_EXCEPTION( p + p2 == answer, "[Test] Points should be equal" );
    OPENGEODE_EXCEPTION( p * 2 == answer, "[Test] Points should be equal" );
    OPENGEODE_EXCEPTION(
        p - p2 == geode::Point3D{}, "[Test] Points should be equal" );
    OPENGEODE_EXCEPTION( answer / 2 == p, "[Test] Points should be equal" );
}

void test_interpolation()
{
    geode::AttributeManager manager;
    manager.resize( 10 );
    auto attribute = manager.find_or_create_attribute< geode::VariableAttribute,
        geode::Point< 3 > >(
        "point_3", geode::Point3D{ { 10., 11., 12. } }, { false, true } );
    OPENGEODE_EXCEPTION( attribute->default_value().value( 0 ) == 10.,
        "[Test] Wrong default value" );
    OPENGEODE_EXCEPTION( attribute->default_value().value( 1 ) == 11.,
        "[Test] Wrong default value" );
    OPENGEODE_EXCEPTION( attribute->default_value().value( 2 ) == 12.,
        "[Test] Wrong default value" );
    attribute->set_value( 3, geode::Point3D{ { 1., 2., 3. } } );
    attribute->set_value( 7, geode::Point3D{ { 2., 5., 7. } } );
    manager.interpolate_attribute_value( { { 1, 7 }, { 0.5, 0.3 } }, 4 );

    OPENGEODE_EXCEPTION(
        attribute->value( 3 ).inexact_equal( geode::Point3D{ { 1., 2., 3. } } ),
        "[Test] Value 3 Should be equal to [ 1., 2., 3. ], not ",
        attribute->value( 3 ).string() );
    OPENGEODE_EXCEPTION( attribute->value( 4 ).inexact_equal(
                             geode::Point3D{ { 5.6, 7., 8.1 } } ),
        "[Test] Value 4 Should be equal to [ 5.6, 7., 8.1 ], not ",
        attribute->value( 4 ).string() );
    OPENGEODE_EXCEPTION(
        attribute->value( 6 ).inexact_equal( geode::Point3D{ { 10, 11, 12 } } ),
        "[Test] Value 6 Should be equal to [ 10, 11, 12 ], not ",
        attribute->value( 6 ).string() );
    OPENGEODE_EXCEPTION(
        attribute->value( 7 ).inexact_equal( geode::Point3D{ { 2., 5., 7. } } ),
        "[Test] Value 7 Should be equal to [ 2., 5., 7. ], not ",
        attribute->value( 7 ).string() );
}

void test()
{
    test_comparison();
    test_operators();
    test_interpolation();
}

OPENGEODE_TEST( "point" )