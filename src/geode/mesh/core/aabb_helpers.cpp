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

#include <geode/mesh/core/aabb_helpers.h>

#include <memory>

#include <geode/mesh/core/triangulated_surface.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/basic_objects.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree(
        const TriangulatedSurface< dimension >& mesh )
    {
        absl::FixedArray< BoundingBox< dimension > > box_vector(
            mesh.nb_polygons() );
        for( const auto p : Range{ mesh.nb_polygons() } )
        {
            for( const auto v : Range{ mesh.nb_polygon_vertices( p ) } )
            {
                box_vector[p].add_point(
                    mesh.point( mesh.polygon_vertex( { p, v } ) ) );
            }
        }
        return AABBTree< dimension >( box_vector );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > >
        DistanceToTriangle< dimension >::operator()(
            const Point< dimension >& query, index_t cur_box ) const
    {
        const auto& v0 = mesh_.point( mesh_.polygon_vertex( { cur_box, 0 } ) );
        const auto& v1 = mesh_.point( mesh_.polygon_vertex( { cur_box, 1 } ) );
        const auto& v2 = mesh_.point( mesh_.polygon_vertex( { cur_box, 2 } ) );
        return point_triangle_distance(
            query, Triangle< dimension >{ v0, v1, v2 } );
    }

    template < index_t dimension >
    double DistanceToTriangle< dimension >::operator()(
        const Point< dimension >& point1,
        const Point< dimension >& point2 ) const
    {
        return Vector< dimension >{ point1, point2 }.length();
    }

    template opengeode_mesh_api AABBTree2D create_aabb_tree< 2 >(
        const TriangulatedSurface2D& );
    template opengeode_mesh_api AABBTree3D create_aabb_tree< 3 >(
        const TriangulatedSurface3D& );

    template class opengeode_mesh_api DistanceToTriangle< 2 >;
    template class opengeode_mesh_api DistanceToTriangle< 3 >;

} // namespace geode
