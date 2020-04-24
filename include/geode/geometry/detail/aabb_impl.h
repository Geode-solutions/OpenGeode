/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

        Impl( absl::Span< const BoundingBox< dimension > > bboxes );

        index_t nb_bboxes() const
        {
            return mapping_morton_.size();
        }

        const BoundingBox< dimension >& node( index_t i ) const
        {
            OPENGEODE_ASSERT( i < tree_.size(), "query out of tree" );
            return tree_[i];
        }

        void set_node( index_t i, const BoundingBox< dimension >& box )
        {
            OPENGEODE_ASSERT( i < tree_.size(), "query out of tree" );
            tree_[i] = box;
        }

        void add_box( index_t i, const BoundingBox< dimension >& box )
        {
            OPENGEODE_ASSERT( i < tree_.size(), "query out of tree" );
            tree_[i].add_box( box );
        }

        index_t mapping_morton( index_t i )
        {
            return mapping_morton_[i];
        }

        /*!
         * @brief Gets the number of nodes in the tree subset
         */
        index_t max_node_index(
            index_t node_index, index_t box_begin, index_t box_end ) const;

        /*!
         * @brief The recursive instruction used in initialize_tree()
         */
        void initialize_tree_recursive(
            absl::Span< const BoundingBox< dimension > > bboxes,
            index_t node_index,
            index_t element_begin,
            index_t element_end );

        /*!
         * @brief The recursive instruction used in closest_element_box()
         */
        template < typename ACTION >
        void closest_element_box_recursive( const Point< dimension >& query,
            index_t& nearest_box,
            Point< dimension >& nearest_point,
            double& distance,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            const ACTION& action ) const;

        template < class ACTION >
        void bbox_intersect_recursive( const BoundingBox< dimension >& box,
            index_t node_index,
            index_t element_begin,
            index_t element_end,
            ACTION& action ) const;

        template < class ACTION >
        void self_intersect_recursive( index_t node_index1,
            index_t element_begin1,
            index_t element_end1,
            index_t node_index2,
            index_t element_begin2,
            index_t element_end2,
            ACTION& action ) const;

    private:
        std::vector< BoundingBox< dimension > > tree_;
        std::vector< index_t > mapping_morton_;
    };

    template < index_t dimension >
    index_t AABBTree< dimension >::nb_bboxes() const
    {
        return impl_->nb_bboxes();
    }

    template < index_t dimension >
    const BoundingBox< dimension >& AABBTree< dimension >::node(
        index_t i ) const
    {
        return impl_->node( i );
    }

    template < index_t dimension >
    template < typename EvalDistance >
    std::tuple< index_t, Point< dimension >, double >
        AABBTree< dimension >::closest_element_box(
            const Point< dimension >& query, const EvalDistance& action ) const
    {
        index_t box_begin{ 0 };
        index_t box_end{ nb_bboxes() };
        index_t node_index{ Impl::ROOT_INDEX };
        while( !is_leaf( box_begin, box_end ) )
        {
            index_t box_middle;
            index_t child_left;
            index_t child_right;
            get_recursive_iterators( node_index, box_begin, box_end, box_middle,
                child_left, child_right );
            if( action( ( node( child_left ).min() + node( child_left ).max() )
                            / 2.,
                    query )
                < action(
                      ( node( child_right ).min() + node( child_right ).max() )
                          / 2.,
                      query ) )
            {
                box_end = box_middle;
                node_index = child_left;
            }
            else
            {
                box_begin = box_middle;
                node_index = child_right;
            }
        }

        const auto nearest_box = impl_->mapping_morton( box_begin );
        const auto nearest_point =
            get_point_hint_from_box( node( box_begin ), nearest_box );
        const auto distance = action( query, nearest_point );

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
        impl_->bbox_intersect_recursive(
            box, Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    template < class EvalIntersection >
    void AABBTree< dimension >::compute_self_element_bbox_intersections(
        EvalIntersection& action ) const
    {
        impl_->self_intersect_recursive( Impl::ROOT_INDEX, 0, nb_bboxes(),
            Impl::ROOT_INDEX, 0, nb_bboxes(), action );
    }

    template < index_t dimension >
    double point_box_signed_distance(
        const Point< dimension >& point, const BoundingBox< dimension >& box );

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
            const auto cur_box = mapping_morton_[box_begin];
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
        index_t box_middle, child_left, child_right;
        get_recursive_iterators( node_index, box_begin, box_end, box_middle,
            child_left, child_right );

        const auto distance_left =
            point_box_signed_distance( query, node( child_left ) );
        const auto distance_right =
            point_box_signed_distance( query, node( child_right ) );

        // Traverse the "nearest" child first, so that it has more chances
        // to prune the traversal of the other child.
        if( distance_left < distance_right )
        {
            if( distance_left < distance )
            {
                closest_element_box_recursive< ACTION >( query, nearest_box,
                    nearest_point, distance, child_left, box_begin, box_middle,
                    action );
            }
            if( distance_right < distance )
            {
                closest_element_box_recursive< ACTION >( query, nearest_box,
                    nearest_point, distance, child_right, box_middle, box_end,
                    action );
            }
        }
        else
        {
            if( distance_right < distance )
            {
                closest_element_box_recursive< ACTION >( query, nearest_box,
                    nearest_point, distance, child_right, box_middle, box_end,
                    action );
            }
            if( distance_left < distance )
            {
                closest_element_box_recursive< ACTION >( query, nearest_box,
                    nearest_point, distance, child_left, box_begin, box_middle,
                    action );
            }
        }
    }

    template < index_t dimension >
    template < typename ACTION >
    void AABBTree< dimension >::Impl::bbox_intersect_recursive(
        const BoundingBox< dimension >& box,
        index_t node_index,
        index_t element_begin,
        index_t element_end,
        ACTION& action ) const
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "Node out of tree range" );
        OPENGEODE_ASSERT( element_begin != element_end, "" );

        // Prune sub-tree that does not have intersection
        if( !box.bboxes_overlap( node( node_index ) ) )
        {
            return;
        }

        // Leaf case
        if( is_leaf( element_begin, element_end ) )
        {
            // @todo Check if the box is not intersecting itself
            action( mapping_morton_[element_begin] );
            return;
        }

        index_t box_middle, child_left, child_right;
        get_recursive_iterators( node_index, element_begin, element_end,
            box_middle, child_left, child_right );

        bbox_intersect_recursive< ACTION >(
            box, child_left, element_begin, box_middle, action );
        bbox_intersect_recursive< ACTION >(
            box, child_right, box_middle, element_end, action );
    }

    template < index_t dimension >
    template < class ACTION >
    void AABBTree< dimension >::Impl::self_intersect_recursive(
        index_t node_index1,
        index_t element_begin1,
        index_t element_end1,
        index_t node_index2,
        index_t element_begin2,
        index_t element_end2,
        ACTION& action ) const
    {
        OPENGEODE_ASSERT( element_end1 != element_begin1, "" );
        OPENGEODE_ASSERT( element_end2 != element_begin2, "" );

        // Since we are intersecting the AABBTree with *itself*,
        // we can prune half of the cases by skipping the test
        // whenever node2's polygon index interval is greated than
        // node1's polygon index interval.
        if( element_end2 <= element_begin1 )
        {
            return;
        }

        // The acceleration is here:
        if( !node( node_index1 ).bboxes_overlap( node( node_index2 ) ) )
        {
            return;
        }

        // Simple case: leaf - leaf intersection.
        if( is_leaf( element_begin1, element_end1 )
            && is_leaf( element_begin2, element_end2 ) )
        {
            if( node_index1 == node_index2 )
            {
                return;
            }
            action( mapping_morton_[element_begin1],
                mapping_morton_[element_begin2] );
            return;
        }

        // If node2 has more polygons than node1, then
        //   intersect node2's two children with node1
        // else
        //   intersect node1's two children with node2
        if( element_end2 - element_begin2 > element_end1 - element_begin1 )
        {
            index_t middle_box2, child_left2, child_right2;
            get_recursive_iterators( node_index2, element_begin2, element_end2,
                middle_box2, child_left2, child_right2 );
            self_intersect_recursive< ACTION >( node_index1, element_begin1,
                element_end1, child_left2, element_begin2, middle_box2,
                action );
            self_intersect_recursive< ACTION >( node_index1, element_begin1,
                element_end1, child_right2, middle_box2, element_end2, action );
        }
        else
        {
            index_t middle_box1, child_left1, child_right1;
            get_recursive_iterators( node_index1, element_begin1, element_end1,
                middle_box1, child_left1, child_right1 );
            self_intersect_recursive< ACTION >( child_left1, element_begin1,
                middle_box1, node_index2, element_begin2, element_end2,
                action );
            self_intersect_recursive< ACTION >( child_right1, middle_box1,
                element_end1, node_index2, element_begin2, element_end2,
                action );
        }
    }
} // namespace geode
