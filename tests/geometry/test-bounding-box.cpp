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

#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/tests/common.h>

void test()
{
    geode::BoundingBox2D box;
    box.add_point( { { -1, -1 } } );
    box.add_point( { { 1, 1 } } );

    geode::BoundingBox2D box2{ box };
    box2.add_point( { { -2, -2 } } );
    box2.add_point( { { 0, 0 } } );

    box2.add_box( box );
    OPENGEODE_EXCEPTION( box2.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox union computation" );
    OPENGEODE_EXCEPTION( box2.max() == geode::Point2D( { 1, 1 } ),
        "[Test] Error in BoundingBox union computation" );

    OPENGEODE_EXCEPTION( box2.contains( { { 0, 0 } } ),
        "[Test] BBox should contain this point" );
    OPENGEODE_EXCEPTION( !box2.contains( { { 10, 0 } } ),
        "[Test] BBox should not contain this point" );

    OPENGEODE_EXCEPTION( box2.contains( { { 0, 0 } } ),
        "[Test] BBox should contain this point" );
    OPENGEODE_EXCEPTION( !box2.contains( { { 10, 0 } } ),
        "[Test] BBox should not contain this point" );

    const geode::BoundingBox2D copy_box = box2;
    OPENGEODE_EXCEPTION( copy_box.min() == geode::Point2D( { -2, -2 } )
                             && copy_box.max() == geode::Point2D( { 1, 1 } ),
        "[Test] Copied BBox has wrong extension" );
}

OPENGEODE_TEST( "bounding-box" )