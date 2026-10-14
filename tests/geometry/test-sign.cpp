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

#include <algorithm>
#include <cmath>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/sign.hpp>

#include <geode/tests/common.hpp>

void test_triangle_sign_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    const auto side = geode::triangle_area_sign( triangle2D );
    geode::OpenGeodeGeometryException::test(
        side == geode::SIDE::positive, "Wrong result for triangle_area_sign" );
}

void test_triangle_sign_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 1.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 2.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };

    const auto side_z = geode::triangle_area_sign( triangle3D, 2 );
    geode::OpenGeodeGeometryException::test( side_z == geode::SIDE::positive,
        "Wrong result for triangle_area_sign and axis 2" );

    const auto side_y = geode::triangle_area_sign( triangle3D, 1 );

    geode::OpenGeodeGeometryException::test( side_y == geode::SIDE::negative,
        "Wrong result for triangle_area_sign and axis 1" );

    const auto side_x = geode::triangle_area_sign( triangle3D, 0 );
    geode::OpenGeodeGeometryException::test( side_x == geode::SIDE::negative,
        "Wrong result for triangle_area_sign and axis 0" );
}

void test_triangle_sign()
{
    test_triangle_sign_2d();
    test_triangle_sign_3d();
}
geode::Polygon2D make_polygon( const std::vector< geode::Point2D >& points )
{
    std::vector< geode::RefPoint2D > vertices;
    for( const auto& point : points )
    {
        vertices.emplace_back( point );
    }
    return geode::Polygon2D{ std::move( vertices ) };
}

void test_polygon_sign()
{
    // Concave CCW polygon whose first fan triangle is clockwise
    std::vector< geode::Point2D > points{ geode::Point2D{ { 0, 0 } },
        geode::Point2D{ { 2, 1 } }, geode::Point2D{ { 4, 0 } },
        geode::Point2D{ { 2, 3 } } };
    const auto ccw = make_polygon( points );
    geode::OpenGeodeGeometryException::test(
        geode::polygon_area_sign( ccw ) == geode::SIGN::positive,
        "Wrong sign for concave CCW polygon" );
    std::reverse( points.begin(), points.end() );
    const auto cw = make_polygon( points );
    geode::OpenGeodeGeometryException::test(
        geode::polygon_area_sign( cw ) == geode::SIGN::negative,
        "Wrong sign for concave CW polygon" );

    // Star-shaped polygons with alternating radius, every rotation of the
    // vertex order
    constexpr geode::index_t NB_BRANCHES{ 7 };
    std::vector< geode::Point2D > star;
    for( const auto v : geode::Range{ 2 * NB_BRANCHES } )
    {
        const auto angle = M_PI * v / NB_BRANCHES;
        const auto radius = v % 2 == 0 ? 3. : 1.;
        star.emplace_back( std::array< double, 2 >{
            { radius * std::cos( angle ), radius * std::sin( angle ) } } );
    }
    for( const auto rotation : geode::Range{ star.size() } )
    {
        geode_unused( rotation );
        std::rotate( star.begin(), star.begin() + 1, star.end() );
        geode::OpenGeodeGeometryException::test(
            geode::polygon_area_sign( make_polygon( star ) )
                == geode::SIGN::positive,
            "Wrong sign for star polygon" );
    }

    // Collinear vertices and degenerate polygon
    const std::vector< geode::Point2D > collinear_points{
        geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 0 } },
        geode::Point2D{ { 2, 0 } }, geode::Point2D{ { 2, 2 } }
    };
    const auto with_collinear = make_polygon( collinear_points );
    geode::OpenGeodeGeometryException::test(
        geode::polygon_area_sign( with_collinear ) == geode::SIGN::positive,
        "Wrong sign for polygon with collinear vertices" );
    const std::vector< geode::Point2D > flat_points{ geode::Point2D{ { 0, 0 } },
        geode::Point2D{ { 1, 0 } }, geode::Point2D{ { 2, 0 } } };
    const auto flat = make_polygon( flat_points );
    geode::OpenGeodeGeometryException::test(
        geode::polygon_area_sign( flat ) == geode::SIGN::zero,
        "Wrong sign for flat polygon" );
}

void test()
{
    test_triangle_sign();
    test_polygon_sign();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "sign" )