/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#define PYTHON_ATTRIBUTE_TYPE( type )                                          \
    typeid( type ).name(), &AttributeManager::find_attribute< type >

namespace geode
{
    void define_attribute_manager( pybind11::module& module )
    {
        pybind11::class_< AttributeManager >( module, "AttributeManager" )
            .def( "attribute_names", &AttributeManager::attribute_names )
            .def( "attribute_type", &AttributeManager::attribute_type )
            .def( "attribute_exists", &AttributeManager::attribute_exists )
            .def( "nb_elements", &AttributeManager::nb_elements )
            .def( PYTHON_ATTRIBUTE_TYPE( bool ) )
            .def( PYTHON_ATTRIBUTE_TYPE( int ) )
            .def( PYTHON_ATTRIBUTE_TYPE( unsigned int ) )
            .def( PYTHON_ATTRIBUTE_TYPE( float ) )
            .def( PYTHON_ATTRIBUTE_TYPE( double ) );
    }
} // namespace geode
