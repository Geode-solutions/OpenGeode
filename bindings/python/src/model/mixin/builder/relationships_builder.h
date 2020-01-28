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

#include <geode/model/mixin/builder/relationships_builder.h>

namespace geode
{
    void define_relationships_builder( pybind11::module& module )
    {
        pybind11::class_< RelationshipsBuilder >(
            module, "RelationshipsBuilder" )
            .def( pybind11::init< Relationships& >() )
            .def( "register_component",
                &RelationshipsBuilder::register_component )
            .def( "unregister_component",
                &RelationshipsBuilder::unregister_component )
            .def( "add_boundary_relation",
                &RelationshipsBuilder::add_boundary_relation )
            .def( "add_internal_relation",
                &RelationshipsBuilder::add_internal_relation )
            .def( "add_item_in_collection",
                &RelationshipsBuilder::add_item_in_collection );
    }
} // namespace geode
