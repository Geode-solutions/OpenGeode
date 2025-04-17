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

#pragma once

#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/mapping.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/builder/block_collections_builder.hpp>
#include <geode/model/mixin/builder/blocks_builder.hpp>
#include <geode/model/mixin/builder/corner_collections_builder.hpp>
#include <geode/model/mixin/builder/corners_builder.hpp>
#include <geode/model/mixin/builder/line_collections_builder.hpp>
#include <geode/model/mixin/builder/lines_builder.hpp>
#include <geode/model/mixin/builder/model_boundaries_builder.hpp>
#include <geode/model/mixin/builder/surface_collections_builder.hpp>
#include <geode/model/mixin/builder/surfaces_builder.hpp>
#include <geode/model/mixin/builder/topology_builder.hpp>
#include <geode/model/mixin/core/block_collections.hpp>
#include <geode/model/mixin/core/blocks.hpp>
#include <geode/model/mixin/core/corner_collections.hpp>
#include <geode/model/mixin/core/corners.hpp>
#include <geode/model/mixin/core/line_collections.hpp>
#include <geode/model/mixin/core/lines.hpp>
#include <geode/model/mixin/core/model_boundaries.hpp>
#include <geode/model/mixin/core/surface_collections.hpp>
#include <geode/model/mixin/core/surfaces.hpp>
#include <geode/model/mixin/core/topology.hpp>
#include <geode/model/representation/core/mapping.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    ALIAS_3D( Point );
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

        explicit BRepBuilder( BRep& brep );
        BRepBuilder( BRepBuilder&& other ) = default;

        ModelCopyMapping copy( const BRep& brep );

        void replace_components_meshes_by_others(
            BRep&& other, const ModelCopyMapping& mapping );

        ModelCopyMapping copy_components( const BRep& brep );

        void copy_components( ModelCopyMapping& mapping, const BRep& brep );

        void copy_component_geometry(
            const ModelCopyMapping& mapping, const BRep& brep );

        [[nodiscard]] const uuid& add_corner();

        [[nodiscard]] const uuid& add_corner( const MeshImpl& impl );

        [[nodiscard]] const uuid& add_line();

        [[nodiscard]] const uuid& add_line( const MeshImpl& impl );

        [[nodiscard]] const uuid& add_surface();

        [[nodiscard]] const uuid& add_surface( const MeshImpl& impl );

        [[nodiscard]] const uuid& add_block();

        [[nodiscard]] const uuid& add_block( const MeshImpl& impl );

        [[nodiscard]] const uuid& add_model_boundary();

        [[nodiscard]] const uuid& add_corner_collection();

        [[nodiscard]] const uuid& add_line_collection();

        [[nodiscard]] const uuid& add_surface_collection();

        [[nodiscard]] const uuid& add_block_collection();

        void add_corner( const uuid& corner_id );

        void add_corner( const uuid& corner_id, const MeshImpl& impl );

        void add_line( const uuid& line_id );

        void add_line( const uuid& line_id, const MeshImpl& impl );

        void add_surface( const uuid& surface_id );

        void add_surface( const uuid& surface_id, const MeshImpl& impl );

        void add_block( const uuid& block_id );

        void add_block( const uuid& block_id, const MeshImpl& impl );

        void add_model_boundary( const uuid& model_boundary_id );

        void add_corner_collection( const uuid& corner_collection_id );

        void add_line_collection( const uuid& line_collection_id );

        void add_surface_collection( const uuid& surface_collection_id );

        void add_block_collection( const uuid& block_collection_id );

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
            const Corner3D& corner, const CornerCollection3D& collection );

        void add_line_in_line_collection(
            const Line3D& line, const LineCollection3D& collection );

        void add_surface_in_surface_collection(
            const Surface3D& surface, const SurfaceCollection3D& collection );

        void add_block_in_block_collection(
            const Block3D& block, const BlockCollection3D& collection );

        void set_point( index_t unique_vertex, const Point3D& point );

    private:
        BRep& brep_;
    };
} // namespace geode
