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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "bounding_box.h"
#include "nn_search.h"
#include "point.h"
#include "vector.h"

namespace pybind11
{
    namespace detail
    {
        template < typename Type >
        struct type_caster< absl::FixedArray< Type > >
            : list_caster< absl::FixedArray< Type >, Type >
        {
        };
    } // namespace detail
} // namespace pybind11

PYBIND11_MODULE( OpenGeode_py_geometry, module )
{
    module.doc() = "OpenGeode Python binding for geometry";
    geode::define_point( module );
    geode::define_vector( module );
    geode::define_bounding_box( module );
    geode::define_nn_search( module );
}