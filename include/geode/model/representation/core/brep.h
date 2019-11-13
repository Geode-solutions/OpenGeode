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
#include <geode/model/mixin/core/add_components.h>
#include <geode/model/mixin/core/blocks.h>
#include <geode/model/mixin/core/corners.h>
#include <geode/model/mixin/core/lines.h>
#include <geode/model/mixin/core/model_boundaries.h>
#include <geode/model/mixin/core/surfaces.h>
#include <geode/model/mixin/core/topology.h>

namespace geode
{
    ALIAS_3D( Block );
    ALIAS_3D( ModelBoundary );
    ALIAS_3D( Corner );
    ALIAS_3D( Line );
    ALIAS_3D( Surface );
} // namespace geode

namespace geode
{
    /*!
     * A BRep (for Boundary Representation) is a 3D model composed of
     * Corners, Lines, Surfaces and Blocks.
     * This class provides classes for range-based iteration on Component
     * boundaries and incidences.
     */
    class opengeode_model_api BRep : public Topology,
                                     public AddComponents< 3,
                                         Corners,
                                         Lines,
                                         Surfaces,
                                         Blocks,
                                         ModelBoundaries >
    {
    public:
        class opengeode_model_api BoundaryCornerRange
            : public Relationships::BoundaryRangeIterator,
              public BeginEnd< BoundaryCornerRange >
        {
        public:
            BoundaryCornerRange( const BRep& brep, const Line3D& line );

            const Corner3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api BoundaryLineRange
            : public Relationships::BoundaryRangeIterator,
              public BeginEnd< BoundaryLineRange >
        {
        public:
            BoundaryLineRange( const BRep& brep, const Surface3D& surface );

            const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api BoundarySurfaceRange
            : public Relationships::BoundaryRangeIterator,
              public BeginEnd< BoundarySurfaceRange >
        {
        public:
            BoundarySurfaceRange( const BRep& brep, const Block3D& block );

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api IncidentLineRange
            : public Relationships::IncidenceRangeIterator,
              public BeginEnd< IncidentLineRange >
        {
        public:
            IncidentLineRange( const BRep& brep, const Corner3D& corner );

            const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api IncidentSurfaceRange
            : public Relationships::IncidenceRangeIterator,
              public BeginEnd< IncidentSurfaceRange >
        {
        public:
            IncidentSurfaceRange( const BRep& brep, const Line3D& line );

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api IncidentBlockRange
            : public Relationships::IncidenceRangeIterator,
              public BeginEnd< IncidentBlockRange >
        {
        public:
            IncidentBlockRange( const BRep& brep, const Surface3D& surface );

            const Block3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api InternalCornerRange
            : public Relationships::InternalRangeIterator,
              public BeginEnd< InternalCornerRange >
        {
        public:
            InternalCornerRange( const BRep& brep, const Surface3D& surface );

            InternalCornerRange( const BRep& brep, const Block3D& block );

            void operator++();

            const Corner3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api InternalLineRange
            : public Relationships::InternalRangeIterator,
              public BeginEnd< InternalLineRange >
        {
        public:
            InternalLineRange( const BRep& brep, const Surface3D& surface );

            InternalLineRange( const BRep& brep, const Block3D& block );

            void operator++();

            const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api InternalSurfaceRange
            : public Relationships::InternalRangeIterator,
              public BeginEnd< InternalSurfaceRange >
        {
        public:
            InternalSurfaceRange( const BRep& brep, const Block3D& block );

            void operator++();

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api EmbeddedSurfaceRange
            : public Relationships::EmbeddingRangeIterator,
              public BeginEnd< EmbeddedSurfaceRange >
        {
        public:
            EmbeddedSurfaceRange( const BRep& brep, const Corner3D& corner );

            EmbeddedSurfaceRange( const BRep& brep, const Line3D& line );

            void operator++();

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api EmbeddedBlockRange
            : public Relationships::EmbeddingRangeIterator,
              public BeginEnd< EmbeddedBlockRange >
        {
        public:
            EmbeddedBlockRange( const BRep& brep, const Corner3D& corner );

            EmbeddedBlockRange( const BRep& brep, const Line3D& line );

            EmbeddedBlockRange( const BRep& brep, const Surface3D& surface );

            void operator++();

            const Block3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api ItemSurfaceRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< ItemSurfaceRange >
        {
        public:
            ItemSurfaceRange(
                const BRep& brep, const ModelBoundary3D& boundary );

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

    public:
        BoundaryCornerRange boundaries( const Line3D& line ) const;

        BoundaryLineRange boundaries( const Surface3D& surface ) const;

        BoundarySurfaceRange boundaries( const Block3D& block ) const;

        IncidentLineRange incidences( const Corner3D& corner ) const;

        IncidentSurfaceRange incidences( const Line3D& line ) const;

        IncidentBlockRange incidences( const Surface3D& surface ) const;

        index_t nb_internal_corners( const Surface3D& surface ) const;

        InternalCornerRange internal_corners( const Surface3D& surface ) const;

        index_t nb_internal_lines( const Surface3D& surface ) const;

        InternalLineRange internal_lines( const Surface3D& surface ) const;

        index_t nb_internal_corners( const Block3D& block ) const;

        InternalCornerRange internal_corners( const Block3D& block ) const;

        index_t nb_internal_lines( const Block3D& block ) const;

        InternalLineRange internal_lines( const Block3D& block ) const;

        index_t nb_internal_surfaces( const Block3D& block ) const;

        InternalSurfaceRange internal_surfaces( const Block3D& block ) const;

        index_t nb_embedded_surfaces( const Corner3D& corner ) const;

        EmbeddedSurfaceRange embedded_surfaces( const Corner3D& corner ) const;

        index_t nb_embedded_surfaces( const Line3D& line ) const;

        EmbeddedSurfaceRange embedded_surfaces( const Line3D& line ) const;

        index_t nb_embedded_blocks( const Corner3D& corner ) const;

        EmbeddedBlockRange embedded_blocks( const Corner3D& corner ) const;

        index_t nb_embedded_blocks( const Line3D& line ) const;

        EmbeddedBlockRange embedded_blocks( const Line3D& line ) const;

        index_t nb_embedded_blocks( const Surface3D& surface ) const;

        EmbeddedBlockRange embedded_blocks( const Surface3D& surface ) const;

        ItemSurfaceRange items( const ModelBoundary3D& boundary ) const;

        bool is_closed( const Line3D& line ) const;

        bool is_closed( const Surface3D& surface ) const;

        static std::string native_extension_static()
        {
            return "og_brep";
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }
    };
} // namespace geode