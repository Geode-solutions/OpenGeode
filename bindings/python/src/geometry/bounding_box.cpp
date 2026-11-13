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

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#define PYTHON_BOUNDING_BOX( dimension )                                       \
    const auto name##dimension =                                               \
        "BoundingBox" + std::to_string( dimension ) + "D";                     \
    define_string_representation(                                              \
        pybind11::class_< BoundingBox##dimension##D >(                         \
            module, name##dimension.c_str(), "Axis-aligned bounding box" ) )   \
        .def( pybind11::init<>(), "Build an empty bounding box" )              \
        .def( "add_box", &BoundingBox##dimension##D::add_box,                  \
            pybind11::arg( "box" ),                                            \
            "Extend the bounding box to contain the given box" )               \
        .def( "add_point", &BoundingBox##dimension##D::add_point,              \
            pybind11::arg( "point" ),                                          \
            "Extend the bounding box to contain the given point" )             \
        .def( "extends", &BoundingBox##dimension##D::extends,                  \
            pybind11::arg( "length" ),                                         \
            "Move each face of the bounding box outwards by length, i.e. "     \
            "grow it by length on both sides in every direction" )             \
        .def( "contains_point",                                                \
            static_cast< bool ( BoundingBox##dimension##D::* )(                \
                const Point< dimension >& ) const >(                           \
                &BoundingBox##dimension##D::contains ),                        \
            pybind11::arg( "point" ),                                          \
            "Return True if the point is inside the bounding box" )            \
        .def( "contains_bbox",                                                 \
            static_cast< bool ( BoundingBox##dimension##D::* )(                \
                const BoundingBox< dimension >& ) const >(                     \
                &BoundingBox##dimension##D::contains ),                        \
            pybind11::arg( "bbox" ),                                           \
            "Return True if the given box is inside the bounding box" )        \
        .def( "intersects_bounding_box",                                       \
            static_cast< bool ( BoundingBox##dimension##D::* )(                \
                const BoundingBox< dimension >& ) const >(                     \
                &BoundingBox##dimension##D::intersects ),                      \
            pybind11::arg( "bbox" ),                                           \
            "Return True if the two bounding boxes intersect" )                \
        .def( "signed_distance", &BoundingBox##dimension##D::signed_distance,  \
            pybind11::arg( "point" ),                                          \
            "Return the distance from the point to the bounding box: 0 if "    \
            "the point is inside" )                                            \
        .def( "min", &BoundingBox##dimension##D::min,                          \
            "Return the corner with the smallest coordinates" )                \
        .def( "max", &BoundingBox##dimension##D::max,                          \
            "Return the corner with the largest coordinates" )                 \
        .def( "center", &BoundingBox##dimension##D::center,                    \
            "Return the center of the bounding box" )                          \
        .def( "diagonal", &BoundingBox##dimension##D::diagonal,                \
            "Return the vector from the min to the max corner" )               \
        .def( "smallest_length", &BoundingBox##dimension##D::smallest_length,  \
            "Return the axis and the length of the smallest side of the "      \
            "bounding box" )                                                   \
        .def( "largest_length", &BoundingBox##dimension##D::largest_length,    \
            "Return the axis and the length of the largest side of the "       \
            "bounding box" )                                                   \
        .def( "n_volume", &BoundingBox##dimension##D::n_volume,                \
            "Return the length (1D), area (2D) or volume (3D) of the "         \
            "bounding box" )

namespace geode
{
    void define_bounding_box( pybind11::module& module )
    {
        PYTHON_BOUNDING_BOX( 2 );
        PYTHON_BOUNDING_BOX( 3 );
    }
} // namespace geode
