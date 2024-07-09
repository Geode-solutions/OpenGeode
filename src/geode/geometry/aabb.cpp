/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

namespace geode
{
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
    AABBTree< dimension >::AABBTree( AABBTree&& ) noexcept = default;
    template < index_t dimension >
    AABBTree< dimension >::~AABBTree() = default;

    template < index_t dimension >
    AABBTree< dimension >& AABBTree< dimension >::operator=(
        AABBTree&& ) noexcept = default;

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

    template class opengeode_geometry_api AABBTree< 2 >;
    template class opengeode_geometry_api AABBTree< 3 >;
} // namespace geode
