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

#pragma once

#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Block );
    FORWARD_DECLARATION_DIMENSION_CLASS( Surface );
    FORWARD_DECLARATION_DIMENSION_CLASS( Line );
    ALIAS_3D( Block );
    ALIAS_2D_AND_3D( Surface );
    ALIAS_2D_AND_3D( Line );
    class BRep;
    class Section;
} // namespace geode

namespace geode
{
    struct BlockPolyhedronFacet
    {
        BlockPolyhedronFacet() = default;
        BlockPolyhedronFacet( PolyhedronFacet facet_in )
            : facet{ std::move( facet_in ) }
        {
        }

        PolyhedronFacet facet;
        PolygonVertices vertices;
    };

    struct BlockPolyhedraFacetVertices
    {
        index_t nb_facets() const
        {
            index_t counter{ 0 };
            if( oriented_polyhedron_facet )
            {
                counter++;
            }
            if( opposite_polyhedron_facet )
            {
                counter++;
            }
            return counter;
        }

        absl::optional< BlockPolyhedronFacet > oriented_polyhedron_facet;
        absl::optional< BlockPolyhedronFacet > opposite_polyhedron_facet;
    };

    struct SurfacePolygonEdge
    {
        SurfacePolygonEdge() = default;
        SurfacePolygonEdge( PolygonEdge edge_in ) : edge{ std::move( edge_in ) }
        {
        }

        PolygonEdge edge;
        std::array< index_t, 2 > vertices;
    };

    struct SurfacePolygonsEdgeVertices
    {
        index_t nb_edges() const
        {
            index_t counter{ 0 };
            if( oriented_edge )
            {
                counter++;
            }
            if( opposite_edge )
            {
                counter++;
            }
            return counter;
        }

        absl::optional< SurfacePolygonEdge > oriented_edge;
        absl::optional< SurfacePolygonEdge > opposite_edge;
    };

    PolygonVertices opengeode_model_api polygon_unique_vertices(
        const BRep& model, const Surface3D& surface, index_t polygon_id );

    newPolyhedraAroundFacet opengeode_model_api
        block_mesh_polyhedra_from_surface_polygon( const BRep& model,
            const Block3D& block,
            const Surface3D& surface,
            index_t polygon_id );

    absl::InlinedVector< BlockPolyhedronFacet, 2 > opengeode_model_api
        block_vertices_from_surface_polygon( const BRep& model,
            const Block3D& block,
            const Surface3D& surface,
            index_t polygon_id );

    BlockPolyhedraFacetVertices opengeode_model_api
        oriented_block_vertices_from_surface_polygon( const BRep& model,
            const Block3D& block,
            const Surface3D& surface,
            index_t polygon_id );

    absl::InlinedVector< SurfacePolygonEdge, 2 >
        opengeode_model_api surface_vertices_from_line_edge( const BRep& model,
            const Surface3D& surface,
            const Line3D& line,
            index_t edge_id );

    SurfacePolygonsEdgeVertices opengeode_model_api
        oriented_surface_vertices_from_line_edge( const BRep& model,
            const Surface3D& surface,
            const Line3D& line,
            index_t edge_id );

    absl::InlinedVector< SurfacePolygonEdge, 2 > opengeode_model_api
        surface_vertices_from_line_edge( const Section& model,
            const Surface2D& surface,
            const Line2D& line,
            index_t edge_id );

    SurfacePolygonsEdgeVertices opengeode_model_api
        oriented_surface_vertices_from_line_edge( const Section& model,
            const Surface2D& surface,
            const Line2D& line,
            index_t edge_id );
} // namespace geode