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

#include <geode/model/helpers/component_mesh_polygons.h>

#include <absl/container/inlined_vector.h>

#include <geode/mesh/core/detail/vertex_cycle.h>

#include <geode/model/helpers/component_mesh_edges.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    absl::FixedArray< std::vector< geode::index_t > >
        block_facet_from_model_unique_vertices( const geode::BRep& model,
            const geode::Block3D& block,
            absl::Span< const geode::index_t > facet_unique_vertices )
    {
        absl::FixedArray< std::vector< geode::index_t > >
            block_facet_from_unique_vertices( facet_unique_vertices.size() );
        for( const auto polygon_vertex_id :
            geode::LIndices{ facet_unique_vertices } )
        {
            block_facet_from_unique_vertices[polygon_vertex_id] =
                model.mesh_component_vertices(
                    facet_unique_vertices[polygon_vertex_id], block.id() );
        }
        return block_facet_from_unique_vertices;
    }

    geode::newPolyhedraAroundFacet block_mesh_polyhedra_around_triangle(
        const geode::SolidMesh3D& mesh,
        absl::Span< const std::vector< geode::index_t > >
            block_facet_from_unique_vertices )
    {
        geode::newPolyhedraAroundFacet polyhedra_around_polygon_facet;
        for( const auto first_vertex_id : block_facet_from_unique_vertices[0] )
        {
            for( const auto second_vertex_id :
                block_facet_from_unique_vertices[1] )
            {
                for( const auto third_vertex_id :
                    block_facet_from_unique_vertices[2] )
                {
                    const auto polyhedra_from_facet_vertices =
                        mesh.polyhedra_from_facet_vertices( { { first_vertex_id,
                            second_vertex_id, third_vertex_id } } );
                    for( const auto& polyhedron :
                        polyhedra_from_facet_vertices )
                    {
                        polyhedra_around_polygon_facet.emplace_back(
                            polyhedron );
                    }
                }
            }
        }
        return polyhedra_around_polygon_facet;
    }

    geode::newPolyhedraAroundFacet block_mesh_polyhedra_around_quadrangle(
        const geode::SolidMesh3D& mesh,
        absl::Span< const std::vector< geode::index_t > >
            block_facet_from_unique_vertices )
    {
        geode::newPolyhedraAroundFacet polyhedra_around_polygon_facet;
        for( const auto first_vertex_id : block_facet_from_unique_vertices[0] )
        {
            for( const auto second_vertex_id :
                block_facet_from_unique_vertices[1] )
            {
                for( const auto third_vertex_id :
                    block_facet_from_unique_vertices[2] )
                {
                    for( const auto fourth_vertex_id :
                        block_facet_from_unique_vertices[3] )
                    {
                        const auto polyhedra_from_facet_vertices =
                            mesh.polyhedra_from_facet_vertices(
                                { { first_vertex_id, second_vertex_id,
                                    third_vertex_id, fourth_vertex_id } } );
                        for( const auto& polyhedron :
                            polyhedra_from_facet_vertices )
                        {
                            polyhedra_around_polygon_facet.emplace_back(
                                polyhedron );
                        }
                    }
                }
            }
        }
        return polyhedra_around_polygon_facet;
    }

    geode::newPolyhedraAroundFacet
        block_mesh_polyhedra_from_unique_vertices_facet(
            const geode::BRep& model,
            const geode::Block3D& block,
            absl::Span< const geode::index_t > facet_unique_vertices )
    {
        const auto block_facet_from_unique_vertices =
            block_facet_from_model_unique_vertices(
                model, block, facet_unique_vertices );
        if( facet_unique_vertices.size() == 3 )
        {
            return block_mesh_polyhedra_around_triangle(
                block.mesh(), block_facet_from_unique_vertices );
        }
        if( facet_unique_vertices.size() == 4 )
        {
            return block_mesh_polyhedra_around_quadrangle(
                block.mesh(), block_facet_from_unique_vertices );
        }
        OPENGEODE_ASSERT_NOT_REACHED(
            "[helpers::block_mesh_polyhedra_from_unique_vertices_facet] "
            "Function is not yet implemented for facets other than "
            "triangles and quadrangles." );
        return {};
    }

    std::pair< absl::InlinedVector< geode::BlockPolyhedronFacet, 2 >,
        absl::InlinedVector< geode::BlockPolyhedronFacet, 2 > >
        polygon_vertices_to_block_facets_vertices( const geode::BRep& model,
            const geode::Block3D& block,
            const geode::PolygonVertices& polygon_unique_vertices )
    {
        const auto nb_vertices = polygon_unique_vertices.size();
        absl::InlinedVector< geode::BlockPolyhedronFacet, 2 >
            facets_block_vertices;
        absl::InlinedVector< geode::BlockPolyhedronFacet, 2 >
            facets_unique_vertices;
        const auto& block_mesh = block.mesh();
        for( const auto& polyhedron_facet :
            block_mesh_polyhedra_from_unique_vertices_facet(
                model, block, polygon_unique_vertices ) )
        {
            auto& f_block_vertices =
                facets_block_vertices.emplace_back( polyhedron_facet );
            f_block_vertices.vertices.resize( nb_vertices );
            auto& f_unique_vertices =
                facets_unique_vertices.emplace_back( polyhedron_facet );
            f_unique_vertices.vertices.resize( nb_vertices );
            for( const auto facet_vertex_id : geode::LRange{ nb_vertices } )
            {
                const auto block_vertex = block_mesh.polyhedron_facet_vertex(
                    { polyhedron_facet, facet_vertex_id } );
                f_block_vertices.vertices[facet_vertex_id] = block_vertex;
                f_unique_vertices.vertices[facet_vertex_id] =
                    model.unique_vertex(
                        { block.component_id(), block_vertex } );
            }
        }
        return { facets_block_vertices, facets_unique_vertices };
    }

    /*!
     * Returns the result of polygon_vertices_to_block_facets_vertices, but with
     * the lists oriented so that the first PolygonVertices of each vector is
     * linked to the facet with the same orientation as the given vertices.
     */
    std::tuple< absl::InlinedVector< geode::BlockPolyhedronFacet, 2 >,
        absl::InlinedVector< geode::BlockPolyhedronFacet, 2 >,
        bool >
        oriented_polygon_vertices_to_block_facets_vertices(
            const geode::BRep& model,
            const geode::Block3D& block,
            const geode::PolygonVertices& polygon_unique_vertices )
    {
        auto output = polygon_vertices_to_block_facets_vertices(
            model, block, polygon_unique_vertices );
        auto& facets_block_vertices = std::get< 0 >( output );
        auto& facets_unique_vertices = std::get< 1 >( output );

        if( facets_block_vertices.empty() )
        {
            return std::make_tuple( std::move( facets_block_vertices ),
                std::move( facets_unique_vertices ), false );
        }

        geode::detail::OrientedVertexCycle< geode::PolygonVertices >
            polygon_unique_vertices_cycle{ polygon_unique_vertices };
        if( polygon_unique_vertices_cycle.is_opposite(
                { facets_unique_vertices[0].vertices } ) )
        {
            return std::make_tuple( std::move( facets_block_vertices ),
                std::move( facets_unique_vertices ), false );
        }
        else if( facets_block_vertices.size() >= 2 )
        {
            OPENGEODE_ASSERT( polygon_unique_vertices_cycle.is_opposite(
                                  { facets_unique_vertices[1].vertices } ),
                "[block_vertices_from_surface_polygon] The block facets "
                "found from the polygon vertices have the same "
                "orientation." );
        }
        return std::make_tuple( std::move( facets_block_vertices ),
            std::move( facets_unique_vertices ), true );
    }

    absl::InlinedVector< geode::BlockPolyhedronFacet, 2 >
        polygon_and_facet_to_block_vertices(
            const geode::PolygonVertices& polygon_unique_vertices,
            absl::Span< const geode::BlockPolyhedronFacet >
                facets_block_vertices,
            absl::Span< const geode::BlockPolyhedronFacet >
                facets_unique_vertices )
    {
        absl::InlinedVector< geode::BlockPolyhedronFacet, 2 >
            p_to_block_vertices;
        for( const auto facet_id :
            geode::LRange{ facets_block_vertices.size() } )
        {
            const auto& facet_unique_vertices =
                facets_unique_vertices[facet_id];
            const auto& facet_uvertices = facet_unique_vertices.vertices;
            const auto& facet_block_vertices = facets_block_vertices[facet_id];
            auto& block_vertices =
                p_to_block_vertices.emplace_back( facet_unique_vertices.facet );
            for( const auto polygon_unique_vertex : polygon_unique_vertices )
            {
                const auto it =
                    absl::c_find( facet_uvertices, polygon_unique_vertex );
                const auto unique_vertex_in_facet =
                    static_cast< geode::index_t >(
                        std::distance( facet_uvertices.begin(), it ) );
                OPENGEODE_EXCEPTION(
                    unique_vertex_in_facet != facet_uvertices.size(),
                    "[block_vertices_from_surface_polygon] Could not find a "
                    "unique vertex matching a polygon of the surface and the "
                    "polyhedra of the block around it." );
                block_vertices.vertices.emplace_back(
                    facet_block_vertices.vertices[unique_vertex_in_facet] );
            }
        }
        return p_to_block_vertices;
    }

    template < geode::index_t dimension, class ModelType >
    std::array< std::vector< geode::index_t >, 2 >
        surface_edge_from_model_unique_vertices( const ModelType& model,
            const geode::Surface< dimension >& surface,
            absl::Span< const geode::index_t > edge_unique_vertices )
    {
        std::array< std::vector< geode::index_t >, 2 >
            surface_edge_from_unique_vertices;
        for( const auto edge_vertex_id : geode::LRange{ 2 } )
        {
            surface_edge_from_unique_vertices[edge_vertex_id] =
                model.mesh_component_vertices(
                    edge_unique_vertices[edge_vertex_id], surface.id() );
        }
        return surface_edge_from_unique_vertices;
    }

    template < geode::index_t dimension, class ModelType >
    absl::InlinedVector< geode::PolygonEdge, 2 >
        surface_mesh_polygons_from_unique_vertices_edge( const ModelType& model,
            const geode::Surface< dimension >& surface,
            absl::Span< const geode::index_t > edge_unique_vertices )
    {
        const auto surface_edge_from_unique_vertices =
            surface_edge_from_model_unique_vertices< dimension, ModelType >(
                model, surface, edge_unique_vertices );
        absl::InlinedVector< geode::PolygonEdge, 2 > polygons_around_edge;
        for( const auto first_vertex_id : surface_edge_from_unique_vertices[0] )
        {
            for( const auto second_vertex_id :
                surface_edge_from_unique_vertices[1] )
            {
                const auto polygons_from_edge_vertices =
                    surface.mesh().polygons_from_edge_vertices(
                        { { first_vertex_id, second_vertex_id } } );
                for( const auto& polygon : polygons_from_edge_vertices )
                {
                    polygons_around_edge.emplace_back( polygon );
                }
            }
        }
        return polygons_around_edge;
    }

    template < geode::index_t dimension, class ModelType >
    std::pair< absl::InlinedVector< geode::SurfacePolygonEdge, 2 >,
        absl::InlinedVector< geode::SurfacePolygonEdge, 2 > >
        edge_vertices_to_surface_vertices( const ModelType& model,
            const geode::Surface< dimension >& surface,
            absl::Span< const geode::index_t > edge_unique_vertices )
    {
        absl::InlinedVector< geode::SurfacePolygonEdge, 2 >
            surface_edges_vertices;
        absl::InlinedVector< geode::SurfacePolygonEdge, 2 >
            surface_edges_unique_vertices;
        const auto& surface_mesh = surface.mesh();
        for( const auto& polygon_edge :
            surface_mesh_polygons_from_unique_vertices_edge< dimension,
                ModelType >( model, surface, edge_unique_vertices ) )
        {
            auto& e_surface_vertices =
                surface_edges_vertices.emplace_back( polygon_edge );
            auto& e_unique_vertices =
                surface_edges_unique_vertices.emplace_back( polygon_edge );
            for( const auto edge_vertex_id : geode::LRange{ 2 } )
            {
                const auto surface_vertex = surface_mesh.polygon_edge_vertex(
                    polygon_edge, edge_vertex_id );
                e_surface_vertices.vertices[edge_vertex_id] = surface_vertex;
                e_unique_vertices.vertices[edge_vertex_id] =
                    model.unique_vertex(
                        { surface.component_id(), surface_vertex } );
            }
        }
        return { surface_edges_vertices, surface_edges_unique_vertices };
    }

    /*!
     * Returns the result of edge_vertices_to_surface_vertices, but with
     * the lists oriented so that the first PolygonVertices of each vector is
     * linked to the facet with the same orientation as the given vertices.
     */
    template < geode::index_t dimension, class ModelType >
    std::tuple< absl::InlinedVector< geode::SurfacePolygonEdge, 2 >,
        absl::InlinedVector< geode::SurfacePolygonEdge, 2 >,
        bool >
        oriented_edge_vertices_to_surface_vertices( const ModelType& model,
            const geode::Surface< dimension >& surface,
            const std::array< geode::index_t, 2 >& edge_unique_vertices )
    {
        auto output = edge_vertices_to_surface_vertices< dimension, ModelType >(
            model, surface, edge_unique_vertices );
        auto& surface_edges_vertices = std::get< 0 >( output );
        auto& surface_edges_unique_vertices = std::get< 1 >( output );

        if( surface_edges_vertices.empty() )
        {
            return std::make_tuple( std::move( surface_edges_vertices ),
                std::move( surface_edges_unique_vertices ), false );
        }

        geode::detail::OrientedVertexCycle< std::array< geode::index_t, 2 > >
            edge_unique_vertices_cycle{ edge_unique_vertices };
        if( edge_unique_vertices_cycle.is_opposite(
                { surface_edges_unique_vertices[0].vertices } ) )
        {
            return std::make_tuple( std::move( surface_edges_vertices ),
                std::move( surface_edges_unique_vertices ), false );
        }
        else if( surface_edges_vertices.size() >= 2 )
        {
            OPENGEODE_ASSERT(
                edge_unique_vertices_cycle.is_opposite(
                    { surface_edges_unique_vertices[1].vertices } ),
                "[block_vertices_from_surface_polygon] The block facets "
                "found from the polygon vertices have the same "
                "orientation." );
        }
        return std::make_tuple( std::move( surface_edges_vertices ),
            std::move( surface_edges_unique_vertices ), true );
    }

    absl::InlinedVector< geode::SurfacePolygonEdge, 2 >
        edge_info_to_surface_vertices(
            absl::Span< const geode::index_t > line_edge_unique_vertices,
            absl::Span< const geode::SurfacePolygonEdge >
                surface_edges_vertices,
            absl::Span< const geode::SurfacePolygonEdge >
                surface_edges_unique_vertices )
    {
        absl::InlinedVector< geode::SurfacePolygonEdge, 2 >
            e_to_surface_vertices;
        for( const auto edge_id :
            geode::LRange{ surface_edges_vertices.size() } )
        {
            const auto& surface_edge_unique_vertices =
                surface_edges_unique_vertices[edge_id];
            const auto& surface_edge_uvertices =
                surface_edge_unique_vertices.vertices;
            const auto& surface_edge_vertices = surface_edges_vertices[edge_id];
            auto& surface_vertices = e_to_surface_vertices.emplace_back(
                surface_edge_unique_vertices.edge );
            for( const auto edge_vertex_id : geode::LRange{ 2 } )
            {
                const auto it = absl::c_find( surface_edge_uvertices,
                    line_edge_unique_vertices[edge_vertex_id] );
                const auto unique_vertex_in_surface_edge =
                    static_cast< geode::index_t >(
                        std::distance( surface_edge_uvertices.begin(), it ) );
                OPENGEODE_EXCEPTION( unique_vertex_in_surface_edge
                                         != surface_edge_uvertices.size(),
                    "[surface_vertices_from_line_edge] Could not find a unique "
                    "vertex matching an edge of the line and the polygons of "
                    "the surface around it." );
                surface_vertices.vertices[edge_vertex_id] =
                    surface_edge_vertices
                        .vertices[unique_vertex_in_surface_edge];
            }
        }
        return e_to_surface_vertices;
    }

    template < geode::index_t dimension, class ModelType >
    absl::InlinedVector< geode::SurfacePolygonEdge, 2 >
        surface_vertices_from_model_line_edge( const ModelType& model,
            const geode::Surface< dimension >& surface,
            const geode::Line< dimension >& line,
            geode::index_t edge_id )
    {
        OPENGEODE_EXCEPTION( model.is_boundary( line, surface )
                                 || model.is_internal( line, surface ),
            "[helpers::surface_vertices_from_line_edge] The given line "
            "is neither boundary nor internal to the given surface in the "
            "given model." );
        const auto edge_unique_vertices =
            geode::edge_unique_vertices( model, line, edge_id );
        const auto edges_surface_and_unique_vertices =
            edge_vertices_to_surface_vertices< dimension, ModelType >(
                model, surface, edge_unique_vertices );
        return edge_info_to_surface_vertices( edge_unique_vertices,
            edges_surface_and_unique_vertices.first,
            edges_surface_and_unique_vertices.second );
    }

    template < geode::index_t dimension, class ModelType >
    geode::SurfacePolygonsEdgeVertices
        oriented_surface_vertices_from_model_line_edge( const ModelType& model,
            const geode::Surface< dimension >& surface,
            const geode::Line< dimension >& line,
            geode::index_t edge_id )
    {
        OPENGEODE_EXCEPTION( model.is_boundary( line, surface )
                                 || model.is_internal( line, surface ),
            "[helpers::oriented_surface_vertices_from_line_edge] The given "
            "line is neither boundary nor internal to the given surface in the "
            "given model." );
        const auto edge_unique_vertices =
            geode::edge_unique_vertices( model, line, edge_id );
        const auto output =
            oriented_edge_vertices_to_surface_vertices< dimension, ModelType >(
                model, surface, edge_unique_vertices );
        const auto& surface_edges_vertices = std::get< 0 >( output );
        if( surface_edges_vertices.empty() )
        {
            return {};
        }
        const auto& surface_edges_unique_vertices = std::get< 1 >( output );
        const auto v_are_oriented = std::get< 2 >( output );
        geode::SurfacePolygonsEdgeVertices e_to_surface_vertices;
        auto e_surface_vertices =
            edge_info_to_surface_vertices( edge_unique_vertices,
                surface_edges_vertices, surface_edges_unique_vertices );
        if( v_are_oriented )
        {
            e_to_surface_vertices.oriented_edge =
                std::move( e_surface_vertices[0] );
            if( surface_edges_vertices.size() == 2 )
            {
                e_to_surface_vertices.opposite_edge =
                    std::move( e_surface_vertices[1] );
            }
        }
        else
        {
            e_to_surface_vertices.opposite_edge =
                std::move( e_surface_vertices[0] );
            if( surface_edges_vertices.size() == 2 )
            {
                e_to_surface_vertices.oriented_edge =
                    std::move( e_surface_vertices[1] );
            }
        }
        return e_to_surface_vertices;
    }
} // namespace

