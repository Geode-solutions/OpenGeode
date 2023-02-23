/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/image/core/greyscale_color.h>
#include <geode/image/core/rgb_color.h>

#include <geode/tests/common.h>

void test_comparison()
{
    const geode::RGBColor color1{ 3, 254, 68 };
    const geode::RGBColor color2 = color1;

    OPENGEODE_EXCEPTION( color1 == color2, "[Test] Colors should be the same" );
    const geode::RGBColor color3{ 4, 254, 68 };
    OPENGEODE_EXCEPTION(
        color1 != color3, "[Test] Colors should be different" );
}

void test_color_attribute()
{
    geode::AttributeManager manager;
    manager.resize( 1 );
    const auto rgb_attribute =
        manager.find_or_create_attribute< geode::VariableAttribute,
            geode::RGBColor >( "rgb_color", geode::RGBColor{} );
    rgb_attribute->set_value( 0, { 3, 254, 68 } );
    const auto greyscale_attribute =
        manager.find_or_create_attribute< geode::VariableAttribute,
            geode::GreyscaleColor >(
            "greyscale_color", geode::GreyscaleColor{} );
    greyscale_attribute->set_value( 0, { 67 } );
    OPENGEODE_EXCEPTION( rgb_attribute->is_genericable(),
        "[TEST] Attribute on RGBColor should be genericable." );
    OPENGEODE_EXCEPTION( greyscale_attribute->is_genericable(),
        "[TEST] Attribute on GreyScale should be genericable." );
    OPENGEODE_EXCEPTION(
        rgb_attribute->generic_value( 0 ) == 3, "[TEST] Wrong generic value" );
    OPENGEODE_EXCEPTION( greyscale_attribute->generic_value( 0 ) == 67,
        "[TEST] Wrong generic value" );
}

void test()
{
    test_comparison();
    test_color_attribute();
}

OPENGEODE_TEST( "colors" )