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

#include <pybind11/native_enum.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <absl/container/fixed_array.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>
#include <absl/hash/hash.h>
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
    /*!
     * Import the extension module defining the types used by a binding module.
     * In the build tree the modules are top-level modules; in the wheel they
     * live in a package (opengeode.lib.opengeode_py_basic...): when the
     * top-level module does not exist, the sibling of the module being
     * initialized is imported. Any other error (the dependency is found but
     * fails to import) is not masked.
     */
    inline void import_binding_dependency(
        const pybind11::module_& module, const std::string& name )
    {
        try
        {
            pybind11::module_::import( name.c_str() );
        }
        catch( pybind11::error_already_set& error )
        {
            const auto package =
                module.attr( "__name__" ).cast< std::string >();
            const auto dot = package.rfind( '.' );
            if( dot == std::string::npos
                || !error.matches( PyExc_ModuleNotFoundError ) )
            {
                throw;
            }
            // The name of the missing module is None for an error raised by
            // hand
            const auto missing = error.value().attr( "name" );
            if( !pybind11::isinstance< pybind11::str >( missing )
                || missing.cast< std::string >() != name )
            {
                throw;
            }
            pybind11::module_::import(
                ( package.substr( 0, dot + 1 ) + name ).c_str() );
        }
    }
} // namespace geode

namespace geode
{
    template < typename Type >
    class TypedObject;
}

namespace pybind11
{
    namespace detail
    {
        // Use the Python name of Type in the signatures
        template < typename Type >
        struct handle_type_name< geode::TypedObject< Type > >
        {
            static constexpr auto name = make_caster< Type >::name;
        };
    } // namespace detail
} // namespace pybind11

namespace geode
{
    template < typename Class, typename Getter, typename... Options >
    pybind11::class_< Class, Options... > define_string_representation(
        pybind11::class_< Class, Options... > python_class, Getter&& getter )
    {
        python_class
            .def( "__str__", getter,
                "Return a readable description of the object" )
            .def( "__repr__", std::forward< Getter >( getter ),
                "Return a readable description of the object" );
        return python_class;
    }

    template < typename Class, typename... Options >
    pybind11::class_< Class, Options... > define_string_representation(
        pybind11::class_< Class, Options... > python_class )
    {
        python_class.def( "string", &Class::string,
            "Return a readable description of the object" );
        return define_string_representation(
            std::move( python_class ), &Class::string );
    }

    /*!
     * Same as pybind11::class_::def_readwrite, but the setter argument is
     * called "value" instead of "arg0" in the Python signatures.
     */
    template < typename Class,
        typename Member,
        typename Owner,
        typename... Options >
    void def_readwrite_named(
        pybind11::class_< Class, Options... >& python_class,
        const char* name,
        Member Owner::* member,
        const char* doc )
    {
        const pybind11::cpp_function getter(
            [member]( const Class& object ) -> const Member& {
                return object.*member;
            },
            pybind11::is_method( python_class ) );
        const pybind11::cpp_function setter(
            [member]( Class& object, const Member& value ) {
                object.*member = value;
            },
            pybind11::is_method( python_class ), pybind11::arg( "value" ) );
        python_class.def_property( name, getter, setter,
            pybind11::return_value_policy::reference_internal, doc );
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
     * Same as checked() for a method inherited from a class that does not
     * know its size (CRS managers): the Python signature is typed with Mesh,
     * which gives the size.
     */
    template < typename Mesh,
        typename Class,
        typename Return,
        typename... Args,
        typename... Checks >
    auto checked_as( std::string_view name,
        Return ( Class::*method )( Args... ) const,
        Checks... checks )
    {
        static_assert( sizeof...( Checks ) <= sizeof...( Args ) );
        return [name = std::string{ name }, method, checks...](
                   const Mesh& mesh, Args... args ) -> Return {
            detail::run_checks( name, mesh, std::tie( args... ),
                std::index_sequence_for< Checks... >{}, checks... );
            return ( static_cast< const Class& >( mesh ).*method )( args... );
        };
    }

    /*!
     * Python object known to be an instance of the bound class Type. It is
     * typed Type in the generated signatures, unlike a plain object.
     */
    template < typename Type >
    class TypedObject : public pybind11::object
    {
    public:
        PYBIND11_OBJECT_DEFAULT( TypedObject, pybind11::object, is_instance )

        static bool is_instance( PyObject* object )
        {
            return pybind11::isinstance< Type >( pybind11::handle{ object } );
        }
    };

    /*!
     * Return a builder to Python and remember the mesh it modifies in its
     * `_mesh` attribute, to check the indices given to the builder. Use it
     * for every function returning a mesh builder.
     */
    template < typename Builder, typename Mesh >
    TypedObject< Builder > builder_with_mesh(
        std::unique_ptr< Builder > builder, const Mesh& mesh )
    {
        auto result = pybind11::reinterpret_steal< TypedObject< Builder > >(
            pybind11::cast( std::move( builder ) ).release() );
        result.attr( "_mesh" ) =
            pybind11::cast( &mesh, pybind11::return_value_policy::reference );
        return result;
    }

    /*!
     * Wrap the static `create` function of a mesh builder with
     * builder_with_mesh().
     */
    template < typename Return, typename Mesh >
    auto builder_creator( Return ( *create )( Mesh& ) )
    {
        return [create]( Mesh& mesh ) {
            return builder_with_mesh( create( mesh ), mesh );
        };
    }

    /*!
     * Same as checked_as() for a builder method: the sizes are read from the
     * mesh stored by builder_with_mesh() (no check if there is none). The
     * Python signature is typed with Builder.
     */
    template < typename Builder,
        typename Mesh,
        typename Class,
        typename Return,
        typename... Args,
        typename... Checks >
    auto checked_builder( std::string_view name,
        Return ( Class::*method )( Args... ),
        Checks... checks )
    {
        static_assert( sizeof...( Checks ) <= sizeof...( Args ) );
        return [name = std::string{ name }, method, checks...](
                   TypedObject< Builder > self, Args... args ) -> Return {
            if( pybind11::hasattr( self, "_mesh" ) )
            {
                detail::run_checks( name,
                    self.attr( "_mesh" ).template cast< const Mesh& >(),
                    std::tie( args... ), std::index_sequence_for< Checks... >{},
                    checks... );
            }
            return ( static_cast< Class& >( self.template cast< Builder& >() )
                     .*method )( args... );
        };
    }
} // namespace geode
