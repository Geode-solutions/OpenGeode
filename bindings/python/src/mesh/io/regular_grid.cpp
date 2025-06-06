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

#include <string>

#include "../../basic/factory.hpp"
#include "../../basic/input.hpp"
#include "../../common.hpp"

#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/io/regular_grid_input.hpp>
#include <geode/mesh/io/regular_grid_output.hpp>

#define PYTHON_REGULAR_GRID_IO( dimension )                                    \
    const auto save##dimension =                                               \
        "save_regular_grid" + std::to_string( dimension ) + "D";               \
    module.def( save##dimension.c_str(), &save_regular_grid< dimension > );    \
    const auto load##dimension =                                               \
        "load_regular_grid" + std::to_string( dimension ) + "D";               \
    module.def( load##dimension.c_str(),                                       \
        static_cast< std::unique_ptr< RegularGrid< dimension > > ( * )(        \
            std::string_view ) >( &load_regular_grid< dimension > ) );         \
    const auto check##dimension = "check_regular_grid_missing_files"           \
                                  + std::to_string( dimension ) + "D";         \
    module.def( check##dimension.c_str(),                                      \
        &check_regular_grid_missing_files< dimension > );                      \
    const auto loadable##dimension =                                           \
        "is_regular_grid_loadable" + std::to_string( dimension ) + "D";        \
    module.def(                                                                \
        loadable##dimension.c_str(), &is_regular_grid_loadable< dimension > ); \
    PYTHON_INPUT_MESH_CLASS( std::unique_ptr< RegularGrid< dimension > >,      \
        "RegularGrid" + std::to_string( dimension ) + "D" );                   \
    const auto saveable##dimension =                                           \
        "is_regular_grid_saveable" + std::to_string( dimension ) + "D";        \
    module.def(                                                                \
        saveable##dimension.c_str(), &is_regular_grid_saveable< dimension > ); \
    PYTHON_FACTORY_CLASS( RegularGridInputFactory##dimension##D );             \
    PYTHON_FACTORY_CLASS( RegularGridOutputFactory##dimension##D )

namespace geode
{
    void define_regular_grid_io( pybind11::module& module )
    {
        PYTHON_REGULAR_GRID_IO( 2 );
        PYTHON_REGULAR_GRID_IO( 3 );
    }
} // namespace geode
