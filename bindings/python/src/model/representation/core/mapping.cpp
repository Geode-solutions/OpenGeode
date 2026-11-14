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

#include <geode/model/representation/core/mapping.hpp>

namespace geode
{
    void define_copy_mapping( pybind11::module& module )
    {
        pybind11::class_< ModelCopyMapping >( module, "ModelCopyMapping",
            "Mapping, for each component type, between the uuids of the "
            "components of a source model and the uuids of their copies" )
            .def( pybind11::init<>(), "Build an empty mapping" )
            .def( pybind11::init( []( ModelCopyMapping& mapping ) {
                return ModelCopyMapping{ std::move( mapping ) };
            } ),
                pybind11::arg( "mapping" ),
                "Build a mapping by taking over the content of mapping, which "
                "is emptied" )
            .def(
                "at",
                []( ModelCopyMapping& mapping,
                    const ComponentType& type ) -> ModelCopyMapping::Mapping& {
                    if( !mapping.has_mapping_type( type ) )
                    {
                        throw pybind11::key_error( absl::StrCat(
                            "[at] no mapping for ", python_repr( type ) ) );
                    }
                    return mapping.at( type );
                },
                pybind11::arg( "type" ),
                pybind11::return_value_policy::reference_internal,
                "Return the BijectiveMapping of the components of the given "
                "type, modifiable; raise KeyError if there is none for this "
                "type" );
    }
    void define_generic_mapping( pybind11::module& module )
    {
        pybind11::class_< ModelGenericMapping >( module, "ModelGenericMapping",
            "Mapping, for each component type, between the uuids of the "
            "components of a model before and after an operation, a component "
            "possibly corresponding to several others" )
            .def( pybind11::init<>(), "Build an empty mapping" )
            .def( pybind11::init( []( ModelGenericMapping& mapping ) {
                return ModelGenericMapping{ std::move( mapping ) };
            } ),
                pybind11::arg( "mapping" ),
                "Build a mapping by taking over the content of mapping, which "
                "is emptied" )
            .def(
                "at",
                []( ModelGenericMapping& mapping, const ComponentType& type )
                    -> ModelGenericMapping::Mapping& {
                    if( !mapping.has_mapping_type( type ) )
                    {
                        throw pybind11::key_error( absl::StrCat(
                            "[at] no mapping for ", python_repr( type ) ) );
                    }
                    return mapping.at( type );
                },
                pybind11::arg( "type" ),
                pybind11::return_value_policy::reference_internal,
                "Return the GenericMapping of the components of the given "
                "type, modifiable; raise KeyError if there is none for this "
                "type" );
    }
} // namespace geode
