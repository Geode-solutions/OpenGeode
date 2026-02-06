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

#pragma once

#include <absl/container/flat_hash_map.h>

#include <geode/basic/pimpl.hpp>

#include <geode/mesh/helpers/ray_tracing.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Block );
    FORWARD_DECLARATION_DIMENSION_CLASS( Surface );
    FORWARD_DECLARATION_DIMENSION_CLASS( AABBTree );
    ALIAS_3D( Block );
    ALIAS_3D( AABBTree );
    ALIAS_2D( Surface );
    class BRep;
    class Section;
    struct uuid;
} // namespace geode

namespace geode
{
    class opengeode_model_api SectionRayTracing
    {
    public:
        SectionRayTracing( const Section& section );
        ~SectionRayTracing();

        bool is_point_inside_surface(
            const Point2D& point, const Surface2D& surface );

        [[nodiscard]] std::optional< uuid > surface_containing_point(
            const Point2D& point );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    class opengeode_model_api BRepRayTracing
    {
    public:
        using BoundarySurfaceIntersections = absl::flat_hash_map< uuid,
            std::vector< RayTracing3D::PolygonDistance > >;

        BRepRayTracing( const BRep& brep );
        ~BRepRayTracing();

        [[nodiscard]] BoundarySurfaceIntersections
            find_intersections_with_boundaries(
                const InfiniteLine3D& infinite_line, const Block3D& block );

        [[nodiscard]] bool is_point_inside_block(
            const Point3D& point, const Block3D& block );

        [[nodiscard]] std::optional< uuid > block_containing_point(
            const Point3D& point );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    [[nodiscard]] bool opengeode_model_api is_point_inside_closed_surface(
        const Point3D& point,
        const SurfaceMesh3D& surface,
        const AABBTree3D& surface_aabb );
} // namespace geode