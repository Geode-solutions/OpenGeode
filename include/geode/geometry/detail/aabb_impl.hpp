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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#pragma once

#include <algorithm>
#include <cmath>

#include <absl/algorithm/container.h>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>

namespace geode
{
    /*!
     * AABB tree structure implementation
     * The tree is stored in a single vector, laid out in depth-first
     * pre-order: a node is always immediately followed by its whole left
     * subtree, so its left child is implicitly at (node_index + 1); only
     * the right child needs an explicit stored offset. This guarantees an
     * exact storage of 2 * nb_bboxes() - 1 nodes (no gaps, unlike a
     * power-of-two implicit heap layout) and keeps a node's left child
     * adjacent in memory, which is friendlier to the cache while
     * descending the tree.
     *                          ROOT
     *                        /      \
     *                      A1        A2
     *                    /    \     /   \
     *                  B1     B2   B3    B4
     *  where B* are the input bboxes
     *  Storage: |ROOT|A1|B1|B2|A2|B3|B4|
     *
     * Each internal node splits its element range along the longest axis
     * of that range's actual bounding box, at the centroid median (rather
     * than a fixed pre-computed order such as a Morton curve). This keeps
     * sibling boxes tight and minimally overlapping, which is what lets
     * queries prune branches instead of having to visit both children.
     */
    template < index_t dimension >
    class AABBTree< dimension >::Impl
    {
    public:
        static constexpr index_t ROOT_INDEX{ 0 };

        struct Iterator
        {
            index_t element_middle;
            index_t child_left;
            index_t child_right;
        };

    private:
        struct Node
        {
            BoundingBox< dimension > box;
            // Index of the right child in tree_. Meaningless for leaves,
            // whose left child slot (node_index + 1) does not exist.
            index_t right_child{ NO_ID };
        };

    public:
        Impl() = default;

        Impl( absl::Span< const BoundingBox< dimension > > bboxes )
            : element_order_( bboxes.size() )
        {
            if( !bboxes.empty() )
            {
                absl::c_iota( element_order_, index_t{ 0 } );
                tree_.resize( 2 * bboxes.size() - 1 );
                index_t next_free_index{ 0 };
                initialize_tree_recursive(
                    bboxes, next_free_index, 0, bboxes.size() );
            }
        }

        [[nodiscard]] index_t nb_bboxes() const
        {
            return element_order_.size();
        }

        [[nodiscard]] static bool is_leaf(
            index_t element_begin, index_t element_end )
        {
            return element_begin + 1 == element_end;
        }

        [[nodiscard]] Iterator get_recursive_iterators( index_t node_index,
            index_t element_begin,
            index_t element_end ) const
        {
            Iterator it;
            it.element_middle =
                element_begin + ( element_end - element_begin ) / 2;
            it.child_left = node_index + 1;
            it.child_right = tree_[node_index].right_child;
            return it;
        }

        [[nodiscard]] const BoundingBox< dimension >& node(
            index_t index ) const
        {
            OpenGeodeGeometryException::check_assertion(
                index < tree_.size(), "query out of tree" );
            return tree_[index].box;
        }

        [[nodiscard]] index_t element_order( index_t index ) const
        {
            return element_order_[index];
        }

        index_t initialize_tree_recursive(
            absl::Span< const BoundingBox< dimension > > bboxes,
            index_t& next_free_index,
            index_t element_begin,
            index_t element_end )
        {
            OpenGeodeGeometryException::check_assertion(
                element_begin != element_end,
                "Begin and End indices should be different" );
            const auto node_index = next_free_index++;
            OpenGeodeGeometryException::check_assertion(
                node_index < tree_.size(), "Node index out of tree" );
            if( is_leaf( element_begin, element_end ) )
            {
                tree_[node_index].box = bboxes[element_order_[element_begin]];
                return node_index;
            }
            BoundingBox< dimension > range_box;
            for( const auto i : Range{ element_begin, element_end } )
            {
                range_box.add_box( bboxes[element_order_[i]] );
            }
            const auto axis = std::get< 0 >( range_box.largest_length() );
            const auto element_middle =
                element_begin + ( element_end - element_begin ) / 2;
            std::nth_element( element_order_.begin() + element_begin,
                element_order_.begin() + element_middle,
                element_order_.begin() + element_end,
                [&bboxes, axis]( index_t index1, index_t index2 ) {
                    return bboxes[index1].center().value( axis )
                           < bboxes[index2].center().value( axis );
                } );
            const auto left_index = initialize_tree_recursive(
                bboxes, next_free_index, element_begin, element_middle );
            const auto right_index = initialize_tree_recursive(
                bboxes, next_free_index, element_middle, element_end );
            tree_[node_index].right_child = right_index;
            // before box_union
            tree_[node_index].box.add_box( node( left_index ) );
            tree_[node_index].box.add_box( node( right_index ) );
            return node_index;
        }

