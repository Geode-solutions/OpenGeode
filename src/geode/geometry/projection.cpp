

#include <geode/geometry/projection.h>

#include <geode/basic/logger.h>

#include <geode/geometry/vector.h>

#include <geode/geometry/distance.h>

namespace geode
{
    template < index_t dimension >
    Point< dimension > point_segment_projection(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto barycenter = segment.barycenter();
        const auto length = segment.length();
        if( length <= global_epsilon )
        {
            return barycenter;
        }
        const auto norm_dir = segment.normalized_direction();
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
