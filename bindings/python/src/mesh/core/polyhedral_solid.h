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

#include <geode/mesh/core/polyhedral_solid.h>

#define PYTHON_POLYHEDRAL_SOLID( dimension )                                   \
    const auto name##dimension =                                               \
        "PolyhedralSolid" + std::to_string( dimension ) + "D";                 \
    pybind11::class_< PolyhedralSolid##dimension##D, VertexSet >(              \
        module, name##dimension.c_str() )                                      \
        .def( "create",                                                        \
            ( std::unique_ptr< PolyhedralSolid##dimension##D >( * )() )        \
                & PolyhedralSolid##dimension##D::create )                      \
        .def( "clone", &PolyhedralSolid##dimension##D::clone )                 \
        .def( "point", &PolyhedralSolid##dimension##D::point )                 \
        .def( "nb_polyhedra", &PolyhedralSolid##dimension##D::nb_polyhedra )   \
        .def( "nb_facets", &PolyhedralSolid##dimension##D::nb_facets )         \
        .def( "nb_edges", &PolyhedralSolid##dimension##D::nb_edges )           \
        .def( "nb_polyhedron_vertices",                                        \
            &PolyhedralSolid##dimension##D::nb_polyhedron_vertices )           \
        .def( "nb_polyhedron_facets",                                          \
            &PolyhedralSolid##dimension##D::nb_polyhedron_facets )             \
        .def( "nb_polyhedron_facet_vertices",                                  \
            &PolyhedralSolid##dimension##D::nb_polyhedron_facet_vertices )     \
        .def( "polyhedron_vertex",                                             \
            &PolyhedralSolid##dimension##D::polyhedron_vertex )                \
        .def( "polyhedron_facet",                                              \
            &PolyhedralSolid##dimension##D::polyhedron_facet )                 \
        .def( "polyhedron_facet_vertex",                                       \
            &PolyhedralSolid##dimension##D::polyhedron_facet_vertex )          \
        .def(                                                                  \
            "facet_vertices", &PolyhedralSolid##dimension##D::facet_vertices ) \
        .def( "edge_vertices", &PolyhedralSolid##dimension##D::edge_vertices ) \
        .def( "facet_from_vertices",                                           \
            &PolyhedralSolid##dimension##D::facet_from_vertices )              \
        .def( "edge_from_vertices",                                            \
            &PolyhedralSolid##dimension##D::edge_from_vertices )               \
        .def( "polyhedron_adjacent",                                           \
            &PolyhedralSolid##dimension##D::polyhedron_adjacent )              \
        .def( "polyhedron_adjacent_facet",                                     \
            &PolyhedralSolid##dimension##D::polyhedron_adjacent_facet )        \
        .def( "is_polyhedron_facet_on_border",                                 \
            &PolyhedralSolid##dimension##D::is_polyhedron_facet_on_border )    \
        .def( "polyhedron_facets_on_border",                                   \
            &PolyhedralSolid##dimension##D::polyhedron_facets_on_border )      \
        .def( "polyhedron_volume",                                             \
            &PolyhedralSolid##dimension##D::polyhedron_volume )                \
        .def( "facet_area", &PolyhedralSolid##dimension##D::facet_area )       \
        .def( "edge_length", &PolyhedralSolid##dimension##D::edge_length )     \
        .def( "polyhedron_barycenter",                                         \
            &PolyhedralSolid##dimension##D::polyhedron_barycenter )            \
        .def( "facet_barycenter",                                              \
            &PolyhedralSolid##dimension##D::facet_barycenter )                 \
        .def( "edge_barycenter",                                               \
            &PolyhedralSolid##dimension##D::edge_barycenter )                  \
        .def( "polyhedra_around_vertex",                                       \
            &PolyhedralSolid##dimension##D::polyhedra_around_vertex )          \
        .def( "polyhedra_around_edge",                                         \
            &PolyhedralSolid##dimension##D::polyhedra_around_edge )            \
        .def( "polyhedra_from_facet",                                          \
            &PolyhedralSolid##dimension##D::polyhedra_from_facet )             \
        .def( "polyhedron_attribute_manager",                                  \
            &PolyhedralSolid##dimension##D::polyhedron_attribute_manager,      \
            pybind11::return_value_policy::reference )                         \
        .def( "facet_attribute_manager",                                       \
            &PolyhedralSolid##dimension##D::facet_attribute_manager,           \
            pybind11::return_value_policy::reference )                         \
        .def( "edge_attribute_manager",                                        \
            &PolyhedralSolid##dimension##D::edge_attribute_manager,            \
            pybind11::return_value_policy::reference )                         \
        .def( "bounding_box", &PolyhedralSolid##dimension##D::bounding_box )

namespace geode
{
    void define_polyhedral_solid( pybind11::module& module )
    {
        PYTHON_POLYHEDRAL_SOLID( 3 );

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
