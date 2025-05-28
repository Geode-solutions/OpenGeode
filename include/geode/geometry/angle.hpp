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

namespace geode
{
    class opengeode_geometry_api Angle
    {
    public:
        [[nodiscard]] static Angle create_from_radians( double radians );
        [[nodiscard]] static Angle create_from_degrees( double degrees );

        [[nodiscard]] double radians() const;
        [[nodiscard]] double degrees() const;

        [[nodiscard]] double sin() const;
        [[nodiscard]] double cos() const;
        [[nodiscard]] double tan() const;

        [[nodiscard]] bool inexact_equal( const Angle& other ) const;
        [[nodiscard]] bool operator==( const Angle& other ) const;
        [[nodiscard]] bool operator<( const Angle& other ) const;
        [[nodiscard]] bool operator>( const Angle& other ) const;
        [[nodiscard]] Angle operator+( const Angle& other ) const;
        [[nodiscard]] Angle operator-( const Angle& other ) const;
        [[nodiscard]] Angle operator*( double scalar ) const;
        [[nodiscard]] Angle operator/( double scalar ) const;

        // 2pi excluded
        [[nodiscard]] Angle normalized_between_0_and_2pi() const;
        // minus pi excluded
        [[nodiscard]] Angle normalized_between_minuspi_and_pi() const;
        // pi excluded
        [[nodiscard]] Angle normalized_between_0_and_pi() const;

    private:
        explicit Angle( double radians ) : radians_( radians ) {}

    private:
        double radians_;
    };

} // namespace geode
