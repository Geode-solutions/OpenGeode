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

#include <geode/mesh/helpers/convert_point_set.h>

#include <geode/basic/range.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/helpers/private/copy.h>

namespace geode
{
    std::unique_ptr< PointSet2D > convert_point_set3d_into_2d(
        const PointSet3D& point_set3d, index_t axis_to_remove )
    {
        auto point_set2d = PointSet2D::create();
        auto builder2d = PointSetBuilder2D::create( *point_set2d );
        detail::copy_points3d_into_2d(
            point_set3d, *builder2d, axis_to_remove );
        detail::copy_attributes( point_set3d.vertex_attribute_manager(),
            point_set2d->vertex_attribute_manager() );
        detail::copy_meta_info( point_set3d, *builder2d );
        return point_set2d;
    }

    std::unique_ptr< PointSet3D > convert_point_set2d_into_3d(
        const PointSet2D& point_set2d, index_t axis_to_add )
    {
        auto point_set3d = PointSet3D::create();
        auto builder3d = PointSetBuilder3D::create( *point_set3d );
        detail::copy_points2d_into_3d( point_set2d, *builder3d, axis_to_add );
        detail::copy_attributes( point_set2d.vertex_attribute_manager(),
            point_set3d->vertex_attribute_manager() );
        detail::copy_meta_info( point_set2d, *builder3d );
        return point_set3d;
    }
} // namespace geode