        template < typename ACTION >
        void closest_element_box_recursive( const Point< dimension >& query,
            index_t& nearest_box,
            double& squared_distance,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            const ACTION& action ) const
        {
            OpenGeodeGeometryException::check_assertion(
                node_index < tree_.size(), "node out of tree" );
            OpenGeodeGeometryException::check_assertion(
                element_begin != element_end,
                "Begin and End indices should be different" );

            // If node is a leaf: compute point-element distance
            // and replace current if nearer
            if( is_leaf( element_begin, element_end ) )
            {
                const auto cur_box = element_order( element_begin );
                const auto cur_distance = action( query, cur_box );
                const auto cur_squared_distance = cur_distance * cur_distance;
                if( cur_squared_distance < squared_distance )
                {
                    nearest_box = cur_box;
                    squared_distance = cur_squared_distance;
                }
                return;
            }
            const auto it = get_recursive_iterators(
                node_index, element_begin, element_end );
            const auto squared_distance_left =
                node( it.child_left ).squared_signed_distance( query );
            const auto squared_distance_right =
                node( it.child_right ).squared_signed_distance( query );

            // Traverse the "nearest" child first, so that it has more chances
            // to prune the traversal of the other child.
            if( squared_distance_left < squared_distance_right )
            {
                if( squared_distance_left < squared_distance )
                {
                    closest_element_box_recursive( query, nearest_box,
                        squared_distance, it.child_left, element_begin,
                        it.element_middle, action );
                }
                if( squared_distance_right < squared_distance )
                {
                    closest_element_box_recursive( query, nearest_box,
                        squared_distance, it.child_right, it.element_middle,
                        element_end, action );
                }
            }
            else
            {
                if( squared_distance_right < squared_distance )
                {
                    closest_element_box_recursive( query, nearest_box,
                        squared_distance, it.child_right, it.element_middle,
                        element_end, action );
                }
                if( squared_distance_left < squared_distance )
                {
                    closest_element_box_recursive( query, nearest_box,
                        squared_distance, it.child_left, element_begin,
                        it.element_middle, action );
                }
            }
        }

        template < typename ACTION >
        bool self_intersect_recursive( index_t node_index1,
            index_t element_begin1,
            index_t element_end1,
            index_t node_index2,
            index_t element_begin2,
            index_t element_end2,
            ACTION& action ) const
        {
            OpenGeodeGeometryException::check_assertion(
                element_end1 != element_begin1,
                "No iteration allowed start == end" );
            OpenGeodeGeometryException::check_assertion(
                element_end2 != element_begin2,
                "No iteration allowed start == end" );

            // Since we are intersecting the AABBTree with *itself*,
            // we can prune half of the cases by skipping the test
            // whenever node2's polygon index interval is greated than
            // node1's polygon index interval.
            if( element_end2 <= element_begin1 )
            {
                return false;
            }

            // The acceleration is here:
            if( !node( node_index1 ).epsilon_intersects( node( node_index2 ) ) )
            {
                return false;
            }

            // Simple case: leaf - leaf intersection.
            if( is_leaf( element_begin1, element_end1 )
                && is_leaf( element_begin2, element_end2 ) )
            {
                if( node_index1 == node_index2 )
                {
                    return false;
                }
                return action( element_order( element_begin1 ),
                    element_order( element_begin2 ) );
            }

            // If node2 has more polygons than node1, then
            //   intersect node2's two children with node1
            // else
            //   intersect node1's two children with node2
            if( element_end2 - element_begin2 > element_end1 - element_begin1 )
            {
                const auto it = get_recursive_iterators(
                    node_index2, element_begin2, element_end2 );
                if( self_intersect_recursive( node_index1, element_begin1,
                        element_end1, it.child_left, element_begin2,
                        it.element_middle, action ) )
                {
                    return true;
                }
                return self_intersect_recursive( node_index1, element_begin1,
                    element_end1, it.child_right, it.element_middle,
                    element_end2, action );
            }
            const auto it = get_recursive_iterators(
                node_index1, element_begin1, element_end1 );
            if( self_intersect_recursive( it.child_left, element_begin1,
                    it.element_middle, node_index2, element_begin2,
                    element_end2, action ) )
            {
                return true;
            }
            return self_intersect_recursive( it.child_right, it.element_middle,
                element_end1, node_index2, element_begin2, element_end2,
                action );
        }

