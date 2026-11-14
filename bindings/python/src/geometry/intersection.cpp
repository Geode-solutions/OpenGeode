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
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/point.hpp>

#define PYTHON_INTERSECTION( dimension )                                       \
    const auto line_sphere##dimension =                                        \
        "line_sphere_intersection" + std::to_string( dimension ) + "D";        \
    module.def( line_sphere##dimension.c_str(),                                \
        static_cast< IntersectionResult<                                       \
            absl::InlinedVector< Point< dimension >, 2 > > ( * )(              \
            const InfiniteLine< dimension >&, const Sphere< dimension >& ) >(  \
            &line_sphere_intersection ),                                       \
        pybind11::arg( "line" ), pybind11::arg( "sphere" ),                    \
        "Intersect an infinite line and a sphere and return an "               \
        "IntersectionResult holding the intersection points" );                \
    const auto segment_sphere##dimension =                                     \
        "segment_sphere_intersection" + std::to_string( dimension ) + "D";     \
    module.def( segment_sphere##dimension.c_str(),                             \
        static_cast< IntersectionResult<                                       \
            absl::InlinedVector< Point< dimension >, 2 > > ( * )(              \
            const Segment< dimension >&, const Sphere< dimension >& ) >(       \
            &segment_sphere_intersection ),                                    \
        pybind11::arg( "segment" ), pybind11::arg( "sphere" ),                 \
        "Intersect a segment and a sphere and return an IntersectionResult "   \
        "holding the intersection points" )

#define PYTHON_INTERSECTION_RESULT( Type )                                     \
    const auto result##Type = std::string( "IntersectionResult" ) + #Type;     \
    pybind11::class_< IntersectionResult< Type > >( module,                    \
        result##Type.c_str(),                                                  \
        "Result of an intersection computation: type, and the intersection "   \
        "when there is one" )                                                  \
        .def( "has_intersection",                                              \
            &IntersectionResult< Type >::has_intersection,                     \
            "Return True if the objects intersect" )                           \
        .def_readonly( "result", &IntersectionResult< Type >::result,          \
            "Intersection point(s), meaningful only if has_intersection()" )   \
        .def_readonly( "type", &IntersectionResult< Type >::type,              \
            "Kind of result: none, intersect, parallel or incorrect" );

namespace geode
{
    void define_intersection( pybind11::module& module )
    {
        pybind11::native_enum< INTERSECTION_TYPE >( module, "IntersectionType",
            "enum.Enum",
            "Outcome of an intersection computation: none (too far), "
            "intersect, parallel (no unique point) or incorrect (computed "
            "points are not within the global epsilon of the objects)" )
            .value( "none", INTERSECTION_TYPE::none )
            .value( "intersect", INTERSECTION_TYPE::intersect )
            .value( "parallel", INTERSECTION_TYPE::parallel )
            .value( "incorrect", INTERSECTION_TYPE::incorrect )
            .finalize();

        PYTHON_INTERSECTION_RESULT( Point2D );
        PYTHON_INTERSECTION_RESULT( Point3D );
        using InlinedVectorPoint2D = absl::InlinedVector< Point2D, 2 >;
        PYTHON_INTERSECTION_RESULT( InlinedVectorPoint2D );
        using InlinedVectorPoint3D = absl::InlinedVector< Point3D, 2 >;
        PYTHON_INTERSECTION_RESULT( InlinedVectorPoint3D );
        PYTHON_INTERSECTION_RESULT( InfiniteLine3D );
        PYTHON_INTERSECTION( 2 );
        PYTHON_INTERSECTION( 3 );
        module.def( "line_plane_intersection3D", &line_plane_intersection,
            pybind11::arg( "line" ), pybind11::arg( "plane" ),
            "Intersect an infinite line and a plane and return an "
            "IntersectionResult holding the point" );
        module.def( "segment_plane_intersection3D", &segment_plane_intersection,
            pybind11::arg( "segment" ), pybind11::arg( "plane" ),
            "Intersect a segment and a plane and return an IntersectionResult "
            "holding the point (no point if the segment lies in the plane)" );
        module.def( "segment_triangle_intersection3D",
            &segment_triangle_intersection, pybind11::arg( "segment" ),
            pybind11::arg( "triangle" ),
            "Intersect a segment and a triangle and return an "
            "IntersectionResult holding the point" );
        module.def( "line_triangle_intersection3D", &line_triangle_intersection,
            pybind11::arg( "line" ), pybind11::arg( "triangle" ),
            "Intersect an infinite line and a triangle and return an "
            "IntersectionResult holding the point" );
        module.def( "line_line_intersection2D", &line_line_intersection,
            pybind11::arg( "line0" ), pybind11::arg( "line1" ),
            "Intersect two 2D infinite lines and return an IntersectionResult "
            "holding the point" );
        module.def( "segment_segment_intersection2D",
            &segment_segment_intersection, pybind11::arg( "segment0" ),
            pybind11::arg( "segment1" ),
            "Intersect two 2D segments and return an IntersectionResult "
            "holding the point" );
        module.def( "segment_line_intersection2D", &segment_line_intersection,
            pybind11::arg( "segment" ), pybind11::arg( "line" ),
            "Intersect a 2D segment and an infinite line and return an "
            "IntersectionResult holding the point" );
        module.def( "segment_cylinder_intersection3D",
            &segment_cylinder_intersection, pybind11::arg( "segment" ),
            pybind11::arg( "cylinder" ),
            "Intersect a segment and a cylinder and return an "
            "IntersectionResult holding the intersection points" );
        module.def( "line_cylinder_intersection3D", &line_cylinder_intersection,
            pybind11::arg( "line" ), pybind11::arg( "cylinder" ),
            "Intersect an infinite line and a cylinder and return an "
            "IntersectionResult holding the intersection points" );
        module.def( "triangle_circle_intersection3D",
            &triangle_circle_intersection, pybind11::arg( "triangle" ),
            pybind11::arg( "circle" ),
            "Intersect a triangle and a circle and return an "
            "IntersectionResult holding the intersection points" );
        module.def( "plane_circle_intersection3D", &plane_circle_intersection,
            pybind11::arg( "plane" ), pybind11::arg( "circle" ),
            "Intersect a plane and a circle and return an IntersectionResult "
            "holding the intersection points" );
    }
} // namespace geode
