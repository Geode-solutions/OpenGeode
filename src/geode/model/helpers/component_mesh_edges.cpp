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

#include <geode/model/helpers/component_mesh_edges.h>

#include <geode/basic/algorithm.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/component_mesh_vertices.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < class Edges >
    void filter_edges( Edges& edges )
    {
        for( auto& edge : edges )
        {
            geode::sort_unique( edge.second );
        }
    }

    template < class ModelType >
    geode::ComponentMeshVertexPairs model_edge_pairs( const ModelType& model,
        const std::array< geode::index_t, 2 >& edge_unique_vertices,
        const geode::ComponentType& type )
    {
        return geode::component_mesh_vertex_pairs(
            model.component_mesh_vertices( edge_unique_vertices[0] ),
            model.component_mesh_vertices( edge_unique_vertices[1] ), type );
    }

    template < class ModelType >
    std::array< geode::index_t, 2 > edge_unique_vertices(
        const ModelType& model,
        const geode::ComponentID& component_id,
        const std::array< geode::index_t, 2 >& edge_vertices )
    {
        std::array< geode::index_t, 2 > edge_unique_vertices;
        for( const auto v : geode::LRange{ 2 } )
        {
            edge_unique_vertices[v] =
                model.unique_vertex( { component_id, edge_vertices[v] } );
        }
        return edge_unique_vertices;
    }

    template < class ModelType, geode::index_t dimension >
    geode::ModelComponentMeshEdges::LineEdges line_edges(
        const ModelType& model,
        const std::array< geode::index_t, 2 >& edge_unique_vertices )
    {
        const auto line_pairs = model_edge_pairs( model, edge_unique_vertices,
            geode::Line< dimension >::component_type_static() );
        if( line_pairs.empty() )
        {
            return {};
        }
        geode::ModelComponentMeshEdges::LineEdges edges;
        edges.reserve( line_pairs.size() );
        for( const auto& line_pair : line_pairs )
        {
            const auto& line = model.line( line_pair.first.id() );
            const auto& mesh = line.mesh();
            for( const auto& pair : line_pair.second )
            {
                for( const auto& edge_vertex :
                    mesh.edges_around_vertex( pair.first ) )
                {
                    const auto opposite_vertex =
                        mesh.edge_vertex( edge_vertex.opposite() );
                    if( opposite_vertex == pair.second )
                    {
                        edges[line.id()].emplace_back( edge_vertex.edge_id );
                    }
                }
            }
        }
        filter_edges( edges );
        return edges;
    }

    template < class ModelType, geode::index_t dimension >
    geode::ModelComponentMeshEdges::SurfaceEdges surface_edges(
        const ModelType& model,
        const std::array< geode::index_t, 2 >& edge_unique_vertices )
    {
        const auto surface_pairs =
            model_edge_pairs( model, edge_unique_vertices,
                geode::Surface< dimension >::component_type_static() );
        if( surface_pairs.empty() )
        {
            return {};
        }
        geode::ModelComponentMeshEdges::SurfaceEdges edges;
        edges.reserve( surface_pairs.size() );
        for( const auto& surface_pair : surface_pairs )
        {
            const auto& surface = model.surface( surface_pair.first.id() );
            const auto& mesh = surface.mesh();
            for( const auto& pair : surface_pair.second )
            {
                for( auto& polygon_vertex : mesh.polygons_from_edge_vertices(
                         std::array< geode::index_t, 2 >{
                             pair.first, pair.second } ) )
                {
                    edges[surface.id()].emplace_back(
                        std::move( polygon_vertex ) );
                }
            }
        }
        filter_edges( edges );
        return edges;
    }

    geode::BRepComponentMeshEdges::BlockEdges block_edges(
        const geode::BRep& model,
        const std::array< geode::index_t, 2 >& edge_unique_vertices )
    {
        const auto block_pairs = model_edge_pairs( model, edge_unique_vertices,
            geode::Block3D::component_type_static() );
        if( block_pairs.empty() )
        {
            return {};
        }
        geode::BRepComponentMeshEdges::BlockEdges edges;
        edges.reserve( block_pairs.size() );
        for( const auto& block_pair : block_pairs )
        {
            const auto& block = model.block( block_pair.first.id() );
            const auto& mesh = block.mesh();
            for( const auto& pair : block_pair.second )
            {
                if( auto edge = mesh.polyhedron_facet_edge_from_vertices(
                        { pair.first, pair.second } ) )
                {
                    edges[block.id()].emplace_back( std::move( edge.value() ) );
                }
            }
        }
        filter_edges( edges );
        return edges;
    }

    template < class ModelType, geode::index_t dimension >
    void model_component_mesh_edges( geode::ModelComponentMeshEdges& edges,
        const ModelType& model,
        const std::array< geode::index_t, 2 >& edge_unique_vertices )
    {
        edges.line_edges =
            line_edges< ModelType, dimension >( model, edge_unique_vertices );
        edges.surface_edges = surface_edges< ModelType, dimension >(
            model, edge_unique_vertices );
    }

    geode::SectionComponentMeshEdges section_component_mesh_edges(
        const geode::Section& section,
        const std::array< geode::index_t, 2 >& edge_unique_vertices )
    {
        geode::SectionComponentMeshEdges edges;
        model_component_mesh_edges< geode::Section, 2 >(
            edges, section, edge_unique_vertices );
        return edges;
    }

    geode::BRepComponentMeshEdges brep_component_mesh_edges(
        const geode::BRep& brep,
        const std::array< geode::index_t, 2 >& edge_unique_vertices )
    {
        geode::BRepComponentMeshEdges edges;
        model_component_mesh_edges< geode::BRep, 3 >(
            edges, brep, edge_unique_vertices );
        edges.block_edges = block_edges( brep, edge_unique_vertices );
        return edges;
    }
} // namespace

