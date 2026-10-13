/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/basic/logger.hpp>

#include <geode/geometry/frame.hpp>
#include <geode/geometry/frame_transform.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

void test_elongation_directions()
{
    const geode::Frame3D frame{ { geode::Vector3D{ { 1e-3, 0, 0 } },
        geode::Vector3D{ { 0, 0, 10 } }, geode::Vector3D{ { 0, 1, 0 } } } };
    geode::OpenGeodeGeometryException::test(
        frame.max_elongation_direction() == 1,
        "Wrong max elongation direction" );
    geode::OpenGeodeGeometryException::test(
        frame.min_elongation_direction() == 0,
        "Wrong min elongation direction" );
}

void test_frame_transform_permutation()
{
    const geode::Frame3D identity;
    const geode::Frame3D cycle{ { geode::Vector3D{ { 0, 0, 1 } },
        geode::Vector3D{ { 1, 0, 0 } }, geode::Vector3D{ { 0, 1, 0 } } } };
    const geode::FrameTransform3D transform{ identity, cycle };
    geode::OpenGeodeGeometryException::test(
        transform.direction( 0 ) == 1 && transform.direction( 1 ) == 2
            && transform.direction( 2 ) == 0,
        "Wrong FrameTransform directions for a 3-cycle" );
    // apply(Vector) converts coordinates in the "to" frame into coordinates in
    // the "from" frame
    const auto result = transform.apply( geode::Vector3D{ { 1, 2, 3 } } );
    geode::OpenGeodeGeometryException::test(
        result == geode::Vector3D{ { 2, 3, 1 } },
        "Wrong FrameTransform apply for a 3-cycle: ", result.string() );
    const auto frame = transform.apply( identity );
    for( const auto d : geode::LRange{ 3 } )
    {
        geode::OpenGeodeGeometryException::test(
            frame.direction( transform.direction( d ) )
                == identity.direction( d ) * transform.orientation( d ),
            "Wrong FrameTransform apply on frame for a 3-cycle" );
    }
}

void test_frame_transform_tie()
{
    const geode::Frame2D diagonal{ { geode::Vector2D{ { 1, 1 } },
        geode::Vector2D{ { 1, -1 } } } };
    const geode::Frame2D identity;
    const geode::FrameTransform2D transform{ diagonal, identity };
    geode::OpenGeodeGeometryException::test(
        transform.direction( 0 ) != transform.direction( 1 ),
        "FrameTransform directions should be a permutation" );
    geode::OpenGeodeGeometryException::test(
        transform.direction( 0 ) == 0 && transform.orientation( 0 ) == 1
            && transform.orientation( 1 ) == -1,
        "Wrong FrameTransform on tie" );

    const geode::Frame3D from{ { geode::Vector3D{ { 1, 0.9, 0 } },
        geode::Vector3D{ { 1, 0.8, 0 } }, geode::Vector3D{ { 0, 0, 1 } } } };
    const geode::FrameTransform3D transform3d{ from, geode::Frame3D{} };
    geode::OpenGeodeGeometryException::test(
        transform3d.direction( 0 ) != transform3d.direction( 1 )
            && transform3d.direction( 2 ) == 2,
        "FrameTransform3D directions should be a permutation" );
}

void test()
{
    test_elongation_directions();
    test_frame_transform_permutation();
    test_frame_transform_tie();
}

OPENGEODE_TEST( "frame" )
