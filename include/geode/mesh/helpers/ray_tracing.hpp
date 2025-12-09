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

#pragma once

#include <optional>

#include <geode/basic/pimpl.hpp>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    ALIAS_2D( EdgedCurve );
    ALIAS_3D( SurfaceMesh );
    ALIAS_2D_AND_3D( BoundingBox );
} // namespace geode

namespace geode
{
    class opengeode_mesh_api RayTracing2D
    {
    public:
        struct EdgeDistance
        {
            EdgeDistance() = default;

            EdgeDistance( index_t edge_in,
                double distance_in,
                POSITION position_in,
                Point2D point_in )
                : edge{ edge_in },
                  distance{ distance_in },
                  position{ position_in },
                  point{ std::move( point_in ) }
            {
            }

            [[nodiscard]] bool operator<( const EdgeDistance& other ) const
            {
                return std::fabs( distance ) < std::fabs( other.distance );
            }

            index_t edge{ NO_ID };
            double distance{ 0 };
            POSITION position{ POSITION::outside };
            Point2D point;
        };

    public:
        [[deprecated]] RayTracing2D(
            const EdgedCurve2D& mesh, const Ray2D& ray );
        [[deprecated]] RayTracing2D(
            const EdgedCurve2D& mesh, const InfiniteLine2D& infinite_line );
        RayTracing2D( const EdgedCurve2D& mesh,
            const BoundingBox2D& bbox,
            const Ray2D& ray );
        RayTracing2D( const EdgedCurve2D& mesh,
            const BoundingBox2D& bbox,
            const InfiniteLine2D& infinite_line );
        RayTracing2D( const EdgedCurve2D& mesh,
            const Point2D& origin,
            const OwnerSegment2D& segment );
        RayTracing2D( RayTracing2D&& other ) noexcept;
        ~RayTracing2D();

        [[nodiscard]] std::optional< EdgeDistance > closest_edge() const;

        [[nodiscard]] std::optional< absl::FixedArray< EdgeDistance > >
            closest_edges( index_t nb_closest_wanted ) const;

        [[nodiscard]] std::vector< EdgeDistance > all_intersections() const;

        [[nodiscard]] bool operator()( index_t edge_id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    class opengeode_mesh_api RayTracing3D
    {
    public:
        struct PolygonDistance
        {
            PolygonDistance() = default;

            PolygonDistance( index_t polygon_in,
                double distance_in,
                POSITION position_in,
                Point3D point_in )
                : polygon{ polygon_in },
                  distance{ distance_in },
                  position{ position_in },
                  point{ std::move( point_in ) }
            {
            }

            [[nodiscard]] bool operator<( const PolygonDistance& other ) const
            {
                return std::fabs( distance ) < std::fabs( other.distance );
            }

            index_t polygon{ NO_ID };
            double distance{ 0 };
            POSITION position{ POSITION::outside };
            Point3D point;
        };

    public:
        [[deprecated]] RayTracing3D(
            const SurfaceMesh3D& mesh, const Ray3D& ray );
        [[deprecated]] RayTracing3D(
            const SurfaceMesh3D& mesh, const InfiniteLine3D& infinite_line );
        RayTracing3D( const SurfaceMesh3D& mesh,
            const BoundingBox3D& bbox,
            const Ray3D& ray );
        RayTracing3D( const SurfaceMesh3D& mesh,
            const BoundingBox3D& bbox,
            const InfiniteLine3D& infinite_line );
        RayTracing3D( const SurfaceMesh3D& mesh,
            const Point3D& origin,
            const OwnerSegment3D& segment );
        RayTracing3D( RayTracing3D&& other ) noexcept;
        ~RayTracing3D();

        [[nodiscard]] std::optional< PolygonDistance > closest_polygon() const;

        [[nodiscard]] std::optional< absl::FixedArray< PolygonDistance > >
            closest_polygons( index_t nb_closest_wanted ) const;

        [[nodiscard]] std::vector< PolygonDistance > all_intersections() const;

        [[nodiscard]] bool operator()( index_t polygon_id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
