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

#include <chrono>
#include <random>
#include <string>
#include <vector>

#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/is_point_inside.hpp>
#include <geode/geometry/nn_search.hpp>
#include <geode/geometry/position.hpp>
#include <geode/geometry/projection.hpp>
#include <geode/geometry/quality.hpp>
#include <geode/geometry/rotation.hpp>

#include <geode/tests/common.hpp>

namespace
{
    constexpr geode::index_t NB_INPUTS{ 10000 };
    volatile double sink{ 0 };

    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -10, 10 };
    std::uniform_real_distribution< double > unit_distribution{ -1, 1 };

    double random_value()
    {
        return distribution( generator );
    }

    template < geode::index_t dimension >
    geode::Point< dimension > random_point()
    {
        geode::Point< dimension > point;
        for( const auto d : geode::LRange{ dimension } )
        {
            point.set_value( d, random_value() );
        }
        return point;
    }

    template < geode::index_t dimension >
    geode::Vector< dimension > random_vector( double scale )
    {
        geode::Vector< dimension > vector;
        for( const auto d : geode::LRange{ dimension } )
        {
            vector.set_value( d, scale * unit_distribution( generator ) );
        }
        return vector;
    }

    template < geode::index_t dimension >
    std::vector< geode::Point< dimension > > random_points( geode::index_t nb )
    {
        std::vector< geode::Point< dimension > > points;
        points.reserve( nb );
        for( const auto unused : geode::Range{ nb } )
        {
            geode_unused( unused );
            points.emplace_back( random_point< dimension >() );
        }
        return points;
    }

    template < typename Function >
    void bench( const std::string& name,
        const std::string& input,
        geode::index_t nb_calls,
        const Function& function )
    {
        double accumulator{ 0 };
        // warm up
        for( const auto i : geode::Range{ std::min( nb_calls, NB_INPUTS ) } )
        {
            accumulator += function( i % NB_INPUTS );
        }
        const auto start = std::chrono::steady_clock::now();
        for( const auto i : geode::Range{ nb_calls } )
        {
            accumulator += function( i % NB_INPUTS );
        }
        const auto end = std::chrono::steady_clock::now();
        sink = sink + accumulator;
        const auto ns =
            std::chrono::duration< double, std::nano >( end - start ).count()
            / nb_calls;
        geode::Logger::info( "BENCH|", name, "|", input, "|", ns );
    }

    template < typename Result >
    double nb_results( const Result& result )
    {
        return result.has_intersection() ? result.result->size() : 0.;
    }

    void bench_point_distances()
    {
        const auto points0 = random_points< 3 >( NB_INPUTS );
        const auto points1 = random_points< 3 >( NB_INPUTS );
        const auto queries = random_points< 3 >( NB_INPUTS );
        bench( "point_segment_distance(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::point_segment_distance(
                    queries[i], { points0[i], points1[i] } );
            } );
        bench( "point_segment_projection(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::point_segment_projection(
                    queries[i], { points0[i], points1[i] } )
                    .value( 0 );
            } );
        bench( "point_line_distance(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::point_line_distance( queries[i],
                    geode::InfiniteLine3D{
                        geode::Segment3D{ points0[i], points1[i] } } );
            } );
        const auto queries2d = random_points< 2 >( NB_INPUTS );
        const auto points2d = random_points< 2 >( NB_INPUTS );
        bench( "point_segment_distance(2D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::point_segment_distance( queries2d[i],
                    { points2d[i], points2d[( i + 1 ) % NB_INPUTS] } );
            } );
        bench( "point_triangle_distance(2D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >(
                    geode::point_triangle_distance( queries2d[i],
                        { points2d[i], points2d[( i + 1 ) % NB_INPUTS],
                            points2d[( i + 2 ) % NB_INPUTS] } ) );
            } );
        const geode::Point2D a{ { 0, 0 } };
        const geode::Point2D b{ { 1, 0 } };
        const geode::Point2D c{ { 2, 0 } };
        bench( "point_triangle_distance(2D)", "flat triangle", 1000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::point_triangle_distance(
                    queries2d[i], { a, b, c } ) );
            } );
        bench( "point_triangle_distance(3D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::point_triangle_distance(
                    queries[i], { points0[i], points1[i],
                                    points0[( i + 1 ) % NB_INPUTS] } ) );
            } );
        bench( "point_triangle_distance(3D)", "degenerated triangle", 1000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::point_triangle_distance(
                    queries[i], { points0[i], points1[i], points1[i] } ) );
            } );
    }

    void bench_triangle_distances()
    {
        const auto points = random_points< 3 >( NB_INPUTS );
        const auto shifted = random_points< 3 >( NB_INPUTS );
        bench( "segment_triangle_distance", "random", 500000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::segment_triangle_distance(
                    { shifted[i], shifted[( i + 1 ) % NB_INPUTS] },
                    { points[i], points[( i + 1 ) % NB_INPUTS],
                        points[( i + 2 ) % NB_INPUTS] } ) );
            } );
        bench( "triangle_triangle_distance", "random", 200000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::triangle_triangle_distance(
                    { shifted[i], shifted[( i + 1 ) % NB_INPUTS],
                        shifted[( i + 2 ) % NB_INPUTS] },
                    { points[i], points[( i + 1 ) % NB_INPUTS],
                        points[( i + 2 ) % NB_INPUTS] } ) );
            } );
    }

    void bench_intersections()
    {
        const auto points0 = random_points< 3 >( NB_INPUTS );
        const auto points1 = random_points< 3 >( NB_INPUTS );
        const geode::Point3D center{ { 1, 2, 3 } };
        const geode::Sphere3D sphere{ center, 5 };
        bench( "line_sphere_intersection(3D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return nb_results( geode::line_sphere_intersection(
                    geode::InfiniteLine3D{ { points0[i], points1[i] } },
                    sphere ) );
            } );
        bench( "segment_sphere_intersection(3D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return nb_results( geode::segment_sphere_intersection(
                    { points0[i], points1[i] }, sphere ) );
            } );
        const geode::Plane plane{
            geode::Vector3D{ { 0.2, 0.3, 1 } }.normalize(), center
        };
        bench( "segment_plane_intersection", "random", 1000000,
            [&]( geode::index_t i ) {
                return geode::segment_plane_intersection(
                    { points0[i], points1[i] }, plane )
                    .has_intersection();
            } );
        const geode::Point3D axis0{ { -5, 0, 0 } };
        const geode::Point3D axis1{ { 5, 1, 0 } };
        const geode::Cylinder cylinder{ { axis0, axis1 }, 4 };
        bench( "line_cylinder_intersection", "random", 500000,
            [&]( geode::index_t i ) {
                return nb_results( geode::line_cylinder_intersection(
                    geode::InfiniteLine3D{ { points0[i], points1[i] } },
                    cylinder ) );
            } );
        bench( "segment_cylinder_intersection", "random", 500000,
            [&]( geode::index_t i ) {
                return nb_results( geode::segment_cylinder_intersection(
                    { points0[i], points1[i] }, cylinder ) );
            } );
        const auto points2d0 = random_points< 2 >( NB_INPUTS );
        const auto points2d1 = random_points< 2 >( NB_INPUTS );
        bench( "segment_segment_intersection(2D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return geode::segment_segment_intersection(
                    { points2d0[i], points2d1[i] },
                    { points2d0[( i + 1 ) % NB_INPUTS],
                        points2d1[( i + 1 ) % NB_INPUTS] } )
                    .has_intersection();
            } );
        bench( "segment_line_intersection(2D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return geode::segment_line_intersection(
                    { points2d0[i], points2d1[i] },
                    geode::InfiniteLine2D{ { points2d0[( i + 1 ) % NB_INPUTS],
                        points2d1[( i + 1 ) % NB_INPUTS] } } )
                    .has_intersection();
            } );
        const geode::Frame2D frame{ { geode::Vector2D{ { 3, 1 } },
            geode::Vector2D{ { -1, 2 } } } };
        const geode::Point2D center2d{ { 1, 1 } };
        const geode::Ellipse2D ellipse{ center2d, frame };
        bench( "segment_ellipse_intersection(2D)", "random", 500000,
            [&]( geode::index_t i ) {
                return nb_results( geode::segment_ellipse_intersection(
                    { points2d0[i], points2d1[i] }, ellipse ) );
            } );
        std::vector< geode::OwnerPlane > planes;
        planes.reserve( NB_INPUTS );
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            planes.emplace_back(
                random_vector< 3 >( 1 ).normalize(), points0[i] );
        }
        const geode::Circle circle{ plane, 6 };
        bench( "plane_circle_intersection", "random", 1000000,
            [&]( geode::index_t i ) {
                return nb_results(
                    geode::plane_circle_intersection( planes[i], circle ) );
            } );
        bench( "triangle_circle_intersection", "random", 500000,
            [&]( geode::index_t i ) {
                return nb_results( geode::triangle_circle_intersection(
                    { points0[i], points1[i], points0[( i + 1 ) % NB_INPUTS] },
                    circle ) );
            } );
    }

    void bench_objects()
    {
        const auto points0 = random_points< 3 >( NB_INPUTS );
        const auto points1 = random_points< 3 >( NB_INPUTS );
        bench( "Triangle3D::is_degenerated", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::Triangle3D{ points0[i], points1[i],
                    points0[( i + 1 ) % NB_INPUTS] }
                    .is_degenerated();
            } );
        bench( "Tetrahedron::is_degenerated", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::Tetrahedron{ points0[i], points1[i],
                    points0[( i + 1 ) % NB_INPUTS],
                    points1[( i + 1 ) % NB_INPUTS] }
                    .is_degenerated();
            } );
        const auto points2d = random_points< 2 >( NB_INPUTS );
        bench( "Polygon2D::minimum_height", "random 6 vertices", 1000000,
            [&]( geode::index_t i ) {
                std::vector< geode::RefPoint2D > vertices;
                for( const auto v : geode::LRange{ 6 } )
                {
                    vertices.emplace_back( points2d[( i + v ) % NB_INPUTS] );
                }
                return geode::Polygon2D{ std::move( vertices ) }
                    .minimum_height();
            } );
        std::vector< geode::RefPoint2D > star;
        std::vector< geode::Point2D > star_points;
        for( const auto v : geode::Range{ 20 } )
        {
            const auto angle = 2 * M_PI * v / 20.;
            const auto radius = v % 2 == 0 ? 10. : 4.;
            star_points.emplace_back( geode::Point2D{
                { radius * std::cos( angle ), radius * std::sin( angle ) } } );
        }
        for( const auto& point : star_points )
        {
            star.emplace_back( point );
        }
        const geode::Polygon2D polygon{ star };
        bench( "is_point_inside_polygon", "20-vertex star", 1000000,
            [&]( geode::index_t i ) {
                return geode::is_point_inside_polygon( points2d[i], polygon );
            } );
        bench( "Circle::bounding_box", "random normals", 2000000,
            [&]( geode::index_t i ) {
                const geode::Plane plane{
                    geode::Vector3D{ points1[i] }.normalize(), points0[i]
                };
                return geode::Circle{ plane, 2 }.bounding_box().min().value(
                    0 );
            } );
        bench( "rotate", "random", 2000000, [&]( geode::index_t i ) {
            return geode::rotate( points0[i], geode::Vector3D{ points1[i] },
                points1[i].value( 0 ) )
                .value( 0 );
        } );
        const std::hash< geode::Point3D > hasher;
        bench(
            "std::hash<Point3D>", "random", 5000000, [&]( geode::index_t i ) {
                return static_cast< double >( hasher( points0[i] ) & 1 );
            } );
        bench( "Tetrahedron qualities (4)", "random", 1000000,
            [&]( geode::index_t i ) {
                const geode::Tetrahedron tetra{ points0[i], points1[i],
                    points0[( i + 1 ) % NB_INPUTS],
                    points1[( i + 1 ) % NB_INPUTS] };
                return geode::tetrahedron_aspect_ratio( tetra )
                       + geode::tetrahedron_volume_to_edge_ratio( tetra )
                       + geode::tetrahedron_volume_to_facet_ratio( tetra )
                       + geode::tetrahedron_collapse_aspect_ratio( tetra );
            } );
    }

    void bench_bounding_box()
    {
        const auto points0 = random_points< 3 >( NB_INPUTS );
        const auto points1 = random_points< 3 >( NB_INPUTS );
        geode::BoundingBox3D box;
        box.add_point( geode::Point3D{ { -1, -2, -3 } } );
        box.add_point( geode::Point3D{ { 2, 1, 3 } } );
        bench( "BoundingBox3D::intersects(Triangle3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return box.intersects( geode::Triangle3D{
                    points0[i], points1[i], points0[( i + 1 ) % NB_INPUTS] } );
            } );
        bench( "BoundingBox3D::epsilon_intersects(Triangle3D)", "random",
            2000000, [&]( geode::index_t i ) {
                return box.epsilon_intersects( geode::Triangle3D{
                    points0[i], points1[i], points0[( i + 1 ) % NB_INPUTS] } );
            } );
        geode::BoundingBox1D box1d;
        box1d.add_point( geode::Point1D{ { -1 } } );
        box1d.add_point( geode::Point1D{ { 2 } } );
        bench( "BoundingBox1D::intersects(Ray1D)", "random", 5000000,
            [&]( geode::index_t i ) {
                const geode::Point1D origin{ { points0[i].value( 0 ) } };
                const geode::Vector1D direction{ { points1[i].value( 0 ) } };
                return box1d.intersects( geode::Ray1D{ direction, origin } );
            } );
    }

    void bench_nn_search()
    {
        const geode::NNSearch3D search{ random_points< 3 >( 100000 ) };
        const auto queries = random_points< 3 >( NB_INPUTS );
        bench( "NNSearch3D::closest_neighbor", "100k points", 1000000,
            [&]( geode::index_t i ) {
                return search.closest_neighbor( queries[i] );
            } );
        bench( "NNSearch3D::neighbors(k=5)", "100k points", 500000,
            [&]( geode::index_t i ) {
                return search.neighbors( queries[i], 5 ).front();
            } );
    }

    void bench_predicates()
    {
        const auto points = random_points< 3 >( NB_INPUTS );
        bench( "point_side_to_segment(2D)", "collinear +-1ulp", 2000000,
            [&]( geode::index_t i ) {
                const auto x0 = points[i].value( 0 );
                const auto x1 = points[i].value( 1 );
                const auto x2 = points[i].value( 2 );
                const geode::Point2D p0{ { x0, x0 } };
                const geode::Point2D p1{ { x1, x1 } };
                const geode::Point2D q{ { x2, std::nextafter( x2, 1e300 ) } };
                return static_cast< double >(
                    geode::point_side_to_segment( q, { p0, p1 } ) );
            } );
        bench( "point_side_to_triangle", "coplanar +-1ulp", 1000000,
            [&]( geode::index_t i ) {
                const auto& p = points[i];
                const auto& r = points[( i + 1 ) % NB_INPUTS];
                const geode::Point3D q0{ { p.value( 0 ), p.value( 0 ), 1 } };
                const geode::Point3D q1{ { p.value( 1 ), p.value( 1 ), 1 } };
                const geode::Point3D q2{ { p.value( 2 ), p.value( 2 ), 5 } };
                const geode::Point3D q{ { r.value( 0 ),
                    std::nextafter( r.value( 0 ), 1e300 ), r.value( 1 ) } };
                return static_cast< double >(
                    geode::point_side_to_triangle( q, { q0, q1, q2 } ) );
            } );
    }
} // namespace

void test()
{
    geode::OpenGeodeGeometryLibrary::initialize();
    bench_point_distances();
    bench_triangle_distances();
    bench_intersections();
    bench_objects();
    bench_bounding_box();
    bench_nn_search();
    bench_predicates();
}

OPENGEODE_TEST( "benchmark-tier2-robustness" )
