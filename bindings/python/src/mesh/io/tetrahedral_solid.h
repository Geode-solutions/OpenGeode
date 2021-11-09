/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include "../../basic/factory.h"

#include <geode/mesh/io/tetrahedral_solid_input.h>
#include <geode/mesh/io/tetrahedral_solid_output.h>

#define PYTHON_TETRAHEDRAL_SOLID_IO( dimension )                               \
    const auto save##dimension =                                               \
        "save_tetrahedral_solid" + std::to_string( dimension ) + "D";          \
    module.def(                                                                \
        save##dimension.c_str(), &save_tetrahedral_solid< dimension > );       \
    const auto load##dimension =                                               \
        "load_tetrahedral_solid" + std::to_string( dimension ) + "D";          \
    module.def( load##dimension.c_str(),                                       \
        ( std::unique_ptr< TetrahedralSolid< dimension > >( * )(               \
            absl::string_view ) )                                              \
            & load_tetrahedral_solid< dimension > );                           \
    PYTHON_FACTORY_CLASS( TetrahedralSolidInputFactory##dimension##D );        \
    PYTHON_FACTORY_CLASS( TetrahedralSolidOutputFactory##dimension##D )

namespace geode
{
    void define_tetrahedral_solid_io( pybind11::module& module )
    {
        PYTHON_TETRAHEDRAL_SOLID_IO( 3 );
    }
} // namespace geode
