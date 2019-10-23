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
    ALIAS_3D( Block );
    ALIAS_3D( Corner );
    ALIAS_3D( Line );
    ALIAS_3D( ModelBoundary );
    ALIAS_3D( Surface );

    class BRep;

    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modification of a BRep
     */
    class opengeode_model_api BRepBuilder : public TopologyBuilder,
                                            public AddComponentsBuilders< 3,
                                                Corners,
                                                Lines,
                                                Surfaces,
                                                Blocks,
                                                ModelBoundaries >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( BRepBuilder );

    public:
        struct ComponentMapping
        {
            using Mapping = std::unordered_map< uuid, uuid >;
            Mapping corners;
            Mapping lines;
            Mapping surfaces;
            Mapping blocks;
            Mapping model_boundaries;
        };

    public:
        BRepBuilder( BRep& brep );

        void copy( const BRep& brep );

        ComponentMapping copy_components( const BRep& brep );

        void copy_component_relationships( const ComponentMapping& mapping, const BRep& brep );

        void copy_component_geometry( const ComponentMapping& mapping, const BRep& brep );

        const uuid& add_corner();

        const uuid& add_corner( const MeshType& type );

        const uuid& add_line();

        const uuid& add_line( const MeshType& type );

        const uuid& add_surface();

        const uuid& add_surface( const MeshType& type );

        const uuid& add_block();

        const uuid& add_block( const MeshType& type );

        const uuid& add_model_boundary();

        void remove_corner( const Corner3D& corner );

        void remove_line( const Line3D& line );

        void remove_surface( const Surface3D& surface );

        void remove_block( const Block3D& block );

        void remove_model_boundary( const ModelBoundary3D& boundary );

        void add_corner_line_relationship(
            const Corner3D& corner, const Line3D& line );

        void add_line_surface_relationship(
            const Line3D& line, const Surface3D& surface );

        void add_surface_block_relationship(
            const Surface3D& surface, const Block3D& block );

        void add_line_surface_internal_relationship(
            const Line3D& line, const Surface3D& surface );

        void add_surface_block_internal_relationship(
            const Surface3D& surface, const Block3D& block );

        void add_surface_in_model_boundary(
            const Surface3D& surface, const ModelBoundary3D& boundary );

    private:
        BRep& brep_;
    };
} // namespace geode
