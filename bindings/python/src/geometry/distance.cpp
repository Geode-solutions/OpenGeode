/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include "../common.hpp"

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>

#define PYTHON_DISTANCE( dimension )                                           \
    const auto point_point_distance##dimension =                               \
        "point_point_distance" + std::to_string( dimension ) + "D";            \
    module.def( point_point_distance##dimension.c_str(),                       \
        &point_point_distance< dimension >, pybind11::arg( "point0" ),         \
        pybind11::arg( "point1" ),                                             \
        "Return the Euclidean distance between two points" );                  \
    const auto point_segment_distance##dimension =                             \
        "point_segment_distance" + std::to_string( dimension ) + "D";          \
    module.def( point_segment_distance##dimension.c_str(),                     \
        &point_segment_distance< dimension >, pybind11::arg( "point" ),        \
        pybind11::arg( "segment" ),                                            \
        "Return the smallest distance between a point and a segment" );        \
    const auto segment_segment_distance##dimension =                           \
        "segment_segment_distance" + std::to_string( dimension ) + "D";        \
    module.def( segment_segment_distance##dimension.c_str(),                   \
        &segment_segment_distance< dimension >, pybind11::arg( "segment0" ),   \
        pybind11::arg( "segment1" ),                                           \
        "Return (distance, closest point on segment0, closest point on "       \
        "segment1) for the smallest distance between two segments" );          \
    const auto segment_line_distance##dimension =                              \
        "segment_line_distance" + std::to_string( dimension ) + "D";           \
    module.def( segment_line_distance##dimension.c_str(),                      \
        &segment_line_distance< dimension >, pybind11::arg( "segment" ),       \
        pybind11::arg( "line" ),                                               \
        "Return (distance, closest point on the segment, closest point on "    \
        "the line) for the smallest distance between a segment and an "        \
        "infinite line" );                                                     \
    const auto point_line_distance##dimension =                                \
        "point_line_distance" + std::to_string( dimension ) + "D";             \
    module.def( point_line_distance##dimension.c_str(),                        \
        &point_line_distance< dimension >, pybind11::arg( "point" ),           \
        pybind11::arg( "line" ),                                               \
        "Return the smallest distance between a point and an infinite line" ); \
    const auto point_triangle_distance##dimension =                            \
        "point_triangle_distance" + std::to_string( dimension ) + "D";         \
    module.def( point_triangle_distance##dimension.c_str(),                    \
        &point_triangle_distance< dimension >, pybind11::arg( "point" ),       \
        pybind11::arg( "triangle" ),                                           \
        "Return (distance, closest point on the triangle) for the smallest "   \
        "distance between a point and a triangle" );                           \
    const auto point_sphere_distance##dimension =                              \
        "point_sphere_distance" + std::to_string( dimension ) + "D";           \
    module.def( point_sphere_distance##dimension.c_str(),                      \
        &point_sphere_distance< dimension >, pybind11::arg( "point" ),         \
        pybind11::arg( "sphere" ),                                             \
        "Return (distance, closest point on the sphere) between a point and "  \
        "a sphere" );                                                          \
    const auto point_sphere_signed_distance##dimension =                       \
        "point_sphere_signed_distance" + std::to_string( dimension ) + "D";    \
    module.def( point_sphere_signed_distance##dimension.c_str(),               \
        &point_sphere_signed_distance< dimension >, pybind11::arg( "point" ),  \
        pybind11::arg( "sphere" ),                                             \
        "Return (signed distance, closest point on the sphere) between a "     \
        "point and a sphere: positive outside, negative inside" );             \
    const auto point_ball_distance##dimension =                                \
        "point_ball_distance" + std::to_string( dimension ) + "D";             \
    module.def( point_ball_distance##dimension.c_str(),                        \
        &point_ball_distance< dimension >, pybind11::arg( "point" ),           \
        pybind11::arg( "sphere" ),                                             \
        "Return (distance, closest point on the ball) between a point and a "  \
        "ball: the distance is 0 if the point is inside" )

namespace geode
{
    void define_distance( pybind11::module& module )
    {
        PYTHON_DISTANCE( 2 );
        PYTHON_DISTANCE( 3 );
        module.def( "point_line_signed_distance2D", &point_line_signed_distance,
            pybind11::arg( "point" ), pybind11::arg( "line" ),
            "Return the smallest signed distance between a 2D point and an "
            "infinite line (sign given by the side of the line)" );
        module.def( "point_triangle_signed_distance3D",
            &point_triangle_signed_distance, pybind11::arg( "point" ),
            pybind11::arg( "triangle" ),
            "Return (signed distance, closest point on the triangle) between a "
            "point and a triangle, the sign being given by the triangle "
            "normal" );
        module.def( "line_triangle_distance3D", &line_triangle_distance,
            pybind11::arg( "line" ), pybind11::arg( "triangle" ),
            "Return (distance, closest point on the line, closest point on the "
            "triangle) for the smallest distance between an infinite line and "
            "a triangle" );
        module.def( "segment_triangle_distance3D", &segment_triangle_distance,
            pybind11::arg( "segment" ), pybind11::arg( "triangle" ),
            "Return (distance, closest point on the segment, closest point on "
            "the triangle) for the smallest distance between a segment and a "
            "triangle" );
        module.def( "point_tetrahedron_distance3D", &point_tetrahedron_distance,
            pybind11::arg( "point" ), pybind11::arg( "tetra" ),
            "Return (distance, nearest point on the tetrahedron) between a "
            "point and a tetrahedron" );
        module.def( "point_plane_distance3D", &point_plane_distance,
            pybind11::arg( "point" ), pybind11::arg( "plane" ),
            "Return (distance, nearest point on the plane) between a point and "
            "a plane" );
        module.def( "point_plane_signed_distance3D",
            &point_plane_signed_distance, pybind11::arg( "point" ),
            pybind11::arg( "plane" ),
            "Return (signed distance, nearest point on the plane) between a "
            "point and a plane, the sign being given by the plane normal" );
        module.def( "point_circle_distance3D", &point_circle_distance,
            pybind11::arg( "point" ), pybind11::arg( "circle" ),
            "Return (distance, closest point on the circle) between a point "
            "and a circle" );
        module.def( "point_circle_signed_distance3D",
            &point_circle_signed_distance, pybind11::arg( "point" ),
            pybind11::arg( "circle" ),
            "Return (signed distance, closest point on the circle) between a "
            "point and a circle, the sign being given by the side of the "
            "circle plane" );
        module.def( "point_disk_distance3D", &point_disk_distance,
            pybind11::arg( "point" ), pybind11::arg( "disk" ),
            "Return (distance, closest point on the disk) between a point and "
            "a disk: the distance is 0 if the point is inside" );
    }
} // namespace geode
