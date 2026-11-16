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

#include <geode/model/mixin/core/component.hpp>

#define PYTHON_COMPONENT( dimension )                                          \
    const auto name##dimension =                                               \
        "Component" + std::to_string( dimension ) + "D";                       \
    pybind11::class_< Component##dimension##D, Identifier,                     \
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Base class of the components of a model (Corner, Line, Surface, "     \
        "Block, ModelBoundary and the collections): it has a uuid, an "        \
        "optional name, a component type and an active flag" )                 \
        .def( "component_type", &Component##dimension##D::component_type,      \
            "Return the type of the component, for instance 'Surface'" )       \
        .def( "component_id", &Component##dimension##D::component_id,          \
            "Return the ComponentID (type and uuid) of the component" )        \
        .def( "is_active", &Component##dimension##D::is_active,                \
            "Return True if the component is active, which is the case by "    \
            "default" )

namespace geode
{
    void define_component( pybind11::module& module )
    {
        PYTHON_COMPONENT( 2 );
        PYTHON_COMPONENT( 3 );
    }
} // namespace geode
