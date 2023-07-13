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

#pragma once

#include <absl/container/flat_hash_map.h>

#include <geode/basic/uuid.h>

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
    std::array< index_t, 2 > opengeode_model_api edge_unique_vertices(
        const Section& section, const Line2D& line, index_t edge );

    std::array< index_t, 2 > opengeode_model_api edge_unique_vertices(
        const Section& section,
        const Surface2D& surface,
        const PolygonEdge& edge );

    std::array< index_t, 2 > opengeode_model_api edge_unique_vertices(
        const BRep& brep, const Line3D& line, index_t edge );

    std::array< index_t, 2 > opengeode_model_api edge_unique_vertices(
        const BRep& brep, const Surface3D& surface, const PolygonEdge& edge );

    std::array< index_t, 2 > opengeode_model_api edge_unique_vertices(
        const BRep& brep,
        const Block3D& block,
        const PolyhedronFacetEdge& edge );

    struct ModelComponentMeshEdges
    {
        using LineEdges = absl::flat_hash_map< uuid, std::vector< index_t > >;
        using SurfaceEdges =
            absl::flat_hash_map< uuid, std::vector< PolygonEdge > >;
        std::array< index_t, 2 > edge_unique_vertices;
        LineEdges line_edges;
        SurfaceEdges surface_edges;
    };

    struct SectionComponentMeshEdges : public ModelComponentMeshEdges
    {
    };

    SectionComponentMeshEdges opengeode_model_api component_mesh_edges(
        const Section& section,
        const std::array< index_t, 2 >& edge_unique_vertices );

    SectionComponentMeshEdges opengeode_model_api component_mesh_edges(
        const Section& section, const Line2D& line, index_t edge );

    SectionComponentMeshEdges opengeode_model_api component_mesh_edges(
        const Section& section,
        const Surface2D& surface,
        const PolygonEdge& edge );

    struct BRepComponentMeshEdges : public ModelComponentMeshEdges
    {
        using BlockEdges =
            absl::flat_hash_map< uuid, std::vector< PolyhedronFacetEdge > >;

        BlockEdges block_edges;
    };

    BRepComponentMeshEdges opengeode_model_api component_mesh_edges(
        const BRep& brep,
        const std::array< index_t, 2 >& edge_unique_vertices );

    BRepComponentMeshEdges opengeode_model_api component_mesh_edges(
        const BRep& brep, const Line3D& line, index_t edge );

    BRepComponentMeshEdges opengeode_model_api component_mesh_edges(
        const BRep& brep, const Surface3D& surface, const PolygonEdge& edge );

    BRepComponentMeshEdges opengeode_model_api component_mesh_edges(
        const BRep& brep,
        const Block3D& block,
        const PolyhedronFacetEdge& edge );

    namespace detail
    {
        template < typename Model >
        ModelComponentMeshEdges::LineEdges line_component_mesh_edges(
            const Model& model,
            const std::array< index_t, 2 >& edge_unique_vertices );

        template < typename Model >
        ModelComponentMeshEdges::SurfaceEdges surface_component_mesh_edges(
            const Model& model,
            const std::array< index_t, 2 >& edge_unique_vertices );

        BRepComponentMeshEdges::BlockEdges opengeode_model_api
            block_component_mesh_edges( const BRep& brep,
                const std::array< index_t, 2 >& edge_unique_vertices );
    } // namespace detail
} // namespace geode