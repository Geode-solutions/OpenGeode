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

#include <geode/basic/range.hpp>

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/point.hpp>

namespace
{
    // RefPoint-based objects (Segment, Triangle...) only reference their
    // points: return copies so the Python points do not alias the object
    // storage (which may live in a mesh).
    template < typename Object >
    auto owned_vertices( const Object& object )
    {
        const auto& vertices = object.vertices();
        std::array< geode::Point< Object::dim >,
            std::tuple_size_v< std::decay_t< decltype( vertices ) > > >
            result;
        for( const auto vertex : geode::LIndices{ vertices } )
        {
            result[vertex] = vertices[vertex];
        }
        return result;
    }
} // namespace

#define PYTHON_SEGMENT( dimension )                                            \
    const auto segment##dimension =                                            \
        "Segment" + std::to_string( dimension ) + "D";                         \
    pybind11::class_< Segment##dimension##D >(                                 \
        module, segment##dimension.c_str(), "Segment between two points" )     \
        .def( pybind11::init< const Point< dimension >&,                       \
                  const Point< dimension >& >(),                               \
            pybind11::keep_alive< 1, 2 >(), pybind11::keep_alive< 1, 3 >(),    \
            pybind11::arg( "point0" ), pybind11::arg( "point1" ),              \
            "Build a segment from its two end points. The points are kept "    \
            "alive by the segment" )                                           \
        .def( "direction", &Segment##dimension##D::direction,                  \
            "Return the unit vector going from the first to the second "       \
            "point. Raise an error if the segment is too short" )              \
        .def( "barycenter", &Segment##dimension##D::barycenter,                \
            "Return the middle point of the segment" )                         \
        .def( "length", &Segment##dimension##D::length,                        \
            "Return the length of the segment" )                               \
        .def( "vertices", &owned_vertices< Segment##dimension##D >,            \
            "Return copies of the two end points" )                            \
        .def( "bounding_box", &Segment##dimension##D::bounding_box,            \
            "Return the bounding box of the segment" )

#define PYTHON_INFINITE_LINE( dimension )                                      \
    const auto line##dimension =                                               \
        "InfiniteLine" + std::to_string( dimension ) + "D";                    \
    pybind11::class_< InfiniteLine##dimension##D >( module,                    \
        line##dimension.c_str(),                                               \
        "Infinite line defined by a direction and an origin" )                 \
        .def( pybind11::init< const Vector< dimension >&,                      \
                  const Point< dimension >& >(),                               \
            pybind11::keep_alive< 1, 3 >(), pybind11::arg( "direction" ),      \
            pybind11::arg( "origin" ),                                         \
            "Build a line through the origin point with the given "            \
            "direction (normalized). The origin is kept alive by the line" )   \
        .def( pybind11::init< const Segment< dimension >& >(),                 \
            pybind11::keep_alive< 1, 2 >(), pybind11::arg( "segment" ),        \
            "Build the line supporting the segment, whose origin is its "      \
            "first point. The segment is kept alive by the line" )             \
        .def( "origin", &InfiniteLine##dimension##D::origin,                   \
            "Return a point of the line" )                                     \
        .def( "direction", &InfiniteLine##dimension##D::direction,             \
            "Return the direction of the line, normalized to length 1" )

