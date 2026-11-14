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

#include <pybind11/typing.h>

#include "../common.hpp"

#include <geode/basic/mapping.hpp>

namespace geode
{
    namespace detail
    {
        /*!
         * Copy of a map of a mapping (in2out_map, out2in_map) as a Python
         * dict, in the order of insertion of the keys. The values of a
         * GenericMapping are lists.
         */
        template < typename Key, typename Value, typename Map >
        pybind11::typing::Dict< Key, Value > mapping_dict( const Map& map )
        {
            pybind11::typing::Dict< Key, Value > result;
            for( const auto& [key, value] : map )
            {
                result[pybind11::cast(
                    key, pybind11::return_value_policy::copy )] =
                    pybind11::cast(
                        value, pybind11::return_value_policy::copy );
            }
            return result;
        }
    } // namespace detail

    /*!
     * Bind the API of BijectiveMapping< T1, T2 > (one-to-one mapping) on its
     * Python class: lookups (a missing key raises KeyError) and modifiers.
     */
    template < typename T1, typename T2, typename... Options >
    void define_bijective_mapping_api(
        pybind11::class_< BijectiveMapping< T1, T2 >, Options... >& mapping )
    {
        using Mapping = BijectiveMapping< T1, T2 >;
        mapping
            .def( "has_mapping_input", &Mapping::has_mapping_input,
                pybind11::arg( "input" ),
                "Return True if the value is mapped as an input" )
            .def( "has_mapping_output", &Mapping::has_mapping_output,
                pybind11::arg( "output" ),
                "Return True if the value is mapped as an output" )
            .def( "in2out",
                mapping_lookup< Mapping >(
                    "in2out", &Mapping::has_mapping_input, &Mapping::in2out ),
                pybind11::arg( "input" ),
                "Return the output mapped to the input. Raise KeyError if "
                "there is none" )
            .def( "out2in",
                mapping_lookup< Mapping >(
                    "out2in", &Mapping::has_mapping_output, &Mapping::out2in ),
                pybind11::arg( "output" ),
                "Return the input mapped to the output. Raise KeyError if "
                "there is none" )
            .def( "size", &Mapping::size, "Return the number of pairs" )
            .def( "__len__", &Mapping::size, "Return the number of pairs" )
            .def(
                "in2out_map",
                []( const Mapping& self ) {
                    return detail::mapping_dict< T1, T2 >( self.in2out_map() );
                },
                "Return a copy of the mapping as a dict from each input to "
                "its output, in the order the inputs were added (an input "
                "mapped again keeps its place)" )
            .def(
                "out2in_map",
                []( const Mapping& self ) {
                    return detail::mapping_dict< T2, T1 >( self.out2in_map() );
                },
                "Return a copy of the mapping as a dict from each output to "
                "its input, in the order the outputs were added (an output "
                "mapped again keeps its place)" )
            .def( "map", &Mapping::map, pybind11::arg( "input" ),
                pybind11::arg( "output" ),
                "Map the input to the output. The mapping stays one-to-one: "
                "a previous pair of the input or of the output is removed" )
            .def( "erase_in", &Mapping::erase_in, pybind11::arg( "input" ),
                "Remove the pair of the input; nothing is done if the input "
                "is not mapped" )
            .def( "erase_out", &Mapping::erase_out, pybind11::arg( "output" ),
                "Remove the pair of the output; nothing is done if the "
                "output is not mapped" )
            .def( "clear", &Mapping::clear, "Remove all the pairs" );
    }

