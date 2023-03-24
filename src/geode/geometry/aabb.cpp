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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/geometry/aabb.h>

#include <async++.h>

#include <geode/geometry/point.h>
#include <geode/geometry/points_sort.h>
#include <geode/geometry/vector.h>

#include <algorithm>
#include <numeric>

namespace
{
    template < geode::index_t dimension >
    std::vector< geode::index_t > sort(
        absl::Span< const geode::BoundingBox< dimension > > bboxes )
    {
        absl::FixedArray< geode::Point< dimension > > points( bboxes.size() );
        async::parallel_for( async::irange( size_t{ 0 }, bboxes.size() ),
            [&bboxes, &points]( size_t i ) {
                points[i] = bboxes[i].min() + bboxes[i].max();
            } );
        return geode::morton_mapping< dimension >( points );
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension >::Impl::Impl(
        absl::Span< const BoundingBox< dimension > > bboxes )
        : tree_( bboxes.empty()
                     ? ROOT_INDEX
                     : max_node_index_recursive( ROOT_INDEX, 0, bboxes.size() )
                           + ROOT_INDEX ),
          mapping_morton_( sort( bboxes ) )
    {
        if( !bboxes.empty() )
        {
            initialize_tree_recursive( bboxes, ROOT_INDEX, 0, bboxes.size() );
        }
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::Impl::max_node_index_recursive(
        index_t node_index, index_t box_begin, index_t box_end )
    {
        OPENGEODE_ASSERT( box_end > box_begin,
            "End box index should be after Begin box index" );
        if( is_leaf( box_begin, box_end ) )
        {
            return node_index;
        }
        const auto it =
            get_recursive_iterators( node_index, box_begin, box_end );
        return std::max(
            max_node_index_recursive( it.child_left, box_begin, it.middle_box ),
            max_node_index_recursive(
                it.child_right, it.middle_box, box_end ) );
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::Impl::nb_bboxes() const
    {
        return mapping_morton_.size();
    }

    template < index_t dimension >
    bool AABBTree< dimension >::Impl::is_leaf(
        index_t box_begin, index_t box_end )
    {
        return box_begin + 1 == box_end;
    }

    template < index_t dimension >
    typename AABBTree< dimension >::Impl::Iterator
        AABBTree< dimension >::Impl::get_recursive_iterators(
            index_t node_index, index_t box_begin, index_t box_end )
    {
        Iterator it;
        it.middle_box = box_begin + ( box_end - box_begin ) / 2;
        it.child_left = 2 * node_index;
        it.child_right = 2 * node_index + 1;
        return it;
    }

    template < index_t dimension >
    const BoundingBox< dimension >& AABBTree< dimension >::Impl::node(
        index_t index ) const
    {
        OPENGEODE_ASSERT( index < tree_.size(), "query out of tree" );
        return tree_[index];
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::Impl::mapping_morton( index_t index ) const
    {
        return mapping_morton_[index];
    }

    template < index_t dimension >
    AABBTree< dimension >::AABBTree()
    {
    }

    template < index_t dimension >
    AABBTree< dimension >::AABBTree(
        absl::Span< const BoundingBox< dimension > > bboxes )
        : impl_{ bboxes }
    {
    }

    template < index_t dimension >
    AABBTree< dimension >::AABBTree( AABBTree&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    AABBTree< dimension >::~AABBTree() // NOLINT
    {
    }

    template < index_t dimension >
    AABBTree< dimension >& AABBTree< dimension >::operator=( AABBTree&& other )
    {
        *impl_ = std::move( *other.impl_ );
        return *this;
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::nb_bboxes() const
    {
        return impl_->nb_bboxes();
    }

    template < index_t dimension >
    const BoundingBox< dimension >& AABBTree< dimension >::bounding_box() const
    {
        OPENGEODE_EXCEPTION( impl_->nb_bboxes() != 0,
            "[AABBTree::bounding_box] Cannot return "
            "the bounding_box of an empty AABBTree." );
        return impl_->node( Impl::ROOT_INDEX );
    }

    template < index_t dimension >
    std::vector< index_t > AABBTree< dimension >::containing_boxes(
        const Point< dimension >& query ) const
    {
        if( nb_bboxes() == 0 )
        {
            return {};
        }
        std::vector< index_t > result;
        impl_->containing_boxes_recursive(
            Impl::ROOT_INDEX, 0, nb_bboxes(), query, result );
        return result;
    }

    template < index_t dimension >
    void AABBTree< dimension >::Impl::containing_boxes_recursive(
        index_t node_index,
        index_t element_begin,
        index_t element_end,
        const Point< dimension >& query,
        std::vector< index_t >& result ) const
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "Node index out of tree" );
        OPENGEODE_ASSERT( element_begin != element_end,
            "Begin and End indices should be different" );
        if( !node( node_index ).contains( query ) )
        {
            return;
        }
        if( is_leaf( element_begin, element_end ) )
        {
            result.push_back( mapping_morton( element_begin ) );
            return;
        }
        const auto it =
            get_recursive_iterators( node_index, element_begin, element_end );
        containing_boxes_recursive(
            it.child_left, element_begin, it.middle_box, query, result );
        containing_boxes_recursive(
            it.child_right, it.middle_box, element_end, query, result );
    }

    /**
     * \brief Computes the hierarchy of bounding boxes recursively.
     * \param[in] bboxes the array of bounding boxes
     * \param[in] node_index the index of the root of the subtree
     * \param[in] element_begin first box index in the vector \p bboxes
     * \param[in] element_end one position past the last box index in the vector
     * \p
     * bboxes
     */
    template < index_t dimension >
    void AABBTree< dimension >::Impl::initialize_tree_recursive(
        absl::Span< const BoundingBox< dimension > > bboxes,
        index_t node_index,
        index_t element_begin,
        index_t element_end )
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "Node index out of tree" );
        OPENGEODE_ASSERT( element_begin != element_end,
            "Begin and End indices should be different" );
        if( is_leaf( element_begin, element_end ) )
        {
            tree_[node_index] = bboxes[mapping_morton_[element_begin]];
            return;
        }
        const auto it =
            get_recursive_iterators( node_index, element_begin, element_end );
        OPENGEODE_ASSERT(
            it.child_left < tree_.size(), "Left index out of tree" );
        OPENGEODE_ASSERT(
            it.child_right < tree_.size(), "Right index out of tree" );
        initialize_tree_recursive(
            bboxes, it.child_left, element_begin, it.middle_box );
        initialize_tree_recursive(
            bboxes, it.child_right, it.middle_box, element_end );
        // before box_union
        tree_[node_index].add_box( node( it.child_left ) );
        tree_[node_index].add_box( node( it.child_right ) );
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::Impl::closest_element_box_hint(
        const Point< dimension >& query ) const
    {
        index_t box_begin{ 0 };
        index_t box_end{ nb_bboxes() };
        index_t node_index{ Impl::ROOT_INDEX };
        while( !is_leaf( box_begin, box_end ) )
        {
            const auto it =
                get_recursive_iterators( node_index, box_begin, box_end );
            if( node( it.child_left ).signed_distance( query )
                < node( it.child_right ).signed_distance( query ) )
            {
                box_end = it.middle_box;
                node_index = it.child_left;
            }
            else
            {
                box_begin = it.middle_box;
                node_index = it.child_right;
            }
        }

        return mapping_morton( box_begin );
    }

    template class opengeode_geometry_api AABBTree< 2 >::Impl;
    template class opengeode_geometry_api AABBTree< 3 >::Impl;

    template class opengeode_geometry_api AABBTree< 2 >;
    template class opengeode_geometry_api AABBTree< 3 >;
} // namespace geode
