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
        : Relationships::BoundaryRangeIterator( brep, line.id() ), brep_( brep )
    {
    }

    BRep::BoundaryCornerRange::BoundaryCornerRange(
        const BoundaryCornerRange& range )
        : Relationships::BoundaryRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::BoundaryCornerRange::~BoundaryCornerRange() = default;

    auto BRep::BoundaryCornerRange::begin() const -> const BoundaryCornerRange&
    {
        return *this;
    }

    auto BRep::BoundaryCornerRange::end() const -> const BoundaryCornerRange&
    {
        return *this;
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
          brep_( brep )
    {
    }

    BRep::BoundaryLineRange::BoundaryLineRange( const BoundaryLineRange& range )
        : Relationships::BoundaryRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::BoundaryLineRange::~BoundaryLineRange() = default;

    auto BRep::BoundaryLineRange::begin() const -> const BoundaryLineRange&
    {
        return *this;
    }

    auto BRep::BoundaryLineRange::end() const -> const BoundaryLineRange&
    {
        return *this;
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
          brep_( brep )
    {
    }

    BRep::BoundarySurfaceRange::BoundarySurfaceRange(
        const BoundarySurfaceRange& range )
        : Relationships::BoundaryRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::BoundarySurfaceRange::~BoundarySurfaceRange() = default;

    auto BRep::BoundarySurfaceRange::begin() const
        -> const BoundarySurfaceRange&
    {
        return *this;
    }

    auto BRep::BoundarySurfaceRange::end() const -> const BoundarySurfaceRange&
    {
        return *this;
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
          brep_( brep )
    {
    }

    BRep::IncidentLineRange::IncidentLineRange( const IncidentLineRange& range )
        : Relationships::IncidenceRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::IncidentLineRange::~IncidentLineRange() = default;

    auto BRep::IncidentLineRange::begin() const -> const IncidentLineRange&
    {
        return *this;
    }

    auto BRep::IncidentLineRange::end() const -> const IncidentLineRange&
    {
        return *this;
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
          brep_( brep )
    {
    }

    BRep::IncidentSurfaceRange::IncidentSurfaceRange(
        const IncidentSurfaceRange& range )
        : Relationships::IncidenceRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::IncidentSurfaceRange::~IncidentSurfaceRange() = default;

    auto BRep::IncidentSurfaceRange::begin() const
        -> const IncidentSurfaceRange&
    {
        return *this;
    }

    auto BRep::IncidentSurfaceRange::end() const -> const IncidentSurfaceRange&
    {
        return *this;
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
          brep_( brep )
    {
    }

    BRep::IncidentBlockRange::IncidentBlockRange(
        const IncidentBlockRange& range )
        : Relationships::IncidenceRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::IncidentBlockRange::~IncidentBlockRange() = default;

    auto BRep::IncidentBlockRange::begin() const -> const IncidentBlockRange&
    {
        return *this;
    }

    auto BRep::IncidentBlockRange::end() const -> const IncidentBlockRange&
    {
        return *this;
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
          brep_( brep )
    {
        next_filtered_internal_iterator< Line3D >( *this );
    }

    BRep::InternalLineRange::InternalLineRange( const InternalLineRange& range )
        : Relationships::InternalRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::InternalLineRange BRep::internal_lines( const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::InternalLineRange::~InternalLineRange() = default;

    auto BRep::InternalLineRange::begin() const -> const InternalLineRange&
    {
        return *this;
    }

    auto BRep::InternalLineRange::end() const -> const InternalLineRange&
    {
        return *this;
    }

    BRep::InternalLineRange::InternalLineRange(
        const BRep& brep, const Block3D& block )
        : Relationships::InternalRangeIterator( brep, block.id() ),
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
          brep_( brep )
    {
        next_filtered_internal_iterator< Corner3D >( *this );
    }

    BRep::InternalCornerRange::InternalCornerRange(
        const InternalCornerRange& range )
        : Relationships::InternalRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::InternalCornerRange::~InternalCornerRange() = default;

    auto BRep::InternalCornerRange::begin() const -> const InternalCornerRange&
    {
        return *this;
    }

    auto BRep::InternalCornerRange::end() const -> const InternalCornerRange&
    {
        return *this;
    }

    BRep::InternalCornerRange BRep::internal_corners(
        const Block3D& block ) const
    {
        return { *this, block };
    }

    BRep::InternalCornerRange::InternalCornerRange(
        const BRep& brep, const Block3D& block )
        : Relationships::InternalRangeIterator( brep, block.id() ),
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
          brep_( brep )
    {
        next_filtered_internal_iterator< Surface3D >( *this );
    }

    BRep::InternalSurfaceRange::InternalSurfaceRange(
        const InternalSurfaceRange& range )
        : Relationships::InternalRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::InternalSurfaceRange::~InternalSurfaceRange() = default;

    auto BRep::InternalSurfaceRange::begin() const
        -> const InternalSurfaceRange&
    {
        return *this;
    }

    auto BRep::InternalSurfaceRange::end() const -> const InternalSurfaceRange&
    {
        return *this;
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

    BRep::EmbeddingSurfaceRange BRep::embedding_surfaces(
        const Corner3D& corner ) const
    {
        return { *this, corner };
    }

    BRep::EmbeddingSurfaceRange::EmbeddingSurfaceRange(
        const BRep& brep, const Corner3D& corner )
        : Relationships::EmbeddingRangeIterator( brep, corner.id() ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Surface3D >( *this );
    }

    BRep::EmbeddingSurfaceRange::EmbeddingSurfaceRange(
        const EmbeddingSurfaceRange& range )
        : Relationships::EmbeddingRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::EmbeddingSurfaceRange::~EmbeddingSurfaceRange() = default;

    auto BRep::EmbeddingSurfaceRange::begin() const
        -> const EmbeddingSurfaceRange&
    {
        return *this;
    }

    auto BRep::EmbeddingSurfaceRange::end() const
        -> const EmbeddingSurfaceRange&
    {
        return *this;
    }

    BRep::EmbeddingSurfaceRange BRep::embedding_surfaces(
        const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::EmbeddingSurfaceRange::EmbeddingSurfaceRange(
        const BRep& brep, const Line3D& line )
        : Relationships::EmbeddingRangeIterator( brep, line.id() ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Surface3D >( *this );
    }

    void BRep::EmbeddingSurfaceRange::operator++()
    {
        Relationships::EmbeddingRangeIterator::operator++();
        next_filtered_embedding_iterator< Surface3D >( *this );
    }

    const Surface3D& BRep::EmbeddingSurfaceRange::operator*() const
    {
        return brep_.surface(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    BRep::EmbeddingBlockRange BRep::embedding_blocks(
        const Corner3D& corner ) const
    {
        return { *this, corner };
    }

    BRep::EmbeddingBlockRange::EmbeddingBlockRange(
        const BRep& brep, const Corner3D& corner )
        : Relationships::EmbeddingRangeIterator( brep, corner.id() ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    BRep::EmbeddingBlockRange BRep::embedding_blocks( const Line3D& line ) const
    {
        return { *this, line };
    }

    BRep::EmbeddingBlockRange::EmbeddingBlockRange(
        const BRep& brep, const Line3D& line )
        : Relationships::EmbeddingRangeIterator( brep, line.id() ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    BRep::EmbeddingBlockRange::EmbeddingBlockRange(
        const EmbeddingBlockRange& range )
        : Relationships::EmbeddingRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::EmbeddingBlockRange::~EmbeddingBlockRange() = default;

    auto BRep::EmbeddingBlockRange::begin() const -> const EmbeddingBlockRange&
    {
        return *this;
    }

    auto BRep::EmbeddingBlockRange::end() const -> const EmbeddingBlockRange&
    {
        return *this;
    }

    BRep::EmbeddingBlockRange BRep::embedding_blocks(
        const Surface3D& surface ) const
    {
        return { *this, surface };
    }

    BRep::EmbeddingBlockRange::EmbeddingBlockRange(
        const BRep& brep, const Surface3D& surface )
        : Relationships::EmbeddingRangeIterator( brep, surface.id() ),
          brep_( brep )
    {
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    void BRep::EmbeddingBlockRange::operator++()
    {
        Relationships::EmbeddingRangeIterator::operator++();
        next_filtered_embedding_iterator< Block3D >( *this );
    }

    const Block3D& BRep::EmbeddingBlockRange::operator*() const
    {
        return brep_.block(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    BRep::ItemSurfaceRange::ItemSurfaceRange(
        const BRep& brep, const ModelBoundary3D& boundary )
        : Relationships::ItemRangeIterator( brep, boundary.id() ), brep_( brep )
    {
    }

    BRep::ItemSurfaceRange::ItemSurfaceRange( const ItemSurfaceRange& range )
        : Relationships::ItemRangeIterator{ range }, brep_( range.brep_ )
    {
    }

    BRep::ItemSurfaceRange::~ItemSurfaceRange() = default;

    auto BRep::ItemSurfaceRange::begin() const -> const ItemSurfaceRange&
    {
        return *this;
    }

    auto BRep::ItemSurfaceRange::end() const -> const ItemSurfaceRange&
    {
        return *this;
    }

    const Surface3D& BRep::ItemSurfaceRange::operator*() const
    {
        return brep_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    BRep::BRep() = default;

    BRep::BRep( BRep&& brep ) noexcept
        : Topology{ std::move( brep ) },
          Corners3D{ std::move( brep ) },
          Lines3D{ std::move( brep ) },
          Surfaces3D{ std::move( brep ) },
          Blocks3D{ std::move( brep ) },
          ModelBoundaries3D{ std::move( brep ) },
          Identifier{ std::move( brep ) }
    {
    }

    BRep& BRep::operator=( BRep&& brep )
    {
        Topology::operator=( std::move( brep ) );
        Corners3D::operator=( std::move( brep ) );
        Lines3D::operator=( std::move( brep ) );
        Surfaces3D::operator=( std::move( brep ) );
        Blocks3D::operator=( std::move( brep ) );
        ModelBoundaries3D::operator=( std::move( brep ) );
        Identifier::operator=( std::move( brep ) );
        return *this;
    }

    BRep::~BRep() = default;

    BRep::ItemSurfaceRange BRep::model_boundary_items(
        const ModelBoundary3D& boundary ) const
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

    index_t BRep::nb_embedding_surfaces( const Corner3D& corner ) const
    {
        return detail::count_relationships( embedding_surfaces( corner ) );
    }

    index_t BRep::nb_embedding_surfaces( const Line3D& line ) const
    {
        return detail::count_relationships( embedding_surfaces( line ) );
    }

    index_t BRep::nb_embedding_blocks( const Corner3D& corner ) const
    {
        return detail::count_relationships( embedding_blocks( corner ) );
    }

    index_t BRep::nb_embedding_blocks( const Line3D& line ) const
    {
        return detail::count_relationships( embedding_blocks( line ) );
    }

    index_t BRep::nb_embedding_blocks( const Surface3D& surface ) const
    {
        return detail::count_relationships( embedding_blocks( surface ) );
    }

    bool BRep::is_closed( const Line3D& line ) const
    {
        return nb_boundaries( line.id() ) < 2;
    }

    bool BRep::is_closed( const Surface3D& surface ) const
    {
        return nb_boundaries( surface.id() ) == 0;
    }

    bool BRep::is_boundary( const Corner3D& corner, const Line3D& line ) const
    {
        return Relationships::is_boundary( corner.id(), line.id() );
    }

    bool BRep::is_boundary( const Line3D& line, const Surface3D& surface ) const
    {
        return Relationships::is_boundary( line.id(), surface.id() );
    }

    bool BRep::is_boundary(
        const Surface3D& surface, const Block3D& block ) const
    {
        return Relationships::is_boundary( surface.id(), block.id() );
    }

    bool BRep::is_internal(
        const Corner3D& corner, const Surface3D& surface ) const
    {
        return Relationships::is_internal( corner.id(), surface.id() );
    }

    bool BRep::is_internal( const Line3D& line, const Surface3D& surface ) const
    {
        return Relationships::is_internal( line.id(), surface.id() );
    }

    bool BRep::is_internal( const Corner3D& corner, const Block3D& block ) const
    {
        return Relationships::is_internal( corner.id(), block.id() );
    }

    bool BRep::is_internal( const Line3D& line, const Block3D& block ) const
    {
        return Relationships::is_internal( line.id(), block.id() );
    }

    bool BRep::is_internal(
        const Surface3D& surface, const Block3D& block ) const
    {
        return Relationships::is_internal( surface.id(), block.id() );
    }

    bool BRep::is_model_boundary_item(
        const Surface3D& surface, const ModelBoundary3D& boundary ) const
    {
        return Relationships::is_item( surface.id(), boundary.id() );
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
