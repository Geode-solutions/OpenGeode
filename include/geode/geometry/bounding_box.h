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

#pragma once

#include <geode/basic/pimpl.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    /*!
     * Bounding box implemented for 2D and 3D points.
     */
    template < index_t dimension >
    class BoundingBox
    {
    public:
        BoundingBox();
        BoundingBox( const BoundingBox& other );
        BoundingBox& operator=( const BoundingBox& other );
        BoundingBox( BoundingBox&& other ) noexcept;
        BoundingBox& operator=( BoundingBox&& other ) noexcept;
        ~BoundingBox();

        void add_box( const BoundingBox< dimension >& box );

        void add_point( const Point< dimension >& point );

        bool contains( const Point< dimension >& point ) const;

        bool intersects( const BoundingBox< dimension >& bbox ) const;

        bool intersects( const Ray< dimension >& ray ) const;

        /*!
         * Returns the distance between the point and the box.
         * If the point is inside the box, the distance is negative.
         */
        double signed_distance( const Point< dimension >& point ) const;

        const Point< dimension >& min() const;

        const Point< dimension >& max() const;

        Point< dimension > center() const;

        Vector< dimension > diagonal() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_1D_AND_2D_AND_3D( BoundingBox );
} // namespace geode
