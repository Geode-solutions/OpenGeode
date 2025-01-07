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

#include <geode/basic/cached_value.hpp>

#include <geode/geometry/common.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/square_matrix.hpp>

namespace geode
{
    template < index_t dimension >
    class CoordinateSystem : public Frame< dimension >
    {
    public:
        CoordinateSystem();
        CoordinateSystem(
            std::array< Vector< dimension >, dimension > directions,
            Point< dimension > origin );
        CoordinateSystem( Point< dimension > origin,
            const std::array< Point< dimension >, dimension >& other_points );

        [[nodiscard]] const Point< dimension >& origin() const;

        void set_origin( Point< dimension > origin );

        void set_directions(
            std::array< Vector< dimension >, dimension > directions );

        [[nodiscard]] Point< dimension > coordinates(
            const Point< dimension >& global_coordinates ) const;

        [[nodiscard]] Point< dimension > global_coordinates(
            const Point< dimension >& coordinates ) const;

        [[nodiscard]] std::string string() const;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        Point< dimension > origin_{};
        CachedValue< SquareMatrix< dimension > > global_to_local_matrix_;
    };
    ALIAS_2D_AND_3D( CoordinateSystem );
} // namespace geode