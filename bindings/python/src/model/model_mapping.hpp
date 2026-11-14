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

#pragma once

#include <string>
#include <type_traits>
#include <vector>

#include <absl/strings/str_cat.h>

#include "../common.hpp"

#include <geode/model/mixin/core/component_type.hpp>
#include <geode/model/representation/core/mapping.hpp>

namespace geode
{
    namespace detail
    {
        template < typename Mapping >
        struct is_uuid_list : std::false_type
        {
        };

        template <>
        struct is_uuid_list< std::vector< uuid > > : std::true_type
        {
        };
    } // namespace detail

    /*!
     * Bind ModelMapping< Mapping > (one Mapping per component type) as the
     * Python class name. mapping_name is the Python name of Mapping, used in
     * the docstrings. A Mapping which is a list of uuids
     * (ModelAddedComponentMapping...) is returned as a copy by at.
     */
    template < typename Mapping >
    pybind11::class_< ModelMapping< Mapping > > define_model_mapping(
        pybind11::module& module,
        const char* name,
        const char* doc,
        std::string_view mapping_name )
    {
        using Model = ModelMapping< Mapping >;
        pybind11::class_< Model > mapping( module, name, doc );
        mapping.def( pybind11::init<>(), "Build an empty mapping" )
            .def( pybind11::init( []( Model& other ) {
                return Model{ std::move( other ) };
            } ),
                pybind11::arg( "mapping" ),
                "Build a mapping by taking over the content of mapping, "
                "which is emptied" )
            .def( "has_mapping_type", &Model::has_mapping_type,
                pybind11::arg( "type" ),
                "Return True if there is a mapping for the components of the "
                "given type" )
            .def(
                "component_types",
                []( const Model& self ) {
                    std::vector< ComponentType > types;
                    types.reserve( self.components_mappings().size() );
                    for( const auto& [type, unused] :
                        self.components_mappings() )
                    {
                        types.push_back( type );
                    }
                    absl::c_sort( types, []( const ComponentType& lhs,
                                             const ComponentType& rhs ) {
                        return lhs.get() < rhs.get();
                    } );
                    return types;
                },
                "Return the list of the component types having a mapping, "
                "sorted by name" )
            .def(
                "at",
                []( Model& self, const ComponentType& type ) -> Mapping& {
                    if( !self.has_mapping_type( type ) )
                    {
                        throw pybind11::key_error( absl::StrCat(
                            "[at] no mapping for ", python_repr( type ) ) );
                    }
                    return self.at( type );
                },
                pybind11::arg( "type" ),
                pybind11::return_value_policy::reference_internal,
                ( detail::is_uuid_list< Mapping >::value
                        ? absl::StrCat( "Return a copy of the ", mapping_name,
                              " of the components of the given type; raise "
                              "KeyError if there is none for this type" )
                        : absl::StrCat( "Return the ", mapping_name,
                              " of the components of the given type, "
                              "modifiable; raise KeyError if there is none for "
                              "this type" ) )
                    .c_str() )
            .def( "emplace", &Model::emplace, pybind11::arg( "type" ),
                pybind11::arg( "mapping" ),
                absl::StrCat( "Add a copy of the ", mapping_name,
                    " as the mapping of the components of the given type; "
                    "raise OpenGeodeDataError if this type already has one" )
                    .c_str() )
            .def( "remove", &Model::remove, pybind11::arg( "type" ),
                "Remove the mapping of the components of the given type; "
                "nothing is done if there is none" );
        return mapping;
    }
} // namespace geode
