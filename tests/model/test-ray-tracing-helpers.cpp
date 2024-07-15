/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/model/helpers/ray_tracing.h>

#include <geode/geometry/bounding_box.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/tests/common.h>

void test()
{
    geode::OpenGeodeModelLibrary::initialize();

    // load a 40x40x40 aligned cubic box brep
    auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "box_brep.og_brep" ) );
    geode::Point3D center( { 20., 20., 20. } );

    //  get the block
    const auto block_id = geode::block_containing_point( brep, center );
    OPENGEODE_EXCEPTION( block_id.has_value(),
        "[Test] fail to recover block_containing_point." );

    // test point iside the block
    geode::Point3D inside( { 0.00001, 0.00001, 0.00001 } );
    geode::Point3D outside( { -0.00001, 0.00001, 0.00001 } );
    OPENGEODE_EXCEPTION( geode::is_point_inside_block(
                             brep, brep.block( block_id.value() ), inside ),
        "[Test] the point named inside should be inside the block." );

    OPENGEODE_EXCEPTION( !geode::is_point_inside_block(
                             brep, brep.block( block_id.value() ), outside ),
        "[Test] the point named outside should be outside the block." );
}

OPENGEODE_TEST( "ray-tracing-helpers" )