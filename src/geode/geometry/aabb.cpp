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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/geometry/aabb.h>

#include <async++.h>

#include <geode/geometry/morton.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <algorithm>
#include <numeric>

namespace
{
    template < geode::index_t dimension >
    double inner_point_box_distance( const geode::Point< dimension >& point,
        const geode::BoundingBox< dimension >& box )
    {
        OPENGEODE_ASSERT( box.contains( point ), "point out of the box" );
        const auto Pmin = point - box.min();
        const auto Pmax = point - box.max();
        auto result = std::numeric_limits< double >::max();
        for( const auto c : geode::LRange{ dimension } )
        {
            const auto local_result = std::min(
                std::abs( Pmin.value( c ) ), std::abs( Pmax.value( c ) ) );
            result = std::min( result, local_result );
        }
        return result;
    }

    template < geode::index_t dimension >
    std::vector< geode::index_t > sort(
        absl::Span< const geode::BoundingBox< dimension > > bboxes )
    {
        absl::FixedArray< geode::Point< dimension > > points( bboxes.size() );
        async::parallel_for( async::irange( size_t{ 0 }, bboxes.size() ),
            [&bboxes, &points](
                size_t i ) { points[i] = bboxes[i].min() + bboxes[i].max(); } );
        return geode::morton_sort< dimension >( points );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension >::Impl::Impl(
        absl::Span< const BoundingBox< dimension > > bboxes )
        : tree_( max_node_index( bboxes ) + ROOT_INDEX ),
          mapping_morton_( sort( bboxes ) )
    {
        initialize_tree( bboxes );
    }

    template < index_t dimension >
    AABBTree< dimension >::AABBTree(
        absl::Span< const BoundingBox< dimension > > bboxes )
        : impl_{ bboxes }
    {
    }

    template < index_t dimension >
    AABBTree< dimension >::~AABBTree() // NOLINT
    {
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::Impl::max_node_index(
        absl::Span< const BoundingBox< dimension > > bboxes ) const
    {
        if( bboxes.empty() )
        {
            return 0;
        }
        return max_node_index_recursive( ROOT_INDEX, 0, bboxes.size() );
    }

    template < index_t dimension >
    index_t AABBTree< dimension >::Impl::max_node_index_recursive(
        index_t node_index, index_t box_begin, index_t box_end ) const
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
    void AABBTree< dimension >::Impl::initialize_tree(
        absl::Span< const BoundingBox< dimension > > bboxes )
    {
        if( !bboxes.empty() )
        {
            initialize_tree_recursive( bboxes, ROOT_INDEX, 0, bboxes.size() );
        }
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
            set_node( node_index, bboxes[mapping_morton_[element_begin]] );
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
        add_box( node_index, node( it.child_left ) );
        add_box( node_index, node( it.child_right ) );
    }

    template < index_t dimension >
    double point_box_signed_distance(
        const Point< dimension >& point, const BoundingBox< dimension >& box )
    {
        bool inside{ true };
        Vector< dimension > result;
        for( const auto c : LRange{ dimension } )
        {
            if( point.value( c ) < box.min().value( c ) )
            {
                inside = false;
                result.set_value( c, point.value( c ) - box.min().value( c ) );
            }
            else if( point.value( c ) > box.max().value( c ) )
            {
                inside = false;
                result.set_value( c, point.value( c ) - box.max().value( c ) );
            }
        }
        if( inside )
        {
            return -inner_point_box_distance( point, box );
        }
        return result.length();
    }

    template double opengeode_geometry_api point_box_signed_distance(
        const Point2D&, const BoundingBox2D& );
    template class opengeode_geometry_api AABBTree< 2 >;

    template double opengeode_geometry_api point_box_signed_distance(
        const Point3D&, const BoundingBox3D& );
    template class opengeode_geometry_api AABBTree< 3 >;
} // namespace geode
