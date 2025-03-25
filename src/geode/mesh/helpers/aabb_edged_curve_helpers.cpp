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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/mesh/helpers/aabb_edged_curve_helpers.hpp>

#include <async++.h>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/projection.hpp>

#include <geode/mesh/core/edged_curve.hpp>

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree(
        const EdgedCurve< dimension >& mesh )
    {
        absl::FixedArray< BoundingBox< dimension > > box_vector(
            mesh.nb_edges() );
        async::parallel_for( async::irange( index_t{ 0 }, mesh.nb_edges() ),
            [&box_vector, &mesh]( index_t e ) {
                BoundingBox< dimension > bbox;
                bbox.add_point( mesh.point( mesh.edge_vertex( { e, 0 } ) ) );
                bbox.add_point( mesh.point( mesh.edge_vertex( { e, 1 } ) ) );
                box_vector[e] = std::move( bbox );
            } );
        return AABBTree< dimension >{ box_vector };
    }

    template < index_t dimension >
    double DistanceToEdge< dimension >::operator()(
        const Point< dimension >& query, index_t cur_box ) const
    {
        const auto segment = mesh_.segment( cur_box );
        return point_segment_distance( query, segment );
    }

    template < index_t dimension >
    double AnisotropicDistanceToEdge< dimension >::operator()(
        const Point< dimension >& query, index_t cur_box ) const
    {
        const auto segment = mesh_.segment( cur_box );
        const auto vertices = segment.vertices();
        const auto point0 = coordinate_system_.coordinates( vertices[0].get() );
        const auto point1 = coordinate_system_.coordinates( vertices[1].get() );
        const auto query_in_metric_space =
            coordinate_system_.coordinates( query );
        geode::Segment< dimension > segment_in_metric_space{ point0, point1 };
        return point_segment_distance( query_in_metric_space, segment );
    }

    template opengeode_mesh_api AABBTree1D create_aabb_tree< 1 >(
        const EdgedCurve1D& );
    template opengeode_mesh_api AABBTree2D create_aabb_tree< 2 >(
        const EdgedCurve2D& );
    template opengeode_mesh_api AABBTree3D create_aabb_tree< 3 >(
        const EdgedCurve3D& );

    template class opengeode_mesh_api DistanceToEdge< 1 >;
    template class opengeode_mesh_api DistanceToEdge< 2 >;
    template class opengeode_mesh_api DistanceToEdge< 3 >;

    template class opengeode_mesh_api AnisotropicDistanceToEdge< 2 >;
    template class opengeode_mesh_api AnisotropicDistanceToEdge< 3 >;
} // namespace geode
