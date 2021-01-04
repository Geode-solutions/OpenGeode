/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/basic/mapping.h>

#include <geode/model/common.h>
#include <geode/model/mixin/builder/add_components_builders.h>
#include <geode/model/mixin/builder/blocks_builder.h>
#include <geode/model/mixin/builder/corners_builder.h>
#include <geode/model/mixin/builder/lines_builder.h>
#include <geode/model/mixin/builder/model_boundaries_builder.h>
#include <geode/model/mixin/builder/surfaces_builder.h>
#include <geode/model/mixin/builder/topology_builder.h>
#include <geode/model/mixin/core/blocks.h>
#include <geode/model/mixin/core/corners.h>
#include <geode/model/mixin/core/lines.h>
#include <geode/model/mixin/core/model_boundaries.h>
#include <geode/model/mixin/core/surfaces.h>
#include <geode/model/mixin/core/topology.h>

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
    ALIAS_3D( Surface );
    ALIAS_3D( SurfaceMesh );
    class BRep;
    struct uuid;

    namespace detail
    {
        class ModelCopyMapping;
    } // namespace detail
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
     */
    class opengeode_model_api BRepBuilder : public TopologyBuilder,
                                            public AddComponentsBuilders< 3,
                                                Corners,
                                                Lines,
                                                Surfaces,
                                                Blocks,
                                                ModelBoundaries >
    {
        OPENGEODE_DISABLE_COPY( BRepBuilder );

    public:
        BRepBuilder( BRep& brep );
        BRepBuilder( BRepBuilder&& ) = default;

        detail::ModelCopyMapping copy( const BRep& brep );

        detail::ModelCopyMapping copy_components( const BRep& brep );

        void copy_component_relationships(
            const detail::ModelCopyMapping& mapping, const BRep& brep );

        void copy_component_geometry(
            const detail::ModelCopyMapping& mapping, const BRep& brep );

        const uuid& add_corner();

        const uuid& add_corner( const MeshImpl& impl );

        const uuid& add_line();

        const uuid& add_line( const MeshImpl& impl );

        const uuid& add_surface();

        const uuid& add_surface( const MeshImpl& impl );

        const uuid& add_block();

        const uuid& add_block( const MeshImpl& impl );

        const uuid& add_model_boundary();

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

    private:
        BRep& brep_;
    };
} // namespace geode
