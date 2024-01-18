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

#include <string>

#include "../../basic/factory.h"
#include "../../basic/input.h"
#include "../../common.h"

#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/triangulated_surface_input.h>
#include <geode/mesh/io/triangulated_surface_output.h>

#define PYTHON_TRIANGULATED_SURFACE_IO( dimension )                            \
    const auto save##dimension =                                               \
        "save_triangulated_surface" + std::to_string( dimension ) + "D";       \
    module.def(                                                                \
        save##dimension.c_str(), &save_triangulated_surface< dimension > );    \
    const auto load##dimension =                                               \
        "load_triangulated_surface" + std::to_string( dimension ) + "D";       \
    module.def( load##dimension.c_str(),                                       \
        static_cast< std::unique_ptr<                                          \
            TriangulatedSurface< dimension > > ( * )( absl::string_view ) >(   \
            &load_triangulated_surface< dimension > ) );                       \
    const auto check##dimension = "check_triangulated_surface_missing_files"   \
                                  + std::to_string( dimension ) + "D";         \
    module.def( check##dimension.c_str(),                                      \
        &check_triangulated_surface_missing_files< dimension > );              \
    const auto loadable##dimension = "is_triangulated_surface_loadable"        \
                                     + std::to_string( dimension ) + "D";      \
    module.def( loadable##dimension.c_str(),                                   \
        &is_triangulated_surface_loadable< dimension > );                      \
    PYTHON_INPUT_MESH_CLASS(                                                   \
        std::unique_ptr< TriangulatedSurface< dimension > >,                   \
        "TriangulatedSurface" + std::to_string( dimension ) + "D" );           \
    const auto saveable##dimension = "is_triangulated_surface_saveable"        \
                                     + std::to_string( dimension ) + "D";      \
    module.def( saveable##dimension.c_str(),                                   \
        &is_triangulated_surface_saveable< dimension > );                      \
    PYTHON_FACTORY_CLASS( TriangulatedSurfaceInputFactory##dimension##D );     \
    PYTHON_FACTORY_CLASS( TriangulatedSurfaceOutputFactory##dimension##D )

namespace geode
{
    void define_triangulated_surface_io( pybind11::module& module )
    {
        PYTHON_TRIANGULATED_SURFACE_IO( 2 );
        PYTHON_TRIANGULATED_SURFACE_IO( 3 );
    }
} // namespace geode
