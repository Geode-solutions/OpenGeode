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

#include <geode/mesh/helpers/hausdorff_distance.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>
#include <geode/mesh/io/triangulated_surface_input.h>

#include <geode/mesh/helpers/aabb_surface_helpers.h>

namespace
{
    double one_sided_hausdorff_distance(
        const geode::TriangulatedSurface3D& mesh_A,
        const geode::TriangulatedSurface3D& mesh_B )
    {
        const auto mesh_B_tree = geode::create_aabb_tree( mesh_B );
        const geode::DistanceToTriangle3D distance_action{ mesh_B };
        double min_dist = 0;
        for( auto v : geode::Range{ mesh_A.nb_vertices() } )
        {
            const auto& query = mesh_A.point( v );
            const auto closest_element =
                mesh_B_tree.closest_element_box( query, distance_action );
            const auto distance = std::get< 1 >( closest_element );
            if( distance > min_dist )
            {
                min_dist = distance;
            }
        }
        return min_dist;
    }
} // namespace

namespace geode
{
    double hausdorff_distance( const TriangulatedSurface3D& mesh_A,
        const TriangulatedSurface3D& mesh_B )
    {
        return std::max( one_sided_hausdorff_distance( mesh_A, mesh_B ),
            one_sided_hausdorff_distance( mesh_B, mesh_A ) );
    }
} // namespace geode
