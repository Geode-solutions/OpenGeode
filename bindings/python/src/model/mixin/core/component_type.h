/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <pybind11/operators.h>

#include <geode/model/mixin/core/component_type.h>

namespace geode
{
    void define_component_type( pybind11::module& module )
    {
        pybind11::class_< ComponentType >( module, "ComponentType" )
            .def( pybind11::init<>() )
            .def( pybind11::init< std::string >() )
            .def( "get", &ComponentType::get )
            .def( "matches", &ComponentType::operator== );

        pybind11::class_< ComponentID >( module, "ComponentID" )
            .def( pybind11::init<>() )
            .def( pybind11::init< ComponentType, uuid >() )
            .def( "id",
                (const uuid& (ComponentID::*) () const&) &ComponentID::id )
            .def( "type", (const ComponentType& (ComponentID::*) ()
                                  const&) &ComponentID::type )
            .def( "string", &ComponentID::string )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self );
    }
} // namespace geode
