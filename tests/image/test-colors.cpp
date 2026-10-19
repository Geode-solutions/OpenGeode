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

#include <absl/container/flat_hash_set.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/image/core/greyscale_color.hpp>
#include <geode/image/core/rgb_color.hpp>

#include <geode/tests/common.hpp>

void test_comparison()
{
    const geode::RGBColor color1{ 3, 254, 68 };
    const geode::RGBColor color2 = color1;

    geode::OpenGeodeImageException::test(
        color1 == color2, "Colors should be the same" );
    const geode::RGBColor color3{ 4, 254, 68 };
    geode::OpenGeodeImageException::test(
        color1 != color3, "Colors should be different" );
}

void test_rgb_addition()
{
    const geode::RGBColor color1{ 200, 100, 0 };
    const geode::RGBColor color2{ 100, 50, 255 };
    const geode::RGBColor average{ 150, 75, 127 };
    geode::OpenGeodeImageException::test( color1 + color2 == average,
        "[TEST] Wrong RGBColor addition: ", ( color1 + color2 ).string() );
    auto color3 = color1;
    color3 += color2;
    geode::OpenGeodeImageException::test( color3 == average,
        "[TEST] Wrong RGBColor addition assignment: ", color3.string() );
}

void test_greyscale_addition()
{
    const geode::GreyscaleColor one{ 1 };
    geode::OpenGeodeImageException::test( one + one == one,
        "[TEST] Wrong GreyscaleColor addition: ", ( one + one ).string() );
    const geode::GreyscaleColor white{ 255 };
    geode::OpenGeodeImageException::test( white + white == white,
        "[TEST] Wrong GreyscaleColor addition: ", ( white + white ).string() );
    auto color = white;
    color += geode::GreyscaleColor{ 1 };
    geode::OpenGeodeImageException::test( color == geode::GreyscaleColor{ 128 },
        "[TEST] Wrong GreyscaleColor addition assignment: ", color.string() );
}

void test_color_attribute()
{
    geode::AttributeManager manager;
    manager.resize( 1 );
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = true;
    attribute_properties.transferable = true;
    geode::AttributeValues< geode::RGBColor > attribute_values;
    attribute_values.default_value = geode::RGBColor{};
    attribute_values.no_value = geode::RGBColor{};
    const auto rgb_attribute_id =
        manager.create_attribute< geode::VariableAttribute, geode::RGBColor >(
            "rgb_color", attribute_values, attribute_properties );
    auto rgb_attribute =
        manager.find_attribute< geode::VariableAttribute, geode::RGBColor >(
            rgb_attribute_id );
    rgb_attribute->set_value( 0, { 3, 254, 68 } );
    geode::AttributeValues< geode::GreyscaleColor > grey_attribute_values;
    grey_attribute_values.default_value = geode::GreyscaleColor{};
    grey_attribute_values.no_value = geode::GreyscaleColor{};
    const auto greyscale_attribute_id =
        manager.create_attribute< geode::VariableAttribute,
            geode::GreyscaleColor >(
            "greyscale_color", grey_attribute_values, attribute_properties );
    auto greyscale_attribute = manager.find_attribute< geode::VariableAttribute,
        geode::GreyscaleColor >( greyscale_attribute_id );
    greyscale_attribute->set_value( 0, geode::GreyscaleColor{ 67 } );
    geode::OpenGeodeImageException::test( rgb_attribute->is_genericable(),
        "[TEST] Attribute on RGBColor should be genericable." );
    geode::OpenGeodeImageException::test( greyscale_attribute->is_genericable(),
        "[TEST] Attribute on GreyScale should be genericable." );
    geode::OpenGeodeImageException::test(
        rgb_attribute->generic_value( 0 ) == 3, "[TEST] Wrong generic value" );
    geode::OpenGeodeImageException::test(
        greyscale_attribute->generic_value( 0 ) == 67,
        "[TEST] Wrong generic value" );
}

