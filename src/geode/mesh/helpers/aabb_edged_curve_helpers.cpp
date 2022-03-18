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

#include <geode/mesh/helpers/aabb_edged_curve_helpers.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>
#include <geode/mesh/core/edged_curve.h>

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree(
        const EdgedCurve< dimension >& mesh )
    {
        absl::FixedArray< BoundingBox< dimension > > box_vector(
            mesh.nb_edges() );
        for( const auto e : Range{ mesh.nb_edges() } )
        {
            box_vector[e].add_point(
                mesh.point( mesh.edge_vertex( { e, 0 } ) ) );
            box_vector[e].add_point(
                mesh.point( mesh.edge_vertex( { e, 1 } ) ) );
        }
        return AABBTree< dimension >{ box_vector };
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > >
        DistanceToEdge< dimension >::operator()(
            const Point< dimension >& query, index_t cur_box ) const
    {
        const auto& v0 = mesh_.point( mesh_.edge_vertex( { cur_box, 0 } ) );
        const auto& v1 = mesh_.point( mesh_.edge_vertex( { cur_box, 1 } ) );
        return point_segment_distance( query, Segment< dimension >{ v0, v1 } );
    }

    template opengeode_mesh_api AABBTree2D create_aabb_tree< 2 >(
        const EdgedCurve2D& );
    template opengeode_mesh_api AABBTree3D create_aabb_tree< 3 >(
        const EdgedCurve3D& );

    template class opengeode_mesh_api DistanceToEdge< 2 >;
    template class opengeode_mesh_api DistanceToEdge< 3 >;

} // namespace geode
