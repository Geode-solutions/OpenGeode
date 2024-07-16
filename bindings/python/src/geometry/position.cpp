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

#include "../common.h"

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/position.hpp>

#define PYTHON_POSITION( dimension )                                           \
    const auto point_segment##dimension =                                      \
        "point_segment_position" + std::to_string( dimension ) + "D";          \
    module.def( point_segment##dimension.c_str(),                              \
        static_cast< POSITION ( * )( const Point< dimension >&,                \
            const Segment< dimension >& ) >( &point_segment_position ) );      \
    const auto point_triangle##dimension =                                     \
        "point_triangle_position" + std::to_string( dimension ) + "D";         \
    module.def( point_triangle##dimension.c_str(),                             \
        static_cast< POSITION ( * )( const Point< dimension >&,                \
            const Triangle< dimension >& ) >( &point_triangle_position ) )

namespace geode
{
    void define_position( pybind11::module& module )
    {
        PYTHON_POSITION( 2 );
        PYTHON_POSITION( 3 );
        module.def( "point_side_to_segment2D", &point_side_to_segment );
        module.def( "point_side_to_line2D", &point_side_to_line );
        module.def( "point_side_to_plane3D", &point_side_to_plane );
        module.def( "point_side_to_triangle3D", &point_side_to_triangle );
        module.def(
            "point_tetrahedron_position3D", &point_tetrahedron_position );
    }
} // namespace geode
