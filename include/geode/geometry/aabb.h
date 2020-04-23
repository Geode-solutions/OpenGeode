/*
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of ASGA nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ASGA BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *     http://www.ring-team.org
 *
 *     RING Project
 *     Ecole Nationale Superieure de Geologie - GeoRessources
 *     2 Rue du Doyen Marcel Roubault - TSA 70605
 *     54518 VANDOEUVRE-LES-NANCY
 *     FRANCE
 */

#pragma once

#include <geode/basic/common.h>
#include <geode/geometry/bounding_box.h>

namespace geode
{
    /*!
     * @brief AABB tree structure
     * @details The tree is store in s single vector following this example:
     *                          ROOT
     *                        /      \
     *                      A1        A2
     *                    /    \     /   \
     *                  B1     B2   B3    B4
     *  where B* are the input bboxes
     *  Storage: |empty|ROOT|A1|A2|B1|B2|B3|B4|
     */
    template < index_t dimension >
    class AABBTree
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( AABBTree );
        OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( dimension );

    public:
        /// The index where to store the root. It starts to one for algorithm
        /// trick.
        static const index_t ROOT_INDEX = 1;

        index_t nb_bboxes() const
        {
            return static_cast< index_t >( mapping_morton_.size() );
        }

        /*!
         * @brief Gets the closest element box to a point
         * @param[in] query the point to test
         * @param[in] action the functor to compute the distance between
         * the \p query and the tree element boxes
         * @return a tuple containing:
         * - the index of the closest element box.
         * - the nearest point on the element box.
         * - the distance between the \p query.
         * and \p nearest_point.
         * @tparam EvalDistance this functor should have an operator() defined
         * like this:
         *  std::tuple< double, Point< dimension > > operator()(
         *      const Point< dimension >& query,
         *      index_t cur_box ) const ;
         * where query is the same than \p query, cur_box is the element box
         * index
         * (e.g. in the case of SurfaceAABBTree, this index is a polygon index).
         * The returned tuple contains the distance of the nearest point and the
         * nearest point computed using the element in the \p cur_box.
         */
        template < typename EvalDistance >
        std::tuple< index_t, Point< dimension >, double > closest_element_box(
            const Point< dimension >& query, const EvalDistance& action ) const
        {
            index_t nearest_box = NO_ID;
            Point< dimension > nearest_point;
            double distance;
            std::tie( nearest_box, nearest_point, distance ) =
                get_nearest_element_box_hint( query, action );
            closest_element_box_recursive< EvalDistance >( query, nearest_box,
                nearest_point, distance, ROOT_INDEX, 0, nb_bboxes(), action );
            OPENGEODE_ASSERT( nearest_box != NO_ID, "No box found" );
            return std::make_tuple( nearest_box, nearest_point, distance );
        }
        /*
         * @brief Computes the intersections between a given
         * box and the element boxes.
         * @param[in] box the box to test
         * @param[in] action The functor to run when an element box intersects
         * \p box
         * @tparam EvalIntersection this functor should have an operator()
         * defined like this:
         * void operator()( index_t cur_box ) ;
         * where cur_box is the element box index
         * (e.g. in the case of AABBTree2D, this index is a polygon index)
         */
        template < class EvalIntersection >
        void compute_bbox_element_bbox_intersections(
            const BoundingBox< dimension >& box,
            EvalIntersection& action ) const
        {
            bbox_intersect_recursive< EvalIntersection >(
                box, ROOT_INDEX, 0, nb_bboxes(), action );
        }
        /*
         * @brief Computes the self intersections of the element boxes.
         * @param[in] action The functor to run when two boxes intersect
         * @tparam EvalIntersection this functor should have an operator()
         * defined like this:
         * void operator()( index_t box1, index_t box2 ) ;
         * where box1 and box2 are the element box indices
         * (e.g. in the case of AABBTree2D, this index is a polygon index)
         */
        template < class EvalIntersection >
        void compute_self_element_bbox_intersections(
            EvalIntersection& action ) const
        {
            self_intersect_recursive< EvalIntersection >( ROOT_INDEX, 0,
                nb_bboxes(), ROOT_INDEX, 0, nb_bboxes(), action );
        }

    protected:
        AABBTree() = default;
        virtual ~AABBTree() = default;

        /*!
         * @brief Builds the tree
         * @details Comptes the morton order and build the tree
         * using the ordered bboxes
         * @param[in] bboxes the set of unordered bboxes
         */
        void initialize_tree(
            const std::vector< BoundingBox< dimension > >& bboxes );

