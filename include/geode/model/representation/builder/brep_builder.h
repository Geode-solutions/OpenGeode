/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/identifier_builder.h>
#include <geode/basic/mapping.h>

#include <geode/model/common.h>
#include <geode/model/mixin/builder/block_collections_builder.h>
#include <geode/model/mixin/builder/blocks_builder.h>
#include <geode/model/mixin/builder/corner_collections_builder.h>
#include <geode/model/mixin/builder/corners_builder.h>
#include <geode/model/mixin/builder/line_collections_builder.h>
#include <geode/model/mixin/builder/lines_builder.h>
#include <geode/model/mixin/builder/model_boundaries_builder.h>
#include <geode/model/mixin/builder/surface_collections_builder.h>
#include <geode/model/mixin/builder/surfaces_builder.h>
#include <geode/model/mixin/builder/topology_builder.h>
#include <geode/model/mixin/core/block_collections.h>
#include <geode/model/mixin/core/blocks.h>
#include <geode/model/mixin/core/corner_collections.h>
#include <geode/model/mixin/core/corners.h>
#include <geode/model/mixin/core/line_collections.h>
#include <geode/model/mixin/core/lines.h>
#include <geode/model/mixin/core/model_boundaries.h>
#include <geode/model/mixin/core/surface_collections.h>
#include <geode/model/mixin/core/surfaces.h>
#include <geode/model/mixin/core/topology.h>
#include <geode/model/representation/core/mapping.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    ALIAS_3D( Block );
    ALIAS_3D( Corner );
    ALIAS_3D( EdgedCurve );
    ALIAS_3D( PointSet );
    ALIAS_3D( SolidMesh );
    ALIAS_3D( Line );
    ALIAS_3D( ModelBoundary );
    ALIAS_3D( CornerCollection );
    ALIAS_3D( LineCollection );
    ALIAS_3D( SurfaceCollection );
    ALIAS_3D( BlockCollection );
    ALIAS_3D( Surface );
    ALIAS_3D( SurfaceMesh );
    class BRep;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modification of a BRep
     * @extends CornersBuilder
     * @extends LinesBuilder
     * @extends SurfacesBuilder
     * @extends BlocksBuilder
     * @extends ModelBoundariesBuilder
     * @extends CornerCollectionsBuilder3D
     * @extends LineCollectionsBuilder3D
     * @extends SurfaceCollectionsBuilder3D
     * @extends BlockCollectionsBuilder3D
     */
    class opengeode_model_api BRepBuilder : public TopologyBuilder,
                                            public CornersBuilder3D,
                                            public LinesBuilder3D,
                                            public SurfacesBuilder3D,
                                            public BlocksBuilder3D,
                                            public ModelBoundariesBuilder3D,
                                            public CornerCollectionsBuilder3D,
                                            public LineCollectionsBuilder3D,
                                            public SurfaceCollectionsBuilder3D,
                                            public BlockCollectionsBuilder3D,
                                            public IdentifierBuilder
    {
        OPENGEODE_DISABLE_COPY( BRepBuilder );

    public:
        static constexpr index_t dim{ 3 };

        BRepBuilder( BRep& brep );
        BRepBuilder( BRepBuilder&& other ) = default;

        ModelCopyMapping copy( const BRep& brep );

        ModelCopyMapping copy_components( const BRep& brep );

        void copy_components( ModelCopyMapping& mapping, const BRep& brep );

        void copy_component_geometry(
            const ModelCopyMapping& mapping, const BRep& brep );

        const uuid& add_corner();

        const uuid& add_corner( const MeshImpl& impl );

        const uuid& add_line();

        const uuid& add_line( const MeshImpl& impl );

        const uuid& add_surface();

        const uuid& add_surface( const MeshImpl& impl );

        const uuid& add_block();

        const uuid& add_block( const MeshImpl& impl );

        const uuid& add_model_boundary();

        const uuid& add_corner_collection();

        const uuid& add_line_collection();

        const uuid& add_surface_collection();

        const uuid& add_block_collection();

        void add_corner( uuid corner_id );

        void add_corner( uuid corner_id, const MeshImpl& impl );

        void add_line( uuid line_id );

        void add_line( uuid line_id, const MeshImpl& impl );

        void add_surface( uuid surface_id );

        void add_surface( uuid surface_id, const MeshImpl& impl );

        void add_block( uuid block_id );

        void add_block( uuid block_id, const MeshImpl& impl );

        void add_model_boundary( uuid model_boundary_id );

        void add_corner_collection( uuid corner_collection_id );

        void add_line_collection( uuid line_collection_id );

        void add_surface_collection( uuid surface_collection_id );

        void add_block_collection( uuid block_collection_id );

        void update_corner_mesh(
            const Corner3D& corner, std::unique_ptr< PointSet3D > mesh );

        void update_line_mesh(
            const Line3D& line, std::unique_ptr< EdgedCurve3D > mesh );

        void update_surface_mesh(
            const Surface3D& surface, std::unique_ptr< SurfaceMesh3D > mesh );

        void update_block_mesh(
            const Block3D& block, std::unique_ptr< SolidMesh3D > mesh );

        void remove_corner( const Corner3D& corner );

        void remove_line( const Line3D& line );

        void remove_surface( const Surface3D& surface );

        void remove_block( const Block3D& block );

        void remove_model_boundary( const ModelBoundary3D& boundary );

        void remove_corner_collection( const CornerCollection3D& collection );

        void remove_line_collection( const LineCollection3D& collection );

        void remove_surface_collection( const SurfaceCollection3D& collection );

        void remove_block_collection( const BlockCollection3D& collection );

        void add_corner_line_boundary_relationship(
            const Corner3D& corner, const Line3D& line );

        void add_line_surface_boundary_relationship(
            const Line3D& line, const Surface3D& surface );

        void add_surface_block_boundary_relationship(
            const Surface3D& surface, const Block3D& block );

        void add_corner_surface_internal_relationship(
            const Corner3D& corner, const Surface3D& surface );

        void add_line_surface_internal_relationship(
            const Line3D& line, const Surface3D& surface );

        void add_corner_block_internal_relationship(
            const Corner3D& corner, const Block3D& block );

        void add_line_block_internal_relationship(
            const Line3D& line, const Block3D& block );

        void add_surface_block_internal_relationship(
            const Surface3D& surface, const Block3D& block );

        void add_surface_in_model_boundary(
            const Surface3D& surface, const ModelBoundary3D& boundary );

        void add_corner_in_corner_collection(
            const Corner3D& surface, const CornerCollection3D& collection );

        void add_line_in_line_collection(
            const Line3D& surface, const LineCollection3D& collection );

        void add_surface_in_surface_collection(
            const Surface3D& surface, const SurfaceCollection3D& collection );

        void add_block_in_block_collection(
            const Block3D& surface, const BlockCollection3D& collection );

    private:
        BRep& brep_;
    };
} // namespace geode
