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

#include <geode/mesh/builder/grid_coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/core/grid_coordinate_reference_system.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>

namespace
{
    template < geode::index_t dimension >
    void define_python_grid_crs_manager_builder( pybind11::module& module )
    {
        using Builder =
            geode::GridCoordinateReferenceSystemManagerBuilder< dimension >;
        const auto name = absl::StrCat(
            "GridCoordinateReferenceSystemManagerBuilder", dimension, "D" );
        pybind11::class_< Builder >( module, name.c_str() )
            .def( pybind11::init< geode::LightRegularGrid< dimension >& >() )
            .def( "register_coordinate_reference_system",
                []( Builder& builder, std::string_view crs_name,
                    std::shared_ptr< geode::GridCoordinateReferenceSystem<
                        dimension > >& crs ) {
                    builder.register_coordinate_reference_system(
                        crs_name, std::move( crs ) );
                } )
            .def( "delete_coordinate_reference_system",
                &Builder::delete_coordinate_reference_system )
            .def( "set_active_coordinate_reference_system",
                &Builder::set_active_coordinate_reference_system );
    }
} // namespace

namespace geode
{
    void define_grid_crs_manager_builder( pybind11::module& module )
    {
        define_python_grid_crs_manager_builder< 2 >( module );
        define_python_grid_crs_manager_builder< 3 >( module );
    }
} // namespace geode
