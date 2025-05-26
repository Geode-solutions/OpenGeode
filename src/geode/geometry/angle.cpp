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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/geometry/angle.hpp>

namespace
{
    double radians_to_degrees( double radians )
    {
        return radians * 180. / M_PI;
    }
    double degrees_to_radians( double degree )
    {
        return degree * M_PI / 180.;
    }
    constexpr double TWO_PI = 2. * M_PI;
    constexpr double HALF_PI = M_PI / 2.;
    constexpr double EPSILON = 1e-10;
} // namespace

namespace geode
{
    Angle Angle::create_from_radians( double radians )
    {
        return Angle( radians );
    }

    Angle Angle::create_from_degrees( double degrees )
    {
        return Angle( degrees_to_radians( degrees ) );
    }

    double Angle::radians() const
    {
        return radians_;
    }

    double Angle::degrees() const
    {
        return radians_to_degrees( radians_ );
    }

    Angle Angle::normalized_0_twopi() const
    {
        double normalized_radians = std::fmod( radians_, TWO_PI );
        if( normalized_radians < 0 )
        {
            normalized_radians += TWO_PI;
        }
        return Angle( normalized_radians );
    }

    Angle Angle::normalized_minuspi_pi() const
    {
        auto normalized = normalized_0_twopi();
        return normalized - Angle( M_PI );
    }

    Angle Angle::normalized_0_pi() const
    {
        double normalized_radians = std::fmod( radians_, M_PI );
        if( normalized_radians < 0 )
        {
            normalized_radians += M_PI;
        }
        return Angle( normalized_radians );
    }

} // namespace geode
