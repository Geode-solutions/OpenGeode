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

#include "../../../common.hpp"

#include <geode/model/mixin/core/relationships.hpp>

namespace geode
{
    void define_relationships( pybind11::module& module )
    {
        pybind11::class_< Relationships >( module, "Relationships" )
            .def( pybind11::init<>() )
            .def( "nb_boundaries", &Relationships::nb_boundaries )
            .def(
                "boundaries",
                []( const Relationships& relationships,
                    const uuid& component_id ) {
                    std::vector< ComponentID > components;
                    for( const auto& component :
                        relationships.boundaries( component_id ) )
                    {
                        components.push_back( component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_incidences", &Relationships::nb_incidences )
            .def(
                "incidences",
                []( const Relationships& relationships,
                    const uuid& component_id ) {
                    std::vector< ComponentID > components;
                    for( const auto& component :
                        relationships.incidences( component_id ) )
                    {
                        components.push_back( component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_internals", &Relationships::nb_internals )
            .def(
                "internals",
                []( const Relationships& relationships,
                    const uuid& component_id ) {
                    std::vector< ComponentID > components;
                    for( const auto& component :
                        relationships.internals( component_id ) )
                    {
                        components.push_back( component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_embeddings", &Relationships::nb_embeddings )
            .def(
                "embeddings",
                []( const Relationships& relationships,
                    const uuid& component_id ) {
                    std::vector< ComponentID > components;
                    for( const auto& component :
                        relationships.embeddings( component_id ) )
                    {
                        components.push_back( component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_items", &Relationships::nb_items )
            .def(
                "items",
                []( const Relationships& relationships,
                    const uuid& component_id ) {
                    std::vector< ComponentID > components;
                    for( const auto& component :
                        relationships.items( component_id ) )
                    {
                        components.push_back( component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "nb_collections", &Relationships::nb_collections )
            .def(
                "collections",
                []( const Relationships& relationships, const uuid& id ) {
                    std::vector< ComponentID > components;
                    for( const auto& component :
                        relationships.collections( id ) )
                    {
                        components.push_back( component );
                    }
                    return components;
                },
                pybind11::return_value_policy::reference )
            .def( "is_boundary", &Relationships::is_boundary )
            .def( "is_internal", &Relationships::is_internal )
            .def( "is_item", &Relationships::is_item );
    }
} // namespace geode
