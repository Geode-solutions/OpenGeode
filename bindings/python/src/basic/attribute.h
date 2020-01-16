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

#include <geode/basic/attribute.h>

#include <pybind11/pybind11.h>

#define PYTHON_ATTRIBUTE_CLASS( type, name )                                   \
    pybind11::class_< ReadOnlyAttribute< type > >( module, name )              \
        .def( "value", &ReadOnlyAttribute< type >::value )

namespace geode
{
    void define_attributes( pybind11::module& module )
    {
        PYTHON_ATTRIBUTE_CLASS( bool, "AttributeBool" );
        PYTHON_ATTRIBUTE_CLASS( int, "AttributeInt" );
        PYTHON_ATTRIBUTE_CLASS( unsigned int, "AttributeUInt" );
        PYTHON_ATTRIBUTE_CLASS( float, "AttributeFloat" );
        PYTHON_ATTRIBUTE_CLASS( double, "AttributeDouble" );
    }
} // namespace geode
