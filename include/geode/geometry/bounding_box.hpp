/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geometry/common.hpp>
#include <geode/geometry/point.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( InfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( Ray );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    class Tetrahedron;
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
        BoundingBox( Point< dimension > min, Point< dimension > max );
        BoundingBox( const BoundingBox& other );
        BoundingBox& operator=( const BoundingBox& other );
        BoundingBox( BoundingBox&& other ) noexcept;
        BoundingBox& operator=( BoundingBox&& other ) noexcept;
        ~BoundingBox();

        void add_box( const BoundingBox< dimension >& box );

        void add_point( const Point< dimension >& point );

        void extends( double length );

        bool contains( const Point< dimension >& point ) const;

        bool contains( const BoundingBox< dimension >& bbox ) const;

        [[nodiscard]] bool intersects(
            const BoundingBox< dimension >& bbox ) const;

        [[nodiscard]] bool intersects( const Ray< dimension >& ray ) const;

        [[nodiscard]] bool intersects(
            const InfiniteLine< dimension >& line ) const;

        /*!
         * Returns true if the element is crossing, is inside, or is containing
         * the bbox
         */
        [[nodiscard]] bool intersects(
            const Segment< dimension >& segment ) const;

        /*!
         * Returns true if the element is crossing, is inside (or is containing
         * the bbox in 2D)
         */
        template < index_t T = dimension >
        [[nodiscard]] typename std::enable_if< T == 2 || T == 3, bool >::type
            intersects( const Triangle< T >& triangle ) const;

        /*!
         * Returns true if the element is crossing, is inside, or is containing
         * the bbox
         */
        template < index_t T = dimension >
        [[nodiscard]] typename std::enable_if< T == 3, bool >::type intersects(
            const Tetrahedron& tetra ) const;

        /*!
         * Returns the distance between the point and the box.
         * If the point is inside the box, the distance is negative.
         */
        [[nodiscard]] double signed_distance(
            const Point< dimension >& point ) const;

        [[nodiscard]] const Point< dimension >& min() const;

        [[nodiscard]] const Point< dimension >& max() const;

        [[nodiscard]] Point< dimension > center() const;

        [[nodiscard]] Vector< dimension > diagonal() const;

        [[nodiscard]] std::tuple< local_index_t, double >
            smallest_length() const;

        [[nodiscard]] std::tuple< local_index_t, double >
            largest_length() const;

        [[nodiscard]] std::string string() const;

    private:
        Point< dimension > min_;
        Point< dimension > max_;
    };
    ALIAS_1D_AND_2D_AND_3D( BoundingBox );
} // namespace geode
