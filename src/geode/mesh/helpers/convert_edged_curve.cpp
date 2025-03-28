/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/helpers/convert_edged_curve.hpp>

#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/helpers/detail/curve_merger.hpp>
#include <geode/mesh/helpers/internal/copy.hpp>

namespace
{
    template < typename CurveFrom, typename BuilderTo >
    void copy_edges( const CurveFrom& curve, BuilderTo& builder )
    {
        for( auto e : geode::Range{ curve.nb_edges() } )
        {
            builder->create_edge(
                curve.edge_vertex( { e, 0 } ), curve.edge_vertex( { e, 1 } ) );
        }
    }
} // namespace

namespace geode
{
    std::unique_ptr< EdgedCurve2D > convert_edged_curve3d_into_2d(
        const EdgedCurve3D& curve3d, index_t axis_to_remove )
    {
        auto curve2d = EdgedCurve2D::create();
        auto builder2d = EdgedCurveBuilder2D::create( *curve2d );
        internal::copy_points3d_into_2d( curve3d, *builder2d, axis_to_remove );
        copy_edges( curve3d, builder2d );
        internal::copy_attributes( curve3d.vertex_attribute_manager(),
            curve2d->vertex_attribute_manager() );
        internal::copy_attributes( curve3d.edge_attribute_manager(),
            curve2d->edge_attribute_manager() );
        internal::copy_meta_info( curve3d, *builder2d );
        return curve2d;
    }

    std::unique_ptr< EdgedCurve3D > convert_edged_curve2d_into_3d(
        const EdgedCurve2D& curve2d,
        index_t axis_to_add,
        double axis_coordinate )
    {
        auto curve3d = EdgedCurve3D::create();
        auto builder3d = EdgedCurveBuilder3D::create( *curve3d );
        internal::copy_points2d_into_3d(
            curve2d, *builder3d, axis_to_add, axis_coordinate );
        copy_edges( curve2d, builder3d );
        internal::copy_attributes( curve2d.vertex_attribute_manager(),
            curve3d->vertex_attribute_manager() );
        internal::copy_attributes( curve2d.edge_attribute_manager(),
            curve3d->edge_attribute_manager() );
        internal::copy_meta_info( curve2d, *builder3d );
        return curve3d;
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > merge_edged_curves( absl::Span<
        const std::reference_wrapper< const EdgedCurve< dimension > > > curves )
    {
        detail::EdgedCurveMerger< dimension > merger{ curves };
        return merger.merge( GLOBAL_EPSILON );
    }

    template std::unique_ptr< EdgedCurve< 2 > >
        opengeode_mesh_api merge_edged_curves( absl::Span<
            const std::reference_wrapper< const EdgedCurve< 2 > > > );
    template std::unique_ptr< EdgedCurve< 3 > >
        opengeode_mesh_api merge_edged_curves( absl::Span<
            const std::reference_wrapper< const EdgedCurve< 3 > > > );
} // namespace geode
