/*
 * Copyright (c) 2019 - 2025 Geode-solutions. All rights reserved.
 */

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/frame.hpp>
#include <geode/geometry/normal_frame_transform.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

void test_normal_frame_transform()
{
    geode::Frame2D from{ { geode::Vector< 2 >{ { 1, 0 } },
        geode::Vector2D{ { 0, 1 } } } };
    geode::Frame2D to{ { geode::Vector2D{ { 1, 0 } },
        geode::Vector2D{ { 0, 0.1 } } } };
    geode::NormalFrameTransform2D frame_transform{ from, to };
    geode::Vector2D test_vector{ { 1, 1 } };
    const auto result = frame_transform.apply( test_vector );
    const auto correct_result = geode::Vector2D{ { 1, 0.1 } };
    OPENGEODE_EXCEPTION( result == correct_result,
        "[Test] Wrong result for normal frame transform" );
    geode::Logger::info( "TEST SUCCESS" );
}

void test()
{
    test_normal_frame_transform();
}

OPENGEODE_TEST( "normal-frame-transform" )