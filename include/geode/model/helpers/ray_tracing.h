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

#pragma once

#include <absl/container/flat_hash_map.h>

#include <geode/mesh/helpers/ray_tracing.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Block );
    ALIAS_3D( Block );
    class BRep;
    struct uuid;
} // namespace geode

namespace geode
{
    using BoundarySurfaceIntersections = absl::flat_hash_map< uuid,
        std::vector< RayTracing3D::PolygonDistance > >;

    BoundarySurfaceIntersections opengeode_model_api
        find_intersections_with_boundaries( const InfiniteLine3D& infinite_line,
            const BRep& brep,
            const Block3D& block );

    bool opengeode_model_api is_point_inside_block(
        const BRep& brep, const Block3D& block, const Point3D& point );

    absl::optional< uuid > opengeode_model_api block_containing_point(
        const BRep& brep, const Point3D& point );

} // namespace geode