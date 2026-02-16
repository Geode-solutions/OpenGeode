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

#include <cmath>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/points_sort.hpp>

namespace geode
{
    /*!
     * AABB tree structure implementation
     * The tree is store in s single vector following this example:
     *                          ROOT
     *                        /      \
     *                      A1        A2
     *                    /    \     /   \
     *                  B1     B2   B3    B4
     *  where B* are the input bboxes
     *  Storage: |empty|ROOT|A1|A2|B1|B2|B3|B4|
     */
    template < index_t dimension >
    class AABBTree< dimension >::Impl
    {
    public:
        static constexpr index_t ROOT_INDEX{ 1 };

        struct Iterator
        {
            index_t element_middle;
            index_t child_left;
            index_t child_right;
        };

    public:
        Impl() = default;

        Impl( absl::Span< const BoundingBox< dimension > > bboxes )
            : mapping_morton_( [&bboxes]() {
                  absl::FixedArray< Point< dimension > > points(
                      bboxes.size() );
                  for( const auto i : Indices{ bboxes } )
                  {
                      points[i] = bboxes[i].min() + bboxes[i].max();
                  }
                  return morton_mapping< dimension >( points );
              }() )
        {
            if( bboxes.empty() )
            {
                tree_.resize( ROOT_INDEX );
            }
            else
            {
                const auto max_node_index =
                    max_node_index_recursive( ROOT_INDEX, 0, bboxes.size() );
                tree_.resize( ROOT_INDEX + max_node_index );
                initialize_tree_recursive(
                    bboxes, ROOT_INDEX, 0, bboxes.size() );
            }
        }

        [[nodiscard]] index_t nb_bboxes() const
        {
            return mapping_morton_.size();
        }

        [[nodiscard]] static bool is_leaf(
            index_t element_begin, index_t element_end )
        {
            return element_begin + 1 == element_end;
        }

        [[nodiscard]] static Iterator get_recursive_iterators(
            index_t node_index, index_t element_begin, index_t element_end )
        {
            Iterator it;
            it.element_middle =
                element_begin + ( element_end - element_begin ) / 2;
            it.child_left = 2 * node_index;
            it.child_right = 2 * node_index + 1;
            return it;
        }

        [[nodiscard]] const BoundingBox< dimension >& node(
            index_t index ) const
        {
            OPENGEODE_ASSERT( index < tree_.size(), "query out of tree" );
            return tree_[index];
        }

        [[nodiscard]] index_t mapping_morton( index_t index ) const
        {
            return mapping_morton_[index];
        }

        [[nodiscard]] static index_t max_node_index_recursive(
            index_t node_index, index_t element_begin, index_t element_end )
        {
            OPENGEODE_ASSERT( element_end > element_begin,
                "End box index should be after Begin box index" );
            if( is_leaf( element_begin, element_end ) )
            {
                return node_index;
            }
            const auto it = get_recursive_iterators(
                node_index, element_begin, element_end );
            const auto node_left = max_node_index_recursive(
                it.child_left, element_begin, it.element_middle );
            const auto node_right = max_node_index_recursive(
                it.child_right, it.element_middle, element_end );
            return std::max( node_left, node_right );
        }

        void initialize_tree_recursive(
            absl::Span< const BoundingBox< dimension > > bboxes,
            index_t node_index,
            index_t element_begin,
            index_t element_end )
        {
            OPENGEODE_ASSERT(
                node_index < tree_.size(), "Node index out of tree" );
            OPENGEODE_ASSERT( element_begin != element_end,
                "Begin and End indices should be different" );
            if( is_leaf( element_begin, element_end ) )
            {
                tree_[node_index] = bboxes[mapping_morton_[element_begin]];
                return;
            }
            const auto it = get_recursive_iterators(
                node_index, element_begin, element_end );
            OPENGEODE_ASSERT(
                it.child_left < tree_.size(), "Left index out of tree" );
            OPENGEODE_ASSERT(
                it.child_right < tree_.size(), "Right index out of tree" );
            initialize_tree_recursive(
                bboxes, it.child_left, element_begin, it.element_middle );
            initialize_tree_recursive(
                bboxes, it.child_right, it.element_middle, element_end );
            // before box_union
            tree_[node_index].add_box( node( it.child_left ) );
            tree_[node_index].add_box( node( it.child_right ) );
        }

