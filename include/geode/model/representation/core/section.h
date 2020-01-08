/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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
    ALIAS_2D( Corner );
    ALIAS_2D( Line );
    ALIAS_2D( Surface );
    ALIAS_2D( ModelBoundary );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    ALIAS_2D( BoundingBox );
} // namespace geode

namespace geode
{
    /*!
     * A Section is a 2D model composed of
     * Corners, Lines, and Surfaces.
     * This class provides classes for range-based iteration on Component
     * boundaries and incidences.
     */
    class opengeode_model_api Section
        : public Topology,
          public AddComponents< 2, Corners, Lines, Surfaces, ModelBoundaries >
    {
    public:
        class opengeode_model_api BoundaryCornerRange
            : public Relationships::BoundaryRangeIterator,
              public BeginEnd< BoundaryCornerRange >
        {
        public:
            BoundaryCornerRange( const Section& section, const Line2D& line );

            const Corner2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api BoundaryLineRange
            : public Relationships::BoundaryRangeIterator,
              public BeginEnd< BoundaryLineRange >
        {
        public:
            BoundaryLineRange(
                const Section& section, const Surface2D& surface );

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api IncidentLineRange
            : public Relationships::IncidenceRangeIterator,
              public BeginEnd< IncidentLineRange >
        {
        public:
            IncidentLineRange( const Section& section, const Corner2D& corner );

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api IncidentSurfaceRange
            : public Relationships::IncidenceRangeIterator,
              public BeginEnd< IncidentSurfaceRange >
        {
        public:
            IncidentSurfaceRange( const Section& section, const Line2D& line );

            const Surface2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api InternalLineRange
            : public Relationships::InternalRangeIterator,
              public BeginEnd< InternalLineRange >
        {
        public:
            InternalLineRange(
                const Section& section, const Surface2D& surface );

            void operator++();

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api InternalCornerRange
            : public Relationships::InternalRangeIterator,
              public BeginEnd< InternalCornerRange >
        {
        public:
            InternalCornerRange(
                const Section& section, const Surface2D& surface );

            void operator++();

            const Corner2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api EmbeddedSurfaceRange
            : public Relationships::EmbeddingRangeIterator,
              public BeginEnd< EmbeddedSurfaceRange >
        {
        public:
            EmbeddedSurfaceRange( const Section& section, const Line2D& line );

            EmbeddedSurfaceRange(
                const Section& section, const Corner2D& corner );

            void operator++();

            const Surface2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api ItemLineRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< ItemLineRange >
        {
        public:
            ItemLineRange(
                const Section& section, const ModelBoundary2D& boundary );

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

    public:
        BoundaryCornerRange boundaries( const Line2D& line ) const;

        BoundaryLineRange boundaries( const Surface2D& surface ) const;

        IncidentLineRange incidences( const Corner2D& corner ) const;

        IncidentSurfaceRange incidences( const Line2D& line ) const;

        index_t nb_internal_corners( const Surface2D& surface ) const;

        InternalCornerRange internal_corners( const Surface2D& surface ) const;

        index_t nb_internal_lines( const Surface2D& surface ) const;

        InternalLineRange internal_lines( const Surface2D& surface ) const;

        index_t nb_embedded_surfaces( const Corner2D& corner ) const;

        EmbeddedSurfaceRange embeddings( const Corner2D& corner ) const;

        index_t nb_embedded_surfaces( const Line2D& line ) const;

        EmbeddedSurfaceRange embeddings( const Line2D& line ) const;

        ItemLineRange items( const ModelBoundary2D& boundary ) const;

        bool is_closed( const Line2D& line ) const;

        /*!
         * Compute the bounding box from component meshes
         */
        BoundingBox2D bounding_box() const;

        static std::string native_extension_static()
        {
            return "og_sctn";
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }
    };
} // namespace geode