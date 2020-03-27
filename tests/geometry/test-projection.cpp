

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/projection.h>

int main()
{
    try
    {
        // Tests for 2D cases
        const geode::Point2D a{ { 1.0, 5.0 } };
        const geode::Point2D b{ { -1.0, -5.0 } };
        const geode::Segment2D segment2D{ a, b };

        geode::Point2D closest_point;

        closest_point = geode::point_segment_projection( a, segment2D );
        OPENGEODE_EXCEPTION( closest_point == a,
            "[Test] Wrong result for point_segment_projection with query point "
            "a" );

        closest_point = geode::point_segment_projection( b, segment2D );
        OPENGEODE_EXCEPTION( closest_point == b,
            "[Test] Wrong result for point_segment_projection with query point "
            "b" );

        const geode::Point2D q1{ { 0.0, 0.0 } };
        closest_point = geode::point_segment_projection( q1, segment2D );
        OPENGEODE_EXCEPTION( closest_point == q1,
            "[Test] Wrong result for point_segment_projection with query point "
            "q1" );

        const geode::Point2D q2{ { 10.0, 10.0 } };
        closest_point = geode::point_segment_projection( q2, segment2D );
        OPENGEODE_EXCEPTION( closest_point == a,
            "[Test] Wrong result for point_segment_projection with query point "
            "q2" );

        const geode::Point2D q3{ { 5.0, -1.0 } };
        closest_point = geode::point_segment_projection( q3, segment2D );
        const geode::Point2D result_q3{ { 0.0, 0.0 } };
        OPENGEODE_EXCEPTION( closest_point == result_q3,
            "[Test] Wrong result for point_segment_projection with query point "
            "q3" );

        const geode::Point2D q4{ { 5.5, 1.5 } };
        closest_point = geode::point_segment_projection( q4, segment2D );
        const geode::Point2D result_q4{ { 0.5, 2.5 } };
        OPENGEODE_EXCEPTION( closest_point == result_q4,
            "[Test] Wrong result for point_segment_projection with query point "
            "q3" );

        // Tests for 3D cases
        // TODO

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