        template < typename ACTION >
        void closest_element_box_recursive( const Point< dimension >& query,
            index_t& nearest_box,
            double& distance,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            const ACTION& action ) const
        {
            OPENGEODE_ASSERT( node_index < tree_.size(), "node out of tree" );
            OPENGEODE_ASSERT( element_begin != element_end,
                "Begin and End indices should be different" );

            // If node is a leaf: compute point-element distance
            // and replace current if nearer
            if( is_leaf( element_begin, element_end ) )
            {
                const auto cur_box = mapping_morton( element_begin );
                Point< dimension > cur_nearest_point;
                const auto cur_distance = action( query, cur_box );
                if( cur_distance < distance )
                {
                    nearest_box = cur_box;
                    distance = cur_distance;
                }
                return;
            }
            const auto it = get_recursive_iterators(
                node_index, element_begin, element_end );
            const auto distance_left =
                node( it.child_left ).signed_distance( query );
            const auto distance_right =
                node( it.child_right ).signed_distance( query );

            // Traverse the "nearest" child first, so that it has more chances
            // to prune the traversal of the other child.
            if( distance_left < distance_right )
            {
                if( distance_left < distance )
                {
                    closest_element_box_recursive( query, nearest_box, distance,
                        it.child_left, element_begin, it.element_middle,
                        action );
                }
                if( distance_right < distance )
                {
                    closest_element_box_recursive( query, nearest_box, distance,
                        it.child_right, it.element_middle, element_end,
                        action );
                }
            }
            else
            {
                if( distance_right < distance )
                {
                    closest_element_box_recursive( query, nearest_box, distance,
                        it.child_right, it.element_middle, element_end,
                        action );
                }
                if( distance_left < distance )
                {
                    closest_element_box_recursive( query, nearest_box, distance,
                        it.child_left, element_begin, it.element_middle,
                        action );
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
            OPENGEODE_ASSERT( element_end1 != element_begin1,
                "No iteration allowed start == end" );
            OPENGEODE_ASSERT( element_end2 != element_begin2,
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
                return action( mapping_morton( element_begin1 ),
                    mapping_morton( element_begin2 ) );
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
            OPENGEODE_ASSERT( element_end1 != element_begin1,
                "No iteration allowed start == end" );
            OPENGEODE_ASSERT( element_end2 != element_begin2,
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
                return action( mapping_morton( element_begin1 ),
                    other_tree.impl_->mapping_morton( element_begin2 ) );
            }

            // If node2 has more polygons than node1, then
            //   intersect node2's two children with node1
            // else
            //   intersect node1's two children with node2
            if( element_end2 - element_begin2 > element_end1 - element_begin1 )
            {
                const auto it = get_recursive_iterators(
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
            OPENGEODE_ASSERT(
                node_index < tree_.size(), "Node out of tree range" );
            OPENGEODE_ASSERT( element_begin != element_end,
                "No iteration allowed start == end" );

            // Prune sub-tree that does not have intersection
            if( !box_filter( node( node_index ) ) )
            {
                return false;
            }

            if( is_leaf( element_begin, element_end ) )
            {
                return action( mapping_morton( element_begin ) );
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
                if( node( it.child_left ).signed_distance( query )
                    < node( it.child_right ).signed_distance( query ) )
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

            return mapping_morton( element_begin );
        }

        void containing_boxes_recursive( index_t node_index,
            index_t element_begin,
            index_t element_end,
            const Point< dimension >& query,
            std::vector< index_t >& result ) const
        {
            OPENGEODE_ASSERT(
                node_index < tree_.size(), "Node index out of tree" );
            OPENGEODE_ASSERT( element_begin != element_end,
                "Begin and End indices should be different" );
            if( !node( node_index ).epsilon_contains( query ) )
            {
                return;
            }
            if( is_leaf( element_begin, element_end ) )
            {
                result.push_back( mapping_morton( element_begin ) );
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
        std::vector< BoundingBox< dimension > > tree_;
        std::vector< index_t > mapping_morton_;
    };

    template < index_t dimension >
    template < typename EvalDistance >
    std::tuple< index_t, double > AABBTree< dimension >::closest_element_box(
        const Point< dimension >& query, const EvalDistance& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return std::make_tuple( NO_ID, 0 );
        }
        auto nearest_box = impl_->closest_element_box_hint( query );
        auto distance = action( query, nearest_box );
        impl_->closest_element_box_recursive( query, nearest_box, distance,
            Impl::ROOT_INDEX, 0, nb_bboxes(), action );
        OPENGEODE_ASSERT( nearest_box != NO_ID, "No box found" );
        return std::make_tuple( nearest_box, distance );
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
