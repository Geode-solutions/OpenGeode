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
#include <chrono>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/barycentric_coordinates.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/nn_search.hpp>
#include <geode/geometry/points_sort.hpp>
#include <geode/geometry/position.hpp>
#include <geode/geometry/quality.hpp>

#include <geode/tests/common.hpp>

namespace
{
    constexpr geode::index_t NB_INPUTS{ 10000 };
    volatile double sink{ 0 };

    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -10, 10 };
    std::uniform_real_distribution< double > unit_distribution{ -1, 1 };

    template < geode::index_t dimension >
    geode::Point< dimension > random_point()
    {
        geode::Point< dimension > point;
        for( const auto d : geode::LRange{ dimension } )
        {
            point.set_value( d, distribution( generator ) );
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

    void report( const std::string& name, const std::string& input, double ns )
    {
        geode::Logger::info( "BENCH|", name, "|", input, "|", ns );
    }

    // ns per call of function( i ), best of 3 repetitions
    template < typename Function >
    void bench( const std::string& name,
        const std::string& input,
        geode::index_t nb_calls,
        const Function& function )
    {
        double best{ std::numeric_limits< double >::max() };
        for( const auto repetition : geode::Range{ 4 } )
        {
            double accumulator{ 0 };
            const auto start = std::chrono::steady_clock::now();
            for( const auto i : geode::Range{ nb_calls } )
            {
                accumulator += function( i % NB_INPUTS );
            }
            const auto end = std::chrono::steady_clock::now();
            sink = sink + accumulator;
            if( repetition > 0 ) // first one is a warm up
            {
                best = std::min( best,
                    std::chrono::duration< double, std::nano >( end - start )
                            .count()
                        / nb_calls );
            }
        }
        report( name, input, best );
    }

    // ns per item of a whole operation, best of 3 repetitions
    template < typename Function >
    void bench_once( const std::string& name,
        const std::string& input,
        geode::index_t nb_items,
        const Function& function )
    {
        double best{ std::numeric_limits< double >::max() };
        for( const auto unused : geode::Range{ 3 } )
        {
            geode_unused( unused );
            const auto start = std::chrono::steady_clock::now();
            sink = sink + function();
            const auto end = std::chrono::steady_clock::now();
            best = std::min(
                best, std::chrono::duration< double, std::nano >( end - start )
                              .count()
                          / nb_items );
        }
        report( name, input, best );
    }

    void bench_point()
    {
        const auto points = random_points< 3 >( NB_INPUTS + 3 );
        bench( "Vector3D cross+dot+length (client code)", "random", 5000000,
            [&]( geode::index_t i ) {
                const geode::Vector3D a{ points[i], points[i + 1] };
                const geode::Vector3D b{ points[i], points[i + 2] };
                return a.cross( b ).length() + a.dot( b );
            } );
        bench( "Point3D operators (client code)", "random", 5000000,
            [&]( geode::index_t i ) {
                const auto result =
                    ( points[i] + points[i + 1] ) * 0.5 - points[i + 2];
                return result.value( 0 )
                       + ( points[i] < points[i + 2] ? 1. : 0. );
            } );
    }

    void bench_position()
    {
        const auto points = random_points< 3 >( NB_INPUTS + 4 );
        const auto points2d = random_points< 2 >( NB_INPUTS + 4 );
        std::vector< geode::Point3D > on_segments;
        std::vector< geode::Point2D > on_segments2d;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            on_segments.push_back(
                points[i] + ( points[i + 1] - points[i] ) * 0.37 );
            on_segments2d.push_back(
                points2d[i] + ( points2d[i + 1] - points2d[i] ) * 0.37 );
        }
        bench( "point_tetrahedron_position", "random", 1000000,
            [&]( geode::index_t i ) {
                return static_cast< double >( geode::point_tetrahedron_position(
                    points[( i + 7 ) % NB_INPUTS],
                    geode::Tetrahedron{ points[i], points[i + 1], points[i + 2],
                        points[i + 3] } ) );
            } );
        bench( "point_tetrahedron_position", "barycenter", 1000000,
            [&]( geode::index_t i ) {
                const geode::Tetrahedron tetra{ points[i], points[i + 1],
                    points[i + 2], points[i + 3] };
                return static_cast< double >( geode::point_tetrahedron_position(
                    tetra.barycenter(), tetra ) );
            } );
        bench( "point_triangle_position(3D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return static_cast< double >( geode::point_triangle_position(
                    points[( i + 7 ) % NB_INPUTS],
                    geode::Triangle3D{
                        points[i], points[i + 1], points[i + 2] } ) );
            } );
        bench( "point_triangle_position(3D)", "barycenter (in plane)", 1000000,
            [&]( geode::index_t i ) {
                const geode::Triangle3D triangle{ points[i], points[i + 1],
                    points[i + 2] };
                return static_cast< double >( geode::point_triangle_position(
                    triangle.barycenter(), triangle ) );
            } );
        bench( "point_triangle_position(2D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return static_cast< double >( geode::point_triangle_position(
                    points2d[( i + 7 ) % NB_INPUTS],
                    geode::Triangle2D{
                        points2d[i], points2d[i + 1], points2d[i + 2] } ) );
            } );
        bench( "are_points_aligned(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return static_cast< double >( geode::are_points_aligned(
                    points[i], points[i + 1], points[i + 2] ) );
            } );
        bench( "point_segment_position(3D)", "points on segment", 1000000,
            [&]( geode::index_t i ) {
                return static_cast< double >(
                    geode::point_segment_position( on_segments[i],
                        geode::Segment3D{ points[i], points[i + 1] } ) );
            } );
        bench( "point_segment_position(2D)", "points on segment", 1000000,
            [&]( geode::index_t i ) {
                return static_cast< double >(
                    geode::point_segment_position( on_segments2d[i],
                        geode::Segment2D{ points2d[i], points2d[i + 1] } ) );
            } );
    }

    void bench_distance_and_intersection()
    {
        const auto points = random_points< 3 >( NB_INPUTS + 4 );
        std::vector< geode::Point3D > needles;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            needles.push_back( ( points[i] + points[i + 1] ) / 2.
                               + random_vector< 3 >( 1e-3 ) );
        }
        bench( "point_triangle_distance(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::point_triangle_distance(
                    points[( i + 7 ) % NB_INPUTS],
                    geode::Triangle3D{
                        points[i], points[i + 1], points[i + 2] } ) );
            } );
        bench( "point_triangle_distance(3D)", "needles h~1e-3", 2000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::point_triangle_distance(
                    points[( i + 7 ) % NB_INPUTS],
                    geode::Triangle3D{
                        points[i], points[i + 1], needles[i] } ) );
            } );
        const geode::Vector3D offset{ { 0.1, 0.2, 3 } };
        bench( "segment_triangle_intersection", "segments through triangle",
            1000000, [&]( geode::index_t i ) {
                const geode::Triangle3D triangle{ points[i], points[i + 1],
                    points[i + 2] };
                const auto center = triangle.barycenter();
                const auto top = center + offset;
                const auto bottom = center - offset;
                const auto result = geode::segment_triangle_intersection(
                    geode::Segment3D{ top, bottom }, triangle );
                return result.has_intersection() ? 1. : 0.;
            } );
        bench( "line_triangle_intersection", "lines through triangle", 1000000,
            [&]( geode::index_t i ) {
                const geode::Triangle3D triangle{ points[i], points[i + 1],
                    points[i + 2] };
                const auto center = triangle.barycenter();
                const auto result = geode::line_triangle_intersection(
                    geode::InfiniteLine3D{ offset, center }, triangle );
                return result.has_intersection() ? 1. : 0.;
            } );
        bench( "line_plane_intersection", "random", 1000000,
            [&]( geode::index_t i ) {
                const geode::Plane plane{
                    geode::Vector3D{ points[i], points[i + 1] }.normalize(),
                    points[i + 2]
                };
                const auto result = geode::line_plane_intersection(
                    geode::InfiniteLine3D{
                        geode::Vector3D{ points[i + 3], points[i] },
                        points[i + 3] },
                    plane );
                return result.has_intersection() ? 1. : 0.;
            } );
        const auto points2d = random_points< 2 >( NB_INPUTS + 4 );
        bench( "line_line_intersection(2D)", "random", 1000000,
            [&]( geode::index_t i ) {
                const auto result = geode::line_line_intersection(
                    geode::InfiniteLine2D{
                        geode::Segment2D{ points2d[i], points2d[i + 1] } },
                    geode::InfiniteLine2D{ geode::Segment2D{
                        points2d[i + 2], points2d[i + 3] } } );
                return result.has_intersection() ? 1. : 0.;
            } );
    }

    struct HeavyPairAction
    {
        bool operator()( geode::index_t box0, geode::index_t box1 ) const
        {
            volatile double value = box0;
            for( const auto unused : geode::Range{ 300 } )
            {
                geode_unused( unused );
                value = value * 1.0000001 + box1;
            }
            return value > -1;
        }
    };

    void bench_aabb()
    {
        const auto centers = random_points< 3 >( 200000 );
        std::vector< geode::BoundingBox3D > boxes( centers.size() );
        for( const auto i : geode::Indices{ centers } )
        {
            boxes[i].add_point( centers[i] );
            boxes[i].add_point( centers[i] + random_vector< 3 >( 0.3 ) );
            boxes[i].add_point( centers[i] + random_vector< 3 >( 0.3 ) );
        }
        bench_once( "AABBTree3D build (ns/box)", "200k random boxes",
            boxes.size(), [&] {
                const geode::AABBTree3D tree{ boxes };
                return static_cast< double >( tree.nb_bboxes() );
            } );
        const geode::AABBTree3D tree{ boxes };
        const auto queries = random_points< 3 >( NB_INPUTS );
        bench( "AABBTree3D::closest_element_box", "200k boxes, center distance",
            100000, [&]( geode::index_t i ) {
                return std::get< 1 >( tree.closest_element_box( queries[i],
                    [&]( const geode::Point3D& query, geode::index_t element ) {
                        return geode::point_point_distance(
                            query, boxes[element].center() );
                    } ) );
            } );

        std::vector< geode::BoundingBox2D > strips( 20000 );
        for( const auto i : geode::Indices{ strips } )
        {
            const auto y = 10. * i / strips.size();
            strips[i].add_point( geode::Point2D{ { 0, y } } );
            strips[i].add_point( geode::Point2D{ { 100, y + 0.0005 } } );
        }
        std::shuffle( strips.begin(), strips.end(), generator );
        const geode::AABBTree2D strip_tree{ strips };
        const auto queries2d = random_points< 2 >( NB_INPUTS );
        bench( "AABBTree2D box query", "20k shuffled thin strips", 20000,
            [&]( geode::index_t i ) {
                geode::BoundingBox2D box;
                box.add_point( queries2d[i] );
                box.add_point(
                    queries2d[i] + geode::Point2D{ { 0.01, 0.01 } } );
                double nb_found{ 0 };
                auto action = [&nb_found]( geode::index_t ) {
                    nb_found++;
                    return false;
                };
                strip_tree.compute_bbox_element_bbox_intersections(
                    box, action );
                return nb_found;
            } );

        std::vector< geode::BoundingBox3D > small_boxes( 10 );
        for( auto& box : small_boxes )
        {
            const auto point = random_point< 3 >();
            box.add_point( point );
            box.add_point( point + random_vector< 3 >( 3 ) );
        }
        const geode::AABBTree3D small_tree{ small_boxes };
        bench_once( "AABBTree3D compute_other (us total)",
            "10 boxes vs 200k, heavy action", 1000, [&] {
                return static_cast< double >( small_tree
                        .compute_other_element_bbox_intersections(
                            tree, HeavyPairAction{} )
                        .size() );
            } );
    }

    void bench_triangle_storage()
    {
        const auto points = random_points< 2 >( NB_INPUTS + 2 );
        bench_once( "std::vector<OwnerTriangle2D> fill (ns/triangle)", "10k",
            NB_INPUTS, [&] {
                std::vector< geode::OwnerTriangle2D > triangles;
                triangles.reserve( NB_INPUTS );
                for( const auto i : geode::Range{ NB_INPUTS } )
                {
                    triangles.emplace_back(
                        points[i], points[i + 1], points[i + 2] );
                }
                return triangles.back().vertices()[0].value( 0 );
            } );
    }

    void bench_nn_search()
    {
        auto points = random_points< 3 >( 200000 );
        for( const auto i : geode::Range{ 50000 } )
        {
            points.push_back( points[3 * i] + random_vector< 3 >( 1e-4 ) );
        }
        bench_once(
            "NNSearch3D build (ns/point)", "250k points", points.size(), [&] {
                const geode::NNSearch3D search{ points };
                return static_cast< double >( search.nb_points() );
            } );
        const geode::NNSearch3D search{ points };
        bench_once( "NNSearch3D::colocated_index_mapping(1e-3) (ns/point)",
            "250k points, 50k duplicates", points.size(), [&] {
                return static_cast< double >(
                    search.colocated_index_mapping( 1e-3 ).nb_unique_points() );
            } );
        geode::Frame3D frame;
        frame.set_direction( 0, geode::Vector3D{ { 1e-3, 0, 0 } } );
        frame.set_direction( 1, geode::Vector3D{ { 0, 2e-3, 0 } } );
        frame.set_direction( 2, geode::Vector3D{ { 0, 0, 1e-3 } } );
        bench_once( "NNSearch3D::colocated_index_mapping(Frame) (ns/point)",
            "250k points, 50k duplicates", points.size(), [&] {
                return static_cast< double >(
                    search.colocated_index_mapping( frame )
                        .nb_unique_points() );
            } );
    }

    void bench_points_sort()
    {
        const auto points = random_points< 3 >( 500000 );
        std::vector< geode::Point3D > flat_points;
        for( const auto& point : points )
        {
            flat_points.emplace_back(
                geode::Point3D{ { point.value( 0 ), point.value( 1 ), 0 } } );
        }
        bench_once(
            "morton_mapping(3D) (ns/point)", "500k random", points.size(), [&] {
                return static_cast< double >(
                    geode::morton_mapping< 3 >( points ).front() );
            } );
        std::vector< geode::index_t > mapping;
        bench_once( "morton_mapping(3D) (ns/point)", "500k flat (z=0)",
            points.size(), [&] {
                mapping = geode::morton_mapping< 3 >( flat_points );
                return static_cast< double >( mapping.front() );
            } );
        double step{ 0 };
        for( const auto i : geode::Range{ 1, mapping.size() } )
        {
            step += geode::point_point_distance(
                flat_points[mapping[i - 1]], flat_points[mapping[i]] );
        }
        report( "morton_mapping(3D) mean consecutive step (x1e3, not ns)",
            "500k flat (z=0)", 1e3 * step / mapping.size() );
    }

    void bench_bounding_box()
    {
        const auto points = random_points< 3 >( NB_INPUTS + 4 );
        const auto points2d = random_points< 2 >( NB_INPUTS + 4 );
        std::vector< geode::BoundingBox3D > boxes( NB_INPUTS );
        std::vector< geode::BoundingBox2D > boxes2d( NB_INPUTS );
        std::vector< geode::Point3D > crossing0;
        std::vector< geode::Point3D > crossing1;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            const auto corner = random_point< 3 >();
            boxes[i].add_point( corner );
            boxes[i].add_point( corner + geode::Point3D{ { 2, 2, 2 } } );
            const auto corner2d = random_point< 2 >();
            boxes2d[i].add_point( corner2d );
            boxes2d[i].add_point( corner2d + geode::Point2D{ { 2, 2 } } );
            const auto center = boxes[i].center();
            crossing0.push_back(
                center
                + geode::Point3D{
                    { -1.5, 3 * unit_distribution( generator ), 1.2 } } );
            crossing1.push_back(
                center
                + geode::Point3D{
                    { 1.5, 3 * unit_distribution( generator ), -1.3 } } );
        }
        bench( "BoundingBox3D::intersects(Segment3D)", "crossing box bbox",
            2000000, [&]( geode::index_t i ) {
                return static_cast< double >( boxes[i].intersects(
                    geode::Segment3D{ crossing0[i], crossing1[i] } ) );
            } );
        bench( "BoundingBox3D::epsilon_intersects(Segment3D)",
            "crossing box bbox", 2000000, [&]( geode::index_t i ) {
                return static_cast< double >( boxes[i].epsilon_intersects(
                    geode::Segment3D{ crossing0[i], crossing1[i] } ) );
            } );
        bench( "BoundingBox2D::intersects(Triangle2D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return static_cast< double >(
                    boxes2d[i].intersects( geode::Triangle2D{
                        points2d[i], points2d[i + 1], points2d[i + 2] } ) );
            } );
        bench( "BoundingBox3D::intersects(Tetrahedron)", "random", 1000000,
            [&]( geode::index_t i ) {
                return static_cast< double >(
                    boxes[i].intersects( geode::Tetrahedron{ points[i],
                        points[i + 1], points[i + 2], points[i + 3] } ) );
            } );
    }

    void bench_quality_and_barycentric()
    {
        const auto points = random_points< 3 >( NB_INPUTS + 4 );
        bench( "triangle_angle_based_quality(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::triangle_angle_based_quality( geode::Triangle3D{
                    points[i], points[i + 1], points[i + 2] } );
            } );
        bench( "triangle_angle_based_quality(3D)", "degenerated", 200000,
            [&]( geode::index_t i ) {
                return geode::triangle_angle_based_quality(
                    geode::Triangle3D{ points[i], points[i], points[i + 2] } );
            } );
        bench( "safe_triangle_barycentric_coordinates(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::safe_triangle_barycentric_coordinates(
                    points[i + 3], geode::Triangle3D{ points[i], points[i + 1],
                                       points[i + 2] } )[0];
            } );
        bench( "safe_triangle_barycentric_coordinates(3D)", "degenerated",
            200000, [&]( geode::index_t i ) {
                return geode::safe_triangle_barycentric_coordinates(
                    points[i + 3], geode::Triangle3D{ points[i], points[i],
                                       points[i + 2] } )[0];
            } );
        bench( "safe_segment_barycentric_coordinates(3D)", "degenerated",
            200000, [&]( geode::index_t i ) {
                return geode::safe_segment_barycentric_coordinates(
                    points[i + 3],
                    geode::Segment3D{ points[i], points[i] } )[0];
            } );
        bench( "safe_tetrahedron_barycentric_coordinates", "flat", 200000,
            [&]( geode::index_t i ) {
                return geode::safe_tetrahedron_barycentric_coordinates(
                    points[i + 3], geode::Tetrahedron{ points[i], points[i + 1],
                                       points[i + 2], points[i + 1] } )[0];
            } );
        bench( "detail::position_to_index", "all positions", 5000000,
            []( geode::index_t i ) {
                return static_cast< double >( geode::detail::position_to_index(
                    static_cast< geode::POSITION >( i % 21 ) ) );
            } );
        bench( "detail::position_to_string", "all positions", 2000000,
            []( geode::index_t i ) {
                return static_cast< double >( geode::detail::position_to_string(
                    static_cast< geode::POSITION >( i % 21 ) )
                        .size() );
            } );
    }
} // namespace

void test()
{
    bench_point();
    bench_position();
    bench_distance_and_intersection();
    bench_aabb();
    bench_triangle_storage();
    bench_nn_search();
    bench_points_sort();
    bench_bounding_box();
    bench_quality_and_barycentric();
}

OPENGEODE_TEST( "benchmark-tier3-performance" )
