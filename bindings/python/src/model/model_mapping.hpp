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
#include <string_view>
#include <type_traits>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>

#include "../common.hpp"

#include <geode/model/mixin/core/component_type.hpp>
#include <geode/model/representation/core/mapping.hpp>

namespace geode
{
    namespace detail
    {
        template < typename Mapping >
        struct is_std_vector : std::false_type
        {
        };

        template < typename Value, typename Allocator >
        struct is_std_vector< std::vector< Value, Allocator > > : std::true_type
        {
        };

        template < typename Mapping >
        struct is_bijective_mapping : std::false_type
        {
        };

        template < typename T1, typename T2 >
        struct is_bijective_mapping< BijectiveMapping< T1, T2 > >
            : std::true_type
        {
        };

        template < typename Mapping >
        struct is_generic_mapping : std::false_type
        {
        };

        template < typename T1, typename T2 >
        struct is_generic_mapping< GenericMapping< T1, T2 > > : std::true_type
        {
        };

        template < typename Model >
        const typename Model::Mapping& model_mapping_at(
            std::string_view context,
            const Model& model,
            const ComponentType& type )
        {
            if( !model.has_mapping_type( type ) )
            {
                throw pybind11::key_error( absl::StrCat(
                    "[", context, "] no mapping for ", python_repr( type ) ) );
            }
            return model.at( type );
        }

        /*!
         * Lookups and modifiers of the mapping of one component type, done
         * in place on the ModelMapping (no reference to the mapping of a
         * type reaches Python: emplace and remove would invalidate it).
         */
        template < typename Model, typename Input, typename Output >
        void define_model_mapping_per_type(
            pybind11::class_< Model >& mapping, std::string_view mapping_name )
        {
            using Mapping = typename Model::Mapping;
            mapping
                .def(
                    "has_mapping_input",
                    []( const Model& self, const ComponentType& type,
                        const Input& input ) {
                        return self.has_mapping_type( type )
                               && self.at( type ).has_mapping_input( input );
                    },
                    pybind11::arg( "type" ), pybind11::arg( "input" ),
                    "Return True if the value is mapped as an input in the "
                    "mapping of the given type (False if there is none)" )
                .def(
                    "has_mapping_output",
                    []( const Model& self, const ComponentType& type,
                        const Output& output ) {
                        return self.has_mapping_type( type )
                               && self.at( type ).has_mapping_output( output );
                    },
                    pybind11::arg( "type" ), pybind11::arg( "output" ),
                    "Return True if the value is mapped as an output in the "
                    "mapping of the given type (False if there is none)" )
                .def(
                    "in2out",
                    []( const Model& self, const ComponentType& type,
                        const Input& input ) {
                        return mapping_lookup< Mapping >( "in2out",
                            &Mapping::has_mapping_input, &Mapping::in2out )(
                            model_mapping_at( "in2out", self, type ), input );
                    },
                    pybind11::arg( "type" ), pybind11::arg( "input" ),
                    is_generic_mapping< Mapping >::value
                        ? "Return the list of the outputs mapped to the input "
                          "in the mapping of the given type. Raise KeyError if "
                          "there is no mapping for this type or the input is "
                          "not mapped"
                        : "Return the output mapped to the input in the "
                          "mapping of the given type. Raise KeyError if there "
                          "is no mapping for this type or the input is not "
                          "mapped" )
                .def(
                    "out2in",
                    []( const Model& self, const ComponentType& type,
                        const Output& output ) {
                        return mapping_lookup< Mapping >( "out2in",
                            &Mapping::has_mapping_output, &Mapping::out2in )(
                            model_mapping_at( "out2in", self, type ), output );
                    },
                    pybind11::arg( "type" ), pybind11::arg( "output" ),
                    is_generic_mapping< Mapping >::value
                        ? "Return the list of the inputs mapped to the output "
                          "in the mapping of the given type. Raise KeyError if "
                          "there is no mapping for this type or the output is "
                          "not mapped"
                        : "Return the input mapped to the output in the "
                          "mapping of the given type. Raise KeyError if there "
                          "is no mapping for this type or the output is not "
                          "mapped" )
                .def(
                    "map",
                    []( Model& self, const ComponentType& type, Input input,
                        Output output ) {
                        self[type].map(
                            std::move( input ), std::move( output ) );
                    },
                    pybind11::arg( "type" ), pybind11::arg( "input" ),
                    pybind11::arg( "output" ),
                    absl::StrCat( "Map the input to the output in the ",
                        mapping_name,
                        " of the given type, created empty if there is none (",
                        mapping_name, ".map)" )
                        .c_str() )
                .def(
                    "erase_in",
                    []( Model& self, const ComponentType& type, Input input ) {
                        if( self.has_mapping_type( type ) )
                        {
                            self.at( type ).erase_in( std::move( input ) );
                        }
                    },
                    pybind11::arg( "type" ), pybind11::arg( "input" ),
                    absl::StrCat( "Remove the pairs of the input from the ",
                        mapping_name,
                        " of the given type; nothing is done if there is no "
                        "mapping for this type or the input is not mapped" )
                        .c_str() )
                .def(
                    "erase_out",
                    []( Model& self, const ComponentType& type,
                        Output output ) {
                        if( self.has_mapping_type( type ) )
                        {
                            self.at( type ).erase_out( std::move( output ) );
                        }
                    },
                    pybind11::arg( "type" ), pybind11::arg( "output" ),
                    absl::StrCat( "Remove the pairs of the output from the ",
                        mapping_name,
                        " of the given type; nothing is done if there is no "
                        "mapping for this type or the output is not mapped" )
                        .c_str() );
            if constexpr( is_generic_mapping< Mapping >::value )
            {
                mapping.def(
                    "unmap",
                    []( Model& self, const ComponentType& type, Input input,
                        Output output ) {
                        if( self.has_mapping_type( type ) )
                        {
                            self.at( type ).unmap(
                                std::move( input ), std::move( output ) );
                        }
                    },
                    pybind11::arg( "type" ), pybind11::arg( "input" ),
                    pybind11::arg( "output" ),
                    absl::StrCat( "Remove the pair (input, output) from the ",
                        mapping_name,
                        " of the given type; nothing is done if it is not "
                        "mapped" )
                        .c_str() );
            }
        }

