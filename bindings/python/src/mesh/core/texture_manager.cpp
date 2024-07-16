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

#include "../../common.h"

#include <geode/mesh/core/texture1d.hpp>
#include <geode/mesh/core/texture2d.hpp>
#include <geode/mesh/core/texture3d.hpp>
#include <geode/mesh/core/texture_manager.hpp>

#define PYTHON_TEXTURE_MANAGER( dimension )                                    \
    const auto name##dimension =                                               \
        "TextureManager" + std::to_string( dimension ) + "D";                  \
    pybind11::class_< TextureManager< dimension > >(                           \
        module, name##dimension.c_str() )                                      \
        .def( "find_or_create_texture",                                        \
            &TextureManager< dimension >::find_or_create_texture )             \
        .def( "find_texture", &TextureManager< dimension >::find_texture )     \
        .def( "texture_names", &TextureManager< dimension >::texture_names )   \
        .def( "texture_exists", &TextureManager< dimension >::texture_exists ) \
        .def( "delete_texture", &TextureManager< dimension >::delete_texture )

namespace geode
{
    void define_texture_manager( pybind11::module& module )
    {
        PYTHON_TEXTURE_MANAGER( 1 );
        PYTHON_TEXTURE_MANAGER( 2 );
        PYTHON_TEXTURE_MANAGER( 3 );
    }
} // namespace geode
