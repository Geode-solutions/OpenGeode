/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include "../common.h"

#include <geode/basic/attribute.h>

#define PYTHON_ATTRIBUTE_CLASS( type, name )                                   \
    const auto read##name = std::string{ "ReadOnlyAttribute" } + #name;        \
    pybind11::class_< ReadOnlyAttribute< type >, AttributeBase,                \
        std::shared_ptr< ReadOnlyAttribute< type > > >(                        \
        module, read##name.c_str() )                                           \
        .def( "value", &ReadOnlyAttribute< type >::value );                    \
    const auto constant##name = std::string{ "ConstantAttribute" } + #name;    \
    pybind11::class_< ConstantAttribute< type >, ReadOnlyAttribute< type >,    \
        std::shared_ptr< ConstantAttribute< type > > >(                        \
        module, constant##name.c_str() )                                       \
        .def( "constant_value",                                                \
            static_cast< const type& (ConstantAttribute< type >::*) ()         \
                    const >( &ConstantAttribute< type >::value ) )             \
        .def( "set_value", &ConstantAttribute< type >::set_value )             \
        .def( "default_value", &ConstantAttribute< type >::default_value );    \
    const auto variable##name = std::string{ "VariableAttribute" } + #name;    \
    pybind11::class_< VariableAttribute< type >, ReadOnlyAttribute< type >,    \
        std::shared_ptr< VariableAttribute< type > > >(                        \
        module, variable##name.c_str() )                                       \
        .def( "set_value", &VariableAttribute< type >::set_value )             \
        .def( "default_value", &VariableAttribute< type >::default_value );    \
    const auto sparse##name = std::string{ "SparseAttribute" } + #name;        \
    pybind11::class_< SparseAttribute< type >, ReadOnlyAttribute< type >,      \
        std::shared_ptr< SparseAttribute< type > > >(                          \
        module, sparse##name.c_str() )                                         \
        .def( "set_value", &SparseAttribute< type >::set_value )               \
        .def( "default_value", &SparseAttribute< type >::default_value )

namespace geode
{
    void define_attributes( pybind11::module& module )
    {
        pybind11::class_< AttributeProperties >( module, "AttributeProperties" )
            .def( pybind11::init<>() )
            .def( pybind11::init< bool, bool >() )
            .def_readwrite( "assignable", &AttributeProperties::assignable )
            .def_readwrite(
                "interpolable", &AttributeProperties::interpolable );

        pybind11::class_< AttributeBase, std::shared_ptr< AttributeBase > >(
            module, "AttributeBase" )
            .def( "generic_value", &AttributeBase::generic_value )
            .def( "properties", &AttributeBase::properties )
            .def( "is_genericable", &AttributeBase::is_genericable );
        PYTHON_ATTRIBUTE_CLASS( bool, Bool );
        PYTHON_ATTRIBUTE_CLASS( int, Int );
        PYTHON_ATTRIBUTE_CLASS( unsigned int, UInt );
        PYTHON_ATTRIBUTE_CLASS( float, Float );
        PYTHON_ATTRIBUTE_CLASS( double, Double );
    }
} // namespace geode
