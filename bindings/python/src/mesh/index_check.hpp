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

#pragma once

#include "../common.hpp"

#include <array>

#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/vertex_set.hpp>

namespace geode
{
    /*!
     * Checks to give to checked() and checked(), checked_as() and
     * checked_builder(): each one validates one argument of a mesh function
     * against the size of the mesh.
     */
    struct VertexCheck
    {
        template < typename Mesh >
        void operator()(
            std::string_view name, const Mesh& mesh, index_t vertex ) const
        {
            check_index( name, "vertex", vertex, mesh.nb_vertices() );
        }

        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            const std::array< index_t, 2 >& vertices ) const
        {
            for( const auto vertex : vertices )
            {
                ( *this )( name, mesh, vertex );
            }
        }

        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            absl::Span< const index_t > vertices ) const
        {
            for( const auto vertex : vertices )
            {
                ( *this )( name, mesh, vertex );
            }
        }
    };

    struct EdgeCheck
    {
        template < typename Mesh >
        void operator()(
            std::string_view name, const Mesh& mesh, index_t edge ) const
        {
            check_index( name, "edge", edge, mesh.nb_edges() );
        }
    };

    struct EdgeVertexCheck
    {
        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            const EdgeVertex& edge_vertex ) const
        {
            EdgeCheck{}( name, mesh, edge_vertex.edge_id );
            check_index( name, "local vertex", edge_vertex.vertex_id, 2 );
        }
    };

    struct PolygonCheck
    {
        template < typename Mesh >
        void operator()(
            std::string_view name, const Mesh& mesh, index_t polygon ) const
        {
            check_index( name, "polygon", polygon, mesh.nb_polygons() );
        }
    };

    struct PolygonVertexCheck
    {
        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            const PolygonVertex& polygon_vertex ) const
        {
            PolygonCheck{}( name, mesh, polygon_vertex.polygon_id );
            check_index( name, "local vertex", polygon_vertex.vertex_id,
                mesh.nb_polygon_vertices( polygon_vertex.polygon_id ) );
        }
    };

    struct PolygonEdgeCheck
    {
        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            const PolygonEdge& polygon_edge ) const
        {
            PolygonCheck{}( name, mesh, polygon_edge.polygon_id );
            check_index( name, "local edge", polygon_edge.edge_id,
                mesh.nb_polygon_edges( polygon_edge.polygon_id ) );
        }
    };

    struct PolyhedronCheck
    {
        template < typename Mesh >
        void operator()(
            std::string_view name, const Mesh& mesh, index_t polyhedron ) const
        {
            check_index( name, "polyhedron", polyhedron, mesh.nb_polyhedra() );
        }
    };

    struct PolyhedronVertexCheck
    {
        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            PolyhedronCheck{}( name, mesh, polyhedron_vertex.polyhedron_id );
            check_index( name, "local vertex", polyhedron_vertex.vertex_id,
                mesh.nb_polyhedron_vertices(
                    polyhedron_vertex.polyhedron_id ) );
        }
    };

    struct PolyhedronFacetCheck
    {
        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            const PolyhedronFacet& polyhedron_facet ) const
        {
            PolyhedronCheck{}( name, mesh, polyhedron_facet.polyhedron_id );
            check_index( name, "local facet", polyhedron_facet.facet_id,
                mesh.nb_polyhedron_facets( polyhedron_facet.polyhedron_id ) );
        }
    };

    struct PolyhedronFacetVertexCheck
    {
        template < typename Mesh >
        void operator()( std::string_view name,
            const Mesh& mesh,
            const PolyhedronFacetVertex& facet_vertex ) const
        {
            PolyhedronFacetCheck{}( name, mesh, facet_vertex.polyhedron_facet );
            check_index( name, "local facet vertex", facet_vertex.vertex_id,
                mesh.nb_polyhedron_facet_vertices(
                    facet_vertex.polyhedron_facet ) );
        }
    };
} // namespace geode
