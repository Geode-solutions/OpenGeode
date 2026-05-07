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

#include <geode/basic/logger.hpp>
#include <geode/basic/mapping.hpp>

#include <geode/tests/common.hpp>

void test_bijective_mappings()
{
    geode::BijectiveMapping< geode::index_t, double > bijective;
    bijective.map( 0, 42.1 );
    bijective.clear();
    bijective.map( 0, 42.1 );
    geode::OpenGeodeBasicException::test( bijective.has_mapping_input( 0 ),
        "0 should be a key for bijective inputs" );
    geode::OpenGeodeBasicException::test( bijective.has_mapping_output( 42.1 ),
        "42.1 should be a key for bijective outputs" );
    bijective.map( 1, -22 );
    geode::OpenGeodeBasicException::test( bijective.has_mapping_input( 1 ),
        "1 should be a key for bijective inputs" );
    geode::OpenGeodeBasicException::test( bijective.has_mapping_output( -22 ),
        "-22 should be a key for bijective outputs" );
    geode::OpenGeodeBasicException::test(
        bijective.size() == 2, "Size of bijective should be 2" );
    bijective.map( 3, -22 );
    geode::OpenGeodeBasicException::test(
        bijective.size() == 2, "Size of bijective should still be 2" );
    geode::OpenGeodeBasicException::test( bijective.has_mapping_input( 3 ),
        "3 should be a key for bijective inputs" );
    geode::OpenGeodeBasicException::test( !bijective.has_mapping_input( 1 ),
        "1 should not be a key for bijective inputs anymore" );
    bijective.erase_out( 42.1 );
    geode::OpenGeodeBasicException::test( !bijective.has_mapping_output( 42.1 ),
        "42.1 should not be a key for bijective outputs anymore" );
    geode::OpenGeodeBasicException::test( !bijective.has_mapping_input( 0 ),
        "0 should not be a key for bijective inputs anymore" );
    geode::OpenGeodeBasicException::test(
        bijective.size() == 1, "Size of bijective should be 1" );
    bijective.map( 3, 11.1 );
    geode::OpenGeodeBasicException::test(
        bijective.size() == 1, "Size of bijective should still be 1" );
    geode::OpenGeodeBasicException::test( bijective.has_mapping_output( 11.1 ),
        "11.1 should be a key for bijective outputs" );
    geode::OpenGeodeBasicException::test( !bijective.has_mapping_output( -22 ),
        "-22 should not be a key for bijective outputs anymore" );
}

void test_generic_mappings()
{
    geode::GenericMapping< geode::index_t, double > generic;
    generic.map( 0, 42.1 );
    geode::OpenGeodeBasicException::test( generic.has_mapping_input( 0 ),
        "0 should be a key for generic inputs" );
    geode::OpenGeodeBasicException::test( generic.has_mapping_output( 42.1 ),
        "42.1 should be a key for generic outputs" );
    generic.map( 1, -22 );
    geode::OpenGeodeBasicException::test( generic.has_mapping_input( 1 ),
        "1 should be a key for generic inputs" );
    geode::OpenGeodeBasicException::test( generic.has_mapping_output( -22 ),
        "-22 should be a key for generic outputs" );
    geode::OpenGeodeBasicException::test(
        generic.size_in() == 2, "Size in of generic should be 2" );
    geode::OpenGeodeBasicException::test(
        generic.size_out() == 2, "Size out of generic should be 2" );
    generic.map( 7, 42.1 );
    geode::OpenGeodeBasicException::test(
        generic.size_in() == 3, "Size in of generic should be 3" );
    geode::OpenGeodeBasicException::test(
        generic.size_out() == 2, "Size out of generic should still be 2" );
    geode::OpenGeodeBasicException::test( generic.has_mapping_input( 7 ),
        "7 should be a key for generic inputs" );
    geode::OpenGeodeBasicException::test( generic.has_mapping_input( 1 ),
        "1 should still be a key for generic inputs anymore" );
    generic.erase_out( 42.1 );
    geode::OpenGeodeBasicException::test( !generic.has_mapping_output( 42.1 ),
        "42.1 should not be a key for generic outputs anymore" );
    geode::OpenGeodeBasicException::test( !generic.has_mapping_input( 0 ),
        "0 should not be a key for generic inputs anymore" );
    geode::OpenGeodeBasicException::test( !generic.has_mapping_input( 7 ),
        "7 should not be a key for generic inputs anymore" );
    geode::OpenGeodeBasicException::test(
        generic.size_in() == 1, "Size in of generic should be 1" );
    geode::OpenGeodeBasicException::test(
        generic.size_out() == 1, "Size out of generic should be 1" );
    generic.map( 0, -8.0 );
    generic.map( 0, -8.0 );
    generic.map( 0, -8.0 );
    generic.map( 0, -8.0 );
    generic.map( 5, -8.0 );
    geode::OpenGeodeBasicException::test( generic.out2in( -8.0 ).size() == 2,
        "Size of out2in for -8.0 should be 2" );
    generic.erase_in( 0 );
    geode::OpenGeodeBasicException::test( generic.out2in( -8.0 ).size() == 1,
        "Size of out2in for -8.0 should be 1" );
}

void test()
{
    test_bijective_mappings();
    test_generic_mappings();
}

OPENGEODE_TEST( "mappings" )
