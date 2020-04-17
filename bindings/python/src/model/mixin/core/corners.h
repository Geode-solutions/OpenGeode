/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/corners.h>

#define PYTHON_CORNERS( dimension )                                            \
    const auto name##dimension =                                               \
        "Corners" + std::to_string( dimension ) + "D";                         \
    pybind11::class_< Corners##dimension##D >(                                 \
        module, name##dimension.c_str() )                                      \
        .def( "nb_corners", &Corners##dimension##D::nb_corners )               \
        .def( "corner", &Corners##dimension##D::corner,                        \
            pybind11::return_value_policy::reference )                         \
        .def(                                                                  \
            "corners",                                                         \
            []( const Corners##dimension##D& self ) {                          \
                std::vector< const Corner##dimension##D* > components;         \
                for( const auto& component : self.corners() )                  \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference )

namespace geode
{
    void define_corners( pybind11::module& module )
    {
        PYTHON_CORNERS( 2 );
        PYTHON_CORNERS( 3 );
    }
} // namespace geode
