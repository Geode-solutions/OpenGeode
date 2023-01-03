/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/intersection.h>
#include <geode/geometry/point.h>

#define PYTHON_INTERSECTION( dimension )                                       \
    const auto line_sphere##dimension =                                        \
        "line_sphere_intersection" + std::to_string( dimension ) + "D";        \
    module.def( line_sphere##dimension.c_str(),                                \
        ( IntersectionResult<                                                  \
            absl::InlinedVector< Point< dimension >, 2 > >( * )(               \
            const InfiniteLine< dimension >&, const Sphere< dimension >& ) )   \
            & line_sphere_intersection );                                      \
    const auto segment_sphere##dimension =                                     \
        "segment_sphere_intersection" + std::to_string( dimension ) + "D";     \
    module.def( segment_sphere##dimension.c_str(),                             \
        ( IntersectionResult<                                                  \
            absl::InlinedVector< Point< dimension >, 2 > >( * )(               \
            const Segment< dimension >&, const Sphere< dimension >& ) )        \
            & segment_sphere_intersection )

#define PYTHON_INTERSECTION_RESULT( Type )                                     \
    const auto result##Type = std::string( "IntersectionResult" ) + #Type;     \
    pybind11::class_< IntersectionResult< Type > >(                            \
        module, result##Type.c_str() )                                         \
        .def( "has_intersection",                                              \
            &IntersectionResult< Type >::has_intersection )                    \
        .def_readonly( "result", &IntersectionResult< Type >::result )         \
        .def_readonly( "type", &IntersectionResult< Type >::type );

namespace geode
{
    void define_intersection( pybind11::module& module )
    {
        PYTHON_INTERSECTION( 2 );
        PYTHON_INTERSECTION( 3 );
        PYTHON_INTERSECTION_RESULT( Point2D );
        PYTHON_INTERSECTION_RESULT( Point3D );
        using InlinedVectorPoint2D = absl::InlinedVector< Point2D, 2 >;
        PYTHON_INTERSECTION_RESULT( InlinedVectorPoint2D );
        using InlinedVectorPoint3D = absl::InlinedVector< Point3D, 2 >;
        PYTHON_INTERSECTION_RESULT( InlinedVectorPoint3D );
        PYTHON_INTERSECTION_RESULT( InfiniteLine3D );
        module.def(
            "segment_plane_intersection3D", &segment_plane_intersection );
        module.def(
            "segment_triangle_intersection3D", &segment_triangle_intersection );
        module.def( "line_plane_intersection3D", &line_plane_intersection );
        module.def(
            "line_triangle_intersection3D", &line_triangle_intersection );
        module.def( "line_line_intersection2D", &line_line_intersection );
        module.def(
            "segment_segment_intersection2D", &segment_segment_intersection );
        module.def( "segment_line_intersection2D", &segment_line_intersection );
    }
} // namespace geode