namespace geode
{
    PolygonVertices surface_polygon_unique_vertices(
        const BRep& model, const Surface3D& surface, index_t polygon_id )
    {
        const auto& surface_mesh = surface.mesh();
        const auto nb_vertices = surface_mesh.nb_polygon_vertices( polygon_id );
        PolygonVertices polygon_unique_vertices( nb_vertices );
        for( const auto polygon_vertex_id : LRange{ nb_vertices } )
        {
            polygon_unique_vertices[polygon_vertex_id] =
                model.unique_vertex( { surface.component_id(),
                    surface_mesh.polygon_vertex(
                        { polygon_id, polygon_vertex_id } ) } );
        }
        return polygon_unique_vertices;
    }

    newPolyhedraAroundFacet block_mesh_polyhedra_from_surface_polygon(
        const BRep& model,
        const Block3D& block,
        const Surface3D& surface,
        index_t polygon_id )
    {
        OPENGEODE_EXCEPTION( model.is_boundary( surface, block )
                                 || model.is_internal( surface, block ),
            "[helpers::block_vertices_from_surface_polygon] The given surface "
            "is neither boundary nor internal to the given block in the given "
            "model." );
        return block_mesh_polyhedra_from_unique_vertices_facet( model, block,
            surface_polygon_unique_vertices( model, surface, polygon_id ) );
    }

