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

#include <geode/mesh/helpers/aabb_solid_helpers.h>

#include <async++.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/tetrahedral_solid.h>

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

    template opengeode_mesh_api AABBTree3D create_aabb_tree< 3 >(
        const SolidMesh3D& );

    template class opengeode_mesh_api DistanceToTetrahedron< 3 >;

} // namespace geode
