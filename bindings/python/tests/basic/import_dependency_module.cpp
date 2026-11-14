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

#include "../../src/common.hpp"

#include <geode/basic/uuid.hpp>

namespace
{
    struct First
    {
    };

    struct Second
    {
    };
} // namespace

// Test module standing for a downstream binding: it imports the module
// defining uuid by its fully qualified name, as a module of another package
PYBIND11_MODULE( geode_test_import_dependency, module )
{
    module.doc() =
        "Test module importing geode_test_upstream.opengeode_py_basic";
    geode::import_binding_dependency(
        module, "geode_test_upstream.opengeode_py_basic" );
    module.def(
        "same_uuid",
        []( const geode::uuid& id ) {
            return id;
        },
        pybind11::arg( "id" ), "Return the given uuid" );
    // Classes without constructor referring to each other: each one is
    // documented once, without endless recursion
    auto first = pybind11::class_< First >(
        module, "First", "Class without constructor" );
    auto second = pybind11::class_< Second >(
        module, "Second", "Class without constructor" );
    first.attr( "Partner" ) = second;
    second.attr( "Partner" ) = first;
    geode::document_missing_constructors( module );
}
