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

#include <geode/geometry/projection.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/vector.h>

namespace geode
{
    template < index_t dimension >
    Point< dimension > point_segment_projection(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto barycenter = segment.barycenter();
        const auto length = segment.length();
        if( length <= GLOBAL_EPSILON )
        {
            return barycenter;
        }
        const auto norm_dir = segment.direction() / length;
        const auto d = norm_dir.dot( { barycenter, point } );
        if( std::fabs( d ) <= length / 2. )
        {
            return { barycenter + norm_dir * d };
        }
        return d > 0 ? segment.vertices()[1] : segment.vertices()[0];
    }

    template < index_t dimension >
    Point< dimension > point_line_projection(
        const Point< dimension >& point, const InfiniteLine< dimension >& line )
    {
        const auto d = line.direction().dot( { line.origin(), point } );
        return { line.origin() + line.direction() * d };
    }

    template < index_t dimension >
    Point< dimension > point_triangle_projection(
        const Point< dimension >& point, const Triangle< dimension >& triangle )
    {
        return std::get< 1 >( point_triangle_distance( point, triangle ) );
    }

    Point3D point_plane_projection( const Point3D& point, const Plane& plane )
    {
        const Vector3D ori_point{ plane.origin(), point };
        const auto distance = ori_point.dot( plane.normal() );
        return point - plane.normal() * distance;
    }

    template Point1D opengeode_geometry_api point_segment_projection(
        const Point1D&, const Segment1D& );

    template Point2D opengeode_geometry_api point_segment_projection(
        const Point2D&, const Segment2D& );
    template Point2D opengeode_geometry_api point_line_projection(
        const Point2D&, const InfiniteLine2D& );
    template Point2D opengeode_geometry_api point_triangle_projection(
        const Point2D&, const Triangle2D& );

    template Point3D opengeode_geometry_api point_segment_projection(
        const Point3D&, const Segment3D& );
    template Point3D opengeode_geometry_api point_line_projection(
        const Point3D&, const InfiniteLine3D& );
    template Point3D opengeode_geometry_api point_triangle_projection(
        const Point3D&, const Triangle3D& );

} // namespace geode
