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

#include <geode/geometry/basic_objects/circle.h>
#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>

#define PYTHON_DISTANCE( dimension )                                           \
    const auto point_point_distance##dimension =                               \
        "point_point_distance" + std::to_string( dimension ) + "D";            \
    module.def( point_point_distance##dimension.c_str(),                       \
        &point_point_distance< dimension > );                                  \
    const auto point_segment_distance##dimension =                             \
        "point_segment_distance" + std::to_string( dimension ) + "D";          \
    module.def( point_segment_distance##dimension.c_str(),                     \
        &point_segment_distance< dimension > );                                \
    const auto segment_segment_distance##dimension =                           \
        "segment_segment_distance" + std::to_string( dimension ) + "D";        \
    module.def( segment_segment_distance##dimension.c_str(),                   \
        &segment_segment_distance< dimension > );                              \
    const auto segment_line_distance##dimension =                              \
        "segment_line_distance" + std::to_string( dimension ) + "D";           \
    module.def( segment_line_distance##dimension.c_str(),                      \
        &segment_line_distance< dimension > );                                 \
    const auto point_line_distance##dimension =                                \
        "point_line_distance" + std::to_string( dimension ) + "D";             \
    module.def( point_line_distance##dimension.c_str(),                        \
        &point_line_distance< dimension > );                                   \
    const auto point_triangle_distance##dimension =                            \
        "point_triangle_distance" + std::to_string( dimension ) + "D";         \
    module.def( point_triangle_distance##dimension.c_str(),                    \
        &point_triangle_distance< dimension > );                               \
    const auto point_sphere_distance##dimension =                              \
        "point_sphere_distance" + std::to_string( dimension ) + "D";           \
    module.def( point_sphere_distance##dimension.c_str(),                      \
        &point_sphere_distance< dimension > );                                 \
    const auto point_sphere_signed_distance##dimension =                       \
        "point_sphere_signed_distance" + std::to_string( dimension ) + "D";    \
    module.def( point_sphere_signed_distance##dimension.c_str(),               \
        &point_sphere_signed_distance< dimension > );                          \
    const auto point_ball_distance##dimension =                                \
        "point_ball_distance" + std::to_string( dimension ) + "D";             \
    module.def( point_ball_distance##dimension.c_str(),                        \
        &point_ball_distance< dimension > )

namespace geode
{
    void define_distance( pybind11::module& module )
    {
        PYTHON_DISTANCE( 2 );
        PYTHON_DISTANCE( 3 );
        module.def(
            "point_line_signed_distance2D", &point_line_signed_distance );
        module.def( "point_triangle_signed_distance3D",
            &point_triangle_signed_distance );
        module.def( "line_triangle_distance3D", &line_triangle_distance );
        module.def( "segment_triangle_distance3D", &segment_triangle_distance );
        module.def(
            "point_tetrahedron_distance3D", &point_tetrahedron_distance );
        module.def( "point_plane_distance3D", &point_plane_distance );
        module.def(
            "point_plane_signed_distance3D", &point_plane_signed_distance );
        module.def( "point_circle_distance3D", &point_circle_distance );
        module.def(
            "point_circle_signed_distance3D", &point_circle_signed_distance );
        module.def( "point_disk_distance3D", &point_disk_distance );
    }
} // namespace geode
