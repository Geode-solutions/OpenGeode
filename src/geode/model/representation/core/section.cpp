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
        : Relationships::BoundaryRange( section, line.id() ),
          section_( section )
    {
    }

    const Corner2D& Section::LineBoundaryRange::operator*() const
    {
        return section_.corner( Relationships::BoundaryRange::operator*() );
    }

    Section::SurfaceBoundaryRange Section::boundaries(
        const Surface2D& surface ) const
    {
        return { *this, surface };
    }

    Section::SurfaceBoundaryRange::SurfaceBoundaryRange(
        const Section& section, const Surface2D& surface )
        : Relationships::BoundaryRange( section, surface.id() ),
          section_( section )
    {
    }

    const Line2D& Section::SurfaceBoundaryRange::operator*() const
    {
        return section_.line( Relationships::BoundaryRange::operator*() );
    }

    Section::CornerIncidenceRange Section::incidences(
        const Corner2D& corner ) const
    {
        return { *this, corner };
    }

    Section::CornerIncidenceRange::CornerIncidenceRange(
        const Section& section, const Corner2D& corner )
        : Relationships::IncidenceRange( section, corner.id() ),
          section_( section )
    {
    }

    const Line2D& Section::CornerIncidenceRange::operator*() const
    {
        return section_.line( Relationships::IncidenceRange::operator*() );
    }

    Section::LineIncidenceRange Section::incidences( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::LineIncidenceRange::LineIncidenceRange(
        const Section& section, const Line2D& line )
        : Relationships::IncidenceRange( section, line.id() ),
          section_( section )
    {
    }

    const Surface2D& Section::LineIncidenceRange::operator*() const
    {
        return section_.surface( Relationships::IncidenceRange::operator*() );
    }

    Section::SurfaceInternalRange Section::internals(
        const Surface2D& surface ) const
    {
        return { *this, surface };
    }

    Section::SurfaceInternalRange::SurfaceInternalRange(
        const Section& section, const Surface2D& surface )
        : Relationships::InternalRange( section, surface.id() ),
          section_( section )
    {
    }

    const Line2D& Section::SurfaceInternalRange::operator*() const
    {
        return section_.line( Relationships::InternalRange::operator*() );
    }

    Section::LineEmbeddingRange Section::embeddings( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::LineEmbeddingRange::LineEmbeddingRange(
        const Section& section, const Line2D& line )
        : Relationships::EmbeddingRange( section, line.id() ),
          section_( section )
    {
    }

    const Surface2D& Section::LineEmbeddingRange::operator*() const
    {
        return section_.surface( Relationships::EmbeddingRange::operator*() );
    }

    Section::ModelBoundaryItemRange::ModelBoundaryItemRange(
        const Section& section, const ModelBoundary2D& boundary )
        : Relationships::ItemRange( section, boundary.id() ),
          section_( section )
    {
    }

    const Line2D& Section::ModelBoundaryItemRange::operator*() const
    {
        return section_.line( Relationships::ItemRange::operator*() );
    }

    Section::ModelBoundaryItemRange Section::items(
        const ModelBoundary2D& boundary ) const
    {
        return { *this, boundary };
    }
} // namespace geode
