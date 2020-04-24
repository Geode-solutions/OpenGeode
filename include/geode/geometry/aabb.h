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

#include <absl/types/span.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/common.h>

namespace geode
{
    template < index_t dimension >
    class AABBTree
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( AABBTree );
        OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( dimension );

    public:
        AABBTree( absl::Span< const BoundingBox< dimension > > bboxes );
        virtual ~AABBTree() = default;

        index_t nb_bboxes() const;

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
         */
        template < typename EvalDistance >
        std::tuple< index_t, Point< dimension >, double > closest_element_box(
            const Point< dimension >& query, const EvalDistance& action ) const;

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
         */
        template < class EvalIntersection >
        void compute_bbox_element_bbox_intersections(
            const BoundingBox< dimension >& box,
            EvalIntersection& action ) const;

        /*
         * @brief Computes the self intersections of the element boxes.
         * @param[in] action The functor to run when two boxes intersect
         * @tparam EvalIntersection this functor should have an operator()
         * defined like this:
         * void operator()( index_t box1, index_t box2 ) ;
         * where box1 and box2 are the element box indices
         */
        template < class EvalIntersection >
        void compute_self_element_bbox_intersections(
            EvalIntersection& action ) const;

    protected:
        static bool is_leaf( index_t box_begin, index_t box_end )
        {
            return box_begin + 1 == box_end;
        }

        static void get_recursive_iterators( index_t node_index,
            index_t box_begin,
            index_t box_end,
            index_t& middle_box,
            index_t& child_left,
            index_t& child_right )
        {
            middle_box = box_begin + ( box_end - box_begin ) / 2;
            child_left = 2 * node_index;
            child_right = 2 * node_index + 1;
        }

        const BoundingBox< dimension >& node( index_t i ) const;

    private:
        /*!
         * @brief Gets an element point from its box
         * @details This function is used to get a result from the selected hint
         * box
         */
        virtual Point< dimension > get_point_hint_from_box(
            const BoundingBox< dimension >& box, index_t element_id ) const
        {
            geode_unused( element_id );
            return ( box.min() + box.max() ) / 2.;
        }

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( AABBTree );
} // namespace geode

#include <geode/geometry/detail/aabb_impl.h>