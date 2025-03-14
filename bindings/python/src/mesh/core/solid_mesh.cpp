/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
        CoordinateReferenceSystemManagers##dimension##D >(                     \
        module, name##dimension.c_str() )                                      \
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
            pybind11::return_value_policy::reference )                         \
        .def( "are_facets_enabled",                                            \
            &SolidMesh##dimension##D::are_facets_enabled )                     \
        .def( "enable_facets", &SolidMesh##dimension##D::enable_facets )       \
        .def( "disable_facets", &SolidMesh##dimension##D::disable_facets )     \
        .def( "facets",                                                        \
            static_cast< const SolidFacets##dimension##D& (                    \
                SolidMesh##dimension##D::*) () const >(                        \
                &SolidMesh##dimension##D::facets ),                            \
            pybind11::return_value_policy::reference )                         \
        .def( "nb_polyhedron_vertices",                                        \
            &SolidMesh##dimension##D::nb_polyhedron_vertices )                 \
        .def( "nb_polyhedron_facets",                                          \
            &SolidMesh##dimension##D::nb_polyhedron_facets )                   \
        .def( "nb_polyhedron_facet_vertices",                                  \
            &SolidMesh##dimension##D::nb_polyhedron_facet_vertices )           \
        .def(                                                                  \
            "polyhedron_vertex", &SolidMesh##dimension##D::polyhedron_vertex ) \
        .def( "polyhedron_vertices",                                           \
            &SolidMesh##dimension##D::polyhedron_vertices )                    \
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
        .def(                                                                  \
            "polyhedron_volume", &SolidMesh##dimension##D::polyhedron_volume ) \
        .def( "polyhedron_facet_normal",                                       \
            &SolidMesh##dimension##D::polyhedron_facet_normal )                \
        .def( "vertices_around_vertex",                                        \
            &SolidMesh##dimension##D::vertices_around_vertex )                 \
        .def( "polyhedron_around_vertex",                                      \
            &SolidMesh##dimension##D::polyhedron_around_vertex )               \
        .def( "polyhedra_around_vertex",                                       \
            static_cast< const PolyhedraAroundVertex& (                        \
                SolidMesh##dimension##D::*) ( index_t ) const >(               \
                &SolidMesh##dimension##D::polyhedra_around_vertex ) )          \
        .def( "polyhedra_around_polyhedron_vertex",                            \
            static_cast< const PolyhedraAroundVertex& (                        \
                SolidMesh##dimension##D::*) ( const PolyhedronVertex& )        \
                    const >(                                                   \
                &SolidMesh##dimension##D::polyhedra_around_vertex ) )          \
        .def( "polyhedra_around_edge",                                         \
            static_cast< PolyhedraAroundEdge ( SolidMesh##dimension##D::* )(   \
                const std::array< index_t, 2 >& ) const >(                     \
                &SolidMesh##dimension##D::polyhedra_around_edge ) )            \
        .def( "polyhedra_around_edge_with_hint",                               \
            static_cast< PolyhedraAroundEdge ( SolidMesh##dimension##D::* )(   \
                const std::array< index_t, 2 >&, index_t ) const >(            \
                &SolidMesh##dimension##D::polyhedra_around_edge ) )            \
        .def( "polyhedra_from_facet_vertices",                                 \
            &SolidMesh##dimension##D::polyhedra_from_facet_vertices )          \
        .def( "polyhedron_attribute_manager",                                  \
            &SolidMesh##dimension##D::polyhedron_attribute_manager,            \
            pybind11::return_value_policy::reference )                         \
        .def( "bounding_box", &SolidMesh##dimension##D::bounding_box )         \
        .def( "texture_manager", &SolidMesh##dimension##D::texture_manager,    \
            pybind11::return_value_policy::reference )                         \
        .def( "edge_vertices_in_polyhedron",                                   \
            &SolidMesh##dimension##D::edge_vertices_in_polyhedron )            \
        .def( "is_vertex_on_border",                                           \
            &SolidMesh##dimension##D::is_vertex_on_border )                    \
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

        pybind11::class_< PolyhedronVertex >( module, "PolyhedronVertex" )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "string", &PolyhedronVertex::string )
            .def_readwrite( "polyhedron_id", &PolyhedronVertex::polyhedron_id )
            .def_readwrite( "vertex_id", &PolyhedronVertex::vertex_id );

        pybind11::class_< PolyhedronFacet >( module, "PolyhedronFacet" )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "string", &PolyhedronFacet::string )
            .def_readwrite( "polyhedron_id", &PolyhedronFacet::polyhedron_id )
            .def_readwrite( "facet_id", &PolyhedronFacet::facet_id );

        pybind11::class_< PolyhedronFacetVertex >(
            module, "PolyhedronFacetVertex" )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolyhedronFacet, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "string", &PolyhedronFacetVertex::string )
            .def_readwrite(
                "polyhedron_facet", &PolyhedronFacetVertex::polyhedron_facet )
            .def_readwrite( "vertex_id", &PolyhedronFacetVertex::vertex_id );

        pybind11::class_< PolyhedronFacetEdge >( module, "PolyhedronFacetEdge" )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolyhedronFacet, local_index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "string", &PolyhedronFacetEdge::string )
            .def_readwrite(
                "polyhedron_facet", &PolyhedronFacetEdge::polyhedron_facet )
            .def_readwrite( "edge_id", &PolyhedronFacetEdge::edge_id );
    }
} // namespace geode
