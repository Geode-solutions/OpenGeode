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

#include <geode/basic/algorithm.hpp>
#include <geode/basic/identifier.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/blocks.hpp>
#include <geode/model/mixin/core/corner_collections.hpp>
#include <geode/model/mixin/core/corners.hpp>
#include <geode/model/mixin/core/line_collections.hpp>
#include <geode/model/mixin/core/lines.hpp>
#include <geode/model/mixin/core/model_boundaries.hpp>
#include <geode/model/mixin/core/surface_collections.hpp>
#include <geode/model/mixin/core/surfaces.hpp>
#include <geode/model/mixin/core/topology.hpp>

namespace geode
{
    ALIAS_2D( Corner );
    ALIAS_2D( Line );
    ALIAS_2D( Surface );
    ALIAS_2D( ModelBoundary );
    ALIAS_2D( CornerCollection );
    ALIAS_2D( LineCollection );
    ALIAS_2D( SurfaceCollection );
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
     * @extends CornerCollections
     * @extends LineCollections
     * @extends SurfaceCollections
     */
    class opengeode_model_api Section : public Topology,
                                        public Corners2D,
                                        public Lines2D,
                                        public Surfaces2D,
                                        public ModelBoundaries2D,
                                        public CornerCollections2D,
                                        public LineCollections2D,
                                        public SurfaceCollections2D,
                                        public Identifier
    {
    public:
        static constexpr index_t dim{ 2 };
        using Builder = SectionBuilder;
        using MeshComponents = std::tuple< Corners2D, Lines2D, Surfaces2D >;
        using CollectionComponents = std::tuple< ModelBoundaries2D,
            CornerCollections2D,
            LineCollections2D,
            SurfaceCollections2D >;
        using Components = tuple_cat< MeshComponents, CollectionComponents >;

        class opengeode_model_api BoundaryCornerRange
            : public Relationships::BoundaryRangeIterator
        {
        public:
            BoundaryCornerRange( const Section& section, const Line2D& line );
            BoundaryCornerRange( const BoundaryCornerRange& range );
            ~BoundaryCornerRange();

            [[nodiscard]] const BoundaryCornerRange& begin() const;

            [[nodiscard]] const BoundaryCornerRange& end() const;

            [[nodiscard]] const Corner2D& operator*() const;

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

            [[nodiscard]] const BoundaryLineRange& begin() const;

            [[nodiscard]] const BoundaryLineRange& end() const;

            [[nodiscard]] const Line2D& operator*() const;

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

            [[nodiscard]] const IncidentLineRange& begin() const;

            [[nodiscard]] const IncidentLineRange& end() const;

            [[nodiscard]] const Line2D& operator*() const;

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

            [[nodiscard]] const IncidentSurfaceRange& begin() const;

            [[nodiscard]] const IncidentSurfaceRange& end() const;

            [[nodiscard]] const Surface2D& operator*() const;

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

            [[nodiscard]] const InternalLineRange& begin() const;

            [[nodiscard]] const InternalLineRange& end() const;

            void operator++();

            [[nodiscard]] const Line2D& operator*() const;

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

            [[nodiscard]] const InternalCornerRange& begin() const;

            [[nodiscard]] const InternalCornerRange& end() const;

            void operator++();

            [[nodiscard]] const Corner2D& operator*() const;

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

            [[nodiscard]] const EmbeddingSurfaceRange& begin() const;

            [[nodiscard]] const EmbeddingSurfaceRange& end() const;

            void operator++();

            [[nodiscard]] const Surface2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api ItemCornerRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemCornerRange(
                const Section& section, const CornerCollection2D& boundary );
            ItemCornerRange( const ItemCornerRange& range );
            ~ItemCornerRange();

            [[nodiscard]] const ItemCornerRange& begin() const;

            [[nodiscard]] const ItemCornerRange& end() const;

            [[nodiscard]] const Corner2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api ItemLineRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemLineRange(
                const Section& section, const ModelBoundary2D& boundary );
            ItemLineRange(
                const Section& section, const LineCollection2D& boundary );
            ItemLineRange( const ItemLineRange& range );
            ~ItemLineRange();

            [[nodiscard]] const ItemLineRange& begin() const;

            [[nodiscard]] const ItemLineRange& end() const;

            [[nodiscard]] const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api ItemSurfaceRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemSurfaceRange(
                const Section& section, const SurfaceCollection2D& collection );
            ItemSurfaceRange( const ItemSurfaceRange& range );
            ~ItemSurfaceRange();

            [[nodiscard]] const ItemSurfaceRange& begin() const;

            [[nodiscard]] const ItemSurfaceRange& end() const;

            [[nodiscard]] const Surface2D& operator*() const;

        private:
            const Section& section_;
        };

    public:
        Section();
        Section( Section&& section ) noexcept;
        Section& operator=( Section&& section );
        ~Section();

        [[nodiscard]] Section clone() const;

        [[nodiscard]] BoundaryCornerRange boundaries(
            const Line2D& line ) const;

        [[nodiscard]] BoundaryLineRange boundaries(
            const Surface2D& surface ) const;

        [[nodiscard]] IncidentLineRange incidences(
            const Corner2D& corner ) const;

        [[nodiscard]] IncidentSurfaceRange incidences(
            const Line2D& line ) const;

        [[nodiscard]] index_t nb_internal_corners(
            const Surface2D& surface ) const;

        [[nodiscard]] InternalCornerRange internal_corners(
            const Surface2D& surface ) const;

        [[nodiscard]] index_t nb_internal_lines(
            const Surface2D& surface ) const;

        [[nodiscard]] InternalLineRange internal_lines(
            const Surface2D& surface ) const;

        [[nodiscard]] index_t nb_embedding_surfaces(
            const Corner2D& corner ) const;

        [[nodiscard]] EmbeddingSurfaceRange embedding_surfaces(
            const Corner2D& corner ) const;

        [[nodiscard]] index_t nb_embedding_surfaces( const Line2D& line ) const;

        [[nodiscard]] EmbeddingSurfaceRange embedding_surfaces(
            const Line2D& line ) const;

        [[nodiscard]] ItemLineRange model_boundary_items(
            const ModelBoundary2D& boundary ) const;

        [[nodiscard]] ItemCornerRange corner_collection_items(
            const CornerCollection2D& collection ) const;

        [[nodiscard]] ItemLineRange line_collection_items(
            const LineCollection2D& collection ) const;

        [[nodiscard]] ItemSurfaceRange surface_collection_items(
            const SurfaceCollection2D& collection ) const;

        [[nodiscard]] bool is_closed( const Line2D& line ) const;

        [[nodiscard]] bool is_boundary(
            const Corner2D& corner, const Line2D& line ) const;

        [[nodiscard]] bool is_boundary(
            const Line2D& line, const Surface2D& surface ) const;

        [[nodiscard]] bool is_internal(
            const Corner2D& corner, const Surface2D& surface ) const;

        [[nodiscard]] bool is_internal(
            const Line2D& line, const Surface2D& surface ) const;

        [[nodiscard]] bool is_model_boundary_item(
            const Line2D& line, const ModelBoundary2D& boundary ) const;

        [[nodiscard]] bool is_corner_collection_item(
            const Corner2D& surface, const CornerCollection2D& boundary ) const;

        [[nodiscard]] bool is_line_collection_item(
            const Line2D& surface, const LineCollection2D& boundary ) const;

        [[nodiscard]] bool is_surface_collection_item( const Surface2D& surface,
            const SurfaceCollection2D& boundary ) const;

        [[nodiscard]] BoundingBox2D bounding_box() const;

        [[nodiscard]] static std::string_view native_extension_static()
        {
            static const auto extension = "og_sctn";
            return extension;
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }
    };
} // namespace geode