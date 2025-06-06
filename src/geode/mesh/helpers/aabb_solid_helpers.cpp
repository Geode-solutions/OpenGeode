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

#include <geode/mesh/helpers/aabb_solid_helpers.hpp>

#include <async++.h>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/tetrahedral_solid.hpp>

namespace geode
{
    template < index_t dimension >
    AABBTree< dimension > create_aabb_tree( const SolidMesh< dimension >& mesh )
    {
        absl::FixedArray< BoundingBox< dimension > > box_vector(
            mesh.nb_polyhedra() );
        async::parallel_for( async::irange( index_t{ 0 }, mesh.nb_polyhedra() ),
            [&box_vector, &mesh]( index_t p ) {
                BoundingBox< dimension > bbox;
                for( const auto v : LRange{ mesh.nb_polyhedron_vertices( p ) } )
                {
                    bbox.add_point(
                        mesh.point( mesh.polyhedron_vertex( { p, v } ) ) );
                }
                box_vector[p] = std::move( bbox );
            } );
        return AABBTree< dimension >{ box_vector };
    }

    template < index_t dimension >
    double DistanceToTetrahedron< dimension >::operator()(
        const Point< dimension >& query, index_t cur_box ) const
    {
        return std::get< 0 >(
            point_tetrahedron_distance( query, mesh_.tetrahedron( cur_box ) ) );
    }

    template < index_t dimension >
    double AnisotropicDistanceToTetrahedron< dimension >::operator()(
        const Point< dimension >& query, index_t cur_box ) const
    {
        const auto tetrahedron_vertices =
            mesh_.tetrahedron( cur_box ).vertices();
        const auto p0 =
            coordinate_system_.coordinates( tetrahedron_vertices[0] );
        const auto p1 =
            coordinate_system_.coordinates( tetrahedron_vertices[1] );
        const auto p2 =
            coordinate_system_.coordinates( tetrahedron_vertices[2] );
        const auto p3 =
            coordinate_system_.coordinates( tetrahedron_vertices[3] );
        const Tetrahedron tetrahedron_in_metric_space{ p0, p1, p2, p3 };
        const auto query_in_metric_space =
            coordinate_system_.coordinates( query );
        return std::get< 0 >( point_tetrahedron_distance(
            query_in_metric_space, tetrahedron_in_metric_space ) );
    }

    template opengeode_mesh_api AABBTree3D create_aabb_tree< 3 >(
        const SolidMesh3D& );

    template class opengeode_mesh_api DistanceToTetrahedron< 3 >;
    template class opengeode_mesh_api AnisotropicDistanceToTetrahedron< 3 >;

} // namespace geode
