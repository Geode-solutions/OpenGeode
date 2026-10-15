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

#include <random>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

#include <geode/geometry/barycentric_coordinates.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/mensuration.hpp>

#include <geode/tests/common.hpp>

template < geode::index_t nb_coords >
bool check_bary_coords( const std::array< double, nb_coords >& result,
    const std::array< double, nb_coords >& answer )
{
    double sum{ 0 };
    for( const auto i : geode::Range{ nb_coords } )
    {
        if( std::fabs( result[i] - answer[i] ) > geode::GLOBAL_EPSILON )
        {
            geode::Logger::error( "Lambda ", i, " is incorrect." );
            geode::Logger::error(
                "Result = ", result[i], " - Answer = ", answer[i] );
            return false;
        }
        sum += result[i];
    }
    if( std::fabs( sum - 1 ) > geode::GLOBAL_EPSILON )
    {
        geode::Logger::error( "Sum of all lambdas are different of 1" );
        return false;
    }
    return true;
}

void test_segment_barycentric_coordinates()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 2.0, 3.0 } };
    const geode::Segment3D segment{ a, b };

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( a, segment ), { 1, 0 } ),
        "Wrong result for segment_barycentric_coordinates "
        "with query point a" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( b, segment ), { 0, 1 } ),
        "Wrong result for segment_barycentric_coordinates "
        "with query point b" );

    const geode::Point3D q1{ { 0.25, 0.50, 0.75 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( q1, segment ),
            { 0.75, 0.25 } ),
        "Wrong result for segment_barycentric_coordinates "
        "with query point q1" );

    const geode::Point3D q2{ { 2.0, 4.0, 6.0 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 2 >(
            geode::segment_barycentric_coordinates( q2, segment ), { -1, 2 } ),
        "Wrong result for segment_barycentric_coordinates "
        "with query point q2" );
}

void test_triangle_barycentric_coordinates_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( a, triangle2D ),
            { 1, 0, 0 } ),
        "Wrong result for triangle_barycentric_coordinates_2D "
        "with query point a" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( b, triangle2D ),
            { 0, 1, 0 } ),
        "Wrong result for triangle_barycentric_coordinates_2D "
        "with query point b" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( c, triangle2D ),
            { 0, 0, 1 } ),
        "Wrong result for triangle_barycentric_coordinates_2D "
        "with query point c" );

    const geode::Point2D q1{ { 0.25, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q1, triangle2D ),
            { 0.75, 0.25, 0 } ),
        "Wrong result for triangle_barycentric_coordinates_2D "
        "with query point q1" );

    const geode::Point2D q2{ { 0.5, 0.25 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q2, triangle2D ),
            { 0.5, 0.25, 0.25 } ),
        "Wrong result for triangle_barycentric_coordinates_2D "
        "with query point q2" );

    const geode::Point2D q3{ { 0.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q3, triangle2D ),
            { 1, -1, 1 } ),
        "Wrong result for triangle_barycentric_coordinates_2D "
        "with query point q3" );
}

void test_triangle_barycentric_coordinates_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 1.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( a, triangle3D ),
            { 1, 0, 0 } ),
        "Wrong result for triangle_barycentric_coordinates_3D "
        "with query point a" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( b, triangle3D ),
            { 0, 1, 0 } ),
        "Wrong result for triangle_barycentric_coordinates_3D "
        "with query point b" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( c, triangle3D ),
            { 0, 0, 1 } ),
        "Wrong result for triangle_barycentric_coordinates_3D "
        "with query point c" );

    const geode::Point3D q1{ { 1, 0.5, 0.5 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q1, triangle3D ),
            { 0, 0.5, 0.5 } ),
        "Wrong result for triangle_barycentric_coordinates_3D "
        "with query point q1" );

    const geode::Point3D q2{ { 0.5, 0.25, 0.25 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q2, triangle3D ),
            { 0.5, 0.25, 0.25 } ),
        "Wrong result for triangle_barycentric_coordinates_3D "
        "with query point q2" );

    const geode::Point3D q3{ { 2.0, 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 3 >(
            geode::triangle_barycentric_coordinates( q3, triangle3D ),
            { -1, 1, 1 } ),
        "Wrong result for triangle_barycentric_coordinates_3D "
        "with query point q3" );
}

void test_triangle_barycentric_coordinates()
{
    test_triangle_barycentric_coordinates_2d();
    test_triangle_barycentric_coordinates_3d();
}

void test_tetrahedron_barycentric_coordinates()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 0.0, 0.0, 1.0 } };
    const geode::Tetrahedron tetra{ a, b, c, d };

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 4 >(
            geode::tetrahedron_barycentric_coordinates( a, tetra ),
            { 1, 0, 0, 0 } ),
        "Wrong result for tetrahedron_barycentric_coordinates "
        "with query point a" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 4 >(
            geode::tetrahedron_barycentric_coordinates( b, tetra ),
            { 0, 1, 0, 0 } ),
        "Wrong result for tetrahedron_barycentric_coordinates "
        "with query point b" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 4 >(
            geode::tetrahedron_barycentric_coordinates( c, tetra ),
            { 0, 0, 1, 0 } ),
        "Wrong result for tetrahedron_barycentric_coordinates "
        "with query point c" );

    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 4 >(
            geode::tetrahedron_barycentric_coordinates( d, tetra ),
            { 0, 0, 0, 1 } ),
        "Wrong result for tetrahedron_barycentric_coordinates "
        "with query point d" );

    const geode::Point3D q1{ { 0.25, 0.25, 0.25 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 4 >(
            geode::tetrahedron_barycentric_coordinates( q1, tetra ),
            { 0.25, 0.25, 0.25, 0.25 } ),
        "Wrong result for tetrahedron_barycentric_coordinates "
        "with query point q1" );

    const geode::Point3D q2{ { 0.25, 1, -1 } };
    geode::OpenGeodeGeometryException::test(
        check_bary_coords< 4 >(
            geode::tetrahedron_barycentric_coordinates( q2, tetra ),
            { 0.75, 0.25, 1, -1 } ),
        "Wrong result for tetrahedron_barycentric_coordinates "
        "with query point q2" );
}

