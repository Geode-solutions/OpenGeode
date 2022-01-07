/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/attribute_manager.h>
#include <typeinfo>

#define PYTHON_ATTRIBUTE_TYPE( type, suffix )                                  \
    const auto read##suffix = std::string{ "find_attribute_" } + #suffix;      \
    manager.def(                                                               \
        read##suffix.c_str(), &AttributeManager::find_attribute< type > );     \
    const auto constant##suffix =                                              \
        std::string{ "find_or_create_attribute_constant_" } + #suffix;         \
    manager.def( constant##suffix.c_str(),                                     \
        ( std::shared_ptr< ConstantAttribute< type > >( AttributeManager::* )( \
            absl::string_view, type ) )                                        \
            & AttributeManager::find_or_create_attribute< ConstantAttribute,   \
                type > );                                                      \
    const auto variable##suffix =                                              \
        std::string{ "find_or_create_attribute_variable_" } + #suffix;         \
    manager.def( variable##suffix.c_str(),                                     \
        ( std::shared_ptr< VariableAttribute< type > >( AttributeManager::* )( \
            absl::string_view, type ) )                                        \
            & AttributeManager::find_or_create_attribute< VariableAttribute,   \
                type > );                                                      \
    const auto sparse##suffix =                                                \
        std::string{ "find_or_create_attribute_sparse_" } + #suffix;           \
    manager.def( sparse##suffix.c_str(),                                       \
        ( std::shared_ptr< SparseAttribute< type > >( AttributeManager::* )(   \
            absl::string_view, type ) )                                        \
            & AttributeManager::find_or_create_attribute< SparseAttribute,     \
                type > )

namespace geode
{
    void define_attribute_manager( pybind11::module& module )
    {
        pybind11::class_< AttributeManager > manager(
            module, "AttributeManager" );
        manager.def( pybind11::init<>() )
            .def( "find_generic_attribute",
                &AttributeManager::find_generic_attribute )
            .def( "attribute_names", &AttributeManager::attribute_names )
            .def( "attribute_type", &AttributeManager::attribute_type )
            .def( "attribute_exists", &AttributeManager::attribute_exists )
            .def( "nb_elements", &AttributeManager::nb_elements )
            .def( "reserve", &AttributeManager::reserve )
            .def( "resize", &AttributeManager::resize )
            .def( "clear", &AttributeManager::clear )
            .def( "clear_attributes", &AttributeManager::clear_attributes )
            .def( "delete_attribute", &AttributeManager::delete_attribute )
            .def( "delete_elements", &AttributeManager::delete_elements );
        PYTHON_ATTRIBUTE_TYPE( bool, bool );
        PYTHON_ATTRIBUTE_TYPE( int, int );
        PYTHON_ATTRIBUTE_TYPE( unsigned int, uint );
        PYTHON_ATTRIBUTE_TYPE( float, float );
        PYTHON_ATTRIBUTE_TYPE( double, double );
    }
} // namespace geode
