/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

} // namespace geode
