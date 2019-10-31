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

#include <geode/model/representation/core/section.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    Section::LineBoundaryRange Section::boundaries( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::LineBoundaryRange::LineBoundaryRange(
        const Section& section, const Line2D& line )
        : Relationships::BoundaryRangeIterator( section, line.id() ),
          BeginEnd< LineBoundaryRange >( *this ),
          section_( section )
    {
    }

    const Corner2D& Section::LineBoundaryRange::operator*() const
    {
        return section_.corner(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    Section::SurfaceBoundaryRange Section::boundaries(
        const Surface2D& surface ) const
    {
        return { *this, surface };
    }

    Section::SurfaceBoundaryRange::SurfaceBoundaryRange(
        const Section& section, const Surface2D& surface )
        : Relationships::BoundaryRangeIterator( section, surface.id() ),
          BeginEnd< SurfaceBoundaryRange >( *this ),
          section_( section )
    {
    }

    const Line2D& Section::SurfaceBoundaryRange::operator*() const
    {
        return section_.line(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    Section::CornerIncidenceRange Section::incidences(
        const Corner2D& corner ) const
    {
        return { *this, corner };
    }

    Section::CornerIncidenceRange::CornerIncidenceRange(
        const Section& section, const Corner2D& corner )
        : Relationships::IncidenceRangeIterator( section, corner.id() ),
          BeginEnd< CornerIncidenceRange >( *this ),
          section_( section )
    {
    }

    const Line2D& Section::CornerIncidenceRange::operator*() const
    {
        return section_.line(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    Section::LineIncidenceRange Section::incidences( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::LineIncidenceRange::LineIncidenceRange(
        const Section& section, const Line2D& line )
        : Relationships::IncidenceRangeIterator( section, line.id() ),
          BeginEnd< LineIncidenceRange >( *this ),
          section_( section )
    {
    }

    const Surface2D& Section::LineIncidenceRange::operator*() const
    {
        return section_.surface(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    Section::SurfaceInternalRange Section::internals(
        const Surface2D& surface ) const
    {
        return { *this, surface };
    }

    Section::SurfaceInternalRange::SurfaceInternalRange(
        const Section& section, const Surface2D& surface )
        : Relationships::InternalRangeIterator( section, surface.id() ),
          BeginEnd< SurfaceInternalRange >( *this ),
          section_( section )
    {
    }

    const Line2D& Section::SurfaceInternalRange::operator*() const
    {
        return section_.line(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    Section::LineEmbeddingRange Section::embeddings( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::LineEmbeddingRange::LineEmbeddingRange(
        const Section& section, const Line2D& line )
        : Relationships::EmbeddingRangeIterator( section, line.id() ),
          BeginEnd< LineEmbeddingRange >( *this ),
          section_( section )
    {
    }

    const Surface2D& Section::LineEmbeddingRange::operator*() const
    {
        return section_.surface(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    Section::ModelBoundaryItemRange::ModelBoundaryItemRange(
        const Section& section, const ModelBoundary2D& boundary )
        : Relationships::ItemRangeIterator( section, boundary.id() ),
          BeginEnd( *this ),
          section_( section )
    {
    }

    const Line2D& Section::ModelBoundaryItemRange::operator*() const
    {
        return section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    Section::ModelBoundaryItemRange Section::items(
        const ModelBoundary2D& boundary ) const
    {
        return { *this, boundary };
    }
} // namespace geode
