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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/mapping.h>

#include <geode/tests/common.h>

void test_copy_mapping()
{
    geode::ModelCopyMapping copy_mapping;

    std::vector< geode::uuid > uuids_in( 3 );
    std::vector< geode::uuid > uuids_out( 3 );

    geode::BijectiveMapping< geode::uuid > corner_mapping;
    for( const auto i : geode::LRange{ 3 } )
    {
        corner_mapping.map( uuids_in[i], uuids_out[i] );
    }
    copy_mapping.emplace(
        geode::Corner3D::component_type_static(), std::move( corner_mapping ) );
    OPENGEODE_EXCEPTION( !copy_mapping.has_mapping_type(
                             geode::Surface3D::component_type_static() ),
        "[Test] Copy mappings should not exist for Surfaces" );
    OPENGEODE_EXCEPTION( copy_mapping.has_mapping_type(
                             geode::Corner3D::component_type_static() ),
        "[Test] Copy mappings should exist for Corners" );
    const auto& corner_copy_mappings =
        copy_mapping.at( geode::Corner3D::component_type_static() );
    OPENGEODE_EXCEPTION( corner_copy_mappings.size() == 3,
        "[Test] Wrong size for CopyMapping Corners" );
    for( const auto i : geode::LRange{ 3 } )
    {
        OPENGEODE_EXCEPTION(
            corner_copy_mappings.in2out( uuids_in[i] ) == uuids_out[i],
            "[Test] Wrong mapping for CopyMapping Corners" );
    }
}

void test_generic_mapping()
{
    geode::ModelGenericMapping generic_mapping;

    std::vector< geode::uuid > uuids_in( 3 );
    std::vector< geode::uuid > uuids_out( 3 );

    geode::GenericMapping< geode::uuid > corner_mapping;
    for( const auto i : geode::LRange{ 3 } )
    {
        for( const auto j : geode::Range{ i, 3 } )
        {
            corner_mapping.map( uuids_in[i], uuids_out[j] );
            DEBUG( i );
            DEBUG( j );
            DEBUG( "----" );
        }
    }
    generic_mapping.emplace(
        geode::Corner3D::component_type_static(), std::move( corner_mapping ) );
    OPENGEODE_EXCEPTION( !generic_mapping.has_mapping_type(
                             geode::Surface3D::component_type_static() ),
        "[Test] Generic mappings should not exist for Surfaces" );
    OPENGEODE_EXCEPTION( generic_mapping.has_mapping_type(
                             geode::Corner3D::component_type_static() ),
        "[Test] Generic mappings should exist for Corners" );
    const auto& corner_generic_mappings =
        generic_mapping.at( geode::Corner3D::component_type_static() );
    OPENGEODE_EXCEPTION( corner_generic_mappings.size_in() == 3,
        "[Test] Wrong size for CopyMapping Corners in" );
    OPENGEODE_EXCEPTION( corner_generic_mappings.size_out() == 3,
        "[Test] Wrong size for CopyMapping Corners out" );
    for( const auto i : geode::LRange{ 3 } )
    {
        OPENGEODE_EXCEPTION(
            corner_generic_mappings.in2out( uuids_in[i] ).size()
                == static_cast< geode::index_t >( 3 - i ),
            "[Test] Wrong mapping for CopyMapping Corners (in2out)" );
        OPENGEODE_EXCEPTION(
            corner_generic_mappings.out2in( uuids_out[i] ).size()
                == static_cast< geode::index_t >( i + 1 ),
            "[Test] Wrong mapping for CopyMapping Corners (out2in)" );
    }
}

void test()
{
    test_copy_mapping();
    test_generic_mapping();
}

OPENGEODE_TEST( "model-mapping" )