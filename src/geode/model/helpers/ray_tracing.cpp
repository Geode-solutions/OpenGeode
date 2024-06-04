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

#include <geode/model/helpers/ray_tracing.h>

#include <geode/geometry/aabb.h>

#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    geode::BoundarySurfaceIntersections find_intersections_with_boundaries(
        const geode::Ray3D& ray,
        const geode::BRep& brep,
        const geode::Block3D& block )
    {
        geode::BoundarySurfaceIntersections result;
        for( const auto& surface : brep.boundaries( block ) )
        {
            const auto aabb = geode::create_aabb_tree( surface.mesh() );
            geode::RayTracing3D ray_tracing{ surface.mesh(), ray };
            aabb.compute_ray_element_bbox_intersections( ray, ray_tracing );
            result[surface.id()] = ray_tracing.all_intersections();
        }
        return result;
    }

    absl::optional< geode::index_t > count_real_intersections_with_boudaries(
        const geode::Ray3D& ray,
        const geode::BRep& brep,
        const geode::Block3D& block )
    {
        geode::index_t nb_intersections{ 0 };
        const auto tracing =
            find_intersections_with_boundaries( ray, brep, block );
        for( const auto& intersections : tracing )
        {
            for( const auto& intersection : intersections.second )
            {
                if( intersection.position != geode::Position::inside )
                {
                    return absl::nullopt;
                }
                if( std::fabs( intersection.distance )
                    <= geode::global_epsilon )
                {
                    continue;
                }
                else
                {
                    nb_intersections += 1;
                }
            }
        }
        return nb_intersections;
    }
} // namespace

namespace geode
{
    BoundarySurfaceIntersections find_intersections_with_boundaries(
        const InfiniteLine3D& infinite_line,
        const BRep& brep,
        const Block3D& block )
    {
        BoundarySurfaceIntersections result;
        for( const auto& surface : brep.boundaries( block ) )
        {
            const auto aabb = create_aabb_tree( surface.mesh() );
            RayTracing3D ray_tracing{ surface.mesh(), infinite_line };
            aabb.compute_line_element_bbox_intersections(
                infinite_line, ray_tracing );
            result[surface.id()] = ray_tracing.all_intersections();
        }
        return result;
    }

    bool test_point_inside_block(
        const BRep& brep, const Block3D& block, const Point3D& point )
    {
        std::array< Vector3D, 12 > directions = { { { { 0., 0., 1. } },
            { { 0.1, 0., 1. } }, { { 0.3, 0., 1. } }, { { 0.5, 0., 1. } },
            { { 0., 1., 0. } }, { { 0.1, 1., 0. } }, { { 0.3, 1., 0. } },
            { { 0.5, 1., 0. } }, { { 1., 0., 0. } }, { { 1., 0., 0.1 } },
            { { 1., 0., 0.3 } }, { { 1., 0., 0.5 } } } };

        for( const auto& direction : directions )
        {
            const Ray3D ray{ direction, point };
            auto nb_intersections =
                count_real_intersections_with_boudaries( ray, brep, block );

            if( nb_intersections.has_value() )
            {
                return ( nb_intersections.value() % 2 == 1 );
            }
        }
        throw OpenGeodeException{
            "Cannot determine the point is inside the block or not "
            "(ambigous intersection with rays)."
        };
    }

    absl::optional< uuid > block_containing_point(
        const BRep& brep, const Point3D& point )
    {
        for( const auto& block : brep.blocks() )
        {
            if( test_point_inside_block( brep, block, point ) )
            {
                return block.id();
            }
        }
        return absl::nullopt;
    }

} // namespace geode
