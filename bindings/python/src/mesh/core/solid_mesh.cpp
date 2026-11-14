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
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Solid made of polyhedra whose vertices have coordinates" )            \
        .def(                                                                  \
            "__repr__",                                                        \
            []( const SolidMesh##dimension##D& object ) {                      \
                return identifier_repr( object, object.type_name().get(),      \
                    { { "nb_vertices", python_repr( object.nb_vertices() ) },  \
                        { "nb_polyhedra",                                      \
                            python_repr( object.nb_polyhedra() ) } } );        \
            },                                                                 \
            "Return an unambiguous description of the mesh: its type, its "    \
            "id, its name if it has one and its numbers of vertices and "      \
            "polyhedra" )                                                      \
        .def_static( "create",                                                 \
            static_cast< std::unique_ptr< SolidMesh##dimension##D > ( * )() >( \
                &SolidMesh##dimension##D::create ),                            \
            "Create an empty solid mesh with the default data structure" )     \
        .def( "point",                                                         \
            geode::checked_as< SolidMesh##dimension##D >( "point",             \
                &CoordinateReferenceSystemManagers< dimension >::point,        \
                geode::VertexCheck{} ),                                        \
            "Return the coordinates of a vertex in the active coordinate "     \
            "reference system",                                                \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "clone", &SolidMesh##dimension##D::clone,                        \
            "Return a copy of the solid mesh, with the same data structure, "  \
            "name and identifier" )                                            \
        .def( "nb_polyhedra", &SolidMesh##dimension##D::nb_polyhedra,          \
            "Return the number of polyhedra" )                                 \
        .def( "are_edges_enabled",                                             \
            &SolidMesh##dimension##D::are_edges_enabled,                       \
            "Return True if the edges of the solid are enabled" )              \
        .def( "enable_edges", &SolidMesh##dimension##D::enable_edges,          \
            "Compute and enable the edges of the solid, which are then "       \
            "available through edges()" )                                      \
        .def( "disable_edges", &SolidMesh##dimension##D::disable_edges,        \
            "Disable the edges of the solid and free them" )                   \
        .def( "edges",                                                         \
            static_cast< const SolidEdges##dimension##D& (                     \
                SolidMesh##dimension##D::*) () const >(                        \
                &SolidMesh##dimension##D::edges ),                             \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the edges of the solid; raise an error if they are not "   \
            "enabled" )                                                        \
        .def( "are_facets_enabled",                                            \
            &SolidMesh##dimension##D::are_facets_enabled,                      \
            "Return True if the facets of the solid are enabled" )             \
        .def( "enable_facets", &SolidMesh##dimension##D::enable_facets,        \
            "Compute and enable the facets of the solid, which are then "      \
            "available through facets()" )                                     \
        .def( "disable_facets", &SolidMesh##dimension##D::disable_facets,      \
            "Disable the facets of the solid and free them" )                  \
        .def( "facets",                                                        \
            static_cast< const SolidFacets##dimension##D& (                    \
                SolidMesh##dimension##D::*) () const >(                        \
                &SolidMesh##dimension##D::facets ),                            \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the facets of the solid; raise an error if they are not "  \
            "enabled" )                                                        \
        .def( "nb_polyhedron_vertices",                                        \
            geode::checked( "nb_polyhedron_vertices",                          \
                &SolidMesh##dimension##D::nb_polyhedron_vertices,              \
                geode::PolyhedronCheck{} ),                                    \
            "Return the number of vertices of the polyhedron",                 \
            pybind11::arg( "polyhedron_id" ) )                                 \
        .def( "nb_polyhedron_facets",                                          \
            geode::checked( "nb_polyhedron_facets",                            \
                &SolidMesh##dimension##D::nb_polyhedron_facets,                \
                geode::PolyhedronCheck{} ),                                    \
            "Return the number of facets of the polyhedron",                   \
            pybind11::arg( "polyhedron_id" ) )                                 \
        .def( "nb_polyhedron_facet_vertices",                                  \
            geode::checked( "nb_polyhedron_facet_vertices",                    \
                &SolidMesh##dimension##D::nb_polyhedron_facet_vertices,        \
                geode::PolyhedronFacetCheck{} ),                               \
            "Return the number of vertices of the polyhedron facet",           \
            pybind11::arg( "polyhedron_facet" ) )                              \
        .def( "polyhedron_vertex",                                             \
            geode::checked( "polyhedron_vertex",                               \
                &SolidMesh##dimension##D::polyhedron_vertex,                   \
                geode::PolyhedronVertexCheck{} ),                              \
            "Return the index in the mesh of a local vertex of a polyhedron",  \
            pybind11::arg( "polyhedron_vertex" ) )                             \
        .def( "polyhedron_vertices",                                           \
            geode::checked( "polyhedron_vertices",                             \
                &SolidMesh##dimension##D::polyhedron_vertices,                 \
                geode::PolyhedronCheck{} ),                                    \
            "Return the indices in the mesh of all the vertices of the "       \
            "polyhedron",                                                      \
            pybind11::arg( "polyhedron_id" ) )                                 \
        .def( "polyhedron_facet_vertex",                                       \
            geode::checked( "polyhedron_facet_vertex",                         \
                &SolidMesh##dimension##D::polyhedron_facet_vertex,             \
                geode::PolyhedronFacetVertexCheck{} ),                         \
            "Return the index in the mesh of a local vertex of a polyhedron "  \
            "facet",                                                           \
            pybind11::arg( "polyhedron_facet_vertex" ) )                       \
        .def( "polyhedron_facet_vertices",                                     \
            geode::checked( "polyhedron_facet_vertices",                       \
                &SolidMesh##dimension##D::polyhedron_facet_vertices,           \
                geode::PolyhedronFacetCheck{} ),                               \
            "Return the indices in the mesh of all the vertices of the "       \
            "polyhedron facet",                                                \
            pybind11::arg( "polyhedron_facet" ) )                              \
        .def( "polyhedron_adjacent",                                           \
            geode::checked( "polyhedron_adjacent",                             \
                &SolidMesh##dimension##D::polyhedron_adjacent,                 \
                geode::PolyhedronFacetCheck{} ),                               \
            "Return the index of the polyhedron adjacent through the facet, "  \
            "or None if there is none",                                        \
            pybind11::arg( "polyhedron_facet" ) )                              \
        .def( "polyhedron_adjacent_facet",                                     \
            geode::checked( "polyhedron_adjacent_facet",                       \
                &SolidMesh##dimension##D::polyhedron_adjacent_facet,           \
                geode::PolyhedronFacetCheck{} ),                               \
            "Return the facet of the adjacent polyhedron through which the "   \
            "polyhedra are adjacent, or None if there is none",                \
            pybind11::arg( "polyhedron_facet" ) )                              \
        .def( "is_polyhedron_facet_on_border",                                 \
            geode::checked( "is_polyhedron_facet_on_border",                   \
                &SolidMesh##dimension##D::is_polyhedron_facet_on_border,       \
                geode::PolyhedronFacetCheck{} ),                               \
            "Return True if the polyhedron has no adjacent polyhedron "        \
            "through the facet",                                               \
            pybind11::arg( "polyhedron_facet" ) )                              \
        .def( "polyhedron_facets_on_border",                                   \
            geode::checked( "polyhedron_facets_on_border",                     \
                &SolidMesh##dimension##D::polyhedron_facets_on_border,         \
                geode::PolyhedronCheck{} ),                                    \
            "Return the facets of the polyhedron that are on the border",      \
            pybind11::arg( "polyhedron_id" ) )                                 \
        .def( "edge_length",                                                   \
            geode::checked( "edge_length",                                     \
                &SolidMesh##dimension##D::edge_length, geode::VertexCheck{} ), \
            "Return the length of the edge between the two given vertices",    \
            pybind11::arg( "edge_vertices" ) )                                 \
        .def( "polyhedron_barycenter",                                         \
            geode::checked( "polyhedron_barycenter",                           \
                &SolidMesh##dimension##D::polyhedron_barycenter,               \
                geode::PolyhedronCheck{} ),                                    \
            "Return the barycenter of the polyhedron",                         \
            pybind11::arg( "polyhedron_id" ) )                                 \
        .def( "facet_barycenter",                                              \
            geode::checked( "facet_barycenter",                                \
                &SolidMesh##dimension##D::facet_barycenter,                    \
                geode::VertexCheck{} ),                                        \
            "Return the barycenter of the facet defined by the given "         \
            "vertices",                                                        \
            pybind11::arg( "facet_vertices" ) )                                \
        .def( "edge_barycenter",                                               \
            geode::checked( "edge_barycenter",                                 \
                &SolidMesh##dimension##D::edge_barycenter,                     \
                geode::VertexCheck{} ),                                        \
            "Return the barycenter of the edge between the two given "         \
            "vertices",                                                        \
            pybind11::arg( "edge_vertices" ) )                                 \
        .def( "polyhedron_volume",                                             \
            geode::checked( "polyhedron_volume",                               \
                &SolidMesh##dimension##D::polyhedron_volume,                   \
                geode::PolyhedronCheck{} ),                                    \
            "Return the volume of the polyhedron",                             \
            pybind11::arg( "polyhedron_id" ) )                                 \
        .def( "polyhedron_facet_normal",                                       \
            geode::checked( "polyhedron_facet_normal",                         \
                &SolidMesh##dimension##D::polyhedron_facet_normal,             \
                geode::PolyhedronFacetCheck{} ),                               \
            "Return the normal of the polyhedron facet, oriented towards the " \
            "inside of the polyhedron, or None if the facet is degenerated",   \
            pybind11::arg( "polyhedron_facet" ) )                              \
        .def( "vertices_around_vertex",                                        \
            geode::checked( "vertices_around_vertex",                          \
                &SolidMesh##dimension##D::vertices_around_vertex,              \
                geode::VertexCheck{} ),                                        \
            "Return the vertices linked to the vertex by a polyhedron edge; "  \
            "the polyhedron adjacencies must be computed",                     \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "polyhedron_around_vertex",                                      \
            geode::checked( "polyhedron_around_vertex",                        \
                &SolidMesh##dimension##D::polyhedron_around_vertex,            \
                geode::VertexCheck{} ),                                        \
            "Return one polyhedron vertex located at the vertex, or None if "  \
            "no polyhedron uses it",                                           \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "polyhedra_around_vertex",                                       \
            geode::checked( "polyhedra_around_vertex",                         \
                static_cast< const PolyhedraAroundVertex& (                    \
                    SolidMesh##dimension##D::*) ( index_t ) const >(           \
                    &SolidMesh##dimension##D::polyhedra_around_vertex ),       \
                geode::VertexCheck{} ),                                        \
            "Return all the polyhedron vertices located at the vertex; the "   \
            "polyhedron adjacencies must be computed",                         \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "polyhedra_around_polyhedron_vertex",                            \
            geode::checked( "polyhedra_around_polyhedron_vertex",              \
                static_cast< const PolyhedraAroundVertex& (                    \
                    SolidMesh##dimension##D::*) ( const PolyhedronVertex& )    \
                        const >(                                               \
                    &SolidMesh##dimension##D::polyhedra_around_vertex ),       \
                geode::PolyhedronVertexCheck{} ),                              \
            "Return all the polyhedron vertices located at the vertex of the " \
            "given polyhedron vertex; the polyhedron adjacencies must be "     \
            "computed",                                                        \
            pybind11::arg( "polyhedron_vertex" ) )                             \
        .def( "polyhedra_around_edge",                                         \
            geode::checked( "polyhedra_around_edge",                           \
                static_cast< PolyhedraAroundEdge (                             \
                    SolidMesh##dimension##D::* )(                              \
                    const std::array< index_t, 2 >& ) const >(                 \
                    &SolidMesh##dimension##D::polyhedra_around_edge ),         \
                geode::VertexCheck{} ),                                        \
            "Return the indices of all the polyhedra that contain both "       \
            "vertices of the edge; the polyhedron adjacencies must be "        \
            "computed",                                                        \
            pybind11::arg( "vertices" ) )                                      \
        .def( "polyhedra_around_edge_with_hint",                               \
            geode::checked( "polyhedra_around_edge_with_hint",                 \
                static_cast< PolyhedraAroundEdge (                             \
                    SolidMesh##dimension##D::* )(                              \
                    const std::array< index_t, 2 >&, index_t ) const >(        \
                    &SolidMesh##dimension##D::polyhedra_around_edge ),         \
                geode::VertexCheck{}, geode::PolyhedronCheck{} ),              \
            "Return the indices of all the polyhedra that contain both "       \
            "vertices of the edge, starting the search from the polyhedron "   \
            "first_polyhedron, which must contain the edge: the result "       \
            "always contains first_polyhedron, even if it does not contain "   \
            "the edge (no polyhedron is then reliably found around the "       \
            "edge); the polyhedron adjacencies must be computed",              \
            pybind11::arg( "vertices" ), pybind11::arg( "first_polyhedron" ) ) \
        .def( "polyhedra_from_facet_vertices",                                 \
            geode::checked( "polyhedra_from_facet_vertices",                   \
                &SolidMesh##dimension##D::polyhedra_from_facet_vertices,       \
                geode::VertexCheck{} ),                                        \
            "Return all the polyhedron facets made of the given vertices; "    \
            "the polyhedron adjacencies must be computed",                     \
            pybind11::arg( "facet_vertices" ) )                                \
        .def( "polyhedron_attribute_manager",                                  \
            &SolidMesh##dimension##D::polyhedron_attribute_manager,            \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the manager of the attributes associated with the "        \
            "polyhedra" )                                                      \
        .def( "bounding_box", &SolidMesh##dimension##D::bounding_box,          \
            "Return the bounding box of the vertices of the solid mesh" )      \
        .def( "texture_manager",                                               \
            geode::texture_manager_with_elements(                              \
                &SolidMesh##dimension##D::texture_manager, "nb_polyhedra" ),   \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the manager of the textures of the solid mesh, which "     \
            "keeps the mesh alive" )                                           \
        .def( "edge_vertices_in_polyhedron",                                   \
            geode::checked( "edge_vertices_in_polyhedron",                     \
                &SolidMesh##dimension##D::edge_vertices_in_polyhedron,         \
                geode::PolyhedronCheck{}, geode::VertexCheck{} ),              \
            "Return the local indices in the polyhedron of the two given "     \
            "vertices, NO_LID for a vertex that is not in the polyhedron",     \
            pybind11::arg( "polyhedron_id" ),                                  \
            pybind11::arg( "edge_vertices" ) )                                 \
        .def( "is_vertex_on_border",                                           \
            geode::checked( "is_vertex_on_border",                             \
                &SolidMesh##dimension##D::is_vertex_on_border,                 \
                geode::VertexCheck{} ),                                        \
            "Return True if a polyhedron facet around the vertex is on the "   \
            "border; the polyhedron adjacencies must be computed",             \
            pybind11::arg( "vertex_id" ) )                                     \
        .def(                                                                  \
            "is_tetrahedral_type",                                             \
            []( const SolidMesh< dimension >& solid ) {                        \
                return solid.type_name()                                       \
                       == TetrahedralSolid< dimension >::type_name_static();   \
            },                                                                 \
            "Return True if the solid mesh is a tetrahedral solid" )           \
        .def(                                                                  \
            "is_polyhedral_type",                                              \
            []( const SolidMesh< dimension >& solid ) {                        \
                return solid.type_name()                                       \
                       == PolyhedralSolid< dimension >::type_name_static();    \
            },                                                                 \
            "Return True if the solid mesh is a polyhedral solid" )            \
        .def(                                                                  \
            "is_hybrid_type",                                                  \
            []( const SolidMesh< dimension >& solid ) {                        \
                return solid.type_name()                                       \
                       == HybridSolid< dimension >::type_name_static();        \
            },                                                                 \
            "Return True if the solid mesh is a hybrid solid" )

namespace geode
{
    void define_solid_mesh( pybind11::module& module )
    {
        PYTHON_SOLID_MESH( 3 );
    }
} // namespace geode
