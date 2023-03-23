/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/algorithm.h>

#include <geode/mesh/core/detail/vertex_cycle.h>

#include <geode/model/helpers/component_mesh_edges.h>
#include <geode/model/helpers/component_mesh_vertices.h>
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
                model.component_mesh_vertices(
                    facet_unique_vertices[polygon_vertex_id], block.id() );
        }
        return block_facet_from_unique_vertices;
    }

    geode::PolyhedraAroundFacet block_mesh_polyhedra_around_triangle(
        const geode::SolidMesh3D& mesh,
        absl::Span< const std::vector< geode::index_t > >
            block_facet_from_unique_vertices )
    {
        geode::PolyhedraAroundFacet polyhedra_around_polygon_facet;
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

    geode::PolyhedraAroundFacet block_mesh_polyhedra_around_quadrangle(
        const geode::SolidMesh3D& mesh,
        absl::Span< const std::vector< geode::index_t > >
            block_facet_from_unique_vertices )
    {
        geode::PolyhedraAroundFacet polyhedra_around_polygon_facet;
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

    geode::PolyhedraAroundFacet block_mesh_polyhedra_from_unique_vertices_facet(
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

    /*!
     * Returns an array of two vectors of unique vertices, each vector
     * corresponding to the vertices of the model surface associated to the
     * given edge unique vertices.
     */
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
                model.component_mesh_vertices(
                    edge_unique_vertices[edge_vertex_id], surface.id() );
        }
        return surface_edge_from_unique_vertices;
    }

    /*!
     * Returns the PolygonEdges of the given surface from the given model,
     * corresponding to the edge given by its unique vertices.
     */
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

    /*!
     * Returns a pair of SurfacePolygonEdges, the first corresponding to the
     * edges of the model surface (and the indices of their vertices) whose
     * vertices are associated to the same unique vertices as the given edge
     * unique vertices, and the second corresponding to the same edges and the
     * indices of the unique vertices associated to their vertices.
     */
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
     * Returns the result of edge_vertices_to_surface_vertices, and checks if
     * the lists are oriented so that the first PolygonVertices of each vector
     * is linked to the facet with the same orientation as the given vertices.
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

        if( edge_unique_vertices[0]
                == surface_edges_unique_vertices[0].vertices[0]
            && edge_unique_vertices[1]
                   == surface_edges_unique_vertices[0].vertices[1] )
        {
            return std::make_tuple( std::move( surface_edges_vertices ),
                std::move( surface_edges_unique_vertices ), true );
        }
        OPENGEODE_ASSERT(
            edge_unique_vertices[0]
                    == surface_edges_unique_vertices[0].vertices[1]
                && edge_unique_vertices[1]
                       == surface_edges_unique_vertices[0].vertices[0],
            "[block_vertices_from_surface_polygon] The surface edges "
            "found from the polygon vertices don't have the same unique "
            "vertices." );
        return std::make_tuple( std::move( surface_edges_vertices ),
            std::move( surface_edges_unique_vertices ), false );
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

    template < class Polygons >
    void filter_polygons( Polygons& polygons )
    {
        for( auto& polygon : polygons )
        {
            geode::sort_unique( polygon.second );
        }
    }

    geode::ComponentMeshVertexTriplets model_polygon_pairs(
        const geode::BRep& model,
        const geode::PolygonVertices& polygon_unique_vertices,
        const geode::ComponentType& type )
    {
        return geode::component_mesh_vertex_triplets(
            model.component_mesh_vertices( polygon_unique_vertices[0] ),
            model.component_mesh_vertices( polygon_unique_vertices[1] ),
            model.component_mesh_vertices( polygon_unique_vertices[2] ), type );
    }

    geode::BRepComponentMeshPolygons::SurfacePolygons surface_polygons(
        const geode::BRep& model,
        const geode::PolygonVertices& polygon_unique_vertices )
    {
        auto surface_pairs =
            model_polygon_pairs( model, polygon_unique_vertices,
                geode::Surface3D::component_type_static() );
        if( surface_pairs.empty() )
        {
            return {};
        }
        geode::BRepComponentMeshPolygons::SurfacePolygons polygons;
        polygons.reserve( surface_pairs.size() );
        for( auto& surface_pair : surface_pairs )
        {
            const auto& surface = model.surface( surface_pair.first.id() );
            const auto& mesh = surface.mesh();
            for( auto& pair : surface_pair.second )
            {
                absl::c_sort( pair );
                for( auto& polygon_vertex :
                    mesh.polygons_around_vertex( pair[0] ) )
                {
                    auto vertices =
                        mesh.polygon_vertices( polygon_vertex.polygon_id );
                    absl::c_sort( vertices );
                    if( absl::c_equal( pair, vertices ) )
                    {
                        polygons[surface.id()].emplace_back(
                            polygon_vertex.polygon_id );
                        break;
                    }
                }
            }
        }
        filter_polygons( polygons );
        return polygons;
    }

    geode::BRepComponentMeshPolygons::BlockPolygons block_polygons(
        const geode::BRep& model,
        const geode::PolygonVertices& polygon_unique_vertices )
    {
        const auto block_pairs = model_polygon_pairs( model,
            polygon_unique_vertices, geode::Block3D::component_type_static() );
        if( block_pairs.empty() )
        {
            return {};
        }
        geode::BRepComponentMeshPolygons::BlockPolygons polygons;
        polygons.reserve( block_pairs.size() );
        for( const auto& block_pair : block_pairs )
        {
            const auto& block = model.block( block_pair.first.id() );
            const auto& mesh = block.mesh();
            for( const auto& pair : block_pair.second )
            {
                if( auto facet = mesh.polyhedron_facet_from_vertices(
                        { pair[0], pair[1], pair[2] } ) )
                {
                    polygons[block.id()].emplace_back(
                        std::move( facet.value() ) );
                }
            }
        }
        filter_polygons( polygons );
        return polygons;
    }

    geode::BRepComponentMeshPolygons brep_component_mesh_polygons(
        const geode::BRep& brep,
        const geode::PolygonVertices& polygon_unique_vertices )
    {
        geode::BRepComponentMeshPolygons polygons;
        polygons.surface_polygons =
            surface_polygons( brep, polygon_unique_vertices );
        polygons.block_polygons =
            block_polygons( brep, polygon_unique_vertices );
        return polygons;
    }
} // namespace

