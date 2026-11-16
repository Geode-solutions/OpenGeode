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

#include <geode/model/mixin/core/component_registry.hpp>

namespace geode
{
    void define_component_registry( pybind11::module& module )
    {
        pybind11::class_< ComponentRegistry, pybind11::smart_holder >( module,
            "ComponentRegistry",
            "Registry of the uuids of the components of a model, by component "
            "type, separating the components described by a mesh from the "
            "collections" )
            .def( pybind11::init<>(), "Build an empty registry" )
            .def( "mesh_components", &ComponentRegistry::mesh_components,
                "Return a dict giving, for each type of mesh component "
                "(Corner, Line, Surface, Block), the list of the uuids of the "
                "components of this type" )
            .def( "collection_components",
                &ComponentRegistry::collection_components,
                "Return a dict giving, for each type of collection component "
                "(ModelBoundary and the collections), the list of the uuids of "
                "the components of this type" );
    }
} // namespace geode