        bool is_leaf( index_t box_begin, index_t box_end ) const
        {
            return box_begin + 1 == box_end;
        }
        void get_recursive_iterators( index_t node_index,
            index_t box_begin,
            index_t box_end,
            index_t& middle_box,
            index_t& child_left,
            index_t& child_right ) const
        {
            middle_box = box_begin + ( box_end - box_begin ) / 2;
            child_left = 2 * node_index;
            child_right = 2 * node_index + 1;
        }

        const BoundingBox< dimension >& node( index_t i ) const
        {
            OPENGEODE_ASSERT( i < tree_.size(), "querry out of tree" );
            return tree_[i];
        }

        BoundingBox< dimension >& node( index_t i )
        {
            OPENGEODE_ASSERT( i < tree_.size(), "querry out of tree" );
            return tree_[i];
        }

    private:
        /*!
         * @brief Gets the number of nodes in the tree subset
         */
        index_t max_node_index(
            index_t node_index, index_t box_begin, index_t box_end );
        /*!
         * @brief The recursive instruction used in initialize_tree()
         */
        void initialize_tree_recursive(
            const std::vector< BoundingBox< dimension > >& bboxes,
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

        /*!
         * @brief Gets an hint of the result
         * @details Compute the result by approximating each bbox to its
         * barycenter (using given distance evaluator).
         * This result is then used to speed-up the computation by minimizing
         * the distance computation between \p query and the real elements
         * inside the bboxes
         */
        template < class EvalDistance >
        std::tuple< index_t, Point< dimension >, double >
            get_nearest_element_box_hint( const Point< dimension >& query,
                const EvalDistance& eval_distance ) const
        {
            index_t box_begin{ 0 };
            index_t box_end{ nb_bboxes() };
            index_t node_index{ ROOT_INDEX };
            while( !is_leaf( box_begin, box_end ) )
            {
                index_t box_middle;
                index_t child_left;
                index_t child_right;
                get_recursive_iterators( node_index, box_begin, box_end,
                    box_middle, child_left, child_right );
                if( eval_distance( node( child_left ).center(), query )
                    < eval_distance( node( child_right ).center(), query ) )
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

            auto nearest_box = mapping_morton_[box_begin];
            Point< dimension > nearest_point =
                get_point_hint_from_box( tree_[box_begin], nearest_box );
            auto distance = eval_distance( query, nearest_point );
            return std::make_tuple( nearest_box, nearest_point, distance );
        }
        /*!
         * @brief Gets an element point from its box
         * @details This function is used to get a result from the selected hint
         * box
         */
        virtual Point< dimension > get_point_hint_from_box(
            const BoundingBox< dimension >& box, index_t element_id ) const = 0;

    protected:
        std::vector< BoundingBox< dimension > > tree_{};
        std::vector< index_t > mapping_morton_{};
    };

    template < index_t dimension >
    class opengeode_geometry_api BoxAABBTree : public AABBTree< dimension >
    {
    public:
        explicit BoxAABBTree(
            const std::vector< BoundingBox< dimension > >& bboxes );

    private:
        /*!
         * @brief Gets an element point from its box
         * @details In this case, the point is the barycenter of the box
         */
        Point< dimension > get_point_hint_from_box(
            const BoundingBox< dimension >& box,
            index_t element_id ) const override;
    };

    ALIAS_2D_AND_3D( BoxAABBTree );

    template < index_t dimension >
    double inner_point_box_distance(
        const Point< dimension >& p, const BoundingBox< dimension >& B );

    template < index_t dimension >
    double point_box_signed_distance(
        const Point< dimension >& p, const BoundingBox< dimension >& B );

    template < index_t dimension >
    template < typename ACTION >
    void AABBTree< dimension >::closest_element_box_recursive(
        const Point< dimension >& query,
        index_t& nearest_box,
        Point< dimension >& nearest_point,
        double& distance,
        index_t node_index,
        index_t box_begin,
        index_t box_end,
        const ACTION& action ) const
    {
        OPENGEODE_ASSERT( node_index < tree_.size(), "node out of tree range" );
        OPENGEODE_ASSERT( box_begin != box_end, "" );

        // If node is a leaf: compute point-element distance
        // and replace current if nearer
        if( is_leaf( box_begin, box_end ) )
        {
            index_t cur_box = mapping_morton_[box_begin];
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

        double distance_left =
            point_box_signed_distance( query, node( child_left ) );
        double distance_right =
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
    void AABBTree< dimension >::bbox_intersect_recursive(
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
            index_t cur_box = mapping_morton_[element_begin];
            action( cur_box );
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
    void AABBTree< dimension >::self_intersect_recursive( index_t node_index1,
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
