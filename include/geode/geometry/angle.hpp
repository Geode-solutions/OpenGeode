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
    /*!
     * Description of Angle
     */
    class Angle
    {
    public:
        [[nodiscard]] static Angle create_from_radians( double radians );
        [[nodiscard]] static Angle create_from_degrees( double degrees );

        [[nodiscard]] double radians() const;
        [[nodiscard]] double degrees() const;

        [[nodiscard]] double sin() const
        {
            return std::sin( radians_ );
        }
        [[nodiscard]] double cos() const
        {
            return std::cos( radians_ );
        }
        [[nodiscard]] double tan() const
        {
            return std::tan( radians_ );
        }

        [[nodiscard]] bool operator==( const Angle& other ) const
        {
            return std::abs( radians_ - other.radians_ )
                   < GLOBAL_ANGULAR_EPSILON;
        }
        [[nodiscard]] bool operator<( const Angle& other ) const
        {
            return radians_ < other.radians_;
        }
        [[nodiscard]] bool operator>( const Angle& other ) const
        {
            return radians_ > other.radians_;
        }
        [[nodiscard]] Angle operator+( const Angle& other ) const
        {
            return Angle( radians_ + other.radians_ );
        }
        [[nodiscard]] Angle operator-( const Angle& other ) const
        {
            return Angle( radians_ - other.radians_ );
        }
        [[nodiscard]] Angle operator*( double scalar ) const
        {
            return Angle( radians_ * scalar );
        }
        [[nodiscard]] Angle operator/( double scalar ) const
        {
            return Angle( radians_ / scalar );
        }

        [[nodiscard]] Angle normalized_0_twopi() const;
        [[nodiscard]] Angle normalized_minuspi_pi() const;
        [[nodiscard]] Angle normalized_0_pi() const;

    private:
        explicit Angle( double radians ) : radians_( radians ) {}
        double radians_;
    };

} // namespace geode
