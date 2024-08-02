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

#include "../common.hpp"

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#define PYTHON_BOUNDING_BOX( dimension )                                       \
    const auto name##dimension =                                               \
        "BoundingBox" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< BoundingBox##dimension##D >(                             \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init<>() )                                             \
        .def( "add_box", &BoundingBox##dimension##D::add_box )                 \
        .def( "add_point", &BoundingBox##dimension##D::add_point )             \
        .def( "contains", &BoundingBox##dimension##D::contains )               \
        .def( "intersects_bbox",                                               \
            static_cast< bool ( BoundingBox##dimension##D::* )(                \
                const BoundingBox< dimension >& ) const >(                     \
                &BoundingBox##dimension##D::intersects ) )                     \
        .def( "intersects_ray",                                                \
            static_cast< bool ( BoundingBox##dimension##D::* )(                \
                const Ray< dimension >& ) const >(                             \
                &BoundingBox##dimension##D::intersects ) )                     \
        .def( "min", &BoundingBox##dimension##D::min )                         \
        .def( "max", &BoundingBox##dimension##D::max )                         \
        .def( "center", &BoundingBox##dimension##D::center )                   \
        .def( "diagonal", &BoundingBox##dimension##D::diagonal )               \
        .def( "smallest_length", &BoundingBox##dimension##D::smallest_length ) \
        .def(                                                                  \
            "largest_length", &BoundingBox##dimension##D::largest_dimension )

namespace geode
{
    void define_bounding_box( pybind11::module& module )
    {
        PYTHON_BOUNDING_BOX( 2 );
        PYTHON_BOUNDING_BOX( 3 );
    }
} // namespace geode