namespace geode
{
    std::array< index_t, 2 > edge_unique_vertices(
        const Section& section, const Line2D& line, index_t edge )
    {
        return ::edge_unique_vertices(
            section, line.component_id(), line.mesh().edge_vertices( edge ) );
    }

    std::array< index_t, 2 > edge_unique_vertices(
        const BRep& brep, const Line3D& line, index_t edge )
    {
        return ::edge_unique_vertices(
            brep, line.component_id(), line.mesh().edge_vertices( edge ) );
    }

    std::array< index_t, 2 > edge_unique_vertices( const Section& section,
        const Surface2D& surface,
        const PolygonEdge& edge )
    {
        return ::edge_unique_vertices( section, surface.component_id(),
            surface.mesh().polygon_edge_vertices( edge ) );
    }

    std::array< index_t, 2 > edge_unique_vertices(
        const BRep& brep, const Surface3D& surface, const PolygonEdge& edge )
    {
        return ::edge_unique_vertices( brep, surface.component_id(),
            surface.mesh().polygon_edge_vertices( edge ) );
    }

    std::array< index_t, 2 > edge_unique_vertices( const BRep& brep,
        const Block3D& block,
        const PolyhedronFacetEdge& edge )
    {
        return ::edge_unique_vertices( brep, block.component_id(),
            block.mesh().polyhedron_facet_edge_vertices( edge ) );
    }

    SectionComponentMeshEdges component_mesh_edges(
        const Section& section, const Line2D& line, index_t edge )
    {
        return section_component_mesh_edges(
            section, edge_unique_vertices( section, line, edge ) );
    }

    SectionComponentMeshEdges component_mesh_edges( const Section& section,
        const Surface2D& surface,
        const PolygonEdge& edge )
    {
        return section_component_mesh_edges(
            section, edge_unique_vertices( section, surface, edge ) );
    }

    BRepComponentMeshEdges component_mesh_edges(
        const BRep& brep, const Line3D& line, index_t edge )
    {
        return brep_component_mesh_edges(
            brep, edge_unique_vertices( brep, line, edge ) );
    }

    BRepComponentMeshEdges component_mesh_edges(
        const BRep& brep, const Surface3D& surface, const PolygonEdge& edge )
    {
        return brep_component_mesh_edges(
            brep, edge_unique_vertices( brep, surface, edge ) );
    }

    BRepComponentMeshEdges component_mesh_edges( const BRep& brep,
        const Block3D& block,
        const PolyhedronFacetEdge& edge )
    {
        return brep_component_mesh_edges(
            brep, edge_unique_vertices( brep, block, edge ) );
    }
} // namespace geode