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

#include <geode/model/representation/core/brep.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    BRep::LineBoundaryRange BRep::boundaries( const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::LineBoundaryRange::LineBoundaryRange(
        const BRep& brep, const Line3D& line )
        : Relationships::BoundaryRangeIterator( brep, line.id() ),
          BeginEnd< LineBoundaryRange >( *this ),
          brep_( brep )
    {
    }

    const Corner3D& BRep::LineBoundaryRange::operator*() const
    {
        return brep_.corner(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    BRep::SurfaceBoundaryRange BRep::boundaries(
        const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::SurfaceBoundaryRange::SurfaceBoundaryRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::BoundaryRangeIterator( brep, surface.id() ),
          BeginEnd< SurfaceBoundaryRange >( *this ),
          brep_( brep )
    {
    }

    const Line3D& BRep::SurfaceBoundaryRange::operator*() const
    {
        return brep_.line(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    BRep::BlockBoundaryRange BRep::boundaries( const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::BlockBoundaryRange::BlockBoundaryRange(
        const BRep& brep, const Block3D& block )
        : Relationships::BoundaryRangeIterator( brep, block.id() ),
          BeginEnd< BlockBoundaryRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::BlockBoundaryRange::operator*() const
    {
        return brep_.surface(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    BRep::CornerIncidenceRange BRep::incidences( const Corner3D& corner ) const
    {
        return { *this, corner };
    }

    BRep::CornerIncidenceRange::CornerIncidenceRange(
        const BRep& brep, const Corner3D& corner )
        : Relationships::IncidenceRangeIterator( brep, corner.id() ),
          BeginEnd< CornerIncidenceRange >( *this ),
          brep_( brep )
    {
    }

    const Line3D& BRep::CornerIncidenceRange::operator*() const
    {
        return brep_.line(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    BRep::LineIncidenceRange BRep::incidences( const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::LineIncidenceRange::LineIncidenceRange(
        const BRep& brep, const Line3D& line )
        : Relationships::IncidenceRangeIterator( brep, line.id() ),
          BeginEnd< LineIncidenceRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::LineIncidenceRange::operator*() const
    {
        return brep_.surface(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    BRep::SurfaceIncidenceRange BRep::incidences(
        const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::SurfaceIncidenceRange::SurfaceIncidenceRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::IncidenceRangeIterator( brep, surface.id() ),
          BeginEnd< SurfaceIncidenceRange >( *this ),
          brep_( brep )
    {
    }

    const Block3D& BRep::SurfaceIncidenceRange::operator*() const
    {
        return brep_.block(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    BRep::SurfaceInternalRange BRep::internals( const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::SurfaceInternalRange::SurfaceInternalRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::InternalRangeIterator( brep, surface.id() ),
          BeginEnd< SurfaceInternalRange >( *this ),
          brep_( brep )
    {
    }

    const Line3D& BRep::SurfaceInternalRange::operator*() const
    {
        return brep_.line(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    BRep::BlockInternalRange BRep::internals( const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::BlockInternalRange::BlockInternalRange(
        const BRep& brep, const Block3D& block )
        : Relationships::InternalRangeIterator( brep, block.id() ),
          BeginEnd< BlockInternalRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::BlockInternalRange::operator*() const
    {
        return brep_.surface(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    BRep::LineEmbeddingRange BRep::embeddings( const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::LineEmbeddingRange::LineEmbeddingRange(
        const BRep& brep, const Line3D& line )
        : Relationships::EmbeddingRangeIterator( brep, line.id() ),
          BeginEnd< LineEmbeddingRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::LineEmbeddingRange::operator*() const
    {
        return brep_.surface(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    BRep::SurfaceEmbeddingRange BRep::embeddings(
        const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::SurfaceEmbeddingRange::SurfaceEmbeddingRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::EmbeddingRangeIterator( brep, surface.id() ),
          BeginEnd< SurfaceEmbeddingRange >( *this ),
          brep_( brep )
    {
    }

    const Block3D& BRep::SurfaceEmbeddingRange::operator*() const
    {
        return brep_.block(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    BRep::ModelBoundaryItemRange::ModelBoundaryItemRange(
        const BRep& brep, const ModelBoundary3D& boundary )
        : Relationships::ItemRangeIterator( brep, boundary.id() ),
          BeginEnd< ModelBoundaryItemRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::ModelBoundaryItemRange::operator*() const
    {
        return brep_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    BRep::ModelBoundaryItemRange BRep::items(
        const ModelBoundary3D& boundary ) const
    {
        return { *this, boundary };
    }
} // namespace geode