    /*!
     * Bind the API of GenericMapping< T1, T2 > (many-to-many mapping) on its
     * Python class: lookups (a missing key raises KeyError) and modifiers.
     */
    template < typename T1, typename T2, typename... Options >
    void define_generic_mapping_api(
        pybind11::class_< GenericMapping< T1, T2 >, Options... >& mapping )
    {
        using Mapping = GenericMapping< T1, T2 >;
        mapping
            .def( "has_mapping_input", &Mapping::has_mapping_input,
                pybind11::arg( "input" ),
                "Return True if the value is mapped as an input" )
            .def( "has_mapping_output", &Mapping::has_mapping_output,
                pybind11::arg( "output" ),
                "Return True if the value is mapped as an output" )
            .def( "in2out",
                mapping_lookup< Mapping >(
                    "in2out", &Mapping::has_mapping_input, &Mapping::in2out ),
                pybind11::arg( "input" ),
                "Return the list of the outputs mapped to the input, in the "
                "order they were mapped. Raise KeyError if there is none" )
            .def( "out2in",
                mapping_lookup< Mapping >(
                    "out2in", &Mapping::has_mapping_output, &Mapping::out2in ),
                pybind11::arg( "output" ),
                "Return the list of the inputs mapped to the output, in the "
                "order they were mapped. Raise KeyError if there is none" )
            .def( "size_in", &Mapping::size_in,
                "Return the number of mapped inputs" )
            .def( "size_out", &Mapping::size_out,
                "Return the number of mapped outputs" )
            .def(
                "in2out_map",
                []( const Mapping& self ) {
                    return detail::mapping_dict< T1,
                        pybind11::typing::List< T2 > >( self.in2out_map() );
                },
                "Return a copy of the mapping as a dict from each mapped "
                "input to the list of its outputs, in the order the inputs "
                "were added (an input keeps its place until all its pairs "
                "are removed)" )
            .def(
                "out2in_map",
                []( const Mapping& self ) {
                    return detail::mapping_dict< T2,
                        pybind11::typing::List< T1 > >( self.out2in_map() );
                },
                "Return a copy of the mapping as a dict from each mapped "
                "output to the list of its inputs, in the order the outputs "
                "were added (an output keeps its place until all its pairs "
                "are removed)" )
            .def( "map", &Mapping::map, pybind11::arg( "input" ),
                pybind11::arg( "output" ),
                "Add the pair (input, output); nothing is done if it is "
                "already mapped. The other pairs of the input and of the "
                "output are kept" )
            .def( "unmap", &Mapping::unmap, pybind11::arg( "input" ),
                pybind11::arg( "output" ),
                "Remove the pair (input, output); nothing is done if it is "
                "not mapped" )
            .def( "erase_in", &Mapping::erase_in, pybind11::arg( "input" ),
                "Remove all the pairs of the input; nothing is done if the "
                "input is not mapped" )
            .def( "erase_out", &Mapping::erase_out, pybind11::arg( "output" ),
                "Remove all the pairs of the output; nothing is done if the "
                "output is not mapped" )
            .def( "clear", &Mapping::clear, "Remove all the pairs" );
    }

    /*!
     * Bind BijectiveMapping< T1, T2 > as the Python class name: constructors
     * and define_bijective_mapping_api.
     */
    template < typename T1, typename T2 = T1 >
    pybind11::class_< BijectiveMapping< T1, T2 > > define_bijective_mapping(
        pybind11::module& module, const char* name, const char* doc )
    {
        using Mapping = BijectiveMapping< T1, T2 >;
        pybind11::class_< Mapping > mapping( module, name, doc );
        mapping.def( pybind11::init<>(), "Build an empty mapping" )
            .def( pybind11::init( []( Mapping& other ) {
                return Mapping{ std::move( other ) };
            } ),
                pybind11::arg( "mapping" ),
                "Build a mapping by taking over the content of mapping, "
                "which is emptied" );
        define_bijective_mapping_api( mapping );
        return mapping;
    }

    /*!
     * Bind GenericMapping< T1, T2 > as the Python class name: constructors
     * and define_generic_mapping_api.
     */
    template < typename T1, typename T2 = T1 >
    pybind11::class_< GenericMapping< T1, T2 > > define_generic_mapping(
        pybind11::module& module, const char* name, const char* doc )
    {
        using Mapping = GenericMapping< T1, T2 >;
        pybind11::class_< Mapping > mapping( module, name, doc );
        mapping.def( pybind11::init<>(), "Build an empty mapping" )
            .def( pybind11::init( []( Mapping& other ) {
                return Mapping{ std::move( other ) };
            } ),
                pybind11::arg( "mapping" ),
                "Build a mapping by taking over the content of mapping, "
                "which is emptied" );
        define_generic_mapping_api( mapping );
        return mapping;
    }
} // namespace geode
