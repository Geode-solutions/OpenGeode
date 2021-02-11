/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "attribute.h"
#include "attribute_manager.h"
#include "identifier.h"
#include "identifier_builder.h"
#include "mapping.h"
#include "uuid.h"

namespace pybind11
{
    namespace detail
    {
        template < typename Type >
        struct type_caster< absl::FixedArray< Type > >
            : list_caster< absl::FixedArray< Type >, Type >
        {
        };

        template <>
        struct type_caster< absl::string_view >
            : string_caster< absl::string_view, true >
        {
        };
    } // namespace detail
} // namespace pybind11

PYBIND11_MODULE( opengeode_py_basic, module )
{
    pybind11::add_ostream_redirect( module );
    module.doc() = "OpenGeode Python binding for basic";
    module.attr( "NO_ID" ) = geode::NO_ID;
    module.attr( "NO_LID" ) = geode::NO_LID;
    module.attr( "global_epsilon" ) = geode::global_epsilon;
    module.attr( "global_epsilon2" ) = geode::global_epsilon2;
    module.attr( "global_epsilon3" ) = geode::global_epsilon3;
    geode::define_uuid( module );
    geode::define_attributes( module );
    geode::define_attribute_manager( module );
    geode::define_mapping( module );
    geode::define_identifier( module );
    geode::define_identifier_builder( module );
}