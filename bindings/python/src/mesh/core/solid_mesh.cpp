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

#include "../../common.hpp"
#include "../index_check.hpp"

#include <geode/basic/attribute_manager.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_facets.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

#define PYTHON_SOLID_MESH( dimension )                                         \
    const auto name##dimension =                                               \
        "SolidMesh" + std::to_string( dimension ) + "D";                       \
    pybind11::class_< SolidMesh##dimension##D, VertexSet,                      \
        CoordinateReferenceSystemManagers##dimension##D,                       \
        pybind11::smart_holder >( module, name##dimension.c_str() )            \
        .def_static( "create",                                                 \
            static_cast< std::unique_ptr< SolidMesh##dimension##D > ( * )() >( \
                &SolidMesh##dimension##D::create ) )                           \
        .def( "clone", &SolidMesh##dimension##D::clone )                       \
        .def( "nb_polyhedra", &SolidMesh##dimension##D::nb_polyhedra )         \
        .def(                                                                  \
            "are_edges_enabled", &SolidMesh##dimension##D::are_edges_enabled ) \
        .def( "enable_edges", &SolidMesh##dimension##D::enable_edges )         \
        .def( "disable_edges", &SolidMesh##dimension##D::disable_edges )       \
        .def( "edges",                                                         \
            static_cast< const SolidEdges##dimension##D& (                     \
                SolidMesh##dimension##D::*) () const >(                        \
                &SolidMesh##dimension##D::edges ),                             \
            pybind11::return_value_policy::reference_internal )                \
        .def( "are_facets_enabled",                                            \
            &SolidMesh##dimension##D::are_facets_enabled )                     \
        .def( "enable_facets", &SolidMesh##dimension##D::enable_facets )       \
        .def( "disable_facets", &SolidMesh##dimension##D::disable_facets )     \
        .def( "facets",                                                        \
            static_cast< const SolidFacets##dimension##D& (                    \
                SolidMesh##dimension##D::*) () const >(                        \
                &SolidMesh##dimension##D::facets ),                            \
            pybind11::return_value_policy::reference_internal )                \
        .def( "nb_polyhedron_vertices",                                        \
            geode::checked( "nb_polyhedron_vertices",                          \
                &SolidMesh##dimension##D::nb_polyhedron_vertices,              \
                geode::PolyhedronCheck{} ) )                                   \
        .def( "nb_polyhedron_facets",                                          \
            geode::checked( "nb_polyhedron_facets",                            \
                &SolidMesh##dimension##D::nb_polyhedron_facets,                \
                geode::PolyhedronCheck{} ) )                                   \
        .def( "nb_polyhedron_facet_vertices",                                  \
            geode::checked( "nb_polyhedron_facet_vertices",                    \
                &SolidMesh##dimension##D::nb_polyhedron_facet_vertices,        \
                geode::PolyhedronFacetCheck{} ) )                              \
        .def( "polyhedron_vertex",                                             \
            geode::checked( "polyhedron_vertex",                               \
                &SolidMesh##dimension##D::polyhedron_vertex,                   \
                geode::PolyhedronVertexCheck{} ) )                             \
        .def( "polyhedron_vertices",                                           \
            geode::checked( "polyhedron_vertices",                             \
                &SolidMesh##dimension##D::polyhedron_vertices,                 \
                geode::PolyhedronCheck{} ) )                                   \
        .def( "polyhedron_facet_vertex",                                       \
            geode::checked( "polyhedron_facet_vertex",                         \
                &SolidMesh##dimension##D::polyhedron_facet_vertex,             \
                geode::PolyhedronFacetVertexCheck{} ) )                        \
        .def( "polyhedron_facet_vertices",                                     \
            geode::checked( "polyhedron_facet_vertices",                       \
                &SolidMesh##dimension##D::polyhedron_facet_vertices,           \
                geode::PolyhedronFacetCheck{} ) )                              \
        .def( "polyhedron_adjacent",                                           \
            geode::checked( "polyhedron_adjacent",                             \
                &SolidMesh##dimension##D::polyhedron_adjacent,                 \
                geode::PolyhedronFacetCheck{} ) )                              \
        .def( "polyhedron_adjacent_facet",                                     \
            geode::checked( "polyhedron_adjacent_facet",                       \
                &SolidMesh##dimension##D::polyhedron_adjacent_facet,           \
                geode::PolyhedronFacetCheck{} ) )                              \
        .def( "is_polyhedron_facet_on_border",                                 \
            geode::checked( "is_polyhedron_facet_on_border",                   \
                &SolidMesh##dimension##D::is_polyhedron_facet_on_border,       \
                geode::PolyhedronFacetCheck{} ) )                              \
        .def( "polyhedron_facets_on_border",                                   \
            geode::checked( "polyhedron_facets_on_border",                     \
                &SolidMesh##dimension##D::polyhedron_facets_on_border,         \
                geode::PolyhedronCheck{} ) )                                   \
        .def( "edge_length", geode::checked( "edge_length",                    \
                                 &SolidMesh##dimension##D::edge_length,        \
                                 geode::VertexCheck{} ) )                      \
        .def( "polyhedron_barycenter",                                         \
            geode::checked( "polyhedron_barycenter",                           \
                &SolidMesh##dimension##D::polyhedron_barycenter,               \
                geode::PolyhedronCheck{} ) )                                   \
        .def( "facet_barycenter",                                              \
            geode::checked( "facet_barycenter",                                \
                &SolidMesh##dimension##D::facet_barycenter,                    \
                geode::VertexCheck{} ) )                                       \
        .def(                                                                  \
            "edge_barycenter", geode::checked( "edge_barycenter",              \
                                   &SolidMesh##dimension##D::edge_barycenter,  \
                                   geode::VertexCheck{} ) )                    \
        .def( "polyhedron_volume",                                             \
            geode::checked( "polyhedron_volume",                               \
                &SolidMesh##dimension##D::polyhedron_volume,                   \
                geode::PolyhedronCheck{} ) )                                   \
        .def( "polyhedron_facet_normal",                                       \
            geode::checked( "polyhedron_facet_normal",                         \
                &SolidMesh##dimension##D::polyhedron_facet_normal,             \
                geode::PolyhedronFacetCheck{} ) )                              \
        .def( "vertices_around_vertex",                                        \
            geode::checked( "vertices_around_vertex",                          \
                &SolidMesh##dimension##D::vertices_around_vertex,              \
                geode::VertexCheck{} ) )                                       \
        .def( "polyhedron_around_vertex",                                      \
            geode::checked( "polyhedron_around_vertex",                        \
                &SolidMesh##dimension##D::polyhedron_around_vertex,            \
                geode::VertexCheck{} ) )                                       \
        .def( "polyhedra_around_vertex",                                       \
            geode::checked( "polyhedra_around_vertex",                         \
                static_cast< const PolyhedraAroundVertex& (                    \
                    SolidMesh##dimension##D::*) ( index_t ) const >(           \
                    &SolidMesh##dimension##D::polyhedra_around_vertex ),       \
                geode::VertexCheck{} ) )                                       \
        .def( "polyhedra_around_polyhedron_vertex",                            \
            geode::checked( "polyhedra_around_polyhedron_vertex",              \
                static_cast< const PolyhedraAroundVertex& (                    \
                    SolidMesh##dimension##D::*) ( const PolyhedronVertex& )    \
                        const >(                                               \
                    &SolidMesh##dimension##D::polyhedra_around_vertex ),       \
                geode::PolyhedronVertexCheck{} ) )                             \
        .def( "polyhedra_around_edge",                                         \
            geode::checked( "polyhedra_around_edge",                           \
                static_cast< PolyhedraAroundEdge (                             \
                    SolidMesh##dimension##D::* )(                              \
                    const std::array< index_t, 2 >& ) const >(                 \
                    &SolidMesh##dimension##D::polyhedra_around_edge ),         \
                geode::VertexCheck{} ) )                                       \
        .def( "polyhedra_around_edge_with_hint",                               \
            geode::checked( "polyhedra_around_edge_with_hint",                 \
                static_cast< PolyhedraAroundEdge (                             \
                    SolidMesh##dimension##D::* )(                              \
                    const std::array< index_t, 2 >&, index_t ) const >(        \
                    &SolidMesh##dimension##D::polyhedra_around_edge ),         \
                geode::VertexCheck{} ) )                                       \
        .def( "polyhedra_from_facet_vertices",                                 \
            geode::checked( "polyhedra_from_facet_vertices",                   \
                &SolidMesh##dimension##D::polyhedra_from_facet_vertices,       \
                geode::VertexCheck{} ) )                                       \
        .def( "polyhedron_attribute_manager",                                  \
            &SolidMesh##dimension##D::polyhedron_attribute_manager,            \
            pybind11::return_value_policy::reference_internal )                \
        .def( "bounding_box", &SolidMesh##dimension##D::bounding_box )         \
        .def( "texture_manager", &SolidMesh##dimension##D::texture_manager,    \
            pybind11::keep_alive< 0, 1 >() )                                   \
        .def( "edge_vertices_in_polyhedron",                                   \
            geode::checked( "edge_vertices_in_polyhedron",                     \
                &SolidMesh##dimension##D::edge_vertices_in_polyhedron,         \
                geode::PolyhedronCheck{}, geode::VertexCheck{} ) )             \
        .def( "is_vertex_on_border",                                           \
            geode::checked( "is_vertex_on_border",                             \
                &SolidMesh##dimension##D::is_vertex_on_border,                 \
                geode::VertexCheck{} ) )                                       \
        .def( "is_tetrahedral_type",                                           \
            []( const SolidMesh< dimension >& solid ) {                        \
                return solid.type_name()                                       \
                       == TetrahedralSolid< dimension >::type_name_static();   \
            } )                                                                \
        .def( "is_polyhedral_type",                                            \
            []( const SolidMesh< dimension >& solid ) {                        \
                return solid.type_name()                                       \
                       == PolyhedralSolid< dimension >::type_name_static();    \
            } )                                                                \
        .def( "is_hybrid_type", []( const SolidMesh< dimension >& solid ) {    \
            return solid.type_name()                                           \
                   == HybridSolid< dimension >::type_name_static();            \
        } )

namespace geode
{
    void define_solid_mesh( pybind11::module& module )
    {
        PYTHON_SOLID_MESH( 3 );
    }
} // namespace geode
