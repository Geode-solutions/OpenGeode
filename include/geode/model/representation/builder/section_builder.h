/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/model/common.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polygonal_surface.h>

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

#include <geode/mesh/core/mesh_type.h>

namespace geode
{
    ALIAS_2D( Corner );
    ALIAS_2D( Line );
    ALIAS_2D( ModelBoundary );
    ALIAS_2D( Surface );

    class Section;

    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modification of a Section
     */
    class opengeode_model_api SectionBuilder : public TopologyBuilder,
                                               public AddComponentsBuilders< 2,
                                                   Corners,
                                                   Lines,
                                                   Surfaces,
                                                   ModelBoundaries >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( SectionBuilder );

    public:
        struct ComponentMapping
        {
            using Mapping = std::unordered_map< uuid, uuid >;
            Mapping corners;
            Mapping lines;
            Mapping surfaces;
            Mapping model_boundaries;
        };

    public:
        SectionBuilder( Section& section );

        void copy( const Section& section );

        ComponentMapping copy_components( const Section& section );

        void copy_component_relationships(
            const ComponentMapping& mapping, const Section& section );

        void copy_component_geometry(
            const ComponentMapping& mapping, const Section& section );

        const uuid& add_corner();

        const uuid& add_corner( const MeshType& type );

        const uuid& add_line();

        const uuid& add_line( const MeshType& type );

        const uuid& add_surface();

        const uuid& add_surface( const MeshType& type );

        const uuid& add_model_boundary();

        void update_corner_mesh(
            const Corner2D& corner, std::unique_ptr< PointSet2D > mesh );

        void update_line_mesh(
            const Line2D& line, std::unique_ptr< EdgedCurve2D > mesh );

        void update_surface_mesh( const Surface2D& surface,
            std::unique_ptr< PolygonalSurface2D > mesh );

        void remove_corner( const Corner2D& corner );

        void remove_line( const Line2D& line );

        void remove_surface( const Surface2D& surface );

        void remove_model_boundary( const ModelBoundary2D& boundary );

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

    private:
        Section& section_;
    };
} // namespace geode
