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

#include <geode/basic/identifier.h>

#include <geode/model/common.h>
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
    class SectionBuilder;
} // namespace geode

namespace geode
{
    /*!
     * A Section is a 2D model composed of
     * Corners, Lines, and Surfaces.
     * This class provides classes for range-based iteration on Component
     * boundaries and incidences.
     * @extends Corners
     * @extends Lines
     * @extends Surfaces
     * @extends ModelBoundaries
     */
    class opengeode_model_api Section : public Topology,
                                        public Corners2D,
                                        public Lines2D,
                                        public Surfaces2D,
                                        public ModelBoundaries2D,
                                        public Identifier
    {
    public:
        static constexpr index_t dim{ 2 };
        using Builder = SectionBuilder;
        using Components =
            std::tuple< Corner2D, Line2D, Surface2D, ModelBoundary2D >;

        class opengeode_model_api BoundaryCornerRange
            : public Relationships::BoundaryRangeIterator
        {
        public:
            BoundaryCornerRange( const Section& section, const Line2D& line );
            BoundaryCornerRange( const BoundaryCornerRange& range );
            ~BoundaryCornerRange();

            const BoundaryCornerRange& begin() const;

            const BoundaryCornerRange& end() const;

            const Corner2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api BoundaryLineRange
            : public Relationships::BoundaryRangeIterator
        {
        public:
            BoundaryLineRange(
                const Section& section, const Surface2D& surface );
            BoundaryLineRange( const BoundaryLineRange& range );
            ~BoundaryLineRange();

            const BoundaryLineRange& begin() const;

            const BoundaryLineRange& end() const;

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api IncidentLineRange
            : public Relationships::IncidenceRangeIterator
        {
        public:
            IncidentLineRange( const Section& section, const Corner2D& corner );
            IncidentLineRange( const IncidentLineRange& range );
            ~IncidentLineRange();

            const IncidentLineRange& begin() const;

            const IncidentLineRange& end() const;

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api IncidentSurfaceRange
            : public Relationships::IncidenceRangeIterator
        {
        public:
            IncidentSurfaceRange( const Section& section, const Line2D& line );
            IncidentSurfaceRange( const IncidentSurfaceRange& range );
            ~IncidentSurfaceRange();

            const IncidentSurfaceRange& begin() const;

            const IncidentSurfaceRange& end() const;

            const Surface2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api InternalLineRange
            : public Relationships::InternalRangeIterator
        {
        public:
            InternalLineRange(
                const Section& section, const Surface2D& surface );
            InternalLineRange( const InternalLineRange& range );
            ~InternalLineRange();

            const InternalLineRange& begin() const;

            const InternalLineRange& end() const;

            void operator++();

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api InternalCornerRange
            : public Relationships::InternalRangeIterator
        {
        public:
            InternalCornerRange(
                const Section& section, const Surface2D& surface );
            InternalCornerRange( const InternalCornerRange& range );
            ~InternalCornerRange();

            const InternalCornerRange& begin() const;

            const InternalCornerRange& end() const;

            void operator++();

            const Corner2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api EmbeddingSurfaceRange
            : public Relationships::EmbeddingRangeIterator
        {
        public:
            EmbeddingSurfaceRange( const Section& section, const Line2D& line );
            EmbeddingSurfaceRange(
                const Section& section, const Corner2D& corner );
            EmbeddingSurfaceRange( const EmbeddingSurfaceRange& range );
            ~EmbeddingSurfaceRange();

            const EmbeddingSurfaceRange& begin() const;

            const EmbeddingSurfaceRange& end() const;

            void operator++();

            const Surface2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api ItemLineRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemLineRange(
                const Section& section, const ModelBoundary2D& boundary );
            ItemLineRange( const ItemLineRange& range );
            ~ItemLineRange();

            const ItemLineRange& begin() const;

            const ItemLineRange& end() const;

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

    public:
        Section();
        Section( Section&& section );
        Section& operator=( Section&& section );
        ~Section();

        BoundaryCornerRange boundaries( const Line2D& line ) const;

        BoundaryLineRange boundaries( const Surface2D& surface ) const;

        IncidentLineRange incidences( const Corner2D& corner ) const;

        IncidentSurfaceRange incidences( const Line2D& line ) const;

        index_t nb_internal_corners( const Surface2D& surface ) const;

        InternalCornerRange internal_corners( const Surface2D& surface ) const;

        index_t nb_internal_lines( const Surface2D& surface ) const;

        InternalLineRange internal_lines( const Surface2D& surface ) const;

        index_t nb_embedding_surfaces( const Corner2D& corner ) const;

        EmbeddingSurfaceRange embedding_surfaces(
            const Corner2D& corner ) const;

        index_t nb_embedding_surfaces( const Line2D& line ) const;

        EmbeddingSurfaceRange embedding_surfaces( const Line2D& line ) const;

        ItemLineRange model_boundary_items(
            const ModelBoundary2D& boundary ) const;

        bool is_closed( const Line2D& line ) const;

        bool is_boundary( const Corner2D& corner, const Line2D& line ) const;

        bool is_boundary( const Line2D& line, const Surface2D& surface ) const;

        bool is_internal(
            const Corner2D& corner, const Surface2D& surface ) const;

        bool is_internal( const Line2D& line, const Surface2D& surface ) const;

        bool is_model_boundary_item(
            const Line2D& line, const ModelBoundary2D& boundary ) const;

        /*!
         * Compute the bounding box from component meshes
         */
        BoundingBox2D bounding_box() const;

        static absl::string_view native_extension_static()
        {
            static const auto extension = "og_sctn";
            return extension;
        }

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }
    };
} // namespace geode