#define PYTHON_TRIANGLE( dimension )                                           \
    const auto triangle##dimension =                                           \
        "Triangle" + std::to_string( dimension ) + "D";                        \
    define_string_representation(                                              \
        pybind11::class_< Triangle##dimension##D >( module,                    \
            triangle##dimension.c_str(),                                       \
            "Triangle defined by its three vertices" ),                        \
        []( const Triangle##dimension##D& triangle ) {                         \
            const auto& vertices = triangle.vertices();                        \
            return python_call_repr( "Triangle" #dimension "D",                \
                { python_repr( geode::Point< dimension >{ vertices[0] } ),     \
                    python_repr( geode::Point< dimension >{ vertices[1] } ),   \
                    python_repr(                                               \
                        geode::Point< dimension >{ vertices[2] } ) } );        \
        } )                                                                    \
        .def( pybind11::init< const Point< dimension >&,                       \
                  const Point< dimension >&, const Point< dimension >& >(),    \
            pybind11::keep_alive< 1, 2 >(), pybind11::keep_alive< 1, 3 >(),    \
            pybind11::keep_alive< 1, 4 >(), pybind11::arg( "point0" ),         \
            pybind11::arg( "point1" ), pybind11::arg( "point2" ),              \
            "Build a triangle from its three vertices. The points are kept "   \
            "alive by the triangle" )                                          \
        .def( "vertices", &owned_vertices< Triangle##dimension##D >,           \
            "Return copies of the three vertices" )                            \
        .def( "bounding_box", &Triangle##dimension##D::bounding_box,           \
            "Return the bounding box of the triangle" )                        \
        .def( "minimum_height", &Triangle##dimension##D::minimum_height,       \
            "Return the smallest height of the triangle: the distance from "   \
            "the "                                                             \
            "vertex opposite to the longest edge to this edge" )               \
        .def( "is_degenerated", &Triangle##dimension##D::is_degenerated,       \
            "Return True if the smallest height of the triangle is below the " \
            "global epsilon" )                                                 \
        .def( "longest_edge_index",                                            \
            &Triangle##dimension##D::longest_edge_index,                       \
            "Return the index of the longest edge (edge i goes from vertex i " \
            "to "                                                              \
            "vertex i+1)" )                                                    \
        .def( "smallest_edge_index",                                           \
            &Triangle##dimension##D::smallest_edge_index,                      \
            "Return the index of the smallest edge (edge i goes from vertex "  \
            "i to "                                                            \
            "vertex i+1)" )                                                    \
        .def( "barycenter", &Triangle##dimension##D::barycenter,               \
            "Return the centroid of the triangle (mean of its vertices)" )

#define PYTHON_SPHERE( dimension )                                             \
    const auto sphere##dimension =                                             \
        "Sphere" + std::to_string( dimension ) + "D";                          \
    pybind11::class_< Sphere##dimension##D >( module,                          \
        sphere##dimension.c_str(),                                             \
        "Sphere (circle in 2D) defined by its center and radius" )             \
        .def( pybind11::init< const Point< dimension >&, double >(),           \
            pybind11::keep_alive< 1, 2 >(), pybind11::arg( "origin" ),         \
            pybind11::arg( "radius" ),                                         \
            "Build a sphere from its center and its radius. The center is "    \
            "kept alive by the sphere" )                                       \
        .def( "origin", &Sphere##dimension##D::origin,                         \
            "Return the center of the sphere" )                                \
        .def( "radius", &Sphere##dimension##D::radius,                         \
            "Return the radius of the sphere" )                                \
        .def( "bounding_box", &Sphere##dimension##D::bounding_box,             \
            "Return the bounding box of the sphere" )

#define PYTHON_RAY( dimension )                                                \
    const auto ray##dimension = "Ray" + std::to_string( dimension ) + "D";     \
    pybind11::class_< Ray##dimension##D >( module, ray##dimension.c_str(),     \
        "Half line defined by a direction and an origin" )                     \
        .def( pybind11::init< const Vector< dimension >&,                      \
                  const Point< dimension >& >(),                               \
            pybind11::keep_alive< 1, 3 >(), pybind11::arg( "direction" ),      \
            pybind11::arg( "origin" ),                                         \
            "Build the ray starting at the origin point with the given "       \
            "direction (normalized). The origin is kept alive by the ray" )    \
        .def( pybind11::init< const Segment< dimension >& >(),                 \
            pybind11::keep_alive< 1, 2 >(), pybind11::arg( "segment" ),        \
            "Build the ray starting at the first point of the segment and "    \
            "going toward the second. The segment is kept alive by the ray" )  \
        .def( "origin", &Ray##dimension##D::origin,                            \
            "Return the point where the ray starts" )                          \
        .def( "direction", &Ray##dimension##D::direction,                      \
            "Return the direction of the ray, normalized to length 1" )

