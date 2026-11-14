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

#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <pybind11/native_enum.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <absl/container/fixed_array.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/container/inlined_vector.h>
#include <absl/hash/hash.h>
#include <absl/strings/str_cat.h>
#include <absl/types/span.h>

#include <geode/basic/identifier.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/types.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/geometry/point.hpp>

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
     * The name is the one of the module ("opengeode_py_basic") or, for a
     * module of another package (an upstream project), its fully qualified
     * name ("opengeode.opengeode_py_basic").
     * In the build trees the modules are top-level modules: the top-level
     * module of that name (the last part of a qualified name) is imported
     * first. In the wheels they live at the root of their package: when the
     * top-level module does not exist, the qualified name is imported, or for
     * an unqualified name, the module of the same name in the package of the
     * module being initialized. Any other error (the dependency is found but
     * fails to import) is not masked.
     */
    inline void import_binding_dependency(
        const pybind11::module_& module, const std::string& name )
    {
        const auto last_dot = name.rfind( '.' );
        const auto top_level =
            last_dot == std::string::npos ? name : name.substr( last_dot + 1 );
        try
        {
            pybind11::module_::import( top_level.c_str() );
        }
        catch( pybind11::error_already_set& error )
        {
            if( !error.matches( PyExc_ModuleNotFoundError ) )
            {
                throw;
            }
            // The name of the missing module is None for an error raised by
            // hand
            const auto missing = error.value().attr( "name" );
            if( !pybind11::isinstance< pybind11::str >( missing )
                || missing.cast< std::string >() != top_level )
            {
                throw;
            }
            if( last_dot != std::string::npos )
            {
                pybind11::module_::import( name.c_str() );
                return;
            }
            const auto package =
                module.attr( "__name__" ).cast< std::string >();
            const auto dot = package.rfind( '.' );
            if( dot == std::string::npos )
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
    namespace detail
    {
        constexpr auto MISSING_CONSTRUCTOR = " cannot be constructed directly";

        // Python gives every class its own __init__ entry: a class bound
        // without constructor has the slot wrapper of Python
        // (wrapper_descriptor) calling the __init__ of pybind11_object, which
        // raises; the __init__ of a base class is not used by pybind11
        inline bool has_constructor( const pybind11::handle& cls )
        {
            const auto dict = cls.attr( "__dict__" );
            if( !dict.contains( "__init__" ) )
            {
                return true;
            }
            return !PyObject_TypeCheck(
                dict[pybind11::str( "__init__" )].ptr(), &PyWrapperDescr_Type );
        }

        // The first public static method of a class whose public members
        // (its own and those of its bases) are all static methods, empty
        // otherwise
        inline std::string static_only_method( const pybind11::handle& cls )
        {
            std::string first;
            for( const auto& base : cls.attr( "__mro__" ) )
            {
                const auto base_name =
                    base.attr( "__name__" ).cast< std::string >();
                if( base_name == "pybind11_object" || base_name == "object" )
                {
                    continue;
                }
                const pybind11::dict dict( base.attr( "__dict__" ) );
                for( const auto& item : dict )
                {
                    const auto member = item.first.cast< std::string >();
                    if( member.empty() || member.front() == '_' )
                    {
                        continue;
                    }
                    if( !PyObject_TypeCheck(
                            item.second.ptr(), &PyStaticMethod_Type ) )
                    {
                        return {};
                    }
                    if( first.empty() || member < first )
                    {
                        first = member;
                    }
                }
            }
            return first;
        }

        inline void document_missing_constructor( const pybind11::handle& cls )
        {
            const auto name = cls.attr( "__name__" ).cast< std::string >();
            const auto static_method = static_only_method( cls );
            // Only a create of the class itself returns this class
            const auto message =
                cls.attr( "__dict__" ).contains( "create" )
                    ? absl::StrCat( name, MISSING_CONSTRUCTOR,
                          ": use the static method ", name, ".create" )
                : !static_method.empty()
                    ? absl::StrCat( name, MISSING_CONSTRUCTOR, ": ", name,
                          " has only static methods, call them on the class "
                          "(e.g. ",
                          name, ".", static_method, "(...))" )
                    : absl::StrCat( name, MISSING_CONSTRUCTOR,
                          ": get one from the "
                          "functions returning it (a builder, a loader or "
                          "the object owning it)" );
            const pybind11::cpp_function init(
                [message]( pybind11::detail::value_and_holder& ) {
                    throw pybind11::type_error( message );
                },
                pybind11::name( "__init__" ), pybind11::is_method( cls ),
                pybind11::detail::is_new_style_constructor(), message.c_str() );
            pybind11::setattr( cls, "__init__", init );
        }

        inline void document_missing_constructors(
            const pybind11::handle& scope,
            const std::string& module_name,
            absl::flat_hash_set< PyObject* >& visited )
        {
            const auto items = pybind11::reinterpret_borrow< pybind11::dict >(
                scope.attr( "__dict__" ) )
                                   .attr( "copy" )();
            for( const auto& item :
                pybind11::reinterpret_borrow< pybind11::dict >( items ) )
            {
                const auto& value = item.second;
                if( !PyType_Check( value.ptr() )
                    || pybind11::detail::get_type_info(
                           reinterpret_cast< PyTypeObject* >( value.ptr() ) )
                           == nullptr
                    || !pybind11::hasattr( value, "__module__" )
                    || value.attr( "__module__" ).cast< std::string >()
                           != module_name
                    || !visited.insert( value.ptr() ).second )
                {
                    continue;
                }
                if( !has_constructor( value ) )
                {
                    document_missing_constructor( value );
                }
                // A class attribute may alias any class: each one is
                // visited once
                document_missing_constructors( value, module_name, visited );
            }
        }
    } // namespace detail

    /*!
     * Give the bound classes of the module (and their nested classes) that
     * have no constructor an __init__ raising TypeError with a docstring
     * telling how to get an instance, instead of the generic "Initialize
     * self" of Python. Call it at the end of the PYBIND11_MODULE, after every
     * class is bound.
     */
    inline void document_missing_constructors( pybind11::module_& module )
    {
        absl::flat_hash_set< PyObject* > visited;
        detail::document_missing_constructors(
            module, module.attr( "__name__" ).cast< std::string >(), visited );
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
    /*!
     * Return the Python expression of a double: its repr, except for the
     * values that have no literal, which are given by float(...)
     */
    inline std::string python_repr( double value )
    {
        if( std::isnan( value ) )
        {
            return "float('nan')";
        }
        if( std::isinf( value ) )
        {
            return value > 0 ? "float('inf')" : "float('-inf')";
        }
        return pybind11::repr( pybind11::float_( value ) )
            .template cast< std::string >();
    }

    /*!
     * Return the Python repr of the value, e.g. 1.0 for a double, 'text' for
     * a string or Point3D([1.0, 2.0, 3.0]) for a Point3D.
     */
    template < typename Type >
    std::string python_repr( const Type& value )
    {
        return pybind11::repr( pybind11::cast( value ) )
            .template cast< std::string >();
    }

    /*!
     * Return the repr of the coordinates of a point or vector, as a list
     */
    template < index_t dimension >
    std::string coordinates_repr( const Point< dimension >& point )
    {
        std::string result{ "[" };
        for( const auto i : LRange{ dimension } )
        {
            if( i != 0 )
            {
                result += ", ";
            }
            result += python_repr( point.value( i ) );
        }
        return result + "]";
    }

    /*!
     * Return the name of the Python class of the object, which is the name
     * of its most derived bound class (e.g. TriangulatedSurface3D for an
     * object seen as a VertexSet)
     */
    template < typename Type >
    std::string python_type_name( const Type& object )
    {
        return pybind11::type::of(
            pybind11::cast(
                &object, pybind11::return_value_policy::reference ) )
            .attr( "__name__" )
            .template cast< std::string >();
    }

    /*!
     * Return name(arguments...), where each argument is already a Python
     * expression (a positional value or keyword=value).
     */
    inline std::string python_call_repr(
        std::string_view name, std::initializer_list< std::string > arguments )
    {
        std::string result{ name };
        result += '(';
        auto first = true;
        for( const auto& argument : arguments )
        {
            if( !first )
            {
                result += ", ";
            }
            first = false;
            result += argument;
        }
        result += ')';
        return result;
    }

    /*!
     * Return the repr of an object identified by an Identifier:
     * ClassName(id='...', name='...', fields...). The name is only given when
     * the object has one. Each field is a pair (keyword, Python expression).
     */
    inline std::string identifier_repr( const Identifier& object,
        std::string_view type_name,
        std::initializer_list< std::pair< std::string_view, std::string > >
            fields )
    {
        std::string result{ type_name };
        result += "(id=" + python_repr( object.id().string() );
        if( object.name() )
        {
            result += ", name=" + python_repr( object.name().value() );
        }
        for( const auto& field : fields )
        {
            result += ", ";
            result += field.first;
            result += '=';
            result += field.second;
        }
        result += ')';
        return result;
    }

    /*!
     * Define __repr__ with the repr_getter, and __str__ and string() with the
     * string() method of the class, which gives a readable description. The
     * repr must be unambiguous: the type and the value for the value types
     * (eval-able when possible), the type, the id and key counts for the meshes
     * and models.
     */
    template < typename Class, typename Repr, typename... Options >
    pybind11::class_< Class, Options... > define_string_representation(
        pybind11::class_< Class, Options... > python_class, Repr&& repr_getter )
    {
        python_class.def( "string", &Class::string,
            "Return a readable description of the object" );
        return define_string_representation( std::move( python_class ),
            &Class::string, std::forward< Repr >( repr_getter ) );
    }

    /*!
     * Same as above with a custom getter for __str__, and without string().
     */
    template < typename Class,
        typename Getter,
        typename Repr,
        typename... Options >
    pybind11::class_< Class, Options... > define_string_representation(
        pybind11::class_< Class, Options... > python_class,
        Getter&& getter,
        Repr&& repr_getter )
    {
        python_class
            .def( "__str__", std::forward< Getter >( getter ),
                "Return a readable description of the object" )
            .def( "__repr__", std::forward< Repr >( repr_getter ),
                "Return an unambiguous description of the object: its type "
                "and its value, or its id and main counts for the meshes and "
                "models" );
        return python_class;
    }

    /*!
     * Bound class types with value semantics in Python: a std::optional field
     * of such a type is returned as a copy and can be set to None (see
     * def_readwrite_named). Specialize it (std::true_type) for the value
     * classes of other projects.
     */
    template < typename T >
    struct is_python_value_type : std::false_type
    {
    };

    template <>
    struct is_python_value_type< uuid > : std::true_type
    {
    };

    namespace detail
    {
        template < typename Member >
        struct optional_field
        {
            static constexpr bool is_live = false;
        };

        template < typename Value >
        struct optional_field< std::optional< Value > >
        {
            static constexpr bool is_live =
                std::is_base_of_v< pybind11::detail::type_caster_generic,
                    pybind11::detail::make_caster< Value > >
                && !is_python_value_type< Value >::value;
        };

        template < typename Member >
        inline constexpr bool is_plain_value_field =
            std::is_arithmetic_v< Member > || std::is_enum_v< Member >
            || std::is_same_v< Member, std::string >;

        template < typename Member >
        struct is_optional : std::false_type
        {
        };

        template < typename Value >
        struct is_optional< std::optional< Value > > : std::true_type
        {
        };
    } // namespace detail

    /*!
     * Same as pybind11::class_::def_readwrite, but the setter argument is
     * called "value" instead of "arg0" in the Python signatures, and the
     * fields are returned according to their type:
     * - a bound class (type_caster_generic): by reference, keeping its object
     * alive;
     * - a std::optional of a bound class (except is_python_value_type): None
     * or a reference to the contained value, keeping its object alive. The
     * setter refuses None (TypeError) and copy-assigns an engaged optional in
     * place, so that the references held by Python stay valid;
     * - scalars, strings, enums and the other std::optional: by value;
     * - any other type (containers...): as a copy, which its docstring says:
     * their casters forward the reference policy to the contained values,
     * which would refer to storage freed by the next assignment.
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
        constexpr bool by_reference =
            std::is_base_of_v< pybind11::detail::type_caster_generic,
                pybind11::detail::make_caster< Member > >;
        if constexpr( by_reference )
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
        else if constexpr( detail::optional_field< Member >::is_live )
        {
            using Value = typename Member::value_type;
            const pybind11::cpp_function getter(
                [member]( const Class& object )
                    -> std::optional< std::reference_wrapper< const Value > > {
                    const auto& field = object.*member;
                    if( !field )
                    {
                        return std::nullopt;
                    }
                    return std::cref( field.value() );
                },
                pybind11::is_method( python_class ) );
            const pybind11::cpp_function setter(
                [member]( Class& object, const Value& value ) {
                    auto& field = object.*member;
                    if( field )
                    {
                        field.value() = value;
                    }
                    else
                    {
                        field.emplace( value );
                    }
                },
                pybind11::is_method( python_class ),
                pybind11::arg( "value" ).none( false ) );
            const auto live_doc = absl::StrCat( doc,
                " (a live reference that can be modified in place; it cannot "
                "be set to None from Python: the stored object would be "
                "destroyed while Python references it)" );
            python_class.def_property( name, getter, setter,
                pybind11::return_value_policy::reference_internal,
                live_doc.c_str() );
        }
        else
        {
            const pybind11::cpp_function getter(
                [member]( const Class& object ) -> Member {
                    return object.*member;
                },
                pybind11::is_method( python_class ) );
            const pybind11::cpp_function setter(
                [member]( Class& object, const Member& value ) {
                    object.*member = value;
                },
                pybind11::is_method( python_class ), pybind11::arg( "value" ) );
            if constexpr( detail::is_plain_value_field< Member >
                          || detail::is_optional< Member >::value )
            {
                python_class.def_property( name, getter, setter,
                    pybind11::return_value_policy::move, doc );
            }
            else
            {
                const auto copy_doc = absl::StrCat( doc,
                    " (returned as a copy: assign the field to change it)" );
                python_class.def_property( name, getter, setter,
                    pybind11::return_value_policy::move, copy_doc.c_str() );
            }
        }
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

    /*!
     * Raise a Python IndexError if @a direction is not in [0, dimension[.
     */
    inline void check_direction(
        std::string_view context, index_t direction, index_t dimension )
    {
        check_index( context, "direction", direction, dimension );
    }

    /*!
     * Raise a Python IndexError if one of the indices (one per direction) is
     * not in [0, size( direction )[.
     * @param[in] what Kind of the indices (cell index, vertex index...).
     */
    template < typename Indices, typename Size >
    void check_grid_indices( std::string_view context,
        std::string_view what,
        const Indices& indices,
        Size size )
    {
        for( index_t direction = 0; direction < indices.size(); direction++ )
        {
            const index_t nb = size( direction );
            if( indices[direction] >= nb )
            {
                throw pybind11::index_error( absl::StrCat( "[", context, "] ",
                    what, " ", indices[direction], " out of range [0, ", nb,
                    ") in direction ", direction ) );
            }
        }
    }

    /*!
     * Wrap a lookup of a mapping (in2out, out2in, at) so that a key without
     * value raises a Python KeyError naming the key, instead of the
     * IndexError of the C++ container.
     * @param[in] has Function telling whether the key has a value.
     */
    template < typename Mapping,
        typename Key,
        typename Return,
        typename HasClass,
        typename GetClass >
    auto mapping_lookup( std::string_view name,
        bool ( HasClass::*has )( const Key& ) const,
        Return ( GetClass::*get )( const Key& ) const )
    {
        return [name = std::string{ name }, has, get](
                   const Mapping& mapping, const Key& key ) -> Return {
            if( !( mapping.*has )( key ) )
            {
                throw pybind11::key_error( absl::StrCat(
                    "[", name, "] ", python_repr( key ), " is not mapped" ) );
            }
            return ( mapping.*get )( key );
        };
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
