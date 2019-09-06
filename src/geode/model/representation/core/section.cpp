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

#include <geode/basic/bounding_box.h>
#include <geode/basic/vector.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    Section::LineBoundaryRange Section::boundaries( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::LineBoundaryRange::LineBoundaryRange( const Section& section,
        const Line2D& line )
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

    Section::SurfaceBoundaryRange::SurfaceBoundaryRange( const Section& section,
        const Surface2D& surface )
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

    Section::CornerIncidenceRange::CornerIncidenceRange( const Section& section,
        const Corner2D& corner )
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

    Section::LineIncidenceRange::LineIncidenceRange( const Section& section,
        const Line2D& line )
        : Relationships::IncidenceRange( section, line.id() ),
          section_( section )
    {
    }

    const Surface2D& Section::LineIncidenceRange::operator*() const
    {
        return section_.surface( Relationships::IncidenceRange::operator*() );
    }

    void Section::compute_epsilon( double& epsilon ) const
    {
        BoundingBox2D box;
        for( const auto& surface : surfaces() )
        {
            const auto& mesh = surface.mesh();
            for( auto v : Range{ mesh.nb_vertices() } )
            {
                box.add_point( mesh.point( v ) );
            }
        }
        Vector2D diagonal{ box.min(), box.max() };
        epsilon = epsilon_scaling
                  * std::min( diagonal.value( 0 ),
                        std::min( diagonal.value( 1 ), diagonal.value( 2 ) ) );
    }
} // namespace geode