#define PYTHON_BOUNDING_BOX_AND_BASIC_OBJECTS( dimension )                     \
    const auto bbox##dimension =                                               \
        "BoundingBox" + std::to_string( dimension ) + "D";                     \
    auto pybbox##dimension =                                                   \
        module.attr( bbox##dimension.c_str() )                                 \
            .cast< pybind11::class_< BoundingBox##dimension##D > >();          \
    pybbox##dimension.def( "intersects_ray",                                   \
        static_cast< bool ( BoundingBox##dimension##D::* )(                    \
            const Ray< dimension >& ) const >(                                 \
            &BoundingBox##dimension##D::intersects ),                          \
        pybind11::arg( "ray" ),                                                \
        "Return True if the ray crosses, is inside or contains the bounding "  \
        "box" );                                                               \
    pybbox##dimension.def( "intersects_infinite_line",                         \
        static_cast< bool ( BoundingBox##dimension##D::* )(                    \
            const InfiniteLine< dimension >& ) const >(                        \
            &BoundingBox##dimension##D::intersects ),                          \
        pybind11::arg( "line" ),                                               \
        "Return True if the line crosses, is inside or contains the bounding " \
        "box" );                                                               \
    pybbox##dimension.def( "intersects_segment",                               \
        static_cast< bool ( BoundingBox##dimension##D::* )(                    \
            const Segment< dimension >& ) const >(                             \
            &BoundingBox##dimension##D::intersects ),                          \
        pybind11::arg( "segment" ),                                            \
        "Return True if the segment crosses, is inside or contains the "       \
        "bounding box" )

namespace geode
{
    void define_basic_objects( pybind11::module& module )
    {
        pybind11::class_< Plane >(
            module, "Plane", "Plane defined by its normal and a point" )
            .def( pybind11::init< const Vector3D&, const Point3D& >(),
                pybind11::keep_alive< 1, 3 >(), pybind11::arg( "normal" ),
                pybind11::arg( "origin" ),
                "Build a plane from its normal and a point of the plane. The "
                "point is kept alive by the plane" )
            .def( "plane_constant", &Plane::plane_constant,
                "Return the constant d of the plane equation n . x + d = 0, n "
                "being "
                "the unit normal" )
            .def( "origin", &Plane::origin,
                "Return the point of the plane given at the construction" )
            .def( "normal", &Plane::normal,
                "Return the normal of the plane, normalized to length 1" );

        pybind11::class_< Tetrahedron >(
            module, "Tetrahedron", "Tetrahedron defined by its four vertices" )
            .def( pybind11::init< const Point3D&, const Point3D&,
                      const Point3D&, const Point3D& >(),
                pybind11::keep_alive< 1, 2 >(), pybind11::keep_alive< 1, 3 >(),
                pybind11::keep_alive< 1, 4 >(), pybind11::keep_alive< 1, 5 >(),
                pybind11::arg( "point0" ), pybind11::arg( "point1" ),
                pybind11::arg( "point2" ), pybind11::arg( "point3" ),
                "Build a tetrahedron from its four vertices. The points are "
                "kept alive by the tetrahedron" )
            .def( "vertices", &owned_vertices< Tetrahedron >,
                "Return copies of the four vertices" )
            .def( "bounding_box", &Tetrahedron::bounding_box,
                "Return the bounding box of the tetrahedron" );

        pybind11::class_< Circle >( module, "Circle",
            "Circle of the 3D space defined by its supporting plane and "
            "radius" )
            .def( pybind11::init< const Plane&, double >(),
                pybind11::keep_alive< 1, 2 >(), pybind11::arg( "plane" ),
                pybind11::arg( "radius" ),
                "Build a circle from its supporting plane and its radius. The "
                "plane is kept alive by the circle" )
            .def( "plane", &Circle::plane, pybind11::keep_alive< 0, 1 >(),
                "Return the supporting plane of the circle" )
            .def( "radius", &Circle::radius, "Return the radius of the circle" )
            .def( "bounding_box", &Circle::bounding_box,
                "Return the bounding box of the circle" );

        pybind11::native_enum< SIDE >( module, "Side", "enum.Enum",
            "Side of an oriented object on which a point lies, or sign of a "
            "quantity: positive, negative, or zero when on the object" )
            .value( "positive", SIDE::positive )
            .value( "negative", SIDE::negative )
            .value( "zero", SIDE::zero )
            .finalize();

        pybind11::native_enum< POSITION >( module, "Position", "enum.Enum",
            "Position of a point relative to a segment, triangle or "
            "tetrahedron: strictly outside, strictly inside, on a vertex, an "
            "edge or a facet (numbered as the vertices of the object), or "
            "parallel for a parallel or coplanar configuration" )
            .value( "outside", POSITION::outside )
            .value( "inside", POSITION::inside )
            .value( "vertex0", POSITION::vertex0 )
            .value( "vertex1", POSITION::vertex1 )
            .value( "vertex2", POSITION::vertex2 )
            .value( "vertex3", POSITION::vertex3 )
            .value( "edge0", POSITION::edge0 )
            .value( "edge1", POSITION::edge1 )
            .value( "edge2", POSITION::edge2 )
            .value( "edge01", POSITION::edge01 )
            .value( "edge02", POSITION::edge02 )
            .value( "edge03", POSITION::edge03 )
            .value( "edge12", POSITION::edge12 )
            .value( "edge13", POSITION::edge13 )
            .value( "edge23", POSITION::edge23 )
            .value( "facet0", POSITION::facet0 )
            .value( "facet1", POSITION::facet1 )
            .value( "facet2", POSITION::facet2 )
            .value( "facet3", POSITION::facet3 )
            .value( "parallel", POSITION::parallel )
            .finalize();

        PYTHON_SEGMENT( 2 );
        PYTHON_SEGMENT( 3 );
        PYTHON_INFINITE_LINE( 2 );
        PYTHON_INFINITE_LINE( 3 );
        PYTHON_SPHERE( 2 );
        PYTHON_SPHERE( 3 );
        PYTHON_RAY( 2 );
        PYTHON_RAY( 3 );
        PYTHON_TRIANGLE( 2 );
        PYTHON_TRIANGLE( 3 )
            .def( "normal",
                static_cast< std::optional< Vector3D > ( Triangle< 3 >::* )()
                        const >( &Triangle3D::normal ),
                "Return the normal of the triangle, or None if it is "
                "degenerated" )
            .def( "plane",
                static_cast< std::optional< Plane > ( Triangle< 3 >::* )()
                        const >( &Triangle3D::plane ),
                pybind11::keep_alive< 0, 1 >(),
                "Return the supporting plane of the triangle, or None if it is "
                "degenerated" );

        pybind11::class_< Cylinder >( module, "Cylinder",
            "Cylinder defined by its axis (a segment) and radius" )
            .def( pybind11::init< Segment3D, double >(),
                pybind11::keep_alive< 1, 2 >(), pybind11::arg( "axis" ),
                pybind11::arg( "radius" ),
                "Build a cylinder from its axis and its radius. The axis is "
                "kept alive by the cylinder" )
            .def( "axis", &Cylinder::axis, pybind11::keep_alive< 0, 1 >(),
                "Return the axis (a segment) of the cylinder" )
            .def( "radius", &Cylinder::radius,
                "Return the radius of the cylinder" );
        PYTHON_BOUNDING_BOX_AND_BASIC_OBJECTS( 2 );
        PYTHON_BOUNDING_BOX_AND_BASIC_OBJECTS( 3 );
    }
} // namespace geode
