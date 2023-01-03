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

#include <pybind11/operators.h>

#include <geode/geometry/vector.h>

#define PYTHON_VECTOR( dimension )                                             \
    const auto name##dimension = "Vector" + std::to_string( dimension ) + "D"; \
    pybind11::class_< Vector##dimension##D, Point##dimension##D >(             \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init<>() )                                             \
        .def( pybind11::init< std::array< double, dimension > >() )            \
        .def( pybind11::init< const Point##dimension##D&,                      \
            const Point##dimension##D& >() )                                   \
        .def( "length", &Vector##dimension##D::length )                        \
        .def( "length2", &Vector##dimension##D::length2 )                      \
        .def( "normalize", &Vector##dimension##D::normalize )                  \
        .def( "dot", &Vector##dimension##D::dot )

namespace geode
{
    void define_vector( pybind11::module& module )
    {
        PYTHON_VECTOR( 2 );
        PYTHON_VECTOR( 3 ).def( "cross", &Vector3D::cross );
    }
} // namespace geode
