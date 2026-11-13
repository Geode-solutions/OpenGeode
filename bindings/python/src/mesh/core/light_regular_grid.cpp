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

#include "../../common.hpp"

#include <geode/basic/attribute_manager.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/grid_coordinate_reference_system_manager.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>

namespace
{
    template < geode::index_t dimension >
    void define_python_light_regular_grid( pybind11::module& module )
    {
        const auto class_name =
            absl::StrCat( "LightRegularGrid", dimension, "D" );
        pybind11::class_< geode::LightRegularGrid< dimension >,
            geode::Grid< dimension >, geode::Identifier,
            pybind11::smart_holder >( module, class_name.c_str(),
            "Regular grid that only stores its geometry (origin, cell numbers "
            "and lengths), without explicit vertices or cells" )
            .def( pybind11::init< geode::Point< dimension >,
                      std::array< geode::index_t, dimension >,
                      std::array< double, dimension > >(),
                "Build a grid from its origin, the number of cells and the "
                "cell length in each direction, the directions being the axes",
                pybind11::arg( "origin" ), pybind11::arg( "cells_number" ),
                pybind11::arg( "cells_length" ) )
            .def( pybind11::init< geode::Point< dimension >,
                      std::array< geode::index_t, dimension >,
                      std::array< geode::Vector< dimension >, dimension > >(),
                "Build a grid from its origin, the number of cells in each "
                "direction and the vector of a cell along each direction",
                pybind11::arg( "origin" ), pybind11::arg( "cells_number" ),
                pybind11::arg( "directions" ) )
            .def( "native_extension",
                &geode::LightRegularGrid< dimension >::native_extension,
                "Return the file extension of the native format of the grid" )
            .def( "grid_coordinate_reference_system_manager",
                &geode::LightRegularGrid<
                    dimension >::grid_coordinate_reference_system_manager,
                pybind11::return_value_policy::reference_internal,
                "Return the manager of the grid coordinate reference systems" );
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
