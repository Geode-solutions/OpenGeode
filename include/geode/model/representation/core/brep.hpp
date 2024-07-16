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

            const BoundaryCornerRange& begin() const;

            const BoundaryCornerRange& end() const;

            const Corner3D& operator*() const;

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

            const BoundaryLineRange& begin() const;

            const BoundaryLineRange& end() const;

            const Line3D& operator*() const;

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

            const BoundarySurfaceRange& begin() const;

            const BoundarySurfaceRange& end() const;

            const Surface3D& operator*() const;

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

            const IncidentLineRange& begin() const;

            const IncidentLineRange& end() const;

            const Line3D& operator*() const;

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

            const IncidentSurfaceRange& begin() const;

            const IncidentSurfaceRange& end() const;

            const Surface3D& operator*() const;

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

            const IncidentBlockRange& begin() const;

            const IncidentBlockRange& end() const;

            const Block3D& operator*() const;

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

            const InternalCornerRange& begin() const;

            const InternalCornerRange& end() const;

            void operator++();

            const Corner3D& operator*() const;

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

            const InternalLineRange& begin() const;

            const InternalLineRange& end() const;

            void operator++();

            const Line3D& operator*() const;

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

            const InternalSurfaceRange& begin() const;

            const InternalSurfaceRange& end() const;

            void operator++();

            const Surface3D& operator*() const;

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

            const EmbeddingSurfaceRange& begin() const;

            const EmbeddingSurfaceRange& end() const;

            void operator++();

            const Surface3D& operator*() const;

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

            const EmbeddingBlockRange& begin() const;

            const EmbeddingBlockRange& end() const;

            void operator++();

            const Block3D& operator*() const;

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

            const ItemCornerRange& begin() const;

            const ItemCornerRange& end() const;

            const Corner3D& operator*() const;

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

            const ItemLineRange& begin() const;

            const ItemLineRange& end() const;

            const Line3D& operator*() const;

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

            const ItemSurfaceRange& begin() const;

            const ItemSurfaceRange& end() const;

            const Surface3D& operator*() const;

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

            const ItemBlockRange& begin() const;

            const ItemBlockRange& end() const;

            const Block3D& operator*() const;

        private:
            const BRep& brep_;
        };

    public:
        BRep();
        BRep( BRep&& brep ) noexcept;
        BRep& operator=( BRep&& brep );
        ~BRep();

        BRep clone() const;

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

        index_t nb_embedding_surfaces( const Corner3D& corner ) const;

        EmbeddingSurfaceRange embedding_surfaces(
            const Corner3D& corner ) const;

        index_t nb_embedding_surfaces( const Line3D& line ) const;

        EmbeddingSurfaceRange embedding_surfaces( const Line3D& line ) const;

        index_t nb_embedding_blocks( const Corner3D& corner ) const;

        EmbeddingBlockRange embedding_blocks( const Corner3D& corner ) const;

        index_t nb_embedding_blocks( const Line3D& line ) const;

        EmbeddingBlockRange embedding_blocks( const Line3D& line ) const;

        index_t nb_embedding_blocks( const Surface3D& surface ) const;

        EmbeddingBlockRange embedding_blocks( const Surface3D& surface ) const;

        ItemSurfaceRange model_boundary_items(
            const ModelBoundary3D& boundary ) const;

        ItemCornerRange corner_collection_items(
            const CornerCollection3D& collection ) const;

        ItemLineRange line_collection_items(
            const LineCollection3D& collection ) const;

        ItemSurfaceRange surface_collection_items(
            const SurfaceCollection3D& collection ) const;

        ItemBlockRange block_collection_items(
            const BlockCollection3D& collection ) const;

        bool is_closed( const Line3D& line ) const;

        bool is_closed( const Surface3D& surface ) const;

        bool is_boundary( const Corner3D& corner, const Line3D& line ) const;

        bool is_boundary( const Line3D& line, const Surface3D& surface ) const;

        bool is_boundary(
            const Surface3D& surface, const Block3D& block ) const;

        bool is_internal(
            const Corner3D& corner, const Surface3D& surface ) const;

        bool is_internal( const Line3D& line, const Surface3D& surface ) const;

        bool is_internal( const Corner3D& corner, const Block3D& block ) const;

        bool is_internal( const Line3D& line, const Block3D& block ) const;

        bool is_internal(
            const Surface3D& surface, const Block3D& block ) const;

        bool is_model_boundary_item(
            const Surface3D& surface, const ModelBoundary3D& boundary ) const;

        bool is_corner_collection_item(
            const Corner3D& surface, const CornerCollection3D& boundary ) const;

        bool is_line_collection_item(
            const Line3D& surface, const LineCollection3D& boundary ) const;

        bool is_surface_collection_item( const Surface3D& surface,
            const SurfaceCollection3D& boundary ) const;

        bool is_block_collection_item(
            const Block3D& surface, const BlockCollection3D& boundary ) const;

        /*!
         * Compute the bounding box from component meshes
         */
        BoundingBox3D bounding_box() const;

        static std::string_view native_extension_static()
        {
            static const auto extension = "og_brep";
            return extension;
        }

        std::string_view native_extension() const
        {
            return native_extension_static();
        }
    };
} // namespace geode