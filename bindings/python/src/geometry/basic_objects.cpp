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
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/bounding_box.h>
#include <geode/geometry/information.h>

#define PYTHON_SEGMENT( dimension )                                            \
    const auto segment##dimension =                                            \
        "Segment" + std::to_string( dimension ) + "D";                         \
    pybind11::class_< Segment##dimension##D >(                                 \
        module, segment##dimension.c_str() )                                   \
        .def( pybind11::init< const Point< dimension >&,                       \
            const Point< dimension >& >() )                                    \
        .def( "direction", &Segment##dimension##D::direction )                 \
        .def( "normalized_direction",                                          \
            &Segment##dimension##D::normalized_direction )                     \
        .def( "barycenter", &Segment##dimension##D::barycenter )               \
        .def( "length", &Segment##dimension##D::length )                       \
        .def( "vertices", &Segment##dimension##D::vertices )                   \
        .def( "bounding_box", &Segment##dimension##D::bounding_box )

#define PYTHON_INFINITE_LINE( dimension )                                      \
    const auto line##dimension =                                               \
        "InfiniteLine" + std::to_string( dimension ) + "D";                    \
    pybind11::class_< InfiniteLine##dimension##D >(                            \
        module, line##dimension.c_str() )                                      \
        .def( pybind11::init< const Vector< dimension >&,                      \
            const Point< dimension >& >() )                                    \
        .def( pybind11::init< const Segment< dimension >& >() )                \
        .def( "origin", &InfiniteLine##dimension##D::origin )                  \
        .def( "direction", &InfiniteLine##dimension##D::direction )

#define PYTHON_TRIANGLE( dimension )                                           \
    const auto triangle##dimension =                                           \
        "Triangle" + std::to_string( dimension ) + "D";                        \
    pybind11::class_< Triangle##dimension##D >(                                \
        module, triangle##dimension.c_str() )                                  \
        .def( pybind11::init< const Point< dimension >&,                       \
            const Point< dimension >&, const Point< dimension >& >() )         \
        .def( "vertices", &Triangle##dimension##D::vertices )                  \
        .def( "bounding_box", &Triangle##dimension##D::bounding_box )

#define PYTHON_SPHERE( dimension )                                             \
    const auto sphere##dimension =                                             \
        "Sphere" + std::to_string( dimension ) + "D";                          \
    pybind11::class_< Sphere##dimension##D >(                                  \
        module, sphere##dimension.c_str() )                                    \
        .def( pybind11::init< const Point< dimension >&, double >() )          \
        .def( "origin", &Sphere##dimension##D::origin )                        \
        .def( "radius", &Sphere##dimension##D::radius )                        \
        .def( "bounding_box", &Sphere##dimension##D::bounding_box )

namespace geode
{
    void define_basic_objects( pybind11::module& module )
    {
        PYTHON_SEGMENT( 2 );
        PYTHON_SEGMENT( 3 );
        PYTHON_INFINITE_LINE( 2 );
        PYTHON_INFINITE_LINE( 3 );
        PYTHON_SPHERE( 2 );
        PYTHON_SPHERE( 3 );
        PYTHON_TRIANGLE( 2 );
        PYTHON_TRIANGLE( 3 )
            .def( "normal",
                static_cast< absl::optional< Vector3D > ( Triangle< 3 >::* )()
                        const >( &Triangle3D::normal ) )
            .def( "plane",
                static_cast< absl::optional< Plane > ( Triangle< 3 >::* )()
                        const >( &Triangle3D::plane ) );

        pybind11::class_< Plane >( module, "Plane" )
            .def( pybind11::init< const Vector3D&, const Point3D& >() )
            .def( "plane_constant", &Plane::plane_constant )
            .def( "origin", &Plane::origin )
            .def( "normal", &Plane::normal );

        pybind11::class_< Tetrahedron >( module, "Tetrahedron" )
            .def( pybind11::init< const Point3D&, const Point3D&,
                const Point3D&, const Point3D& >() )
            .def( "vertices", &Tetrahedron::vertices )
            .def( "bounding_box", &Tetrahedron::bounding_box );

        pybind11::class_< Circle >( module, "Circle" )
            .def( pybind11::init< const Plane&, double >() )
            .def( "plane", &Circle::plane )
            .def( "radius", &Circle::radius )
            .def( "bounding_box", &Circle::bounding_box );

        pybind11::enum_< Side >( module, "Side" )
            .value( "positive", Side::positive )
            .value( "negative", Side::negative )
            .value( "zero", Side::zero )
            .export_values();

        pybind11::enum_< Position >( module, "Position" )
            .value( "outside", Position::outside )
            .value( "inside", Position::inside )
            .value( "vertex0", Position::vertex0 )
            .value( "vertex1", Position::vertex1 )
            .value( "vertex2", Position::vertex2 )
            .value( "vertex3", Position::vertex3 )
            .value( "edge0", Position::edge0 )
            .value( "edge1", Position::edge1 )
            .value( "edge2", Position::edge2 )
            .value( "edge01", Position::edge01 )
            .value( "edge02", Position::edge02 )
            .value( "edge03", Position::edge03 )
            .value( "edge12", Position::edge12 )
            .value( "edge13", Position::edge13 )
            .value( "edge23", Position::edge23 )
            .value( "facet0", Position::facet0 )
            .value( "facet1", Position::facet1 )
            .value( "facet2", Position::facet2 )
            .value( "facet3", Position::facet3 )
            .value( "parallel", Position::parallel )
            .export_values();
    }
} // namespace geode
