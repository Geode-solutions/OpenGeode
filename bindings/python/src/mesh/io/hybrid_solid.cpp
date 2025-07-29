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

#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/io/hybrid_solid_input.hpp>
#include <geode/mesh/io/hybrid_solid_output.hpp>

#define PYTHON_HYBRID_SOLID_IO( dimension )                                    \
    const auto save##dimension =                                               \
        "save_hybrid_solid" + std::to_string( dimension ) + "D";               \
    module.def( save##dimension.c_str(), &save_hybrid_solid< dimension > );    \
    const auto load##dimension =                                               \
        "load_hybrid_solid" + std::to_string( dimension ) + "D";               \
    module.def( load##dimension.c_str(),                                       \
        static_cast< std::unique_ptr< HybridSolid< dimension > > ( * )(        \
            std::string_view ) >( &load_hybrid_solid< dimension > ) );         \
    const auto check##dimension =                                              \
        "hybrid_solid_additional_files" + std::to_string( dimension ) + "D";   \
    module.def( check##dimension.c_str(),                                      \
        &hybrid_solid_additional_files< dimension > );                         \
    const auto priority##dimension =                                           \
        "hybrid_solid_object_priority" + std::to_string( dimension ) + "D";    \
    module.def( priority##dimension.c_str(),                                   \
        &hybrid_solid_object_priority< dimension > );                          \
    const auto loadable##dimension =                                           \
        "is_hybrid_solid_loadable" + std::to_string( dimension ) + "D";        \
    module.def(                                                                \
        loadable##dimension.c_str(), &is_hybrid_solid_loadable< dimension > ); \
    PYTHON_INPUT_MESH_CLASS( std::unique_ptr< HybridSolid< dimension > >,      \
        "HybridSolid" + std::to_string( dimension ) + "D" );                   \
    const auto saveable##dimension =                                           \
        "is_hybrid_solid_saveable" + std::to_string( dimension ) + "D";        \
    module.def(                                                                \
        saveable##dimension.c_str(), &is_hybrid_solid_saveable< dimension > ); \
    PYTHON_FACTORY_CLASS( HybridSolidInputFactory##dimension##D );             \
    PYTHON_FACTORY_CLASS( HybridSolidOutputFactory##dimension##D )

namespace geode
{
    void define_hybrid_solid_io( pybind11::module& module )
    {
        PYTHON_HYBRID_SOLID_IO( 3 );
    }
} // namespace geode