        template < typename ACTION >
        bool other_intersect_recursive( index_t node_index1,
            index_t element_begin1,
            index_t element_end1,
            const AABBTree< dimension >& other_tree,
            index_t node_index2,
            index_t element_begin2,
            index_t element_end2,
            ACTION& action ) const
        {
            OpenGeodeGeometryException::check_assertion(
                element_end1 != element_begin1,
                "No iteration allowed start == end" );
            OpenGeodeGeometryException::check_assertion(
                element_end2 != element_begin2,
                "No iteration allowed start == end" );

            // The acceleration is here:
            if( !node( node_index1 )
                    .epsilon_intersects(
                        other_tree.impl_->node( node_index2 ) ) )
            {
                return false;
            }

            // Simple case: leaf - leaf intersection.
            if( is_leaf( element_begin1, element_end1 )
                && is_leaf( element_begin2, element_end2 ) )
            {
                return action( element_order( element_begin1 ),
                    other_tree.impl_->element_order( element_begin2 ) );
            }

            // If node2 has more polygons than node1, then
            //   intersect node2's two children with node1
            // else
            //   intersect node1's two children with node2
            if( element_end2 - element_begin2 > element_end1 - element_begin1 )
            {
                const auto it = other_tree.impl_->get_recursive_iterators(
                    node_index2, element_begin2, element_end2 );
                if( other_intersect_recursive( node_index1, element_begin1,
                        element_end1, other_tree, it.child_left, element_begin2,
                        it.element_middle, action ) )
                {
                    return true;
                }
                return other_intersect_recursive( node_index1, element_begin1,
                    element_end1, other_tree, it.child_right, it.element_middle,
                    element_end2, action );
            }
            const auto it = get_recursive_iterators(
                node_index1, element_begin1, element_end1 );
            if( other_intersect_recursive( it.child_left, element_begin1,
                    it.element_middle, other_tree, node_index2, element_begin2,
                    element_end2, action ) )
            {
                return true;
            }
            return other_intersect_recursive( it.child_right, it.element_middle,
                element_end1, other_tree, node_index2, element_begin2,
                element_end2, action );
        }

        template < typename BOX_FILTER, typename ACTION >
        bool generic_intersect_recursive( const BOX_FILTER& box_filter,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            ACTION& action ) const
        {
            OpenGeodeGeometryException::check_assertion(
                node_index < tree_.size(), "Node out of tree range" );
            OpenGeodeGeometryException::check_assertion(
                element_begin != element_end,
                "No iteration allowed start == end" );

            // Prune sub-tree that does not have intersection
            if( !box_filter( node( node_index ) ) )
            {
                return false;
            }

            if( is_leaf( element_begin, element_end ) )
            {
                return action( element_order( element_begin ) );
            }

            const auto it = get_recursive_iterators(
                node_index, element_begin, element_end );
            if( generic_intersect_recursive( box_filter, it.child_left,
                    element_begin, it.element_middle, action ) )
            {
                return true;
            }
            return generic_intersect_recursive( box_filter, it.child_right,
                it.element_middle, element_end, action );
        }

        [[nodiscard]] index_t closest_element_box_hint(
            const Point< dimension >& query ) const
        {
            index_t element_begin{ 0 };
            index_t element_end{ nb_bboxes() };
            index_t node_index{ Impl::ROOT_INDEX };
            while( !is_leaf( element_begin, element_end ) )
            {
                const auto it = get_recursive_iterators(
                    node_index, element_begin, element_end );
                if( node( it.child_left ).squared_signed_distance( query )
                    < node( it.child_right ).squared_signed_distance( query ) )
                {
                    element_end = it.element_middle;
                    node_index = it.child_left;
                }
                else
                {
                    element_begin = it.element_middle;
                    node_index = it.child_right;
                }
            }

            return element_order( element_begin );
        }

