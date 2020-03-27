

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/signed_mensuration.h>

void test_triangle_area()
{
    // Test cases in 2D
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };

    const geode::Triangle2D triangle2D_1{ a, b, c };
    const auto area1 = geode::triangle_area( triangle2D_1 );
    OPENGEODE_EXCEPTION( area1 == 0.5, "[Test] Wrong result for triangle_area "
                                       "with query triangle triangle2D_1" );

    const geode::Triangle2D triangle2D_2{ a, b, a };
    const auto area2 = geode::triangle_area( triangle2D_2 );
    OPENGEODE_EXCEPTION( area2 == 0.0, "[Test] Wrong result for triangle_area "
                                       "with query triangle triangle2D_2" );

    // TODO: Test cases in 3D
}

void test_triangle_signed_area()
{
    // Test cases in 2D
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };

    const geode::Triangle2D triangle2D_1{ a, b, c };
    const auto area1 = geode::triangle_signed_area( triangle2D_1 );
    OPENGEODE_EXCEPTION( area1 == 0.5,
        "[Test] Wrong result for triangle_signed_area "
        "with query triangle triangle2D_1" );

    const geode::Triangle2D triangle2D_2{ a, c, b };
    const auto area2 = geode::triangle_signed_area( triangle2D_2 );
    OPENGEODE_EXCEPTION( area2 == -0.5,
        "[Test] Wrong result for triangle_signed_area "
        "with query triangle triangle2D_2" );

    const geode::Triangle2D triangle2D_3{ a, b, a };
    const auto area3 = geode::triangle_area( triangle2D_3 );
    OPENGEODE_EXCEPTION( area3 == 0.0,
        "[Test] Wrong result for triangle_signed_area "
        "with query triangle triangle2D_3" );

    // TODO: Test cases in 3D
}

void test_tetra_signed_area()
{
    // TODO 3D
}

int main()
{
    try
    {
        test_triangle_area();
        test_triangle_signed_area();
        test_tetra_signed_area();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