        template < typename Mapping >
        struct MappingValues
        {
        };

        template < typename T1, typename T2 >
        struct MappingValues< BijectiveMapping< T1, T2 > >
        {
            using Input = T1;
            using Output = T2;
        };

        template < typename T1, typename T2 >
        struct MappingValues< GenericMapping< T1, T2 > >
        {
            using Input = T1;
            using Output = T2;
        };
    } // namespace detail

    /*!
     * Bind ModelMapping< Mapping > (one Mapping per component type) as the
     * Python class name. mapping_name is the Python name of Mapping, used in
     * the docstrings. at returns a copy of the Mapping of a type: the
     * mappings live in a hash map whose emplace and remove move them, so a
     * reference would dangle. A BijectiveMapping or GenericMapping is
     * queried and modified in place through the methods taking the type
     * (in2out, map, erase_in...).
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
                []( const Model& self, const ComponentType& type ) -> Mapping {
                    return detail::model_mapping_at( "at", self, type );
                },
                pybind11::arg( "type" ),
                ( detail::is_std_vector< Mapping >::value
                        ? absl::StrCat( "Return a copy of the ", mapping_name,
                              " of the components of the given type: changing "
                              "it does not change this mapping (a list has no "
                              "in-place method: replace it with remove then "
                              "emplace); raise KeyError if there is none for "
                              "this type" )
                        : absl::StrCat( "Return a copy of the ", mapping_name,
                              " of the components of the given type: changing "
                              "it does not change this mapping (use map, "
                              "erase_in... with the type), and querying it "
                              "repeatedly is cheaper than calling at in a "
                              "loop; raise KeyError if there is none for this "
                              "type" ) )
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
        if constexpr( detail::is_bijective_mapping< Mapping >::value
                      || detail::is_generic_mapping< Mapping >::value )
        {
            using Values = detail::MappingValues< Mapping >;
            detail::define_model_mapping_per_type< Model,
                typename Values::Input, typename Values::Output >(
                mapping, mapping_name );
        }
        return mapping;
    }
} // namespace geode
