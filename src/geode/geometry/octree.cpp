/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/geometry/octree.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

#include <absl/container/flat_hash_map.h>

#include <bitsery/brief_syntax/array.h>
#include <bitsery/brief_syntax/vector.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

namespace
{
    template < geode::index_t dimension >
    constexpr geode::local_index_t lattice_bits()
    {
        // 64-bit locational code = 1 sentinel bit + dimension * bits:
        // 21 bits in 3D (64 bits), 31 bits in 2D (63 bits, 32 would need 65).
        return dimension == 3 ? 21 : 31;
    }

    template < geode::index_t dimension >
    struct OctreeNode
    {
        geode::local_index_t level{ 0 };
        std::array< uint32_t, dimension > anchor{};
        geode::index_t parent{ geode::NO_ID };
        geode::index_t first_child{ geode::NO_ID };
    };
} // namespace

namespace geode
{
    template < index_t dimension >
    class Octree< dimension >::Impl
    {
        using Node = OctreeNode< dimension >;

    public:
        Impl() = default;

        explicit Impl( const BoundingBox< dimension >& bbox )
            : origin_{ bbox.min() }
        {
            for( const auto d : LRange{ dimension } )
            {
                domain_max_[d] = bbox.max().value( d );
            }
            std::array< double, dimension > sides;
            for( const auto d : LRange{ dimension } )
            {
                sides[d] = bbox.max().value( d ) - bbox.min().value( d );
                OpenGeodeGeometryException::check_exception(
                    sides[d] > GLOBAL_EPSILON, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[Octree] Cannot build an octree on a degenerate "
                    "bounding box (side ",
                    d, " length is ", sides[d], ")" );
            }
            const auto smallest_side =
                *std::min_element( sides.begin(), sides.end() );
            uint64_t nb_roots{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                const auto nb =
                    std::max( 1., std::round( sides[d] / smallest_side ) );
                OpenGeodeGeometryException::check_exception(
                    nb < static_cast< double >( NO_ID ), nullptr,
                    OpenGeodeException::TYPE::data,
                    "[Octree] Bounding box is too elongated" );
                nb_root_cells_[d] = static_cast< index_t >( nb );
                root_cell_length_[d] = sides[d] / nb;
                nb_roots *= nb_root_cells_[d];
                OpenGeodeGeometryException::check_exception( nb_roots < NO_ID,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[Octree] Bounding box is too elongated" );
            }
            initialize_depth();
            create_roots( static_cast< index_t >( nb_roots ) );
        }

        local_index_t max_depth() const
        {
            return max_depth_;
        }

        index_t nb_cells() const
        {
            return static_cast< index_t >( nodes_.size() );
        }

        const std::array< index_t, dimension >& nb_root_cells() const
        {
            return nb_root_cells_;
        }

        const Node& node( index_t cell ) const
        {
            OpenGeodeGeometryException::check_assertion( cell < nb_cells(),
                "[Octree] Invalid cell index ", cell, " (nb cells is ",
                nb_cells(), ")" );
            return nodes_[cell];
        }

        uint32_t cell_size( local_index_t level ) const
        {
            return uint32_t{ 1 } << ( max_depth_ - level );
        }

        /*!
         * The domain upper border is snapped to the input bounding box max:
         * origin + nb * ( side / nb ) may differ from it by rounding.
         */
        double coordinate( local_index_t axis, uint32_t lattice_value ) const
        {
            if( lattice_value == lattice_extent( axis ) )
            {
                return domain_max_[axis];
            }
            return origin_.value( axis )
                   + std::ldexp( static_cast< double >( lattice_value ),
                         -static_cast< int >( max_depth_ ) )
                         * root_cell_length_[axis];
        }

        uint32_t lattice_extent( local_index_t axis ) const
        {
            return nb_root_cells_[axis] << max_depth_;
        }

        BoundingBox< dimension > cell_bounding_box( index_t cell ) const
        {
            const auto& cell_node = node( cell );
            const auto size = cell_size( cell_node.level );
            Point< dimension > min;
            Point< dimension > max;
            for( const auto d : LRange{ dimension } )
            {
                min.set_value( d, coordinate( d, cell_node.anchor[d] ) );
                max.set_value( d, coordinate( d, cell_node.anchor[d] + size ) );
            }
            return { std::move( min ), std::move( max ) };
        }

        std::array< double, dimension > cell_lengths( index_t cell ) const
        {
            const auto cell_level = node( cell ).level;
            std::array< double, dimension > lengths;
            for( const auto d : LRange{ dimension } )
            {
                lengths[d] = std::ldexp(
                    root_cell_length_[d], -static_cast< int >( cell_level ) );
            }
            return lengths;
        }

        std::optional< index_t > cell( local_index_t level,
            const std::array< uint32_t, dimension >& anchor ) const
        {
            if( nodes_.empty() || level > max_depth_ )
            {
                return std::nullopt;
            }
            const auto size = cell_size( level );
            for( const auto d : LRange{ dimension } )
            {
                if( anchor[d] % size != 0 || anchor[d] >= lattice_extent( d ) )
                {
                    return std::nullopt;
                }
            }
            const auto it = lookup_.find( key( level, anchor ) );
            if( it == lookup_.end() )
            {
                return std::nullopt;
            }
            return it->second;
        }

        std::vector< index_t > leaves() const
        {
            std::vector< index_t > result;
            for( const auto cell : Range{ nb_cells() } )
            {
                if( nodes_[cell].first_child == NO_ID )
                {
                    result.push_back( cell );
                }
            }
            return result;
        }

        BoundingBox< dimension > domain_bounding_box() const
        {
            if( nodes_.empty() )
            {
                return {};
            }
            Point< dimension > max;
            for( const auto d : LRange{ dimension } )
            {
                max.set_value( d, coordinate( d, lattice_extent( d ) ) );
            }
            return { origin_, std::move( max ) };
        }

        std::optional< index_t > leaf_containing(
            const Point< dimension >& point ) const
        {
            if( nodes_.empty() || !domain_bounding_box().contains( point ) )
            {
                return std::nullopt;
            }
            index_t cell{ 0 };
            index_t stride{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                cell += root_index( d, point.value( d ) ) * stride;
                stride *= nb_root_cells_[d];
            }
            while( nodes_[cell].first_child != NO_ID )
            {
                const auto& cell_node = nodes_[cell];
                const auto half_size = cell_size( cell_node.level ) / 2;
                index_t local_child{ 0 };
                for( const auto d : LRange{ dimension } )
                {
                    if( point.value( d )
                        >= coordinate( d, cell_node.anchor[d] + half_size ) )
                    {
                        local_child |= index_t{ 1 } << d;
                    }
                }
                cell = cell_node.first_child + local_child;
            }
            return cell;
        }

        index_t refine( index_t leaf )
        {
            const auto parent_node = node( leaf );
            OpenGeodeGeometryException::check_exception(
                parent_node.first_child == NO_ID, nullptr,
                OpenGeodeException::TYPE::data, "[Octree::refine] Cell ", leaf,
                " is not a leaf" );
            OpenGeodeGeometryException::check_exception(
                parent_node.level < max_depth_, nullptr,
                OpenGeodeException::TYPE::data, "[Octree::refine] Cell ", leaf,
                " is already at max depth (", max_depth_, ")" );
            OpenGeodeGeometryException::check_exception(
                nodes_.size() + NB_CHILDREN < NO_ID, nullptr,
                OpenGeodeException::TYPE::data,
                "[Octree::refine] Too many cells" );
            const auto first_child = nb_cells();
            const auto child_level =
                static_cast< local_index_t >( parent_node.level + 1 );
            const auto half_size = cell_size( child_level );
            for( const auto c : Range{ NB_CHILDREN } )
            {
                Node child;
                child.level = child_level;
                child.parent = leaf;
                for( const auto d : LRange{ dimension } )
                {
                    child.anchor[d] = parent_node.anchor[d];
                    if( ( ( c >> d ) & 1 ) != 0 )
                    {
                        child.anchor[d] += half_size;
                    }
                }
                lookup_.emplace(
                    key( child.level, child.anchor ), first_child + c );
                nodes_.push_back( std::move( child ) );
            }
            nodes_[leaf].first_child = first_child;
            cell_attribute_manager_.resize( nb_cells() );
            for( const auto c : Range{ NB_CHILDREN } )
            {
                cell_attribute_manager_.copy_attribute_value(
                    leaf, first_child + c );
            }
            return first_child;
        }

        AttributeManager& cell_attribute_manager() const
        {
            return cell_attribute_manager_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                        archive.object( impl.origin_ );
                        archive.container8b( impl.domain_max_ );
                        archive.container8b( impl.root_cell_length_ );
                        archive.container4b( impl.nb_root_cells_ );
                        archive.container( impl.nodes_, impl.nodes_.max_size(),
                            []( Archive& archive2, Node& cell_node ) {
                                archive2.value1b( cell_node.level );
                                archive2.container4b( cell_node.anchor );
                                archive2.value4b( cell_node.parent );
                                archive2.value4b( cell_node.first_child );
                            } );
                        archive.object( impl.cell_attribute_manager_ );
                        if constexpr( std::is_same_v< Archive, Deserializer > )
                        {
                            impl.initialize_depth();
                            impl.build_lookup();
                        }
                        OpenGeodeGeometryException::check_exception(
                            impl.cell_attribute_manager_.nb_elements()
                                == impl.nb_cells(),
                            nullptr, OpenGeodeException::TYPE::data,
                            "[Octree] Wrong attribute manager size after "
                            "deserialization: file may be corrupted" );
                    } } } );
        }

        void initialize_depth()
        {
            const auto max_nb_roots = *std::max_element(
                nb_root_cells_.begin(), nb_root_cells_.end() );
            local_index_t root_bits{ 0 };
            while( ( uint64_t{ 1 } << root_bits ) < max_nb_roots )
            {
                root_bits++;
            }
            constexpr auto bits = lattice_bits< dimension >();
            OpenGeodeGeometryException::check_exception( root_bits < bits,
                nullptr, OpenGeodeException::TYPE::data,
                "[Octree] Bounding box is too elongated: ", max_nb_roots,
                " root cells along an axis leave no refinement level" );
            root_bits_ = root_bits;
            max_depth_ = static_cast< local_index_t >( bits - root_bits );
        }

        void create_roots( index_t nb_roots )
        {
            nodes_.reserve( nb_roots );
            for( const auto root : Range{ nb_roots } )
            {
                Node root_node;
                auto remainder = root;
                for( const auto d : LRange{ dimension } )
                {
                    root_node.anchor[d] = ( remainder % nb_root_cells_[d] )
                                          << max_depth_;
                    remainder /= nb_root_cells_[d];
                }
                nodes_.push_back( std::move( root_node ) );
            }
            build_lookup();
            cell_attribute_manager_.resize( nb_cells() );
        }

        void build_lookup()
        {
            lookup_.clear();
            lookup_.reserve( nodes_.size() );
            for( const auto cell : Range{ nb_cells() } )
            {
                const auto& cell_node = nodes_[cell];
                OpenGeodeGeometryException::check_exception(
                    cell_node.level <= max_depth_, nullptr,
                    OpenGeodeException::TYPE::data, "[Octree] Cell ", cell,
                    " has an invalid level: file may be corrupted" );
                lookup_.emplace(
                    key( cell_node.level, cell_node.anchor ), cell );
            }
        }

        /*!
         * Locational code: a sentinel bit followed by the bit-interleaved
         * cell coordinates at its level (root_bits + level bits per axis).
         */
        uint64_t key( local_index_t level,
            const std::array< uint32_t, dimension >& anchor ) const
        {
            const auto shift = max_depth_ - level;
            const auto nb_bits = root_bits_ + level;
            uint64_t code{ 1 };
            for( auto bit = nb_bits; bit-- > 0; )
            {
                for( const auto d : LRange{ dimension } )
                {
                    code = ( code << 1 )
                           | ( ( anchor[d] >> ( shift + bit ) ) & 1u );
                }
            }
            return code;
        }

        index_t root_index( local_index_t axis, double value ) const
        {
            const auto nb = nb_root_cells_[axis];
            const auto guess = std::floor(
                ( value - origin_.value( axis ) ) / root_cell_length_[axis] );
            auto index = static_cast< index_t >(
                std::clamp( guess, 0., static_cast< double >( nb - 1 ) ) );
            while(
                index > 0 && value < coordinate( axis, index << max_depth_ ) )
            {
                index--;
            }
            while( index + 1 < nb
                   && value >= coordinate( axis, ( index + 1 ) << max_depth_ ) )
            {
                index++;
            }
            return index;
        }

    private:
        Point< dimension > origin_;
        std::array< double, dimension > domain_max_{};
        std::array< double, dimension > root_cell_length_{};
        std::array< index_t, dimension > nb_root_cells_{};
        local_index_t root_bits_{ 0 };
        local_index_t max_depth_{ 0 };
        std::vector< Node > nodes_;
        absl::flat_hash_map< uint64_t, index_t > lookup_;
        mutable AttributeManager cell_attribute_manager_;
    };

    template < index_t dimension >
    Octree< dimension >::Octree() = default;

    template < index_t dimension >
    Octree< dimension >::Octree( const BoundingBox< dimension >& bbox )
        : impl_{ bbox }
    {
    }

    template < index_t dimension >
    Octree< dimension >::Octree( Octree&& ) noexcept = default;

    template < index_t dimension >
    Octree< dimension >::~Octree() = default;

    template < index_t dimension >
    Octree< dimension >& Octree< dimension >::operator=(
        Octree&& ) noexcept = default;

    template < index_t dimension >
    local_index_t Octree< dimension >::max_depth() const
    {
        return impl_->max_depth();
    }

    template < index_t dimension >
    index_t Octree< dimension >::nb_cells() const
    {
        return impl_->nb_cells();
    }

    template < index_t dimension >
    const std::array< index_t, dimension >&
        Octree< dimension >::nb_root_cells() const
    {
        return impl_->nb_root_cells();
    }

    template < index_t dimension >
    bool Octree< dimension >::is_leaf( index_t cell ) const
    {
        return impl_->node( cell ).first_child == NO_ID;
    }

    template < index_t dimension >
    local_index_t Octree< dimension >::level( index_t cell ) const
    {
        return impl_->node( cell ).level;
    }

    template < index_t dimension >
    std::optional< index_t > Octree< dimension >::parent( index_t cell ) const
    {
        const auto parent_cell = impl_->node( cell ).parent;
        if( parent_cell == NO_ID )
        {
            return std::nullopt;
        }
        return parent_cell;
    }

    template < index_t dimension >
    std::optional< index_t > Octree< dimension >::children( index_t cell ) const
    {
        const auto first_child = impl_->node( cell ).first_child;
        if( first_child == NO_ID )
        {
            return std::nullopt;
        }
        return first_child;
    }

    template < index_t dimension >
    BoundingBox< dimension > Octree< dimension >::cell_bounding_box(
        index_t cell ) const
    {
        return impl_->cell_bounding_box( cell );
    }

    template < index_t dimension >
    Point< dimension > Octree< dimension >::cell_center( index_t cell ) const
    {
        return cell_bounding_box( cell ).center();
    }

    template < index_t dimension >
    std::array< double, dimension > Octree< dimension >::cell_lengths(
        index_t cell ) const
    {
        return impl_->cell_lengths( cell );
    }

    template < index_t dimension >
    const std::array< uint32_t, dimension >& Octree< dimension >::cell_anchor(
        index_t cell ) const
    {
        return impl_->node( cell ).anchor;
    }

    template < index_t dimension >
    std::optional< index_t > Octree< dimension >::cell( local_index_t level,
        const std::array< uint32_t, dimension >& anchor ) const
    {
        return impl_->cell( level, anchor );
    }

    template < index_t dimension >
    std::vector< index_t > Octree< dimension >::leaves() const
    {
        return impl_->leaves();
    }

    template < index_t dimension >
    BoundingBox< dimension > Octree< dimension >::domain_bounding_box() const
    {
        return impl_->domain_bounding_box();
    }

    template < index_t dimension >
    std::optional< index_t > Octree< dimension >::leaf_containing(
        const Point< dimension >& point ) const
    {
        return impl_->leaf_containing( point );
    }

    template < index_t dimension >
    index_t Octree< dimension >::refine( index_t leaf )
    {
        return impl_->refine( leaf );
    }

    template < index_t dimension >
    AttributeManager& Octree< dimension >::cell_attribute_manager() const
    {
        return impl_->cell_attribute_manager();
    }

    template < index_t dimension >
    template < typename Archive >
    void Octree< dimension >::serialize( Archive& serializer )
    {
        serializer.ext( *this, Growable< Archive, Octree >{
                                   { []( Archive& archive, Octree& octree ) {
                                       archive.object( octree.impl_ );
                                   } } } );
    }

    template class opengeode_geometry_api Octree< 2 >;
    template class opengeode_geometry_api Octree< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Octree< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Octree< 3 > );
} // namespace geode
