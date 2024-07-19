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

#pragma once

#include <geode/basic/pimpl.hpp>

#include <geode/geometry/common.hpp>
#include <geode/geometry/point.hpp>

namespace geode
{
    /*!
     * Given a list of points, this class returns neighboring points.
     */
    template < index_t dimension >
    class DynamicNNSearch
    {
        OPENGEODE_DISABLE_COPY( DynamicNNSearch );
        // OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( DynamicNNSearch );

    public:
        explicit DynamicNNSearch( std::vector< Point< dimension > > points );
        DynamicNNSearch( DynamicNNSearch&& other ) noexcept;
        ~DynamicNNSearch();

        index_t nb_points() const;

        const Point< dimension >& point( index_t index ) const;

        /*!
         * Get the neighbors closer than a given distance from the given point
         * or within a sphere
         * @param[in] point The center of the sphere
         * @param[in] threshold_distance The radius of the sphere
         * @return the list of points inside this distance
         */
        std::vector< index_t > radius_neighbors(
            const Point< dimension >& point, double threshold_distance ) const;

        void addPoint( const Point< dimension >& point );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( DynamicNNSearch );
} // namespace geode
