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

#include "../common.hpp"
#include "../numpy_access.hpp"

#include <cmath>

#include <geode/basic/attribute.hpp>
#include <geode/basic/constant_attribute.hpp>
#include <geode/basic/sparse_attribute.hpp>
#include <geode/basic/variable_attribute.hpp>

namespace geode
{
    /*!
     * Layout of the values of an attribute in a numpy array: the scalar
     * type of the array and the number of items of a value (0 for a scalar
     * value, the array has then one dimension, and k for a std::array of k
     * values, the array has then shape (n, k)).
     */
    template < typename Type >
    struct AttributeArrayTraits
    {
        using Scalar = Type;
        static constexpr size_t nb_items = 0;
        static Scalar item( const Type& value, size_t /*unused*/ )
        {
            return value;
        }
        static void set_item( Type& value, size_t /*unused*/, Scalar item )
        {
            value = item;
        }
    };

    template < size_t size >
    struct AttributeArrayTraits< std::array< double, size > >
    {
        using Scalar = double;
        static constexpr size_t nb_items = size;
        static Scalar item( const std::array< double, size >& value, size_t i )
        {
            return value[i];
        }
        static void set_item(
            std::array< double, size >& value, size_t i, Scalar item )
        {
            value[i] = item;
        }
    };

    template < typename Type >
    std::vector< typename AttributeArrayTraits< Type >::Scalar >
        copy_attribute_values( const ArrayLike& values )
    {
        using Scalar = typename AttributeArrayTraits< Type >::Scalar;
        constexpr auto context = "set_values_array";
        if( values.size() == 0 )
        {
            return {};
        }
        if constexpr( std::is_same_v< Scalar, bool > )
        {
            const auto copy = copy_booleans( context, "values", values );
            return { copy.begin(), copy.end() };
        }
        else if constexpr( std::is_integral_v< Scalar > )
        {
            return copy_integers< Scalar >( context, "values", values );
        }
        else if constexpr( std::is_same_v< Scalar, float > )
        {
            // Finite values that are too large for a float would become
            // infinite: reject them instead
            const auto doubles =
                copy_reals< double >( context, "values", values );
            std::vector< float > result( doubles.size() );
            for( size_t i = 0; i < doubles.size(); ++i )
            {
                result[i] = static_cast< float >( doubles[i] );
                if( std::isfinite( doubles[i] ) && !std::isfinite( result[i] ) )
                {
                    throw pybind11::value_error( absl::StrCat( "[", context,
                        "] value ", doubles[i], " at position ", i,
                        " of values is too large for a float attribute" ) );
                }
            }
            return result;
        }
        else
        {
            return copy_reals< Scalar >( context, "values", values );
        }
    }

    template < typename Type >
    pybind11::array_t< typename AttributeArrayTraits< Type >::Scalar >
        attribute_values_array( const VariableAttribute< Type >& attribute )
    {
        using Traits = AttributeArrayTraits< Type >;
        const auto nb_elements = attribute.size();
        std::vector< pybind11::ssize_t > shape{
            static_cast< pybind11::ssize_t >( nb_elements )
        };
        constexpr auto nb_items = std::max< size_t >( Traits::nb_items, 1 );
        if constexpr( Traits::nb_items != 0 )
        {
            shape.push_back( static_cast< pybind11::ssize_t >( nb_items ) );
        }
        return make_array< typename Traits::Scalar >(
            std::move( shape ), [&]( typename Traits::Scalar* data ) {
                for( const auto i : Range{ nb_elements } )
                {
                    const auto& value = attribute.value( i );
                    for( const auto k : LRange{ nb_items } )
                    {
                        data[static_cast< size_t >( i ) * nb_items + k] =
                            Traits::item( value, k );
                    }
                }
            } );
    }

    template < typename Type >
    void attribute_set_values_array(
        VariableAttribute< Type >& attribute, const ArrayLike& values )
    {
        using Traits = AttributeArrayTraits< Type >;
        const auto nb_elements = attribute.size();
        if( nb_elements == 0 && is_empty_input( values ) )
        {
            // Nothing to set, whatever the dtype and shape (e.g. [])
            return;
        }
        if constexpr( Traits::nb_items == 0 )
        {
            check_array_shape( "set_values_array", "values", values,
                { static_cast< pybind11::ssize_t >( nb_elements ) } );
        }
        else
        {
            check_array_shape( "set_values_array", "values", values,
                { static_cast< pybind11::ssize_t >( nb_elements ),
                    static_cast< pybind11::ssize_t >( Traits::nb_items ) } );
        }
        const auto copy = copy_attribute_values< Type >( values );
        constexpr auto nb_items = std::max< size_t >( Traits::nb_items, 1 );
        pybind11::gil_scoped_release release;
        for( const auto i : Range{ nb_elements } )
        {
            Type value{};
            for( const auto k : LRange{ nb_items } )
            {
                Traits::set_item(
                    value, k, copy[static_cast< size_t >( i ) * nb_items + k] );
            }
            attribute.set_value( i, std::move( value ) );
        }
    }