        void containing_boxes_recursive( index_t node_index,
            index_t element_begin,
            index_t element_end,
            const Point< dimension >& query,
            std::vector< index_t >& result ) const
        {
            OpenGeodeGeometryException::check_assertion(
                node_index < tree_.size(), "Node index out of tree" );
            OpenGeodeGeometryException::check_assertion(
                element_begin != element_end,
                "Begin and End indices should be different" );
            if( !node( node_index ).epsilon_contains( query ) )
            {
                return;
            }
            if( is_leaf( element_begin, element_end ) )
            {
                result.push_back( element_order( element_begin ) );
                return;
            }
            const auto it = get_recursive_iterators(
                node_index, element_begin, element_end );
            containing_boxes_recursive( it.child_left, element_begin,
                it.element_middle, query, result );
            containing_boxes_recursive(
                it.child_right, it.element_middle, element_end, query, result );
        }

    private:
        std::vector< Node > tree_;
        std::vector< index_t > element_order_;
    };

    template < index_t dimension >
    template < typename EvalDistance >
    std::tuple< index_t, double > AABBTree< dimension >::closest_element_box(
        const Point< dimension >& query, const EvalDistance& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return { NO_ID, 0 };
        }
        auto nearest_box = impl_->closest_element_box_hint( query );
        auto distance = action( query, nearest_box );
        distance *= distance;
        impl_->closest_element_box_recursive( query, nearest_box, distance,
            Impl::ROOT_INDEX, 0, nb_bboxes(), action );
        OpenGeodeGeometryException::check_assertion(
            nearest_box != NO_ID, "No box found" );
        return { nearest_box, std::sqrt( distance ) };
    }

    template < index_t dimension >
    template < typename EvalDistance >
    void AABBTree< dimension >::compute_point_element_box_distances(
        const Point< dimension >& query, const EvalDistance& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return;
        }
        auto nearest_box = NO_ID;
        auto distance = std::numeric_limits< double >::max();
        impl_->closest_element_box_recursive( query, nearest_box, distance,
            Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_bbox_element_bbox_intersections(
        const BoundingBox< dimension >& box, EvalIntersection& action ) const
    {
        const auto box_filter = [&box]( const auto& inner_box ) {
            return inner_box.epsilon_intersects( box );
        };
        compute_generic_element_bbox_intersections( box_filter, action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_self_element_bbox_intersections(
        EvalIntersection& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return;
        }
        impl_->self_intersect_recursive( Impl::ROOT_INDEX, 0, nb_bboxes(),
            Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_other_element_bbox_intersections(
        const AABBTree< dimension >& other_tree,
        EvalIntersection& action ) const
    {
        if( nb_bboxes() == 0 || other_tree.nb_bboxes() == 0 )
        {
            return;
        }
        impl_->other_intersect_recursive( Impl::ROOT_INDEX, 0, nb_bboxes(),
            other_tree, Impl::ROOT_INDEX, 0, other_tree.nb_bboxes(), action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_ray_element_bbox_intersections(
        const Ray< dimension >& ray, EvalIntersection& action ) const
    {
        const auto box_filter = [&ray]( const auto& box ) {
            return box.epsilon_intersects( ray );
        };
        compute_generic_element_bbox_intersections( box_filter, action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_line_element_bbox_intersections(
        const InfiniteLine< dimension >& line, EvalIntersection& action ) const
    {
        const auto box_filter = [&line]( const auto& box ) {
            return box.epsilon_intersects( line );
        };
        compute_generic_element_bbox_intersections( box_filter, action );
    }

    template < index_t dimension >
    template < class EvalBox, class EvalIntersection >
    void AABBTree< dimension >::compute_generic_element_bbox_intersections(
        const EvalBox& box_filter, EvalIntersection& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return;
        }
        impl_->generic_intersect_recursive(
            box_filter, Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_triangle_element_bbox_intersections(
        const Triangle< dimension >& triangle, EvalIntersection& action ) const
    {
        const auto box_filter = [&triangle]( const auto& box ) {
            return box.epsilon_intersects( triangle );
        };
        compute_generic_element_bbox_intersections( box_filter, action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_segment_element_bbox_intersections(
        const Segment< dimension >& segment, EvalIntersection& action ) const
    {
        const auto box_filter = [&segment]( const auto& box ) {
            return box.epsilon_intersects( segment );
        };
        compute_generic_element_bbox_intersections( box_filter, action );
    }
} // namespace geode
