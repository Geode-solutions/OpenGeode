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
#include <tuple>
#include <utility>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <absl/container/fixed_array.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>
#include <absl/strings/str_cat.h>
#include <absl/types/span.h>

#include <geode/basic/types.hpp>

namespace pybind11
{
    namespace detail
    {
        template < typename Type >
        struct type_caster< absl::FixedArray< Type > >
            : list_caster< absl::FixedArray< Type >, Type >
        {
        };

        template < typename Type, size_t dimension >
        struct type_caster< absl::InlinedVector< Type, dimension > >
            : list_caster< absl::InlinedVector< Type, dimension >, Type >
        {
        };

        template < typename Type >
        struct type_caster< absl::Span< Type > >
            : list_caster< absl::Span< Type >, Type >
        {
            using value_conv = make_caster< Type >;

            bool load( handle src, bool convert )
            {
                cpp_.clear();
                auto s = reinterpret_borrow< sequence >( src );
                cpp_.reserve( s.size() );
                for( auto it : s )
                {
                    value_conv conv;
                    if( !conv.load( it, convert ) )
                        return false;
                    cpp_.push_back( cast_op< Type&& >( std::move( conv ) ) );
                }
                this->value = absl::MakeConstSpan( cpp_ );
                return true;
            }

            std::vector< typename std::remove_const< Type >::type > cpp_;
        };

        template < typename Key, typename Value >
        struct type_caster< absl::flat_hash_map< Key, Value > >
            : map_caster< absl::flat_hash_map< Key, Value >, Key, Value >
        {
        };
    } // namespace detail
} // namespace pybind11

namespace geode
{
    template < typename Class, typename Getter, typename... Options >
    pybind11::class_< Class, Options... > define_string_representation(
        pybind11::class_< Class, Options... > python_class, Getter&& getter )
    {
        python_class.def( "__str__", getter )
            .def( "__repr__", std::forward< Getter >( getter ) );
        return python_class;
    }

    template < typename Class, typename... Options >
    pybind11::class_< Class, Options... > define_string_representation(
        pybind11::class_< Class, Options... > python_class )
    {
        python_class.def( "string", &Class::string );
        return define_string_representation(
            std::move( python_class ), &Class::string );
    }

    /*!
     * Raise a Python IndexError if @a index is not in [0, size[.
     * @param[in] context Name of the Python function being called.
     * @param[in] what Kind of the checked index (vertex, polygon...).
     */
    inline void check_index( std::string_view context,
        std::string_view what,
        index_t index,
        index_t size )
    {
        if( index >= size )
        {
            throw pybind11::index_error( absl::StrCat( "[", context, "] ", what,
                " ", index, " out of range [0, ", size, ")" ) );
        }
    }

    namespace detail
    {
        template < typename Mesh,
            typename Tuple,
            size_t... Indices,
            typename... Checks >
        void run_checks( std::string_view name,
            const Mesh& mesh,
            const Tuple& args,
            std::index_sequence< Indices... > /*unused*/,
            const Checks&... checks )
        {
            ( checks( name, mesh, std::get< Indices >( args ) ), ... );
        }

        /*!
         * Return the object which gives the sizes needed to check the
         * arguments of a call: @a self if it is a Mesh, or the mesh stored
         * by the builder @a self, or nullptr if there is none.
         */
        template < typename Mesh >
        const Mesh* mesh_of( pybind11::handle self )
        {
            if( pybind11::isinstance< Mesh >( self ) )
            {
                return &self.cast< const Mesh& >();
            }
            if( pybind11::hasattr( self, "_mesh" ) )
            {
                return &self.attr( "_mesh" ).cast< const Mesh& >();
            }
            return nullptr;
        }
    } // namespace detail

    /*!
     * Wrap a const member function of Mesh to validate its first arguments
     * before the call. The i-th check is applied to the i-th argument and
     * is called as check( name, mesh, argument ), it throws an IndexError
     * if the argument is out of range. Call policies are given to .def() as
     * usual.
     */
    template < typename Mesh,
        typename Return,
        typename... Args,
        typename... Checks >
    auto checked( std::string_view name,
        Return ( Mesh::*method )( Args... ) const,
        Checks... checks )
    {
        static_assert( sizeof...( Checks ) <= sizeof...( Args ) );
        return [name = std::string{ name }, method, checks...](
                   const Mesh& mesh, Args... args ) -> Return {
            detail::run_checks( name, mesh, std::tie( args... ),
                std::index_sequence_for< Checks... >{}, checks... );
            return ( mesh.*method )( args... );
        };
    }

    /*!
     * Same as checked() for a method of a class that does not know its size
     * (CRS managers, builders). The sizes are read from the Python object
     * `self`, which is a Mesh or a builder created from a Mesh. No check is
     * done if `self` has no associated Mesh.
     */
    template < typename Mesh,
        typename Class,
        typename Return,
        typename... Args,
        typename... Checks >
    auto checked_with_self( std::string_view name,
        Return ( Class::*method )( Args... ),
        Checks... checks )
    {
        static_assert( sizeof...( Checks ) <= sizeof...( Args ) );
        return [name = std::string{ name }, method, checks...](
                   pybind11::object self, Args... args ) -> Return {
            if( const auto* mesh = detail::mesh_of< Mesh >( self ) )
            {
                detail::run_checks( name, *mesh, std::tie( args... ),
                    std::index_sequence_for< Checks... >{}, checks... );
            }
            return ( self.cast< Class& >().*method )( args... );
        };
    }

    template < typename Mesh,
        typename Class,
        typename Return,
        typename... Args,
        typename... Checks >
    auto checked_with_self( std::string_view name,
        Return ( Class::*method )( Args... ) const,
        Checks... checks )
    {
        static_assert( sizeof...( Checks ) <= sizeof...( Args ) );
        return [name = std::string{ name }, method, checks...](
                   pybind11::object self, Args... args ) -> Return {
            if( const auto* mesh = detail::mesh_of< Mesh >( self ) )
            {
                detail::run_checks( name, *mesh, std::tie( args... ),
                    std::index_sequence_for< Checks... >{}, checks... );
            }
            return ( self.cast< const Class& >().*method )( args... );
        };
    }

    /*!
     * Wrap the static `create` function of a builder to remember the Mesh
     * in the builder, to be able to check the indices given to the builder.
     */
    template < typename Return, typename Mesh >
    auto builder_creator( Return ( *create )( Mesh& ) )
    {
        return [create]( Mesh& mesh ) {
            auto builder = pybind11::cast( create( mesh ) );
            builder.attr( "_mesh" ) = pybind11::cast(
                &mesh, pybind11::return_value_policy::reference );
            return builder;
        };
    }
} // namespace geode
