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

    geode::BoundingBox2D box2;
    box2.add_point( { { -2, -2 } } );
    box2.add_point( { { -1, -1 } } );

    OPENGEODE_EXCEPTION( box2.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox initialization" );
    OPENGEODE_EXCEPTION( box2.max() == geode::Point2D( { -1, -1 } ),
        "[Test] Error in BoundingBox initialization" );

    box2.add_box( box );
    OPENGEODE_EXCEPTION( box2.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox union computation" );
    OPENGEODE_EXCEPTION( box2.max() == geode::Point2D( { 1, 1 } ),
        "[Test] Error in BoundingBox union computation" );
    OPENGEODE_EXCEPTION( box.intersects( box2 ), "[Test] BBox should overlap" );

    geode::BoundingBox2D box3;
    box3.add_point( { { 2, 2 } } );
    box3.add_point( { { 3, 3 } } );
    OPENGEODE_EXCEPTION(
        !box.intersects( box3 ), "[Test] BBox should not overlap" );
    OPENGEODE_EXCEPTION( box3.contains( { { 2.5, 2.5 } } ),
        "[Test] BBox should contain this point" );
    OPENGEODE_EXCEPTION( !box3.contains( { { 10, 0 } } ),
        "[Test] BBox should not contain this point" );
    OPENGEODE_EXCEPTION( !box3.contains( { { 0, 0 } } ),
        "[Test] BBox should not contain that point" );

    geode::BoundingBox2D box_negative;
    box_negative.add_point( { { -2, -2 } } );
    box_negative.add_point( { { -1.5, -1.5 } } );
    OPENGEODE_EXCEPTION( box_negative.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox initialization and point additions" );
    OPENGEODE_EXCEPTION( box_negative.max() == geode::Point2D( { -1.5, -1.5 } ),
        "[Test] Error in BoundingBox initialization and point additions" );
    OPENGEODE_EXCEPTION( !box.intersects( box_negative ),
        "[Test] BBox should not overlap box_negative" );

    const geode::BoundingBox2D copy_box = box2;
    OPENGEODE_EXCEPTION( copy_box.min() == geode::Point2D( { -2, -2 } )
                             && copy_box.max() == geode::Point2D( { 1, 1 } ),
        "[Test] Copied BBox has wrong extension" );

    const auto box_neg2 = std::move( box_negative );
    OPENGEODE_EXCEPTION( box_neg2.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox move" );
    OPENGEODE_EXCEPTION( box_neg2.max() == geode::Point2D( { -1.5, -1.5 } ),
        "[Test] Error in BoundingBox move" );

    const geode::BoundingBox2D box_neg3( std::move( box_neg2 ) );
    OPENGEODE_EXCEPTION( box_neg3.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox move constructor" );
    OPENGEODE_EXCEPTION( box_neg3.max() == geode::Point2D( { -1.5, -1.5 } ),
        "[Test] Error in BoundingBox move constructor" );
}

OPENGEODE_TEST( "bounding-box" )
