/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include "../../common.hpp"

#include <geode/basic/attribute_manager.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/light_regular_grid.hpp>

namespace
{
    template < geode::index_t dimension >
    void define_python_light_regular_grid( pybind11::module& module )
    {
        const auto class_name =
            absl::StrCat( "LightRegularGrid", dimension, "D" );
        pybind11::class_< geode::LightRegularGrid< dimension >,
            geode::Grid< dimension >, geode::Identifier >(
            module, class_name.c_str() )
            .def( pybind11::init< geode::Point< dimension >,
                std::array< geode::index_t, dimension >,
                std::array< double, dimension > >() )
            .def( pybind11::init< geode::Point< dimension >,
                std::array< geode::index_t, dimension >,
                std::array< geode::Vector< dimension >, dimension > >() )
            .def( "vertex_attribute_manager",
                &geode::Grid< dimension >::grid_vertex_attribute_manager,
                pybind11::return_value_policy::reference )
            .def( dimension == 2 ? "polygon_attribute_manager"
                                 : "polyhedron_attribute_manager",
                &geode::Grid< dimension >::cell_attribute_manager,
                pybind11::return_value_policy::reference )
            .def( "native_extension",
                &geode::LightRegularGrid< dimension >::native_extension );
    }
} // namespace

namespace geode
{
    void define_light_regular_grid( pybind11::module& module )
    {
        define_python_light_regular_grid< 2 >( module );
        define_python_light_regular_grid< 3 >( module );
    }
} // namespace geode
