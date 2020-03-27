

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/distance.h>

void test_point_segment_distance()
{
    // Tests for 2D cases
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment2D{ a, b };

    double distance;
    geode::Point2D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_segment_distance( a, segment2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == a,
        "[Test] Wrong result for point_segment_distance with query point "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_segment_distance( b, segment2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == b,
        "[Test] Wrong result for point_segment_distance with query point "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q1, segment2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q1,
        "[Test] Wrong result for point_segment_distance with query point "
        "q1" );

    const geode::Point2D q2{ { 10.0, 10.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q2, segment2D );
    OPENGEODE_EXCEPTION( distance == std::sqrt( 106 ) && closest_point == a,
        "[Test] Wrong result for point_segment_distance with query point "
        "q2" );

    const geode::Point2D q3{ { 5.0, -1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q3, segment2D );
    const geode::Point2D result_q3{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 26 ) && closest_point == result_q3,
        "[Test] Wrong result for point_segment_distance with query point "
        "q3" );

    const geode::Point2D q4{ { 5.5, 1.5 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q4, segment2D );
    const geode::Point2D result_q4{ { 0.5, 2.5 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 26 ) && closest_point == result_q4,
        "[Test] Wrong result for point_segment_distance with query point "
        "q4" );
}

void test_point_triangle_distance()
{
    // Tests for 2D cases
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    double distance;
    geode::Point2D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( a, triangle2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == a,
        "[Test] Wrong result for point_triangle_distance with query point "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( b, triangle2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == b,
        "[Test] Wrong result for point_triangle_distance with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0.5 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q1, triangle2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q1,
        "[Test] Wrong result for point_triangle_distance with query point "
        "q1" );

    const geode::Point2D q2{ { 0.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q2, triangle2D );
    const geode::Point2D result_q2{ { 0.5, 0.5 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 2 ) / 2. && closest_point == result_q2,
        "[Test] Wrong result for point_triangle_distance with query point "
        "q2" );

    const geode::Point2D q3{ { 2.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q3, triangle2D );
    OPENGEODE_EXCEPTION( distance == 1 && closest_point == c,
        "[Test] Wrong result for point_triangle_distance with query point "
        "q3" );

    const geode::Point2D q4{ { 0.5, 0.5 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q4, triangle2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q4,
        "[Test] Wrong result for point_triangle_distance with query point "
        "q4" );
}

void test_point_plane_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Triangle3D triangle{ a, b, c };
    const auto plane = triangle.plane();

    double distance;
    geode::Point3D closest_point, answer;

    const geode::Point3D q1{ { 0.5, 0.5, 1 } };
    std::tie( distance, closest_point ) =
        geode::point_plane_distance( q1, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( distance == 1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_distance with query point "
        "q1" );
    std::tie( distance, closest_point ) =
        geode::point_plane_signed_distance( q1, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( distance == 1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_signed_distance with query point "
        "q1" );

    const geode::Point3D q2{ { 0.5, 0.5, -1 } };
    std::tie( distance, closest_point ) =
        geode::point_plane_distance( q2, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( distance == 1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_distance with query point "
        "q2" );
    std::tie( distance, closest_point ) =
        geode::point_plane_signed_distance( q2, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( distance == -1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_signed_distance with query point "
        "q2" );
}

int main()
{
    try
    {
        test_point_segment_distance();
        test_point_triangle_distance();

        // Tests for 3D cases
        // TODO
        test_point_plane_distance();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