    template < typename type >
    void python_attribute_class(
        pybind11::module& module, const std::string& typestr )
    {
        const auto read_name = absl::StrCat( "ReadOnlyAttribute", typestr );
        pybind11::class_< ReadOnlyAttribute< type >, AttributeBase,
            std::shared_ptr< ReadOnlyAttribute< type > > >( module,
            read_name.c_str(), "Attribute whose values can only be read" )
            .def(
                "value",
                []( const ReadOnlyAttribute< type >& attribute,
                    index_t element ) -> const type& {
                    // Only the variable attributes have a size: constant
                    // attributes have the same value for all elements and
                    // sparse attributes return their default value.
                    if( const auto* variable =
                            dynamic_cast< const VariableAttribute< type >* >(
                                &attribute ) )
                    {
                        check_index(
                            "value", "element", element, variable->size() );
                    }
                    return attribute.value( element );
                },
                pybind11::arg( "element" ),
                "Return the value of the element at the given index. Raise "
                "IndexError if a variable attribute has no such element" );
        const auto constant_name = absl::StrCat( "ConstantAttribute", typestr );
        pybind11::class_< ConstantAttribute< type >, ReadOnlyAttribute< type >,
            std::shared_ptr< ConstantAttribute< type > > >( module,
            constant_name.c_str(),
            "Attribute with the same value for all the elements" )
            .def( "constant_value",
                static_cast< const type& (ConstantAttribute< type >::*) ()
                        const >( &ConstantAttribute< type >::value ),
                "Return the value shared by all the elements" )
            .def( "set_value", &ConstantAttribute< type >::set_value,
                pybind11::arg( "value" ),
                "Set the value shared by all the elements" )
            .def( "default_value", &ConstantAttribute< type >::default_value,
                "Return the value shared by all the elements (same as "
                "constant_value)" );
        const auto variable_name = absl::StrCat( "VariableAttribute", typestr );
        pybind11::class_< VariableAttribute< type >, ReadOnlyAttribute< type >,
            std::shared_ptr< VariableAttribute< type > > >( module,
            variable_name.c_str(), "Attribute storing one value per element" )
            .def(
                "set_value",
                []( VariableAttribute< type >& attribute, index_t element,
                    type value ) {
                    check_index(
                        "set_value", "element", element, attribute.size() );
                    attribute.set_value( element, std::move( value ) );
                },
                pybind11::arg( "element" ), pybind11::arg( "value" ),
                "Set the value of the element at the given index. Raise "
                "IndexError if there is no such element" )
            .def( "default_values", &VariableAttribute< type >::default_values,
                "Return the default value and the no-value marker of the "
                "attribute" )
            .def( "values_array", &attribute_values_array< type >,
                "Return the values of all the elements as a new numpy array: "
                "shape (n,) for a value that is a number or a boolean "
                "(dtype bool, int32, uint32, float32 or float64 as the type "
                "of the attribute) and shape (n, k) of float64 for an "
                "attribute whose values are arrays of k floats. The array is "
                "a copy: later changes of the attribute do not modify it" )
            .def( "set_values_array", &attribute_set_values_array< type >,
                pybind11::arg( "values" ),
                "Set the values of all the elements from a numpy array of "
                "the same layout as values_array (n is the number of "
                "elements of the attribute): booleans for a boolean "
                "attribute, integers of any dtype that fit the type of the "
                "attribute for an integer one and floats or integers of any "
                "dtype for a float one. Values are converted to the type of "
                "the "
                "attribute: they are rounded to the nearest float32 for a "
                "float attribute (integers above 2**24 lose precision, a "
                "finite value too large for a float32 is rejected, infinities "
                "and NaN are kept) and integers above 2**53 lose precision "
                "in a double attribute. An empty array, whatever its dtype "
                "and shape (for example []), is accepted if the attribute has "
                "no element. Raise ValueError if the array has another shape "
                "or dtype, or a value that does not fit the type, in which "
                "case the attribute is not modified" );
        const auto sparse_name = absl::StrCat( "SparseAttribute", typestr );
        pybind11::class_< SparseAttribute< type >, ReadOnlyAttribute< type >,
            std::shared_ptr< SparseAttribute< type > > >( module,
            sparse_name.c_str(),
            "Attribute storing only the values different from the default "
            "one" )
            .def( "set_value", &SparseAttribute< type >::set_value,
                pybind11::arg( "element" ), pybind11::arg( "value" ),
                "Set the value of the element at the given index" )
            .def( "default_values", &SparseAttribute< type >::default_values,
                "Return the default value and the no-value marker of the "
                "attribute" );
    }

