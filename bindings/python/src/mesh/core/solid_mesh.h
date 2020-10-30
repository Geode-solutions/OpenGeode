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

#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_facets.h>
#include <geode/mesh/core/solid_mesh.h>

#define PYTHON_SOLID_MESH( dimension )                                         \
    const auto name##dimension =                                               \
        "SolidMesh" + std::to_string( dimension ) + "D";                       \
    pybind11::class_< SolidMesh##dimension##D, VertexSet >(                    \
        module, name##dimension.c_str() )                                      \
        .def_static(                                                           \
            "create", ( std::unique_ptr< SolidMesh##dimension##D >( * )() )    \
                          & SolidMesh##dimension##D::create )                  \
        .def( "clone", &SolidMesh##dimension##D::clone )                       \
        .def( "point", &SolidMesh##dimension##D::point )                       \
        .def( "nb_polyhedra", &SolidMesh##dimension##D::nb_polyhedra )         \
        .def(                                                                  \
            "are_edges_enabled", &SolidMesh##dimension##D::are_edges_enabled ) \
        .def( "enable_edges", &SolidMesh##dimension##D::enable_edges )         \
        .def( "disable_edges", &SolidMesh##dimension##D::disable_edges )       \
        .def( "edges",                                                         \
            ( const SolidEdges##dimension##D& (SolidMesh##dimension##D::*) ()  \
                    const )                                                    \
                & SolidMesh##dimension##D::edges,                              \
            pybind11::return_value_policy::reference )                         \
        .def( "are_facets_enabled",                                            \
            &SolidMesh##dimension##D::are_facets_enabled )                     \
        .def( "enable_facets", &SolidMesh##dimension##D::enable_facets )       \
        .def( "disable_facets", &SolidMesh##dimension##D::disable_facets )     \
        .def( "facets",                                                        \
            ( const SolidFacets##dimension##D& (SolidMesh##dimension##D::*) () \
                    const )                                                    \
                & SolidMesh##dimension##D::facets,                             \
            pybind11::return_value_policy::reference )                         \
        .def( "nb_polyhedron_vertices",                                        \
            &SolidMesh##dimension##D::nb_polyhedron_vertices )                 \
        .def( "nb_polyhedron_facets",                                          \
            &SolidMesh##dimension##D::nb_polyhedron_facets )                   \
        .def( "nb_polyhedron_facet_vertices",                                  \
            &SolidMesh##dimension##D::nb_polyhedron_facet_vertices )           \
        .def(                                                                  \
            "polyhedron_vertex", &SolidMesh##dimension##D::polyhedron_vertex ) \
        .def( "polyhedron_facet_vertex",                                       \
            &SolidMesh##dimension##D::polyhedron_facet_vertex )                \
        .def( "polyhedron_facet_vertices",                                     \
            &SolidMesh##dimension##D::polyhedron_facet_vertices )              \
        .def( "polyhedron_adjacent",                                           \
            &SolidMesh##dimension##D::polyhedron_adjacent )                    \
        .def( "polyhedron_adjacent_facet",                                     \
            &SolidMesh##dimension##D::polyhedron_adjacent_facet )              \
        .def( "is_polyhedron_facet_on_border",                                 \
            &SolidMesh##dimension##D::is_polyhedron_facet_on_border )          \
        .def( "polyhedron_facets_on_border",                                   \
            &SolidMesh##dimension##D::polyhedron_facets_on_border )            \
        .def( "edge_length", &SolidMesh##dimension##D::edge_length )           \
        .def( "polyhedron_barycenter",                                         \
            &SolidMesh##dimension##D::polyhedron_barycenter )                  \
        .def( "facet_barycenter", &SolidMesh##dimension##D::facet_barycenter ) \
        .def( "edge_barycenter", &SolidMesh##dimension##D::edge_barycenter )   \
        .def( "polyhedron_around_vertex",                                      \
            &SolidMesh##dimension##D::polyhedron_around_vertex )               \
        .def( "polyhedra_around_vertex",                                       \
            &SolidMesh##dimension##D::polyhedra_around_vertex )                \
        .def( "polyhedra_around_edge",                                         \
            &SolidMesh##dimension##D::polyhedra_around_edge )                  \
        .def( "polyhedra_from_facet",                                          \
            &SolidMesh##dimension##D::polyhedra_from_facet )                   \
        .def( "polyhedron_attribute_manager",                                  \
            &SolidMesh##dimension##D::polyhedron_attribute_manager,            \
            pybind11::return_value_policy::reference )                         \
        .def( "bounding_box", &SolidMesh##dimension##D::bounding_box )

namespace geode
{
    void define_solid_mesh( pybind11::module& module )
    {
        PYTHON_SOLID_MESH( 3 ).def( "polyhedron_facet_normal",
            &SolidMesh3D::polyhedron_facet_normal< 3 > );

        pybind11::class_< PolyhedronVertex >( module, "PolyhedronVertex" )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def_readwrite( "polyhedron_id", &PolyhedronVertex::polyhedron_id )
            .def_readwrite( "vertex_id", &PolyhedronVertex::vertex_id );

        pybind11::class_< PolyhedronFacet >( module, "PolyhedronFacet" )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def_readwrite( "polyhedron_id", &PolyhedronFacet::polyhedron_id )
            .def_readwrite( "facet_id", &PolyhedronFacet::facet_id );

        pybind11::class_< PolyhedronFacetVertex >(
            module, "PolyhedronFacetVertex" )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolyhedronFacet, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def_readwrite(
                "polyhedron_facet", &PolyhedronFacetVertex::polyhedron_facet )
            .def_readwrite( "vertex_id", &PolyhedronFacetVertex::vertex_id );
    }
} // namespace geode
