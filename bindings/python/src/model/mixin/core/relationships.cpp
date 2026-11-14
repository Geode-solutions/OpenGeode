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
        pybind11::class_< Relationships, pybind11::smart_holder >( module,
            "Relationships",
            "Relationships between the components of a model, each one linking "
            "two components: boundary (a component bounds another one, its "
            "dual is incidence), internal (a component lies inside another one "
            "without bounding it, its dual is embedding) and item (a component "
            "belongs to a collection, its dual is collection). Components are "
            "identified by their uuid. Relationships are modified through "
            "RelationshipsBuilder" )
            .def( pybind11::init<>(), "Build empty Relationships" )
            .def( "nb_boundaries", &Relationships::nb_boundaries,
                pybind11::arg( "component_id" ),
                "Return the number of boundaries of the component, i.e. of "
                "components bounding it (the Corners of a Line, the Lines of a "
                "Surface, the Surfaces of a Block); 0 if the uuid has no "
                "relationship" )
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
                pybind11::arg( "component_id" ),
                pybind11::return_value_policy::reference,
                "Return the ComponentIDs of the boundaries of the component, "
                "i.e. the components bounding it; empty if the uuid has no "
                "relationship" )
            .def( "nb_incidences", &Relationships::nb_incidences,
                pybind11::arg( "component_id" ),
                "Return the number of incidences of the component, i.e. of "
                "components that it bounds (the Lines of a Corner, the "
                "Surfaces of a Line, the Blocks of a Surface); 0 if the uuid "
                "has no relationship" )
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
                pybind11::arg( "component_id" ),
                pybind11::return_value_policy::reference,
                "Return the ComponentIDs of the incidences of the component, "
                "i.e. the components it bounds; empty if the uuid has no "
                "relationship" )
            .def( "nb_internals", &Relationships::nb_internals,
                pybind11::arg( "component_id" ),
                "Return the number of internals of the component, i.e. of "
                "components lying inside it without bounding it (a Corner "
                "inside a Line or a Surface, a Line inside a Surface or a "
                "Block...); 0 if the uuid has no relationship" )
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
                pybind11::arg( "component_id" ),
                pybind11::return_value_policy::reference,
                "Return the ComponentIDs of the internals of the component, "
                "i.e. the components lying inside it without bounding it; "
                "empty if the uuid has no relationship" )
            .def( "nb_embeddings", &Relationships::nb_embeddings,
                pybind11::arg( "component_id" ),
                "Return the number of embeddings of the component, i.e. of "
                "components inside which it lies without bounding them; 0 if "
                "the uuid has no relationship" )
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
                pybind11::arg( "component_id" ),
                pybind11::return_value_policy::reference,
                "Return the ComponentIDs of the embeddings of the component, "
                "i.e. the components inside which it lies without bounding "
                "them; empty if the uuid has no relationship" )
            .def( "nb_items", &Relationships::nb_items,
                pybind11::arg( "component_id" ),
                "Return the number of items of the collection component "
                "(ModelBoundary or xxxCollection); 0 if the uuid has no "
                "relationship" )
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
                pybind11::arg( "component_id" ),
                pybind11::return_value_policy::reference,
                "Return the ComponentIDs of the items of the collection "
                "component (ModelBoundary or xxxCollection); empty if the uuid "
                "has no relationship" )
            .def( "nb_collections", &Relationships::nb_collections,
                pybind11::arg( "id" ),
                "Return the number of collections (ModelBoundary or "
                "xxxCollection) the component is an item of; 0 if the uuid has "
                "no relationship" )
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
                pybind11::arg( "id" ), pybind11::return_value_policy::reference,
                "Return the ComponentIDs of the collections (ModelBoundary or "
                "xxxCollection) the component is an item of; empty if the uuid "
                "has no relationship" )
            .def( "is_boundary", &Relationships::is_boundary,
                pybind11::arg( "boundary" ), pybind11::arg( "incidence" ),
                "Return True if the component boundary bounds the component "
                "incidence (the order matters: it is False when called the "
                "other way round)" )
            .def( "is_internal", &Relationships::is_internal,
                pybind11::arg( "internal" ), pybind11::arg( "embedding" ),
                "Return True if the component internal lies inside the "
                "component embedding without bounding it (the order matters)" )
            .def( "is_item", &Relationships::is_item, pybind11::arg( "item" ),
                pybind11::arg( "collection" ),
                "Return True if the component item belongs to the collection "
                "component (the order matters)" );
    }
} // namespace geode
