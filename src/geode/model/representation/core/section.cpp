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

#include <geode/model/representation/core/section.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/detail/count_relationships.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/detail/clone.hpp>
#include <geode/model/representation/core/internal/helpers.hpp>

namespace geode
{
    Section::BoundaryCornerRange Section::boundaries( const Line2D& line ) const
    {
        return { *this, line };
    }

    Section::BoundaryCornerRange::BoundaryCornerRange(
        const Section& section, const Line2D& line )
        : Relationships::BoundaryRangeIterator( section, line.id() ),
          section_( section )
    {
    }

    Section::BoundaryCornerRange::BoundaryCornerRange(
        const BoundaryCornerRange& range )
        : Relationships::BoundaryRangeIterator{ range },
          section_( range.section_ )
    {
    }

    Section::BoundaryCornerRange::~BoundaryCornerRange() {}

    auto Section::BoundaryCornerRange::begin() const
        -> const BoundaryCornerRange&
    {
        return *this;
    }

    auto Section::BoundaryCornerRange::end() const -> const BoundaryCornerRange&
    {
        return *this;
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
          section_( section )
    {
    }

    Section::BoundaryLineRange::BoundaryLineRange(
        const BoundaryLineRange& range )
        : Relationships::BoundaryRangeIterator{ range },
          section_( range.section_ )
    {
    }

    Section::BoundaryLineRange::~BoundaryLineRange() {}

    auto Section::BoundaryLineRange::begin() const -> const BoundaryLineRange&
    {
        return *this;
    }

    auto Section::BoundaryLineRange::end() const -> const BoundaryLineRange&
    {
        return *this;
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
          section_( section )
    {
    }

    Section::IncidentLineRange::IncidentLineRange(
        const IncidentLineRange& range )
        : Relationships::IncidenceRangeIterator{ range },
          section_( range.section_ )
    {
    }

    Section::IncidentLineRange::~IncidentLineRange() {}

    auto Section::IncidentLineRange::begin() const -> const IncidentLineRange&
    {
        return *this;
    }

    auto Section::IncidentLineRange::end() const -> const IncidentLineRange&
    {
        return *this;
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
          section_( section )
    {
    }

    Section::IncidentSurfaceRange::IncidentSurfaceRange(
        const IncidentSurfaceRange& range )
        : Relationships::IncidenceRangeIterator{ range },
          section_( range.section_ )
    {
    }

    Section::IncidentSurfaceRange::~IncidentSurfaceRange() {}

    auto Section::IncidentSurfaceRange::begin() const
        -> const IncidentSurfaceRange&
    {
        return *this;
    }

    auto Section::IncidentSurfaceRange::end() const
        -> const IncidentSurfaceRange&
    {
        return *this;
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
          section_( section )
    {
        internal::next_filtered_internal_iterator< Line2D >( *this );
    }

    Section::InternalLineRange::InternalLineRange(
        const InternalLineRange& range )
        : Relationships::InternalRangeIterator{ range },
          section_( range.section_ )
    {
    }

    Section::InternalLineRange::~InternalLineRange() {}

    auto Section::InternalLineRange::begin() const -> const InternalLineRange&
    {
        return *this;
    }

    auto Section::InternalLineRange::end() const -> const InternalLineRange&
    {
        return *this;
    }

    void Section::InternalLineRange::operator++()
    {
        Relationships::InternalRangeIterator::operator++();
        internal::next_filtered_internal_iterator< Line2D >( *this );
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
          section_( section )
    {
        internal::next_filtered_internal_iterator< Corner2D >( *this );
    }

    Section::InternalCornerRange::InternalCornerRange(
        const InternalCornerRange& range )
        : Relationships::InternalRangeIterator{ range },
          section_( range.section_ )
    {
    }

    Section::InternalCornerRange::~InternalCornerRange() {}

    auto Section::InternalCornerRange::begin() const
        -> const InternalCornerRange&
    {
        return *this;
    }

    auto Section::InternalCornerRange::end() const -> const InternalCornerRange&
    {
        return *this;
    }

    void Section::InternalCornerRange::operator++()
    {
        Relationships::InternalRangeIterator::operator++();
        internal::next_filtered_internal_iterator< Corner2D >( *this );
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
          section_( section )
    {
        internal::next_filtered_embedding_iterator< Surface2D >( *this );
    }

    Section::EmbeddingSurfaceRange::EmbeddingSurfaceRange(
        const Section& section, const Corner2D& corner )
        : Relationships::EmbeddingRangeIterator( section, corner.id() ),
          section_( section )
    {
        internal::next_filtered_embedding_iterator< Surface2D >( *this );
    }

    Section::EmbeddingSurfaceRange::EmbeddingSurfaceRange(
        const EmbeddingSurfaceRange& range )
        : Relationships::EmbeddingRangeIterator{ range },
          section_( range.section_ )
    {
    }

    Section::EmbeddingSurfaceRange::~EmbeddingSurfaceRange() {}

    auto Section::EmbeddingSurfaceRange::begin() const
        -> const EmbeddingSurfaceRange&
    {
        return *this;
    }

    auto Section::EmbeddingSurfaceRange::end() const
        -> const EmbeddingSurfaceRange&
    {
        return *this;
    }

    void Section::EmbeddingSurfaceRange::operator++()
    {
        Relationships::EmbeddingRangeIterator::operator++();
        internal::next_filtered_embedding_iterator< Surface2D >( *this );
    }

    const Surface2D& Section::EmbeddingSurfaceRange::operator*() const
    {
        return section_.surface(
            Relationships::EmbeddingRangeIterator::operator*().id() );
    }

    Section::ItemCornerRange::ItemCornerRange(
        const Section& section, const CornerCollection2D& collection )
        : Relationships::ItemRangeIterator( section, collection.id() ),
          section_( section )
    {
    }

    Section::ItemCornerRange::ItemCornerRange( const ItemCornerRange& range )
        : Relationships::ItemRangeIterator{ range }, section_( range.section_ )
    {
    }

    Section::ItemCornerRange::~ItemCornerRange() = default;

    auto Section::ItemCornerRange::begin() const -> const ItemCornerRange&
    {
        return *this;
    }

    auto Section::ItemCornerRange::end() const -> const ItemCornerRange&
    {
        return *this;
    }

    const Corner2D& Section::ItemCornerRange::operator*() const
    {
        return section_.corner(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    Section::ItemLineRange::ItemLineRange(
        const Section& section, const ModelBoundary2D& boundary )
        : Relationships::ItemRangeIterator( section, boundary.id() ),
          section_( section )
    {
    }

    Section::ItemLineRange::ItemLineRange(
        const Section& section, const LineCollection2D& collection )
        : Relationships::ItemRangeIterator( section, collection.id() ),
          section_( section )
    {
    }

    Section::ItemLineRange::ItemLineRange( const ItemLineRange& range )
        : Relationships::ItemRangeIterator{ range }, section_( range.section_ )
    {
    }

    Section::ItemLineRange::~ItemLineRange() {}

    auto Section::ItemLineRange::begin() const -> const ItemLineRange&
    {
        return *this;
    }

    auto Section::ItemLineRange::end() const -> const ItemLineRange&
    {
        return *this;
    }

    const Line2D& Section::ItemLineRange::operator*() const
    {
        return section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    Section::ItemSurfaceRange::ItemSurfaceRange(
        const Section& section, const SurfaceCollection2D& collection )
        : Relationships::ItemRangeIterator( section, collection.id() ),
          section_( section )
    {
    }

    Section::ItemSurfaceRange::ItemSurfaceRange( const ItemSurfaceRange& range )
        : Relationships::ItemRangeIterator{ range }, section_( range.section_ )
    {
    }

    Section::ItemSurfaceRange::~ItemSurfaceRange() = default;

    auto Section::ItemSurfaceRange::begin() const -> const ItemSurfaceRange&
    {
        return *this;
    }

    auto Section::ItemSurfaceRange::end() const -> const ItemSurfaceRange&
    {
        return *this;
    }

    const Surface2D& Section::ItemSurfaceRange::operator*() const
    {
        return section_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    Section::Section() = default;

    Section::Section( Section&& section ) noexcept
        : Topology{ std::move( section ) },
          Corners2D{ std::move( section ) },
          Lines2D{ std::move( section ) },
          Surfaces2D{ std::move( section ) },
          ModelBoundaries2D{ std::move( section ) },
          CornerCollections2D{ std::move( section ) },
          LineCollections2D{ std::move( section ) },
          SurfaceCollections2D{ std::move( section ) },
          Identifier{ std::move( section ) }
    {
    }

    Section& Section::operator=( Section&& section )
    {
        Topology::operator=( std::move( section ) );
        Corners2D::operator=( std::move( section ) );
        Lines2D::operator=( std::move( section ) );
        Surfaces2D::operator=( std::move( section ) );
        ModelBoundaries2D::operator=( std::move( section ) );
        CornerCollections2D::operator=( std::move( section ) );
        LineCollections2D::operator=( std::move( section ) );
        SurfaceCollections2D::operator=( std::move( section ) );
        Identifier::operator=( std::move( section ) );
        return *this;
    }

    Section::~Section() = default;

    Section Section::clone() const
    {
        Section model_clone;
        SectionBuilder clone_builder{ model_clone };
        clone_builder.copy_identifier( *this );
        auto mappings = detail::section_clone_mapping( *this );
        clone_builder.copy_components( mappings, *this );
        clone_builder.copy_relationships( mappings, *this );
        clone_builder.copy_component_geometry( mappings, *this );
        return model_clone;
    }

    Section::ItemLineRange Section::model_boundary_items(
        const ModelBoundary2D& boundary ) const
    {
        return { *this, boundary };
    }

    Section::ItemCornerRange Section::corner_collection_items(
        const CornerCollection2D& collection ) const
    {
        return { *this, collection };
    }

    Section::ItemLineRange Section::line_collection_items(
        const LineCollection2D& collection ) const
    {
        return { *this, collection };
    }

    Section::ItemSurfaceRange Section::surface_collection_items(
        const SurfaceCollection2D& collection ) const
    {
        return { *this, collection };
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

    bool Section::is_corner_collection_item(
        const Corner2D& corner, const CornerCollection2D& collection ) const
    {
        return Relationships::is_item( corner.id(), collection.id() );
    }

    bool Section::is_line_collection_item(
        const Line2D& line, const LineCollection2D& collection ) const
    {
        return Relationships::is_item( line.id(), collection.id() );
    }

    bool Section::is_surface_collection_item(
        const Surface2D& surface, const SurfaceCollection2D& collection ) const
    {
        return Relationships::is_item( surface.id(), collection.id() );
    }

    BoundingBox2D Section::bounding_box() const
    {
        if( nb_lines() > 0 )
        {
            return internal::meshes_bounding_box< 2 >( lines() );
        }
        if( nb_surfaces() > 0 )
        {
            return internal::meshes_bounding_box< 2 >( surfaces() );
        }
        return internal::meshes_bounding_box< 2 >( corners() );
    }
} // namespace geode
