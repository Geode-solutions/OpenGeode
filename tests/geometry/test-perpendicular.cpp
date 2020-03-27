

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/geometry/perpendicular.h>

void test_perpendicular()
{
    geode::Vector2D v{ { 1.578, 1e-10 } };
    const auto perp = geode::perpendicular( v ).normalize();

    const auto dot_product =
        v.value( 0 ) * perp.value( 0 ) + v.value( 1 ) * perp.value( 1 );

    OPENGEODE_EXCEPTION( perp.length() == 1 && dot_product == 0.,
        "[Test] Wrong result for normalized_perpendicular" );
}

void test_dot_perpendicular()
{
    const geode::Vector2D v1{ { 0, 1 } };
    const geode::Vector2D v2{ { 1, 0 } };
    const auto dot_perp = geode::dot_perpendicular( v1, v2 );
    const auto dot = geode::dot_perpendicular( geode::perpendicular( v1 ), v2 );

    OPENGEODE_EXCEPTION( dot_perp == -1 && dot == 0,
        "[Test] Wrong result for dot_perpendicular" );
}

int main()
{
    try
    {
        test_perpendicular();
        test_dot_perpendicular();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
