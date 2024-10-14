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

#include <geode/basic/identifier.hpp>

#include <geode/model/common.hpp>
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

namespace geode
{
    ALIAS_3D( Block );
    ALIAS_3D( ModelBoundary );
    ALIAS_3D( CornerCollection );
    ALIAS_3D( LineCollection );
    ALIAS_3D( SurfaceCollection );
    ALIAS_3D( BlockCollection );
    ALIAS_3D( Corner );
    ALIAS_3D( Line );
    ALIAS_3D( Surface );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    ALIAS_3D( BoundingBox );
    class BRepBuilder;
} // namespace geode

namespace geode
{
    /*!
     * A BRep (for Boundary Representation) is a 3D model composed of
     * Corners, Lines, Surfaces and Blocks.
     * This class provides classes for range-based iteration on Component
     * boundaries and incidences.
     * @extends Corners
     * @extends Lines
     * @extends Surfaces
     * @extends Blocks
     * @extends ModelBoundaries
     * @extends CornerCollections
     * @extends LineCollections
     * @extends SurfaceCollections
     * @extends BlockCollections
     */
    class opengeode_model_api BRep : public Topology,
                                     public Corners3D,
                                     public Lines3D,
                                     public Surfaces3D,
                                     public Blocks3D,
                                     public ModelBoundaries3D,
                                     public CornerCollections3D,
                                     public LineCollections3D,
                                     public SurfaceCollections3D,
                                     public BlockCollections3D,
                                     public Identifier
    {
    public:
        static constexpr index_t dim{ 3 };
        using Builder = BRepBuilder;
        using Components = std::tuple< Corner3D,
            Line3D,
            Surface3D,
            Block3D,
            ModelBoundary3D,
            CornerCollection3D,
            LineCollection3D,
            SurfaceCollection3D,
            BlockCollection3D >;

        class opengeode_model_api BoundaryCornerRange
            : public Relationships::BoundaryRangeIterator
        {
        public:
            BoundaryCornerRange( const BRep& brep, const Line3D& line );
            BoundaryCornerRange( const BoundaryCornerRange& range );
            ~BoundaryCornerRange();

            [[nodiscard]] const BoundaryCornerRange& begin() const;

            [[nodiscard]] const BoundaryCornerRange& end() const;

            [[nodiscard]] const Corner3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api BoundaryLineRange
            : public Relationships::BoundaryRangeIterator
        {
        public:
            BoundaryLineRange( const BRep& brep, const Surface3D& surface );
            BoundaryLineRange( const BoundaryLineRange& range );
            ~BoundaryLineRange();

            [[nodiscard]] const BoundaryLineRange& begin() const;

            [[nodiscard]] const BoundaryLineRange& end() const;

            [[nodiscard]] const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api BoundarySurfaceRange
            : public Relationships::BoundaryRangeIterator
        {
        public:
            BoundarySurfaceRange( const BRep& brep, const Block3D& block );
            BoundarySurfaceRange( const BoundarySurfaceRange& range );
            ~BoundarySurfaceRange();

            [[nodiscard]] const BoundarySurfaceRange& begin() const;

            [[nodiscard]] const BoundarySurfaceRange& end() const;

            [[nodiscard]] const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api IncidentLineRange
            : public Relationships::IncidenceRangeIterator
        {
        public:
            IncidentLineRange( const BRep& brep, const Corner3D& corner );
            IncidentLineRange( const IncidentLineRange& range );
            ~IncidentLineRange();

            [[nodiscard]] const IncidentLineRange& begin() const;

            [[nodiscard]] const IncidentLineRange& end() const;

            [[nodiscard]] const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api IncidentSurfaceRange
            : public Relationships::IncidenceRangeIterator
        {
        public:
            IncidentSurfaceRange( const BRep& brep, const Line3D& line );
            IncidentSurfaceRange( const IncidentSurfaceRange& range );
            ~IncidentSurfaceRange();

            [[nodiscard]] const IncidentSurfaceRange& begin() const;

            [[nodiscard]] const IncidentSurfaceRange& end() const;

            [[nodiscard]] const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api IncidentBlockRange
            : public Relationships::IncidenceRangeIterator
        {
        public:
            IncidentBlockRange( const BRep& brep, const Surface3D& surface );
            IncidentBlockRange( const IncidentBlockRange& range );
            ~IncidentBlockRange();

            [[nodiscard]] const IncidentBlockRange& begin() const;

            [[nodiscard]] const IncidentBlockRange& end() const;

            [[nodiscard]] const Block3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api InternalCornerRange
            : public Relationships::InternalRangeIterator
        {
        public:
            InternalCornerRange( const BRep& brep, const Surface3D& surface );
            InternalCornerRange( const BRep& brep, const Block3D& block );
            InternalCornerRange( const InternalCornerRange& range );
            ~InternalCornerRange();

            [[nodiscard]] const InternalCornerRange& begin() const;

            [[nodiscard]] const InternalCornerRange& end() const;

            void operator++();

            [[nodiscard]] const Corner3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api InternalLineRange
            : public Relationships::InternalRangeIterator
        {
        public:
            InternalLineRange( const BRep& brep, const Surface3D& surface );
            InternalLineRange( const BRep& brep, const Block3D& block );
            InternalLineRange( const InternalLineRange& range );
            ~InternalLineRange();

            [[nodiscard]] const InternalLineRange& begin() const;

            [[nodiscard]] const InternalLineRange& end() const;

            void operator++();

            [[nodiscard]] const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api InternalSurfaceRange
            : public Relationships::InternalRangeIterator
        {
        public:
            InternalSurfaceRange( const BRep& brep, const Block3D& block );
            InternalSurfaceRange( const InternalSurfaceRange& range );
            ~InternalSurfaceRange();

            [[nodiscard]] const InternalSurfaceRange& begin() const;

            [[nodiscard]] const InternalSurfaceRange& end() const;

            void operator++();

            [[nodiscard]] const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api EmbeddingSurfaceRange
            : public Relationships::EmbeddingRangeIterator
        {
        public:
            EmbeddingSurfaceRange( const BRep& brep, const Corner3D& corner );
            EmbeddingSurfaceRange( const BRep& brep, const Line3D& line );
            EmbeddingSurfaceRange( const EmbeddingSurfaceRange& range );
            ~EmbeddingSurfaceRange();

            [[nodiscard]] const EmbeddingSurfaceRange& begin() const;

            [[nodiscard]] const EmbeddingSurfaceRange& end() const;

            void operator++();

            [[nodiscard]] const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api EmbeddingBlockRange
            : public Relationships::EmbeddingRangeIterator
        {
        public:
            EmbeddingBlockRange( const BRep& brep, const Corner3D& corner );
            EmbeddingBlockRange( const BRep& brep, const Line3D& line );
            EmbeddingBlockRange( const BRep& brep, const Surface3D& surface );
            EmbeddingBlockRange( const EmbeddingBlockRange& range );
            ~EmbeddingBlockRange();

            [[nodiscard]] const EmbeddingBlockRange& begin() const;

            [[nodiscard]] const EmbeddingBlockRange& end() const;

            void operator++();

            [[nodiscard]] const Block3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api ItemCornerRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemCornerRange(
                const BRep& brep, const CornerCollection3D& boundary );
            ItemCornerRange( const ItemCornerRange& range );
            ~ItemCornerRange();

            [[nodiscard]] const ItemCornerRange& begin() const;

            [[nodiscard]] const ItemCornerRange& end() const;

            [[nodiscard]] const Corner3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api ItemLineRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemLineRange( const BRep& brep, const LineCollection3D& boundary );
            ItemLineRange( const ItemLineRange& range );
            ~ItemLineRange();

            [[nodiscard]] const ItemLineRange& begin() const;

            [[nodiscard]] const ItemLineRange& end() const;

            [[nodiscard]] const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api ItemSurfaceRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemSurfaceRange(
                const BRep& brep, const ModelBoundary3D& boundary );
            ItemSurfaceRange(
                const BRep& brep, const SurfaceCollection3D& collection );
            ItemSurfaceRange( const ItemSurfaceRange& range );
            ~ItemSurfaceRange();

            [[nodiscard]] const ItemSurfaceRange& begin() const;

            [[nodiscard]] const ItemSurfaceRange& end() const;

            [[nodiscard]] const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_model_api ItemBlockRange
            : public Relationships::ItemRangeIterator
        {
        public:
            ItemBlockRange(
                const BRep& brep, const BlockCollection3D& boundary );
            ItemBlockRange( const ItemBlockRange& range );
            ~ItemBlockRange();

            [[nodiscard]] const ItemBlockRange& begin() const;

            [[nodiscard]] const ItemBlockRange& end() const;

            [[nodiscard]] const Block3D& operator*() const;

        private:
            const BRep& brep_;
        };

    public:
        BRep();
        BRep( BRep&& brep ) noexcept;
        BRep& operator=( BRep&& brep );
        ~BRep();

        [[nodiscard]] BRep clone() const;

        [[nodiscard]] BoundaryCornerRange boundaries(
            const Line3D& line ) const;

        [[nodiscard]] BoundaryLineRange boundaries(
            const Surface3D& surface ) const;

        [[nodiscard]] BoundarySurfaceRange boundaries(
            const Block3D& block ) const;

        [[nodiscard]] IncidentLineRange incidences(
            const Corner3D& corner ) const;

        [[nodiscard]] IncidentSurfaceRange incidences(
            const Line3D& line ) const;

        [[nodiscard]] IncidentBlockRange incidences(
            const Surface3D& surface ) const;

        [[nodiscard]] index_t nb_internal_corners(
            const Surface3D& surface ) const;

        [[nodiscard]] InternalCornerRange internal_corners(
            const Surface3D& surface ) const;

        [[nodiscard]] index_t nb_internal_lines(
            const Surface3D& surface ) const;

        [[nodiscard]] InternalLineRange internal_lines(
            const Surface3D& surface ) const;

        [[nodiscard]] index_t nb_internal_corners( const Block3D& block ) const;

        [[nodiscard]] InternalCornerRange internal_corners(
            const Block3D& block ) const;

        [[nodiscard]] index_t nb_internal_lines( const Block3D& block ) const;

        [[nodiscard]] InternalLineRange internal_lines(
            const Block3D& block ) const;

        [[nodiscard]] index_t nb_internal_surfaces(
            const Block3D& block ) const;

        [[nodiscard]] InternalSurfaceRange internal_surfaces(
            const Block3D& block ) const;

        [[nodiscard]] index_t nb_embedding_surfaces(
            const Corner3D& corner ) const;

        [[nodiscard]] EmbeddingSurfaceRange embedding_surfaces(
            const Corner3D& corner ) const;

        [[nodiscard]] index_t nb_embedding_surfaces( const Line3D& line ) const;

        [[nodiscard]] EmbeddingSurfaceRange embedding_surfaces(
            const Line3D& line ) const;

        [[nodiscard]] index_t nb_embedding_blocks(
            const Corner3D& corner ) const;

        [[nodiscard]] EmbeddingBlockRange embedding_blocks(
            const Corner3D& corner ) const;

        [[nodiscard]] index_t nb_embedding_blocks( const Line3D& line ) const;

        [[nodiscard]] EmbeddingBlockRange embedding_blocks(
            const Line3D& line ) const;

        [[nodiscard]] index_t nb_embedding_blocks(
            const Surface3D& surface ) const;

        [[nodiscard]] EmbeddingBlockRange embedding_blocks(
            const Surface3D& surface ) const;

        [[nodiscard]] ItemSurfaceRange model_boundary_items(
            const ModelBoundary3D& boundary ) const;

        [[nodiscard]] ItemCornerRange corner_collection_items(
            const CornerCollection3D& collection ) const;

        [[nodiscard]] ItemLineRange line_collection_items(
            const LineCollection3D& collection ) const;

        [[nodiscard]] ItemSurfaceRange surface_collection_items(
            const SurfaceCollection3D& collection ) const;

        [[nodiscard]] ItemBlockRange block_collection_items(
            const BlockCollection3D& collection ) const;

        [[nodiscard]] bool is_closed( const Line3D& line ) const;

        [[nodiscard]] bool is_closed( const Surface3D& surface ) const;

        [[nodiscard]] bool is_boundary(
            const Corner3D& corner, const Line3D& line ) const;

        [[nodiscard]] bool is_boundary(
            const Line3D& line, const Surface3D& surface ) const;

        [[nodiscard]] bool is_boundary(
            const Surface3D& surface, const Block3D& block ) const;

        [[nodiscard]] bool is_internal(
            const Corner3D& corner, const Surface3D& surface ) const;

        [[nodiscard]] bool is_internal(
            const Line3D& line, const Surface3D& surface ) const;

        [[nodiscard]] bool is_internal(
            const Corner3D& corner, const Block3D& block ) const;

        [[nodiscard]] bool is_internal(
            const Line3D& line, const Block3D& block ) const;

        [[nodiscard]] bool is_internal(
            const Surface3D& surface, const Block3D& block ) const;

        [[nodiscard]] bool is_model_boundary_item(
            const Surface3D& surface, const ModelBoundary3D& boundary ) const;

        [[nodiscard]] bool is_corner_collection_item( const Corner3D& corner,
            const CornerCollection3D& corner_collection ) const;

        [[nodiscard]] bool is_line_collection_item(
            const Line3D& line, const LineCollection3D& line_collection ) const;

        [[nodiscard]] bool is_surface_collection_item( const Surface3D& surface,
            const SurfaceCollection3D& surface_collection ) const;

        [[nodiscard]] bool is_block_collection_item( const Block3D& block,
            const BlockCollection3D& block_collection ) const;

        [[nodiscard]] BoundingBox3D bounding_box() const;

        [[nodiscard]] static std::string_view native_extension_static()
        {
            static const auto extension = "og_brep";
            return extension;
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }
    };
} // namespace geode