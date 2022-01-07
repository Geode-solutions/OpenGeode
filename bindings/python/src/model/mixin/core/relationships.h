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

#include <geode/model/mixin/core/relationships.h>

namespace geode
{
    void define_relationships( pybind11::module& module )
    {
        pybind11::class_< Relationships >( module, "Relationships" )
            .def( pybind11::init<>() )
            .def( "nb_boundaries", &Relationships::nb_boundaries )
            .def( "boundaries", &Relationships::boundaries )
            .def( "nb_incidences", &Relationships::nb_incidences )
            .def( "incidences", &Relationships::incidences )
            .def( "nb_internals", &Relationships::nb_internals )
            .def( "internals", &Relationships::internals )
            .def( "nb_embeddings", &Relationships::nb_embeddings )
            .def( "embeddings", &Relationships::embeddings )
            .def( "nb_items", &Relationships::nb_items )
            .def( "items", &Relationships::items )
            .def( "nb_collections", &Relationships::nb_collections )
            .def(
                "collections",
                []( const Relationships& relationships, const uuid& id ) {
                    std::vector< const ComponentID* > components;
                    for( const auto& component :
                        relationships.collections( id ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "is_boundary", &Relationships::is_boundary )
            .def( "is_internal", &Relationships::is_internal )
            .def( "is_item", &Relationships::is_item );
    }
} // namespace geode