void check_interpolation( geode::AttributeManager& manager,
    const geode::VariableAttribute< geode::RGBColor >& rgb_attribute,
    const geode::VariableAttribute< geode::GreyscaleColor >& grey_attribute,
    absl::Span< const geode::index_t > indices,
    absl::Span< const double > lambdas,
    geode::local_index_t expected )
{
    static constexpr geode::index_t TARGET{ 10 };
    manager.interpolate_attribute_value( { indices, lambdas }, TARGET );
    const geode::RGBColor expected_rgb{ expected, expected, expected };
    geode::OpenGeodeImageException::test(
        rgb_attribute.value( TARGET ) == expected_rgb,
        "[TEST] Wrong RGBColor interpolation: ",
        rgb_attribute.value( TARGET ).string(), " instead of ",
        expected_rgb.string() );
    const geode::GreyscaleColor expected_grey{ expected };
    geode::OpenGeodeImageException::test(
        grey_attribute.value( TARGET ) == expected_grey,
        "[TEST] Wrong GreyscaleColor interpolation: ",
        grey_attribute.value( TARGET ).string(), " instead of ",
        expected_grey.string() );
}

void test_interpolation()
{
    geode::AttributeManager manager;
    manager.resize( 11 );
    geode::AttributeProperties properties;
    properties.interpolable = true;
    static constexpr geode::local_index_t DEFAULT{ 42 };
    geode::AttributeValues< geode::RGBColor > rgb_values;
    rgb_values.default_value = geode::RGBColor{ DEFAULT, DEFAULT, DEFAULT };
    auto rgb_attribute =
        manager.find_attribute< geode::VariableAttribute, geode::RGBColor >(
            manager
                .create_attribute< geode::VariableAttribute, geode::RGBColor >(
                    "rgb", rgb_values, properties ) );
    geode::AttributeValues< geode::GreyscaleColor > grey_values;
    grey_values.default_value = geode::GreyscaleColor{ DEFAULT };
    auto grey_attribute = manager.find_attribute< geode::VariableAttribute,
        geode::GreyscaleColor >(
        manager.create_attribute< geode::VariableAttribute,
            geode::GreyscaleColor >( "grey", grey_values, properties ) );
    const std::array< geode::local_index_t, 4 > values{ 255, 255, 100, 101 };
    for( const auto v : geode::LIndices{ values } )
    {
        rgb_attribute->set_value( v, { values[v], values[v], values[v] } );
        grey_attribute->set_value( v, geode::GreyscaleColor{ values[v] } );
    }
    const auto third = 1. / 3.;
    check_interpolation(
        manager, *rgb_attribute, *grey_attribute, { 0, 1 }, { 0.5, 0.5 }, 255 );
    check_interpolation( manager, *rgb_attribute, *grey_attribute, { 2, 2, 2 },
        { third, third, third }, 100 );
    check_interpolation(
        manager, *rgb_attribute, *grey_attribute, { 2, 3 }, { 0.5, 0.5 }, 101 );
    check_interpolation( manager, *rgb_attribute, *grey_attribute, { 0, 2 },
        { third, 2 * third }, 152 );
    check_interpolation(
        manager, *rgb_attribute, *grey_attribute, { 0, 2 }, { 2, -1 }, 255 );
    check_interpolation(
        manager, *rgb_attribute, *grey_attribute, { 2, 0 }, { 2, -1 }, 0 );
    check_interpolation(
        manager, *rgb_attribute, *grey_attribute, {}, {}, DEFAULT );
}

void test_rgb_hash()
{
    const std::hash< geode::RGBColor > hash;
    geode::OpenGeodeImageException::test(
        hash( { 10, 20, 30 } ) != hash( { 30, 20, 10 } ),
        "[TEST] Permuted channels should not share a hash" );
    geode::OpenGeodeImageException::test(
        hash( { 7, 7, 0 } ) != hash( { 9, 9, 0 } ),
        "[TEST] Colors with equal red and green should not share a hash" );
    absl::flat_hash_set< size_t > hashes;
    for( const auto value : geode::Range{ 256 } )
    {
        const auto channel = static_cast< geode::local_index_t >( value );
        hashes.insert( hash( { channel, channel, channel } ) );
    }
    geode::OpenGeodeImageException::test(
        hashes.size() == 256, "[TEST] Grey colors should not share hashes" );
}

void test()
{
    test_comparison();
    test_rgb_addition();
    test_greyscale_addition();
    test_color_attribute();
    test_interpolation();
    test_rgb_hash();
}

OPENGEODE_TEST( "colors" )