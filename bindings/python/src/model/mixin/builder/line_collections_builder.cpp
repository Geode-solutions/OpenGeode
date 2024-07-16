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

#include <geode/model/mixin/builder/line_collections_builder.hpp>

#define PYTHON_LINE_COLLECTIONS_BUILDER( dimension )                           \
    const auto name##dimension =                                               \
        "LineCollectionsBuilder" + std::to_string( dimension ) + "D";          \
    pybind11::class_< LineCollectionsBuilder##dimension##D >(                  \
        module, name##dimension.c_str() )                                      \
        .def( "set_line_collection_name",                                      \
            &LineCollectionsBuilder##dimension##D::set_line_collection_name )

namespace geode
{
    void define_line_collections_builder( pybind11::module& module )
    {
        PYTHON_LINE_COLLECTIONS_BUILDER( 2 );
        PYTHON_LINE_COLLECTIONS_BUILDER( 3 );
    }
} // namespace geode
