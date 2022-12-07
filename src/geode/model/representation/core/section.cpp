/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polygonal_surface.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/detail/count_relationships.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>

namespace
{
    template < typename Filter, typename Iterator >
    void next_filtered_internal_iterator( Iterator& iterator )
    {
        while(
            iterator.operator!=( iterator )
            && iterator.geode::Relationships::InternalRangeIterator::operator*()
                       .type()
                   != Filter::component_type_static() )
        {
            iterator.geode::Relationships::InternalRangeIterator::operator++();
        }
    }

    template < typename Filter, typename Iterator >
    void next_filtered_embedding_iterator( Iterator& iterator )
    {
        while( iterator.operator!=( iterator )
               && iterator.geode::Relationships::EmbeddingRangeIterator::
                          operator*()
                              .type()
                      != Filter::component_type_static() )
        {
            iterator.geode::Relationships::EmbeddingRangeIterator::operator++();
        }
    }

    template < typename MeshComponentRange >
    geode::BoundingBox2D meshes_bounding_box( MeshComponentRange range )
    {
        geode::BoundingBox2D box;
        for( const auto& component : range )
        {
            box.add_box( component.mesh().bounding_box() );
        }
        return box;
    }
} // namespace

namespace geode
{
    Section::BoundaryCornerRange Section::boundaries( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::BoundaryCornerRange::BoundaryCornerRange(
        const Section& section, const Line2D& line )
        : Relationships::BoundaryRangeIterator( section, line.id() ),
          BeginEnd< BoundaryCornerRange >( *this ),
          section_( section )
    {
    }

    const Corner2D& Section::BoundaryCornerRange::operator*() const
    {
        return section_.corner(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    Section::BoundaryLineRange Section::boundaries(
        const Surface2D& surface ) const
    {
        return { *this, surface };
    }

    Section::BoundaryLineRange::BoundaryLineRange(
        const Section& section, const Surface2D& surface )
        : Relationships::BoundaryRangeIterator( section, surface.id() ),
          BeginEnd< BoundaryLineRange >( *this ),
          section_( section )
    {
    }

    const Line2D& Section::BoundaryLineRange::operator*() const
    {
        return section_.line(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    Section::IncidentLineRange Section::incidences(
        const Corner2D& corner ) const
    {
        return { *this, corner };
    }

    Section::IncidentLineRange::IncidentLineRange(
        const Section& section, const Corner2D& corner )
        : Relationships::IncidenceRangeIterator( section, corner.id() ),
          BeginEnd< IncidentLineRange >( *this ),
          section_( section )
    {
    }

    const Line2D& Section::IncidentLineRange::operator*() const
    {
        return section_.line(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    Section::IncidentSurfaceRange Section::incidences(
        const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::IncidentSurfaceRange::IncidentSurfaceRange(
        const Section& section, const Line2D& line )
        : Relationships::IncidenceRangeIterator( section, line.id() ),
          BeginEnd< IncidentSurfaceRange >( *this ),
          section_( section )
    {
    }

    const Surface2D& Section::IncidentSurfaceRange::operator*() const
    {
        return section_.surface(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    Section::InternalLineRange Section::internal_lines(
        const Surface2D& surface ) const
    {
        return { *this, surface };
    }

    Section::InternalLineRange::InternalLineRange(
        const Section& section, const Surface2D& surface )
        : Relationships::InternalRangeIterator( section, surface.id() ),
          BeginEnd< InternalLineRange >( *this ),
          section_( section )
    {
        next_filtered_internal_iterator< Line2D >( *this );
    }

    void Section::InternalLineRange::operator++()
    {
        Relationships::InternalRangeIterator::operator++();
        next_filtered_internal_iterator< Line2D >( *this );
    }

    const Line2D& Section::InternalLineRange::operator*() const
    {
        return section_.line(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    Section::InternalCornerRange Section::internal_corners(
        const Surface2D& surface ) const
    {
        return { *this, surface };
    }

    Section::InternalCornerRange::InternalCornerRange(
        const Section& section, const Surface2D& surface )
        : Relationships::InternalRangeIterator( section, surface.id() ),
          BeginEnd< InternalCornerRange >( *this ),
          section_( section )
    {
        next_filtered_internal_iterator< Corner2D >( *this );
    }

    void Section::InternalCornerRange::operator++()
    {
        Relationships::InternalRangeIterator::operator++();
        next_filtered_internal_iterator< Corner2D >( *this );
    }

    const Corner2D& Section::InternalCornerRange::operator*() const
    {
        return section_.corner(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    Section::EmbeddingSurfaceRange Section::embedding_surfaces(
        const Corner2D& corner ) const
    {
        return { *this, corner };
    }

    Section::EmbeddingSurfaceRange Section::embedding_surfaces(
        const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::EmbeddingSurfaceRange::EmbeddingSurfaceRange(
        const Section& section, const Line2D& line )
        : Relationships::EmbeddingRangeIterator( section, line.id() ),
          BeginEnd< EmbeddingSurfaceRange >( *this ),
          section_( section )
    {
        next_filtered_embedding_iterator< Surface2D >( *this );
    }

    Section::EmbeddingSurfaceRange::EmbeddingSurfaceRange(
        const Section& section, const Corner2D& corner )
        : Relationships::EmbeddingRangeIterator( section, corner.id() ),
          BeginEnd< EmbeddingSurfaceRange >( *this ),
          section_( section )
    {
        next_filtered_embedding_iterator< Surface2D >( *this );
    }

    void Section::EmbeddingSurfaceRange::operator++()
    {
        Relationships::EmbeddingRangeIterator::operator++();
        next_filtered_embedding_iterator< Surface2D >( *this );
    }

    const Surface2D& Section::EmbeddingSurfaceRange::operator*() const
    {
        return section_.surface(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    Section::ItemLineRange::ItemLineRange(
        const Section& section, const ModelBoundary2D& boundary )
        : Relationships::ItemRangeIterator( section, boundary.id() ),
          BeginEnd( *this ),
          section_( section )
    {
    }

    const Line2D& Section::ItemLineRange::operator*() const
    {
        return section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    Section::Section() {} // NOLINT

    Section::Section( Section&& section )
        : Topology{ std::move( section ) },
          Corners2D{ std::move( section ) },
          Lines2D{ std::move( section ) },
          Surfaces2D{ std::move( section ) },
          ModelBoundaries2D{ std::move( section ) },
          Identifier{ std::move( section ) }
    {
    }

    Section::~Section() {} // NOLINT

    Section::ItemLineRange Section::model_boundary_items(
        const ModelBoundary2D& boundary ) const
    {
        return { *this, boundary };
    }

    index_t Section::nb_internal_corners( const Surface2D& surface ) const
    {
        return detail::count_relationships( internal_corners( surface ) );
    }

    index_t Section::nb_internal_lines( const Surface2D& surface ) const
    {
        return detail::count_relationships( internal_lines( surface ) );
    }

    index_t Section::nb_embedding_surfaces( const Corner2D& corner ) const
    {
        return detail::count_relationships( embedding_surfaces( corner ) );
    }

    index_t Section::nb_embedding_surfaces( const Line2D& line ) const
    {
        return detail::count_relationships( embedding_surfaces( line ) );
    }

    bool Section::is_closed( const Line2D& line ) const
    {
        return nb_boundaries( line.id() ) < 2;
    }

    bool Section::is_boundary(
        const Corner2D& corner, const Line2D& line ) const
    {
        return Relationships::is_boundary( corner.id(), line.id() );
    }

    bool Section::is_boundary(
        const Line2D& line, const Surface2D& surface ) const
    {
        return Relationships::is_boundary( line.id(), surface.id() );
    }

    bool Section::is_internal(
        const Corner2D& corner, const Surface2D& surface ) const
    {
        return Relationships::is_internal( corner.id(), surface.id() );
    }

    bool Section::is_internal(
        const Line2D& line, const Surface2D& surface ) const
    {
        return Relationships::is_internal( line.id(), surface.id() );
    }

    bool Section::is_model_boundary_item(
        const Line2D& line, const ModelBoundary2D& boundary ) const
    {
        return Relationships::is_item( line.id(), boundary.id() );
    }

    BoundingBox2D Section::bounding_box() const
    {
        if( nb_lines() > 0 )
        {
            return meshes_bounding_box( lines() );
        }
        if( nb_surfaces() > 0 )
        {
            return meshes_bounding_box( surfaces() );
        }
        return meshes_bounding_box( corners() );
    }

} // namespace geode
