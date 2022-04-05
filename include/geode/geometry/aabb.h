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

#pragma once

#include <absl/types/span.h>

#include <geode/basic/pimpl.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/bounding_box.h>
#include <geode/geometry/common.h>

namespace geode
{
    template < index_t dimension >
    class AABBTree
    {
        OPENGEODE_DISABLE_COPY( AABBTree );
        OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( dimension );

    public:
        /*!
         * @brief AABB is a search tree for fast spatial request using the
         * bounding box organisation of elements stored in a single container.
         * @param bboxes container containing elements bounding boxes.
         * Each element can then be accessed using the index of its box in the
         * tree which should match the index in its initial container.
         */
        AABBTree( absl::Span< const BoundingBox< dimension > > bboxes );
        AABBTree( AABBTree&& ) = default;
        ~AABBTree();

        /*!
         * @brief Gets the number of boxes in the lower level of the aabb tree.
         * @note This value should match the initial size of the container that
         * store all objects to organize the the aabb tree.
         */
        index_t nb_bboxes() const;

        const BoundingBox< dimension >& bounding_box() const;

        /*!
         * @brief Gets the closest element to a point
         * @param[in] query the point to test
         * @param[in] action the functor to compute the distance between
         * the \p query and the tree element in boxes
         * @return a tuple containing:
         * - the index of the closest element/box.
         * - the nearest point on the element in box.
         * - the distance between the \p query and \p nearest_point.
         *
         * @tparam EvalDistance this functor should have an operator()
         * defined like this: std::tuple< double, Point< dimension > >
         * operator()( const Point< dimension >& query, index_t
         * curent_element_box ) const ; the output tuple contain
         * - a double to store the distance between the point \p query and
         * the element stored in the \p curent_element_box.
         * - a Point< dimension > to store the nearest point from \p query
         * on the object stored in the \p curent_element_box.
         *
         * @note if several elements box cannot be discriminate by the
         * distance computation one of them will be randomly retuned.
         */
        template < typename EvalDistance >
        std::tuple< index_t, Point< dimension >, double > closest_element_box(
            const Point< dimension >& query, const EvalDistance& action ) const;

        /*!
         * @brief Computes the intersections between a given
         * box and the all element boxes.
         * @param[in] box the box to test
         * @param[in] action The functor to run when an element box intersects
         * \p box
         *
         * @tparam EvalIntersection this functor should have an operator()
         * defined like this:
         * void operator()( index_t cur_element_box ) ;
         *
         * @note the operator define what to do with the box \p cur_element_box
         * if it is intersected by the \p box.
         */
        template < class EvalIntersection >
        void compute_bbox_element_bbox_intersections(
            const BoundingBox< dimension >& box,
            EvalIntersection& action ) const;

        /*!
         * @brief Computes the self intersections of the element boxes.
         * @param[in] action The functor to run when two boxes intersect
         * @tparam EvalIntersection this functor should have an operator()
         * defined like this:
         * void operator()( index_t cur_element_box1, index_t cur_element_box2 )
         * ;
         * @note cur_element_box1 and cur_element_box2 are the element box
         * indices that intersect.
         * @note the operator defines what to do when two boxes of the
         * tree ( \p cur_element_box1 and \p cur_element_box2 ) intesect each
         * other (for example: test real intersection between each element in
         * boxes and store the result.)
         */
        template < class EvalIntersection >
        void compute_self_element_bbox_intersections(
            EvalIntersection& action ) const;

        /*!
         * @brief Computes all the intersections of the element boxes between
         * this tree and another one.
         * @param[in] action The functor to run when two boxes intersect
         * @tparam EvalIntersection this functor should have an operator()
         * defined like this:
         * void operator()( index_t cur_element_box1, index_t cur_element_box2 )
         * ;
         * @note cur_element_box1 and cur_element_box2 are the element box
         * indices that intersect in the current tree and in the other tree.
         */
        template < class EvalIntersection >
        void compute_other_element_bbox_intersections(
            const AABBTree< dimension >& other_tree,
            EvalIntersection& action ) const;

        /*!
         * @brief Computes the intersections between a given ray and all
         * element boxes.
         * @param[in] ray The ray to test.
         * @param[in] action The functor to run when a box is intersected by the
         * ray.
         * @tparam EvalIntersection this functor should have an operator()
         * defined like this:
         * void operator()( index_t cur_element_box ) ;
         * @note the operator define what to do with the box \p cur_element_box
         * if it is intersected by the \p ray.
         */
        template < class EvalIntersection >
        void compute_ray_element_bbox_intersections(
            const Ray< dimension >& ray, EvalIntersection& action ) const;

    private:
        static bool is_leaf( index_t box_begin, index_t box_end )
        {
            return box_begin + 1 == box_end;
        }

        struct Iterator
        {
            index_t middle_box;
            index_t child_left;
            index_t child_right;
        };
        static Iterator get_recursive_iterators(
            index_t node_index, index_t box_begin, index_t box_end )
        {
            Iterator it;
            it.middle_box = box_begin + ( box_end - box_begin ) / 2;
            it.child_left = 2 * node_index;
            it.child_right = 2 * node_index + 1;
            return it;
        }

        const BoundingBox< dimension >& node( index_t index ) const;

        index_t mapping_morton( index_t index ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( AABBTree );
} // namespace geode

#include <geode/geometry/detail/aabb_impl.h>
