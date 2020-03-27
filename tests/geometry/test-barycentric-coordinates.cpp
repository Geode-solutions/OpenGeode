

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>

#include <geode/geometry/point.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects.h>

bool check_triangle_bary_coords( const std::array< double, 3 >& result,
    const std::array< double, 3 >& answer )
{
    for( const auto i : geode::Range{ 3 } )
    {
        if( result[i] != answer[i] )
        {
            geode::Logger::error( "Lambda ", i, " is incorrect." );
            geode::Logger::error(
                "Result = ", result[i], " - Answer = ", answer[i] );
            return false;
        }
    }
    if( std::fabs( result[0] + result[1] + result[2] - 1 )
        > geode::global_epsilon )
    {
        geode::Logger::error( "Sum of all lambdas are different of 1" );
        return false;
    }
    return true;
}

void test_triangle_barycentric_coordinates()
{
    // Test cases in 2D
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    OPENGEODE_EXCEPTION(
        check_triangle_bary_coords(
            geode::triangle_barycentric_coordinates( a, triangle2D ),
            { 1, 0, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates "
        "with query point a" );

    OPENGEODE_EXCEPTION(
        check_triangle_bary_coords(
            geode::triangle_barycentric_coordinates( b, triangle2D ),
            { 0, 1, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates "
        "with query point b" );

    OPENGEODE_EXCEPTION(
        check_triangle_bary_coords(
            geode::triangle_barycentric_coordinates( c, triangle2D ),
            { 0, 0, 1 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates "
        "with query point c" );

    const geode::Point2D q1{ { 0.25, 0.0 } };
    OPENGEODE_EXCEPTION(
        check_triangle_bary_coords(
            geode::triangle_barycentric_coordinates( q1, triangle2D ),
            { 0.75, 0.25, 0 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates "
        "with query point q1" );

    const geode::Point2D q2{ { 0.5, 0.25 } };
    OPENGEODE_EXCEPTION(
        check_triangle_bary_coords(
            geode::triangle_barycentric_coordinates( q2, triangle2D ),
            { 0.5, 0.25, 0.25 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates "
        "with query point q2" );

    const geode::Point2D q3{ { 0.0, 1.0 } };
    OPENGEODE_EXCEPTION(
        check_triangle_bary_coords(
            geode::triangle_barycentric_coordinates( q3, triangle2D ),
            { 1, -1, 1 } ),
        "[Test] Wrong result for triangle_barycentric_coordinates "
        "with query point q3" );

    // TODO: Test cases in 3D
}

void test_tetra_barycentric_coordinates()
{
    // TODO 3D
}

int main()
{
    try
    {
        test_triangle_barycentric_coordinates();
        test_tetra_barycentric_coordinates();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
