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

#include <geode/basic/attribute.hpp>
#include <geode/basic/constant_attribute.hpp>
#include <geode/basic/sparse_attribute.hpp>
#include <geode/basic/variable_attribute.hpp>

namespace geode
{
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
                "attribute" );
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
