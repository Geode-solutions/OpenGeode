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

#include "../../../common.hpp"

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/blocks.hpp>

#define PYTHON_BLOCKS( dimension )                                             \
    const auto name##dimension = "Blocks" + std::to_string( dimension ) + "D"; \
    pybind11::class_< Blocks##dimension##D, pybind11::smart_holder >( module,  \
        name##dimension.c_str(),                                               \
        "Storage of the Block components of a model, read through the model "  \
        "class" )                                                              \
        .def( "nb_blocks", &Blocks##dimension##D::nb_blocks,                   \
            "Return the number of Block components of the model" )             \
        .def( "has_block", &Blocks##dimension##D::has_block,                   \
            pybind11::arg( "id" ),                                             \
            "Return True if the model has a Block with the given uuid "        \
            "(False once it is removed)" )                                     \
        .def( "block", &Blocks##dimension##D::block, pybind11::arg( "id" ),    \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the Block with the given uuid; raise an error if the "     \
            "model has none" )                                                 \
        .def(                                                                  \
            "blocks",                                                          \
            []( const Blocks##dimension##D& self ) {                           \
                std::vector< const Block##dimension##D* > components;          \
                for( const auto& component : self.blocks() )                   \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the list of the Block components of the model" )

namespace geode
{
    void define_blocks( pybind11::module& module )
    {
        PYTHON_BLOCKS( 3 );
    }
} // namespace geode
