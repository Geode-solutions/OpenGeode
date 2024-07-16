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

#include "../../../common.hpp"

#include <geode/basic/uuid.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/builder/blocks_builder.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/blocks.hpp>

#define PYTHON_BLOCKS_BUILDER( dimension )                                     \
    const auto name##dimension =                                               \
        "BlocksBuilder" + std::to_string( dimension ) + "D";                   \
    pybind11::class_< BlocksBuilder##dimension##D >(                           \
        module, name##dimension.c_str() )                                      \
        .def( "block_mesh_builder",                                            \
            &BlocksBuilder##dimension##D::block_mesh_builder<                  \
                SolidMesh< dimension > > )                                     \
        .def( "set_block_name", &BlocksBuilder##dimension##D::set_block_name )

namespace geode
{
    void define_blocks_builder( pybind11::module& module )
    {
        PYTHON_BLOCKS_BUILDER( 3 );
    }
} // namespace geode