    template < typename type >
    void python_attribute_values_class(
        pybind11::module& module, const std::string& typestr )
    {
        const auto values_name = absl::StrCat( "AttributeValues", typestr );
        pybind11::class_< AttributeValues< type > > values( module,
            values_name.c_str(),
            "Default value and no-value marker of an attribute" );
        values.def( pybind11::init<>(),
            "Build values with the default value of the type and no no-value" );
        def_readwrite_named( values, "default_value",
            &AttributeValues< type >::default_value,
            "Value given to the elements which are not explicitly set" );
        def_readwrite_named( values, "no_value",
            &AttributeValues< type >::no_value,
            "Value meaning that an element has no value" );
    }

    void define_attributes( pybind11::module& module )
    {
        pybind11::class_< AttributeProperties > properties( module,
            "AttributeProperties",
            "Behavior of an attribute when its elements are modified" );
        properties.def( pybind11::init<>(),
            "Build properties: not assignable, not interpolable, transferable "
            "and "
            "without time" );
        def_readwrite_named( properties, "assignable",
            &AttributeProperties::assignable,
            "True if the value can be assigned to a new element" );
        def_readwrite_named( properties, "interpolable",
            &AttributeProperties::interpolable,
            "True if the value can be interpolated between elements" );
        def_readwrite_named( properties, "transferable",
            &AttributeProperties::transferable,
            "True if the value is transferred when the attributes are "
            "imported into another manager" );
        def_readwrite_named( properties, "time", &AttributeProperties::time,
            "Time of the attribute when it is a step of a time series" );

        python_attribute_values_class< bool >( module, "Bool" );
        python_attribute_values_class< int >( module, "Int" );
        python_attribute_values_class< unsigned int >( module, "UInt" );
        python_attribute_values_class< float >( module, "Float" );
        python_attribute_values_class< double >( module, "Double" );
        python_attribute_values_class< std::array< double, 2 > >(
            module, "ArrayDouble2" );
        python_attribute_values_class< std::array< double, 3 > >(
            module, "ArrayDouble3" );

        pybind11::class_< AttributeBase, std::shared_ptr< AttributeBase > >(
            module, "AttributeBase",
            "Common interface of the attributes, whatever their value type" )
            .def( "generic_value", &AttributeBase::generic_value,
                pybind11::arg( "element" ),
                "Return the value of the element converted to a float. "
                "Only valid if is_genericable()" )
            .def( "generic_item_value", &AttributeBase::generic_item_value,
                pybind11::arg( "element" ), pybind11::arg( "item" ),
                "Return one component (item) of the value of the element "
                "converted to a float. Only valid if is_genericable()" )
            .def( "properties", &AttributeBase::properties,
                "Return the properties of the attribute" )
            .def( "is_genericable", &AttributeBase::is_genericable,
                "Return True if the values can be converted to floats" )
            .def( "nb_items", &AttributeBase::nb_items,
                "Return the number of components of one value" )
            .def( "type", &AttributeBase::type,
                "Return the C++ type id name of the values (for example \"i\" "
                "for int)" )
            .def( "name", &AttributeBase::name,
                "Return the name of the attribute, or None if it has none" );
        python_attribute_class< bool >( module, "Bool" );
        python_attribute_class< int >( module, "Int" );
        python_attribute_class< unsigned int >( module, "UInt" );
        python_attribute_class< float >( module, "Float" );
        python_attribute_class< double >( module, "Double" );
        python_attribute_class< std::array< double, 2 > >(
            module, "ArrayDouble2" );
        python_attribute_class< std::array< double, 3 > >(
            module, "ArrayDouble3" );
    }
} // namespace geode