// Previous exception-based safe implementations
std::array< double, 2 > reference_safe_segment(
    const geode::Point3D& point, const geode::Segment3D& segment )
{
    const auto& vertices = segment.vertices();
    try
    {
        const auto dir = segment.direction();
        const auto dot0 = geode::Vector3D{ vertices[0], point }.dot( dir );
        const auto dot1 = -geode::Vector3D{ vertices[1], point }.dot( dir );
        const auto sum = dot0 + dot1;
        if( sum != 0 )
        {
            return { { dot1 / sum, dot0 / sum } };
        }
    }
    catch( const geode::OpenGeodeException& )
    {
    }
    if( geode::point_point_distance( point, vertices[0].get() )
        < geode::point_point_distance( point, vertices[1].get() ) )
    {
        return { { 1, 0 } };
    }
    return { { 0, 1 } };
}

std::array< double, 3 > reference_safe_triangle(
    const geode::Point3D& point, const geode::Triangle3D& triangle )
{
    const auto& vertices = triangle.vertices();
    const geode::Vector3D v0{ vertices[0], vertices[1] };
    const geode::Vector3D v1{ vertices[0], vertices[2] };
    const geode::Vector3D v2{ vertices[0], point };
    const auto d00 = v0.dot( v0 );
    const auto d01 = v0.dot( v1 );
    const auto d11 = v1.dot( v1 );
    const auto d20 = v2.dot( v0 );
    const auto d21 = v2.dot( v1 );
    const auto denom = d00 * d11 - d01 * d01;
    if( denom != 0 )
    {
        const auto lambda1 = ( d11 * d20 - d01 * d21 ) / denom;
        const auto lambda2 = ( d00 * d21 - d01 * d20 ) / denom;
        return { { 1. - lambda1 - lambda2, lambda1, lambda2 } };
    }
    auto edge = geode::NO_LID;
    double best_length{ 0 };
    for( const auto e : geode::LRange{ 3 } )
    {
        const auto length = geode::point_point_distance(
            vertices[e].get(), vertices[e == 2 ? 0 : e + 1].get() );
        if( length >= best_length )
        {
            edge = e;
            best_length = length;
        }
    }
    const auto next = edge == 2 ? 0 : edge + 1;
    const auto segment_lambdas = reference_safe_segment(
        point, geode::Segment3D{ vertices[edge], vertices[next] } );
    std::array< double, 3 > lambdas;
    lambdas.fill( 0 );
    lambdas[edge] = segment_lambdas[0];
    lambdas[next] = segment_lambdas[1];
    return lambdas;
}

void test_safe_barycentric_coordinates_without_exceptions()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -10, 10 };
    std::uniform_int_distribution< int > choice{ 0, 3 };
    const auto random_point = [&generator, &distribution] {
        return geode::Point3D{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
    };
    for( const auto unused : geode::Range{ 20000 } )
    {
        geode_unused( unused );
        const auto p0 = random_point();
        auto p1 = random_point();
        auto p2 = random_point();
        auto p3 = random_point();
        switch( choice( generator ) )
        {
            case 1:
                // Degenerated: duplicated vertices
                p1 = p0;
                p3 = p2;
                break;
            case 2:
                // Degenerated: aligned or coplanar vertices
                p2 = p0 * 0.25 + p1 * 0.75;
                p3 = p0 * 2 - p1;
                break;
            case 3:
                // Tiny segment
                p1 = p0 + geode::Point3D{ { 1e-7, 0, 0 } };
                break;
            default:
                break;
        }
        const auto query = random_point();
        const geode::Segment3D segment{ p0, p1 };
        geode::OpenGeodeGeometryException::test(
            geode::safe_segment_barycentric_coordinates( query, segment )
                == reference_safe_segment( query, segment ),
            "[Test] Wrong safe segment barycentric coordinates" );
        const geode::Triangle3D triangle{ p0, p1, p2 };
        geode::OpenGeodeGeometryException::test(
            geode::safe_triangle_barycentric_coordinates( query, triangle )
                == reference_safe_triangle( query, triangle ),
            "[Test] Wrong safe triangle barycentric coordinates" );
        const geode::Tetrahedron tetra{ p0, p1, p2, p3 };
        const auto safe_lambdas =
            geode::safe_tetrahedron_barycentric_coordinates( query, tetra );
        try
        {
            geode::OpenGeodeGeometryException::test(
                safe_lambdas
                    == geode::tetrahedron_barycentric_coordinates(
                        query, tetra ),
                "[Test] Wrong safe tetrahedron barycentric coordinates" );
        }
        catch( const geode::OpenGeodeException& )
        {
            double sum{ 0 };
            for( const auto lambda : safe_lambdas )
            {
                sum += lambda;
            }
            geode::OpenGeodeGeometryException::test(
                std::fabs( sum - 1 ) < 1e-12,
                "[Test] Wrong degenerated tetrahedron barycentric "
                "coordinates" );
        }
    }
}

void test()
{
    test_segment_barycentric_coordinates();
    test_triangle_barycentric_coordinates();
    test_tetrahedron_barycentric_coordinates();
    test_safe_barycentric_coordinates_without_exceptions();
}

OPENGEODE_TEST( "barycentric-coordinates" )