    absl::InlinedVector< BlockPolyhedronFacet, 2 >
        block_vertices_from_surface_polygon( const BRep& model,
            const Block3D& block,
            const Surface3D& surface,
            index_t polygon_id )
    {
        OPENGEODE_EXCEPTION( model.is_boundary( surface, block )
                                 || model.is_internal( surface, block ),
            "[helpers::block_vertices_from_surface_polygon] The given surface "
            "is neither boundary nor internal to the given block in the given "
            "model." );
        const auto polygon_unique_vertices =
            surface_polygon_unique_vertices( model, surface, polygon_id );
        const auto facets_block_and_unique_vertices =
            polygon_vertices_to_block_facets_vertices(
                model, block, polygon_unique_vertices );
        return polygon_and_facet_to_block_vertices( polygon_unique_vertices,
            facets_block_and_unique_vertices.first,
            facets_block_and_unique_vertices.second );
    }

    BlockPolyhedraFacetVertices oriented_block_vertices_from_surface_polygon(
        const BRep& model,
        const Block3D& block,
        const Surface3D& surface,
        index_t polygon_id )
    {
        OPENGEODE_EXCEPTION( model.is_boundary( surface, block )
                                 || model.is_internal( surface, block ),
            "[helpers::block_vertices_from_surface_polygon] The given surface "
            "is neither boundary nor internal to the given block in the given "
            "model." );
        const auto polygon_unique_vertices =
            surface_polygon_unique_vertices( model, surface, polygon_id );
        const auto output = oriented_polygon_vertices_to_block_facets_vertices(
            model, block, polygon_unique_vertices );
        const auto& facets_block_vertices = std::get< 0 >( output );
        if( facets_block_vertices.empty() )
        {
            return {};
        }
        const auto& facets_unique_vertices = std::get< 1 >( output );
        const auto v_are_oriented = std::get< 2 >( output );
        BlockPolyhedraFacetVertices p_to_block_vertices;
        auto p_block_vertices =
            polygon_and_facet_to_block_vertices( polygon_unique_vertices,
                facets_block_vertices, facets_unique_vertices );
        if( v_are_oriented )
        {
            p_to_block_vertices.oriented_polyhedron_facet =
                std::move( p_block_vertices[0] );
            if( facets_block_vertices.size() == 2 )
            {
                p_to_block_vertices.opposite_polyhedron_facet =
                    std::move( p_block_vertices[1] );
            }
        }
        else
        {
            p_to_block_vertices.opposite_polyhedron_facet =
                std::move( p_block_vertices[0] );
            if( facets_block_vertices.size() == 2 )
            {
                p_to_block_vertices.oriented_polyhedron_facet =
                    std::move( p_block_vertices[1] );
            }
        }
        return p_to_block_vertices;
    }

