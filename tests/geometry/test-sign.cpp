/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/point.h>
#include <geode/geometry/sign.h>

#include <geode/tests/common.h>

void test_triangle_sign_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    const auto side = geode::triangle_area_sign( triangle2D );
    OPENGEODE_EXCEPTION( side == geode::Side::positive,
        "[Test] Wrong result for triangle_area_sign" );
}

void test_triangle_sign_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 1.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 2.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };

    const auto side_z = geode::triangle_area_sign( triangle3D, 2 );
    OPENGEODE_EXCEPTION( side_z == geode::Side::positive,
        "[Test] Wrong result for triangle_area_sign and axis 2" );

    const auto side_y = geode::triangle_area_sign( triangle3D, 1 );

    OPENGEODE_EXCEPTION( side_y == geode::Side::negative,
        "[Test] Wrong result for triangle_area_sign and axis 1" );

    const auto side_x = geode::triangle_area_sign( triangle3D, 0 );
    OPENGEODE_EXCEPTION( side_x == geode::Side::negative,
        "[Test] Wrong result for triangle_area_sign and axis 0" );
}

void test_triangle_sign()
{
    test_triangle_sign_2d();
    test_triangle_sign_3d();
}
void test()
{
    test_triangle_sign();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "sign" )