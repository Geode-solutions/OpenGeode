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

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/polyhedral_solid.h>

#include <geode/model/mixin/core/block.h>
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
    geode::BoundingBox3D meshes_bounding_box( MeshComponentRange range )
    {
        geode::BoundingBox3D box;
        for( const auto& component : range )
        {
            box.add_box( component.mesh().bounding_box() );
        }
        return box;
    }
} // namespace

namespace geode
{
    BRep::BoundaryCornerRange BRep::boundaries( const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::BoundaryCornerRange::BoundaryCornerRange(
        const BRep& brep, const Line3D& line )
        : Relationships::BoundaryRangeIterator( brep, line.id() ),
          BeginEnd< BoundaryCornerRange >( *this ),
          brep_( brep )
    {
    }

    const Corner3D& BRep::BoundaryCornerRange::operator*() const
    {
        return brep_.corner(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    BRep::BoundaryLineRange BRep::boundaries( const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::BoundaryLineRange::BoundaryLineRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::BoundaryRangeIterator( brep, surface.id() ),
          BeginEnd< BoundaryLineRange >( *this ),
          brep_( brep )
    {
    }

    const Line3D& BRep::BoundaryLineRange::operator*() const
    {
        return brep_.line(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    BRep::BoundarySurfaceRange BRep::boundaries( const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::BoundarySurfaceRange::BoundarySurfaceRange(
        const BRep& brep, const Block3D& block )
        : Relationships::BoundaryRangeIterator( brep, block.id() ),
          BeginEnd< BoundarySurfaceRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::BoundarySurfaceRange::operator*() const
    {
        return brep_.surface(
            Relationships::BoundaryRangeIterator::operator*().id() );
    }

    BRep::IncidentLineRange BRep::incidences( const Corner3D& corner ) const
    {
        return { *this, corner };
    }

    BRep::IncidentLineRange::IncidentLineRange(
        const BRep& brep, const Corner3D& corner )
        : Relationships::IncidenceRangeIterator( brep, corner.id() ),
          BeginEnd< IncidentLineRange >( *this ),
          brep_( brep )
    {
    }

    const Line3D& BRep::IncidentLineRange::operator*() const
    {
        return brep_.line(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    BRep::IncidentSurfaceRange BRep::incidences( const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::IncidentSurfaceRange::IncidentSurfaceRange(
        const BRep& brep, const Line3D& line )
        : Relationships::IncidenceRangeIterator( brep, line.id() ),
          BeginEnd< IncidentSurfaceRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::IncidentSurfaceRange::operator*() const
    {
        return brep_.surface(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    BRep::IncidentBlockRange BRep::incidences( const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::IncidentBlockRange::IncidentBlockRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::IncidenceRangeIterator( brep, surface.id() ),
          BeginEnd< IncidentBlockRange >( *this ),
          brep_( brep )
    {
    }

    const Block3D& BRep::IncidentBlockRange::operator*() const
    {
        return brep_.block(
            Relationships::IncidenceRangeIterator::operator*().id() );
    }

    BRep::InternalLineRange BRep::internal_lines(
        const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::InternalLineRange::InternalLineRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::InternalRangeIterator( brep, surface.id() ),
          BeginEnd< InternalLineRange >( *this ),
          brep_( brep )
    {
        next_filtered_internal_iterator< Line3D >( *this );
    }

    BRep::InternalLineRange BRep::internal_lines( const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::InternalLineRange::InternalLineRange(
        const BRep& brep, const Block3D& block )
        : Relationships::InternalRangeIterator( brep, block.id() ),
          BeginEnd< InternalLineRange >( *this ),
          brep_( brep )
    {
        next_filtered_internal_iterator< Line3D >( *this );
    }

    void BRep::InternalLineRange::operator++()
    {
        Relationships::InternalRangeIterator::operator++();
        next_filtered_internal_iterator< Line3D >( *this );
    }

    const Line3D& BRep::InternalLineRange::operator*() const
    {
        return brep_.line(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    BRep::InternalCornerRange BRep::internal_corners(
        const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::InternalCornerRange::InternalCornerRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::InternalRangeIterator( brep, surface.id() ),
          BeginEnd< InternalCornerRange >( *this ),
          brep_( brep )
    {
        next_filtered_internal_iterator< Corner3D >( *this );
    }

    BRep::InternalCornerRange BRep::internal_corners(
        const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::InternalCornerRange::InternalCornerRange(
        const BRep& brep, const Block3D& block )
        : Relationships::InternalRangeIterator( brep, block.id() ),
          BeginEnd< InternalCornerRange >( *this ),
          brep_( brep )
    {
        next_filtered_internal_iterator< Corner3D >( *this );
    }

    void BRep::InternalCornerRange::operator++()
    {
        Relationships::InternalRangeIterator::operator++();
        next_filtered_internal_iterator< Corner3D >( *this );
    }

    const Corner3D& BRep::InternalCornerRange::operator*() const
    {
        return brep_.corner(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    BRep::InternalSurfaceRange BRep::internal_surfaces(
        const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::InternalSurfaceRange::InternalSurfaceRange(
        const BRep& brep, const Block3D& block )
        : Relationships::InternalRangeIterator( brep, block.id() ),
          BeginEnd< InternalSurfaceRange >( *this ),
          brep_( brep )
    {
        next_filtered_internal_iterator< Surface3D >( *this );
    }

    void BRep::InternalSurfaceRange::operator++()
    {
        Relationships::InternalRangeIterator::operator++();
        next_filtered_internal_iterator< Surface3D >( *this );
    }

    const Surface3D& BRep::InternalSurfaceRange::operator*() const
    {
        return brep_.surface(
            Relationships::InternalRangeIterator::operator*().id() );
    }

    BRep::EmbeddedSurfaceRange BRep::embedded_surfaces(
        const Corner3D& corner ) const
    {
        return { *this, corner };
    }

    BRep::EmbeddedSurfaceRange::EmbeddedSurfaceRange(
        const BRep& brep, const Corner3D& corner )
        : Relationships::EmbeddingRangeIterator( brep, corner.id() ),
          BeginEnd< EmbeddedSurfaceRange >( *this ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Surface3D >( *this );
    }

    BRep::EmbeddedSurfaceRange BRep::embedded_surfaces(
        const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::EmbeddedSurfaceRange::EmbeddedSurfaceRange(
        const BRep& brep, const Line3D& line )
        : Relationships::EmbeddingRangeIterator( brep, line.id() ),
          BeginEnd< EmbeddedSurfaceRange >( *this ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Surface3D >( *this );
    }

    void BRep::EmbeddedSurfaceRange::operator++()
    {
        Relationships::EmbeddingRangeIterator::operator++();
        next_filtered_embedding_iterator< Surface3D >( *this );
    }

    const Surface3D& BRep::EmbeddedSurfaceRange::operator*() const
    {
        return brep_.surface(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    BRep::EmbeddedBlockRange BRep::embedded_blocks(
        const Corner3D& corner ) const
    {
        return { *this, corner };
    }

    BRep::EmbeddedBlockRange::EmbeddedBlockRange(
        const BRep& brep, const Corner3D& corner )
        : Relationships::EmbeddingRangeIterator( brep, corner.id() ),
          BeginEnd< EmbeddedBlockRange >( *this ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    BRep::EmbeddedBlockRange BRep::embedded_blocks( const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::EmbeddedBlockRange::EmbeddedBlockRange(
        const BRep& brep, const Line3D& line )
        : Relationships::EmbeddingRangeIterator( brep, line.id() ),
          BeginEnd< EmbeddedBlockRange >( *this ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    BRep::EmbeddedBlockRange BRep::embedded_blocks(
        const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::EmbeddedBlockRange::EmbeddedBlockRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::EmbeddingRangeIterator( brep, surface.id() ),
          BeginEnd< EmbeddedBlockRange >( *this ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    void BRep::EmbeddedBlockRange::operator++()
    {
        Relationships::EmbeddingRangeIterator::operator++();
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    const Block3D& BRep::EmbeddedBlockRange::operator*() const
    {
        return brep_.block(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    BRep::ItemSurfaceRange::ItemSurfaceRange(
        const BRep& brep, const ModelBoundary3D& boundary )
        : Relationships::ItemRangeIterator( brep, boundary.id() ),
          BeginEnd< ItemSurfaceRange >( *this ),
          brep_( brep )
    {
    }

    const Surface3D& BRep::ItemSurfaceRange::operator*() const
    {
        return brep_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    BRep::ItemSurfaceRange BRep::items( const ModelBoundary3D& boundary ) const
    {
        return { *this, boundary };
    }

    index_t BRep::nb_internal_corners( const Surface3D& surface ) const
    {
        return detail::count_relationships( internal_corners( surface ) );
    }

    index_t BRep::nb_internal_lines( const Surface3D& surface ) const
    {
        return detail::count_relationships( internal_lines( surface ) );
    }

    index_t BRep::nb_internal_corners( const Block3D& block ) const
    {
        return detail::count_relationships( internal_corners( block ) );
    }

    index_t BRep::nb_internal_lines( const Block3D& block ) const
    {
        return detail::count_relationships( internal_lines( block ) );
    }

    index_t BRep::nb_internal_surfaces( const Block3D& block ) const
    {
        return detail::count_relationships( internal_surfaces( block ) );
    }

    index_t BRep::nb_embedded_surfaces( const Corner3D& corner ) const
    {
        return detail::count_relationships( embedded_surfaces( corner ) );
    }

    index_t BRep::nb_embedded_surfaces( const Line3D& line ) const
    {
        return detail::count_relationships( embedded_surfaces( line ) );
    }

    index_t BRep::nb_embedded_blocks( const Corner3D& corner ) const
    {
        return detail::count_relationships( embedded_blocks( corner ) );
    }

    index_t BRep::nb_embedded_blocks( const Line3D& line ) const
    {
        return detail::count_relationships( embedded_blocks( line ) );
    }

    index_t BRep::nb_embedded_blocks( const Surface3D& surface ) const
    {
        return detail::count_relationships( embedded_blocks( surface ) );
    }

    bool BRep::is_closed( const Line3D& line ) const
    {
        return nb_boundaries( line.id() ) < 2;
    }

    bool BRep::is_closed( const Surface3D& surface ) const
    {
        return nb_boundaries( surface.id() ) == 0;
    }

    BoundingBox3D BRep::bounding_box() const
    {
        if( nb_surfaces() > 0 )
        {
            return meshes_bounding_box( surfaces() );
        }
        if( nb_blocks() > 0 )
        {
            return meshes_bounding_box( blocks() );
        }
        if( nb_lines() > 0 )
        {
            return meshes_bounding_box( lines() );
        }
        return meshes_bounding_box( corners() );
    }
} // namespace geode
