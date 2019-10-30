/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/geometry/bounding_box.h>
#include <geode/basic/logger.h>
#include <geode/geometry/point.h>

int main()
{
    using namespace geode;

    try
    {
        BoundingBox2D box;
        box.add_point( { { -1, -1 } } );
        box.add_point( { { 1, 1 } } );
        OPENGEODE_EXCEPTION( box.center() == Point2D( { 0, 0 } ),
            "[Test] Error in BoundingBox center computation" );

        BoundingBox2D box2{ box };
        box2.add_point( { { -2, -2 } } );
        box2.add_point( { { 0, 0 } } );
        OPENGEODE_EXCEPTION(
            box.boxes_overlap( box2 ), "[Test] BBox should overlap" );

        BoundingBox2D box3;
        box3.add_point( { { 2, 2 } } );
        box3.add_point( { { 3, 3 } } );
        OPENGEODE_EXCEPTION(
            !box.boxes_overlap( box3 ), "[Test] BBox should not overlap" );

        BoundingBox2D box_inter;
        std::tie( std::ignore, box_inter ) = box2.box_intersection( box );
        OPENGEODE_EXCEPTION( box_inter.min() == Point2D( { -1, -1 } ),
            "[Test] Error in BoundingBox intersection computation" );
        OPENGEODE_EXCEPTION( box_inter.max() == Point2D( { 1, 1 } ),
            "[Test] Error in BoundingBox intersection computation" );

        BoundingBox2D box_inter3;
        std::tie( std::ignore, box_inter3 ) = box3.box_intersection( box );

        auto box_union = box2.box_union( box );
        OPENGEODE_EXCEPTION( box_union.min() == Point2D( { -2, -2 } ),
            "[Test] Error in BoundingBox union computation" );
        OPENGEODE_EXCEPTION( box_union.max() == Point2D( { 1, 1 } ),
            "[Test] Error in BoundingBox union computation" );

        OPENGEODE_EXCEPTION( box_union.contains( { { 0, 0 } } ),
            "[Test] BBox should contain this point" );
        OPENGEODE_EXCEPTION( !box_union.contains( { { 10, 0 } } ),
            "[Test] BBox should not contain this point" );

        box = box2;
        OPENGEODE_EXCEPTION( box.min() == Point2D( { -2, -2 } )
                                 && box.max() == Point2D( { 1, 1 } ),
            "[Test] Copied BBox has wrong extension" );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
