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
#include <geode/model/mixin/builder/corner_collections_builder.hpp>
#include <geode/model/mixin/builder/corners_builder.hpp>
#include <geode/model/mixin/builder/line_collections_builder.hpp>
#include <geode/model/mixin/builder/lines_builder.hpp>
#include <geode/model/mixin/builder/model_boundaries_builder.hpp>
#include <geode/model/mixin/builder/surface_collections_builder.hpp>
#include <geode/model/mixin/builder/surfaces_builder.hpp>
#include <geode/model/mixin/builder/topology_builder.hpp>
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
    ALIAS_2D( Point );
    ALIAS_2D( Corner );
    ALIAS_2D( EdgedCurve );
    ALIAS_2D( PointSet );
    ALIAS_2D( Line );
    ALIAS_2D( Surface );
    ALIAS_2D( SurfaceMesh );
    ALIAS_2D( ModelBoundary );
    ALIAS_2D( CornerCollection );
    ALIAS_2D( LineCollection );
    ALIAS_2D( SurfaceCollection );
    class Section;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modification of a Section
     * @extends CornersBuilder
     * @extends LinesBuilder
     * @extends SurfacesBuilder
     * @extends ModelBoundariesBuilder
     * @extends CornerCollectionsBuilder2D
     * @extends LineCollectionsBuilder2D
     * @extends SurfaceCollectionsBuilder2D
     */
    class opengeode_model_api SectionBuilder
        : public TopologyBuilder,
          public CornersBuilder2D,
          public LinesBuilder2D,
          public SurfacesBuilder2D,
          public ModelBoundariesBuilder2D,
          public CornerCollectionsBuilder2D,
          public LineCollectionsBuilder2D,
          public SurfaceCollectionsBuilder2D,
          public IdentifierBuilder
    {
        OPENGEODE_DISABLE_COPY( SectionBuilder );

    public:
        static constexpr index_t dim{ 2 };

        explicit SectionBuilder( Section& section );
        SectionBuilder( SectionBuilder&& other ) = default;

        ModelCopyMapping copy( const Section& section );

        ModelCopyMapping copy_components( const Section& section );

        void copy_components(
            ModelCopyMapping& mapping, const Section& section );

        void copy_component_geometry(
            const ModelCopyMapping& mapping, const Section& section );

        [[nodiscard]] const uuid& add_corner();

        [[nodiscard]] const uuid& add_corner( const MeshImpl& impl );

        [[nodiscard]] const uuid& add_line();

        [[nodiscard]] const uuid& add_line( const MeshImpl& impl );

        [[nodiscard]] const uuid& add_surface();

        [[nodiscard]] const uuid& add_surface( const MeshImpl& impl );

        [[nodiscard]] const uuid& add_model_boundary();

        [[nodiscard]] const uuid& add_corner_collection();

        [[nodiscard]] const uuid& add_line_collection();

        [[nodiscard]] const uuid& add_surface_collection();

        void add_corner( uuid corner_id );

        void add_corner( uuid corner_id, const MeshImpl& impl );

        void add_line( uuid line_id );

        void add_line( uuid line_id, const MeshImpl& impl );

        void add_surface( uuid surface_id );

        void add_surface( uuid surface_id, const MeshImpl& impl );

        void add_model_boundary( uuid model_boundary_id );

        void add_corner_collection( uuid corner_collection_id );

        void add_line_collection( uuid line_collection_id );

        void add_surface_collection( uuid surface_collection_id );

        void update_corner_mesh(
            const Corner2D& corner, std::unique_ptr< PointSet2D > mesh );

        void update_line_mesh(
            const Line2D& line, std::unique_ptr< EdgedCurve2D > mesh );

        void update_surface_mesh(
            const Surface2D& surface, std::unique_ptr< SurfaceMesh2D > mesh );

        void remove_corner( const Corner2D& corner );

        void remove_line( const Line2D& line );

        void remove_surface( const Surface2D& surface );

        void remove_model_boundary( const ModelBoundary2D& boundary );

        void remove_corner_collection( const CornerCollection2D& collection );

        void remove_line_collection( const LineCollection2D& collection );

        void remove_surface_collection( const SurfaceCollection2D& collection );

        void add_corner_line_boundary_relationship(
            const Corner2D& corner, const Line2D& line );

        void add_line_surface_boundary_relationship(
            const Line2D& line, const Surface2D& surface );

        void add_corner_surface_internal_relationship(
            const Corner2D& corner, const Surface2D& surface );

        void add_line_surface_internal_relationship(
            const Line2D& line, const Surface2D& surface );

        void add_line_in_model_boundary(
            const Line2D& line, const ModelBoundary2D& boundary );

        void add_corner_in_corner_collection(
            const Corner2D& surface, const CornerCollection2D& collection );

        void add_line_in_line_collection(
            const Line2D& surface, const LineCollection2D& collection );

        void add_surface_in_surface_collection(
            const Surface2D& surface, const SurfaceCollection2D& collection );

        void set_point( index_t unique_vertex, const Point2D& point );

    private:
        Section& section_;
    };
} // namespace geode
