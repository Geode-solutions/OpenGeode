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

#include <geode/geometry/vector.hpp>

#define PYTHON_VECTOR( dimension )                                             \
    const auto name##dimension = "Vector" + std::to_string( dimension ) + "D"; \
    pybind11::class_< Vector##dimension##D, Point##dimension##D >( module,     \
        name##dimension.c_str(), "Vector of the space, derived from Point" )   \
        .def( pybind11::init<>(), "Build a null vector" )                      \
        .def( pybind11::init< std::array< double, dimension > >(),             \
            pybind11::arg( "values" ), "Build a vector from its coordinates" ) \
        .def( pybind11::init< const Point##dimension##D&,                      \
                  const Point##dimension##D& >(),                              \
            pybind11::arg( "from_point" ), pybind11::arg( "to_point" ),        \
            "Build the vector going from the first point to the second one" )  \
        .def( "length", &Vector##dimension##D::length,                         \
            "Return the length of the vector" )                                \
        .def( "length2", &Vector##dimension##D::length2,                       \
            "Return the squared length of the vector" )                        \
        .def( "normalize", &Vector##dimension##D::normalize,                   \
            "Return the vector divided by its length" )                        \
        .def( "dot", &Vector##dimension##D::dot, pybind11::arg( "other" ),     \
            "Return the dot product with the other vector" )

namespace geode
{
    void define_vector( pybind11::module& module )
    {
        PYTHON_VECTOR( 1 );
        PYTHON_VECTOR( 2 );
        PYTHON_VECTOR( 3 ).def( "cross", &Vector3D::cross,
            pybind11::arg( "other" ),
            "Return the cross product with the other vector" );
    }
} // namespace geode
