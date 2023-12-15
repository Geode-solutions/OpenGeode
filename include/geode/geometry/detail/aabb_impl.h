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

#pragma once

#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/aabb.h>

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
            index_t middle_box;
            index_t child_left;
            index_t child_right;
        };

    public:
        Impl() = default;

        Impl( absl::Span< const BoundingBox< dimension > > bboxes );

        index_t nb_bboxes() const;

        static bool is_leaf( index_t box_begin, index_t box_end );

        static Iterator get_recursive_iterators(
            index_t node_index, index_t box_begin, index_t box_end );

        const BoundingBox< dimension >& node( index_t index ) const;

        index_t mapping_morton( index_t index ) const;

        static index_t max_node_index_recursive(
            index_t node_index, index_t box_begin, index_t box_end );

        void initialize_tree_recursive(
            absl::Span< const BoundingBox< dimension > > bboxes,
            index_t node_index,
            index_t element_begin,
            index_t element_end );

        template < typename ACTION >
        void closest_element_box_recursive( const Point< dimension >& query,
            index_t& nearest_box,
            Point< dimension >& nearest_point,
            double& distance,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            const ACTION& action ) const;

        template < typename ACTION >
        bool bbox_intersect_recursive( const BoundingBox< dimension >& box,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            ACTION& action ) const;

        template < typename ACTION >
        bool self_intersect_recursive( index_t node_index1,
            index_t element_begin1,
            index_t element_end1,
            index_t node_index2,
            index_t element_begin2,
            index_t element_end2,
            ACTION& action ) const;

        template < typename ACTION >
        bool other_intersect_recursive( index_t node_index1,
            index_t element_begin1,
            index_t element_end1,
            const AABBTree< dimension >& other_tree,
            index_t node_index2,
            index_t element_begin2,
            index_t element_end2,
            ACTION& action ) const;

        template < typename Line, typename ACTION >
        bool line_intersect_recursive( const Line& line,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            ACTION& action ) const;

        index_t closest_element_box_hint(
            const Point< dimension >& query ) const;

        void containing_boxes_recursive( index_t node_index,
            index_t element_begin,
            index_t element_end,
            const Point< dimension >& query,
            std::vector< index_t >& result ) const;

    private:
        std::vector< BoundingBox< dimension > > tree_;
        std::vector< index_t > mapping_morton_;
    };

    template < index_t dimension >
    template < typename EvalDistance >
    std::tuple< index_t, Point< dimension >, double >
        AABBTree< dimension >::closest_element_box(
            const Point< dimension >& query, const EvalDistance& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return std::make_tuple( NO_ID, query, 0 );
        }
        auto nearest_box = impl_->closest_element_box_hint( query );
        double distance;
        Point< dimension > nearest_point;
        std::tie( distance, nearest_point ) = action( query, nearest_box );

        impl_->closest_element_box_recursive( query, nearest_box, nearest_point,
            distance, Impl::ROOT_INDEX, 0, nb_bboxes(), action );
        OPENGEODE_ASSERT( nearest_box != NO_ID, "No box found" );
        return std::make_tuple( nearest_box, nearest_point, distance );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_bbox_element_bbox_intersections(
        const BoundingBox< dimension >& box, EvalIntersection& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return;
        }
        impl_->bbox_intersect_recursive(
            box, Impl::ROOT_INDEX, 0, nb_bboxes(), action );
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
        if( nb_bboxes() == 0 )
        {
            return;
        }
        impl_->line_intersect_recursive(
            ray, Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_line_element_bbox_intersections(
        const InfiniteLine< dimension >& line, EvalIntersection& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return;
        }
        impl_->line_intersect_recursive(
            line, Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_segment_element_bbox_intersections(
        const Segment< dimension >& segment, EvalIntersection& action ) const
    {
        if( nb_bboxes() == 0 )
        {
            return;
        }
        impl_->line_intersect_recursive(
            segment, Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    template < typename ACTION >
    void AABBTree< dimension >::Impl::closest_element_box_recursive(
        const Point< dimension >& query,
        index_t& nearest_box,
        Point< dimension >& nearest_point,
        double& distance,
        index_t node_index,
        index_t box_begin,
        index_t box_end,
        const ACTION& action ) const
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "node out of tree" );
        OPENGEODE_ASSERT(
            box_begin != box_end, "Begin and End indices should be different" );

        // If node is a leaf: compute point-element distance
        // and replace current if nearer
        if( is_leaf( box_begin, box_end ) )
        {
            const auto cur_box = mapping_morton( box_begin );
            Point< dimension > cur_nearest_point;
            double cur_distance;
            std::tie( cur_distance, cur_nearest_point ) =
                action( query, cur_box );
            if( cur_distance < distance )
            {
                nearest_box = cur_box;
                nearest_point = cur_nearest_point;
                distance = cur_distance;
            }
            return;
        }
        const auto it =
            get_recursive_iterators( node_index, box_begin, box_end );
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
                closest_element_box_recursive( query, nearest_box,
                    nearest_point, distance, it.child_left, box_begin,
                    it.middle_box, action );
            }
            if( distance_right < distance )
            {
                closest_element_box_recursive( query, nearest_box,
                    nearest_point, distance, it.child_right, it.middle_box,
                    box_end, action );
            }
        }
        else
        {
            if( distance_right < distance )
            {
                closest_element_box_recursive( query, nearest_box,
                    nearest_point, distance, it.child_right, it.middle_box,
                    box_end, action );
            }
            if( distance_left < distance )
            {
                closest_element_box_recursive( query, nearest_box,
                    nearest_point, distance, it.child_left, box_begin,
                    it.middle_box, action );
            }
        }
    }

    template < index_t dimension >
    template < typename ACTION >
    bool AABBTree< dimension >::Impl::bbox_intersect_recursive(
        const BoundingBox< dimension >& box,
        index_t node_index,
        index_t element_begin,
        index_t element_end,
        ACTION& action ) const
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "Node out of tree range" );
        OPENGEODE_ASSERT(
            element_begin != element_end, "No iteration allowed start == end" );

        // Prune sub-tree that does not have intersection
        if( !box.intersects( node( node_index ) ) )
        {
            return false;
        }

        if( is_leaf( element_begin, element_end ) )
        {
            return action( mapping_morton( element_begin ) );
        }

        const auto it =
            get_recursive_iterators( node_index, element_begin, element_end );
        if( bbox_intersect_recursive(
                box, it.child_left, element_begin, it.middle_box, action ) )
        {
            return true;
        }
        return bbox_intersect_recursive(
            box, it.child_right, it.middle_box, element_end, action );
    }

    template < index_t dimension >
    template < typename ACTION >
    bool AABBTree< dimension >::Impl::self_intersect_recursive(
        index_t node_index1,
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
        if( !node( node_index1 ).intersects( node( node_index2 ) ) )
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
                    element_end1, it.child_left, element_begin2, it.middle_box,
                    action ) )
            {
                return true;
            }
            return self_intersect_recursive( node_index1, element_begin1,
                element_end1, it.child_right, it.middle_box, element_end2,
                action );
        }
        const auto it = get_recursive_iterators(
            node_index1, element_begin1, element_end1 );
        if( self_intersect_recursive( it.child_left, element_begin1,
                it.middle_box, node_index2, element_begin2, element_end2,
                action ) )
        {
            return true;
        }
        return self_intersect_recursive( it.child_right, it.middle_box,
            element_end1, node_index2, element_begin2, element_end2, action );
    }

    template < index_t dimension >
    template < typename ACTION >
    bool AABBTree< dimension >::Impl::other_intersect_recursive(
        index_t node_index1,
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
                 .intersects( other_tree.impl_->node( node_index2 ) ) )
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
                    it.middle_box, action ) )
            {
                return true;
            }
            return other_intersect_recursive( node_index1, element_begin1,
                element_end1, other_tree, it.child_right, it.middle_box,
                element_end2, action );
        }
        const auto it = get_recursive_iterators(
            node_index1, element_begin1, element_end1 );
        if( other_intersect_recursive( it.child_left, element_begin1,
                it.middle_box, other_tree, node_index2, element_begin2,
                element_end2, action ) )
        {
            return true;
        }
        return other_intersect_recursive( it.child_right, it.middle_box,
            element_end1, other_tree, node_index2, element_begin2, element_end2,
            action );
    }

    template < index_t dimension >
    template < typename Line, typename ACTION >
    bool AABBTree< dimension >::Impl::line_intersect_recursive(
        const Line& line,
        index_t node_index,
        index_t element_begin,
        index_t element_end,
        ACTION& action ) const
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "Node out of tree range" );
        OPENGEODE_ASSERT(
            element_begin != element_end, "No iteration allowed start == end" );

        // Prune sub-tree that does not have intersection
        if( !node( node_index ).intersects( line ) )
        {
            return false;
        }

        if( is_leaf( element_begin, element_end ) )
        {
            return action( mapping_morton( element_begin ) );
        }

        const auto it =
            get_recursive_iterators( node_index, element_begin, element_end );
        if( line_intersect_recursive(
                line, it.child_left, element_begin, it.middle_box, action ) )
        {
            return true;
        }
        return line_intersect_recursive(
            line, it.child_right, it.middle_box, element_end, action );
    }
} // namespace geode
