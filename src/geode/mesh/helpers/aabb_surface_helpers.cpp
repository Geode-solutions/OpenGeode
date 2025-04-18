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

#include <geode/mesh/helpers/aabb_surface_helpers.hpp>

#include <async++.h>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/triangulated_surface.hpp>

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree(
        const SurfaceMesh< dimension >& mesh )
    {
        absl::FixedArray< BoundingBox< dimension > > box_vector(
            mesh.nb_polygons() );
        async::parallel_for( async::irange( index_t{ 0 }, mesh.nb_polygons() ),
            [&box_vector, &mesh]( index_t p ) {
                BoundingBox< dimension > bbox;
                for( const auto v : LRange{ mesh.nb_polygon_vertices( p ) } )
                {
                    bbox.add_point(
                        mesh.point( mesh.polygon_vertex( { p, v } ) ) );
                }
                box_vector[p] = std::move( bbox );
            } );
        return AABBTree< dimension >{ box_vector };
    }

    template < index_t dimension >
    double DistanceToTriangle< dimension >::operator()(
        const Point< dimension >& query, index_t cur_box ) const
    {
        return std::get< 0 >(
            point_triangle_distance( query, mesh_.triangle( cur_box ) ) );
    }

    template < index_t dimension >
    double AnisotropicDistanceToTriangle< dimension >::operator()(
        const Point< dimension >& query, index_t cur_box ) const
    {
        const auto triangle = mesh_.triangle( cur_box );
        const auto vertices = triangle.vertices();
        const auto point0 = coordinate_system_.coordinates( vertices[0].get() );
        const auto point1 = coordinate_system_.coordinates( vertices[1].get() );
        const auto point2 = coordinate_system_.coordinates( vertices[2].get() );
        const Triangle< dimension > triangle_in_metric_space{ point0, point1,
            point2 };
        const auto query_in_metric_space =
            coordinate_system_.coordinates( query );
        return std::get< 0 >( point_triangle_distance(
            query_in_metric_space, triangle_in_metric_space ) );
    }

    template opengeode_mesh_api AABBTree2D create_aabb_tree< 2 >(
        const SurfaceMesh2D& );
    template opengeode_mesh_api AABBTree3D create_aabb_tree< 3 >(
        const SurfaceMesh3D& );

    template class opengeode_mesh_api DistanceToTriangle< 2 >;
    template class opengeode_mesh_api DistanceToTriangle< 3 >;
    template class opengeode_mesh_api AnisotropicDistanceToTriangle< 2 >;
    template class opengeode_mesh_api AnisotropicDistanceToTriangle< 3 >;

} // namespace geode
