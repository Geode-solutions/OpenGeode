/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/mesh/helpers/hausdorff_distance.hpp>

#include <async++.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

#include <geode/mesh/helpers/aabb_surface_helpers.hpp>

namespace
{
    double one_sided_hausdorff_distance(
        const geode::TriangulatedSurface3D& mesh_A,
        const geode::TriangulatedSurface3D& mesh_B )
    {
        const auto mesh_B_tree = geode::create_aabb_tree( mesh_B );
        const geode::DistanceToTriangle3D distance_action{ mesh_B };
        return async::parallel_map_reduce(
            async::irange( geode::index_t{ 0 }, mesh_A.nb_vertices() ), 0.,
            [&mesh_A, &mesh_B_tree, &distance_action]( geode::index_t v ) {
                return std::get< 1 >( mesh_B_tree.closest_element_box(
                    mesh_A.point( v ), distance_action ) );
            },
            []( double lhs, double rhs ) {
                return std::max( lhs, rhs );
            } );
    }
} // namespace

namespace geode
{
    double hausdorff_distance( const TriangulatedSurface3D& mesh_A,
        const TriangulatedSurface3D& mesh_B )
    {
        OpenGeodeMeshException::check_exception(
            mesh_A.nb_polygons() > 0 && mesh_B.nb_polygons() > 0, nullptr,
            OpenGeodeException::TYPE::data,
            "[hausdorff_distance] Hausdorff distance is undefined for an "
            "empty mesh" );
        return std::max( one_sided_hausdorff_distance( mesh_A, mesh_B ),
            one_sided_hausdorff_distance( mesh_B, mesh_A ) );
    }
} // namespace geode