namespace geode
{
    PolygonVertices polygon_unique_vertices(
        const BRep& model, const Block3D& block, const PolyhedronFacet& facet )
    {
        const auto& solid_mesh = block.mesh();
        const auto nb_vertices =
            solid_mesh.nb_polyhedron_facet_vertices( facet );
        PolygonVertices polygon_unique_vertices( nb_vertices );
        for( const auto v : LRange{ nb_vertices } )
        {
            polygon_unique_vertices[v] =
                model.unique_vertex( { block.component_id(),
                    solid_mesh.polyhedron_facet_vertex( { facet, v } ) } );
        }
        return polygon_unique_vertices;
    }

    PolygonVertices polygon_unique_vertices(
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

    BRepComponentMeshPolygons component_mesh_polygons(
        const BRep& brep, const Surface3D& surface, index_t polygon_id )
    {
        return brep_component_mesh_polygons(
            brep, polygon_unique_vertices( brep, surface, polygon_id ) );
    }

    BRepComponentMeshPolygons component_mesh_polygons(
        const BRep& brep, const Block3D& block, const PolyhedronFacet& facet )
    {
        return brep_component_mesh_polygons(
            brep, polygon_unique_vertices( brep, block, facet ) );
    }

    PolyhedraAroundFacet block_mesh_polyhedra_from_surface_polygon(
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
            polygon_unique_vertices( model, surface, polygon_id ) );
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
        const auto polygon_vertices =
            polygon_unique_vertices( model, surface, polygon_id );
        const auto facets_block_and_unique_vertices =
            polygon_vertices_to_block_facets_vertices(
                model, block, polygon_vertices );
        return polygon_and_facet_to_block_vertices( polygon_vertices,
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
        const auto polygon_vertices =
            polygon_unique_vertices( model, surface, polygon_id );
        const auto output = oriented_polygon_vertices_to_block_facets_vertices(
            model, block, polygon_vertices );
        const auto& facets_block_vertices = std::get< 0 >( output );
        if( facets_block_vertices.empty() )
        {
            return {};
        }
        const auto& facets_unique_vertices = std::get< 1 >( output );
        const auto v_are_oriented = std::get< 2 >( output );
        BlockPolyhedraFacetVertices p_to_block_vertices;
        auto p_block_vertices = polygon_and_facet_to_block_vertices(
            polygon_vertices, facets_block_vertices, facets_unique_vertices );
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