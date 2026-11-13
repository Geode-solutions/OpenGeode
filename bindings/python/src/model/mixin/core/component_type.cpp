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

#include <geode/model/mixin/core/component_type.hpp>

namespace geode
{
    void define_component_type( pybind11::module& module )
    {
        define_string_representation(
            pybind11::class_< ComponentType >( module, "ComponentType",
                "Type of a model component, given by a name such as 'Corner', "
                "'Line', 'Surface', 'Block' or 'ModelBoundary'" ),
            []( const ComponentType& type ) {
                return std::string{ type.get() };
            },
            []( const ComponentType& type ) {
                return python_call_repr(
                    "ComponentType", { python_repr( type.get() ) } );
            } )
            .def(
                pybind11::init<>(), "Build a ComponentType with an empty name" )
            .def( pybind11::init< std::string >(), pybind11::arg( "value" ),
                "Build a ComponentType from its name" )
            .def( "get", &ComponentType::get, "Return the name of the type" )
            .def( "matches", &ComponentType::operator==,
                pybind11::arg( "other" ),
                "Return True if both types have the same name" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const ComponentType& type ) {
                return std::hash< ComponentType >{}( type );
            } );

        auto cls_ComponentID =
            define_string_representation(
                pybind11::class_< ComponentID >( module, "ComponentID",
                    "Identifier of a component of a model: its type "
                    "(ComponentType) and its uuid. Hashable: do not modify "
                    "type or id of a ComponentID used as a dict key or in a "
                    "set" ),
                []( const ComponentID& id ) {
                    return python_call_repr( "ComponentID",
                        { python_repr( id.type ), python_repr( id.id ) } );
                } )
                .def( pybind11::init<>(),
                    "Build an undefined ComponentID: type 'undefined' and a "
                    "newly generated uuid" )
                .def( pybind11::init< ComponentType, uuid >(),
                    pybind11::arg( "component_type" ),
                    pybind11::arg( "input_id" ),
                    "Build the identifier of the component of the given type "
                    "and uuid" )

                .def(
                    pybind11::self == pybind11::self, pybind11::arg( "other" ) )
                .def(
                    pybind11::self != pybind11::self, pybind11::arg( "other" ) )
                .def(
                    pybind11::self < pybind11::self, pybind11::arg( "other" ) )
                .def( "__hash__", []( const ComponentID& id ) {
                    return std::hash< ComponentID >{}( id );
                } );
        def_readwrite_named( cls_ComponentID, "type", &ComponentID::type,
            "Type of the component" );
        def_readwrite_named(
            cls_ComponentID, "id", &ComponentID::id, "Uuid of the component" );
    }
} // namespace geode