    absl::InlinedVector< SurfacePolygonEdge, 2 >
        surface_vertices_from_line_edge( const BRep& model,
            const Surface3D& surface,
            const Line3D& line,
            index_t edge_id )
    {
        return surface_vertices_from_model_line_edge< 3, BRep >(
            model, surface, line, edge_id );
    }

    SurfacePolygonsEdgeVertices oriented_surface_vertices_from_line_edge(
        const BRep& model,
        const Surface3D& surface,
        const Line3D& line,
        index_t edge_id )
    {
        return oriented_surface_vertices_from_model_line_edge< 3, BRep >(
            model, surface, line, edge_id );
    }

    absl::InlinedVector< SurfacePolygonEdge, 2 >
        surface_vertices_from_line_edge( const Section& model,
            const Surface2D& surface,
            const Line2D& line,
            index_t edge_id )
    {
        return surface_vertices_from_model_line_edge< 2, Section >(
            model, surface, line, edge_id );
    }

    SurfacePolygonsEdgeVertices oriented_surface_vertices_from_line_edge(
        const Section& model,
        const Surface2D& surface,
        const Line2D& line,
        index_t edge_id )
    {
        return oriented_surface_vertices_from_model_line_edge< 2, Section >(
            model, surface, line, edge_id );
    }
} // namespace geode