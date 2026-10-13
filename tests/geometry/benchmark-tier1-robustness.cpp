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

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/frame_transform.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/intersection_detection.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/normal_frame_transform.hpp>
#include <geode/geometry/position.hpp>
#include <geode/geometry/sign.hpp>

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

    void bench_coordinate_system()
    {
        const auto points = random_points< 3 >( NB_INPUTS );
        const geode::CoordinateSystem3D coordinate_system{
            { geode::Vector3D{ { 1, 0.2, 0 } },
                geode::Vector3D{ { 0, 1, 0.3 } },
                geode::Vector3D{ { 0.1, 0, 1 } } },
            geode::Point3D{ { 1, 2, 3 } }
        };
        bench( "CoordinateSystem3D::coordinates", "random points", 1000000,
            [&]( geode::index_t i ) {
                return coordinate_system.coordinates( points[i] ).value( 0 );
            } );
        auto modified = coordinate_system;
        bench( "CoordinateSystem3D::set_directions+coordinates",
            "random scales", 200000, [&]( geode::index_t i ) {
                const auto scale = 1. + std::fabs( points[i].value( 0 ) );
                modified.set_directions( { geode::Vector3D{ { scale, 0.2, 0 } },
                    geode::Vector3D{ { 0, 1, 0.3 } },
                    geode::Vector3D{ { 0.1, 0, 1 } } } );
                return modified.coordinates( points[i] ).value( 0 );
            } );
    }

    void bench_frame()
    {
        std::vector< geode::Frame3D > frames;
        for( const auto unused : geode::Range{ NB_INPUTS } )
        {
            geode_unused( unused );
            frames.emplace_back(
                std::array< geode::Vector3D, 3 >{ random_vector< 3 >( 1 ),
                    random_vector< 3 >( 1 ), random_vector< 3 >( 1 ) } );
        }
        bench( "Frame3D::max_elongation_direction", "random frames", 2000000,
            [&]( geode::index_t i ) {
                return frames[i].max_elongation_direction();
            } );
        bench( "Frame3D::min_elongation_direction", "random frames", 2000000,
            [&]( geode::index_t i ) {
                return frames[i].min_elongation_direction();
            } );
        bench( "FrameTransform3D::ctor+apply(Vector)", "random frames", 200000,
            [&]( geode::index_t i ) {
                const geode::FrameTransform3D transform{ frames[i],
                    frames[( i + 1 ) % NB_INPUTS] };
                return transform.apply( geode::Vector3D{ { 1, 2, 3 } } )
                    .value( 0 );
            } );
        bench( "NormalFrameTransform3D::ctor+apply(Vector)", "random frames",
            200000, [&]( geode::index_t i ) {
                const geode::NormalFrameTransform3D transform{ frames[i],
                    frames[( i + 1 ) % NB_INPUTS] };
                return transform.apply( geode::Vector3D{ { 1, 2, 3 } } )
                    .value( 0 );
            } );
    }

    void bench_bounding_box()
    {
        std::vector< geode::BoundingBox3D > boxes( NB_INPUTS );
        for( auto& box : boxes )
        {
            const auto point = random_point< 3 >();
            box.add_point( point );
            box.add_point( point + random_vector< 3 >( 1 ) );
        }
        bench( "BoundingBox3D::add_box", "random boxes", 2000000,
            [&]( geode::index_t i ) {
                auto box = boxes[i];
                box.add_box( boxes[( i + 1 ) % NB_INPUTS] );
                return box.min().value( 0 );
            } );
        const geode::AABBTree3D tree{ boxes };
        const auto queries = random_points< 3 >( NB_INPUTS );
        bench( "AABBTree3D::closest_element_box", "10k random boxes", 200000,
            [&]( geode::index_t i ) {
                const auto [box, distance] = tree.closest_element_box(
                    queries[i],
                    [&]( const geode::Point3D& query, geode::index_t element ) {
                        return geode::point_point_distance(
                            query, boxes[element].center() );
                    } );
                return distance;
            } );
    }

    void bench_triangle()
    {
        const auto points = random_points< 3 >( NB_INPUTS + 2 );
        std::vector< geode::OwnerTriangle3D > triangles;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            triangles.emplace_back( points[i], points[i + 1], points[i + 2] );
        }
        bench( "OwnerTriangle3D::set_point+normal", "random triangles", 1000000,
            [&]( geode::index_t i ) {
                auto& triangle = triangles[i];
                triangle.set_point( 0, points[( i + 3 ) % NB_INPUTS] );
                return triangle.normal()
                    .value_or( geode::Vector3D{} )
                    .value( 0 );
            } );
        std::vector< geode::Triangle3D > ref_triangles;
        std::vector< geode::Triangle2D > ref_triangles2d;
        const auto points2d = random_points< 2 >( NB_INPUTS + 2 );
        std::vector< geode::Point3D > needles;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            ref_triangles.emplace_back(
                points[i], points[i + 1], points[i + 2] );
            ref_triangles2d.emplace_back(
                points2d[i], points2d[i + 1], points2d[i + 2] );
            needles.emplace_back( ( points[i] + points[i + 1] ) / 2.
                                  + random_vector< 3 >( 1e-3 ) );
        }
        std::vector< geode::Triangle3D > needle_triangles;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            needle_triangles.emplace_back(
                points[i], points[i + 1], needles[i] );
        }
        bench( "triangle_area(Triangle3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::triangle_area( ref_triangles[i] );
            } );
        bench( "triangle_area(Triangle3D)", "needles h~1e-3", 2000000,
            [&]( geode::index_t i ) {
                return geode::triangle_area( needle_triangles[i] );
            } );
        bench( "triangle_area(Triangle2D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::triangle_area( ref_triangles2d[i] );
            } );
        const geode::Vector3D up{ { 0, 0, 1 } };
        bench( "triangle_signed_area(Triangle3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::triangle_signed_area( ref_triangles[i], up );
            } );

        std::vector< geode::Point3D > in_plane;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            const auto u = std::fabs( unit_distribution( generator ) );
            const auto v = std::fabs( unit_distribution( generator ) );
            in_plane.emplace_back(
                points[i] + ( points[i + 1] - points[i] ) * ( u / 2 )
                + ( points[i + 2] - points[i] ) * ( v / 2 ) );
        }
        bench( "point_triangle_position(3D)", "points on triangle plane",
            1000000, [&]( geode::index_t i ) {
                return static_cast< double >( geode::point_triangle_position(
                    in_plane[i], ref_triangles[i] ) );
            } );
        bench( "point_triangle_position(3D)", "random points", 1000000,
            [&]( geode::index_t i ) {
                return static_cast< double >( geode::point_triangle_position(
                    points[( i + 5 ) % NB_INPUTS], ref_triangles[i] ) );
            } );

        std::vector< geode::Point3D > segment_ends;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            segment_ends.emplace_back( in_plane[i] + random_vector< 3 >( 5 ) );
        }
        bench( "segment_triangle_intersection", "segments through triangle",
            1000000, [&]( geode::index_t i ) {
                const geode::Point3D other = in_plane[i] * 2. - segment_ends[i];
                const auto result = geode::segment_triangle_intersection(
                    { segment_ends[i], other }, ref_triangles[i] );
                return result.has_intersection() ? 1. : 0.;
            } );
        bench( "segment_triangle_intersection", "segments ending on triangle",
            1000000, [&]( geode::index_t i ) {
                const auto result = geode::segment_triangle_intersection(
                    { segment_ends[i], in_plane[i] }, ref_triangles[i] );
                return result.has_intersection() ? 1. : 0.;
            } );
        bench( "segment_triangle_intersection_detection", "coplanar segments",
            1000000, [&]( geode::index_t i ) {
                const auto result =
                    geode::segment_triangle_intersection_detection(
                        { in_plane[i], points[i + 1] }, ref_triangles[i] );
                return static_cast< double >( result.first );
            } );
    }

    void bench_lines_and_segments()
    {
        const auto points2d = random_points< 2 >( NB_INPUTS + 3 );
        bench( "segment_segment_intersection(2D)", "random", 2000000,
            [&]( geode::index_t i ) {
                const auto result = geode::segment_segment_intersection(
                    geode::Segment2D{ points2d[i], points2d[i + 1] },
                    geode::Segment2D{ points2d[i + 2], points2d[i + 3] } );
                return result.has_intersection() ? 1. : 0.;
            } );
        bench( "line_line_intersection(2D)", "random", 2000000,
            [&]( geode::index_t i ) {
                const auto result = geode::line_line_intersection(
                    geode::InfiniteLine2D{
                        geode::Segment2D{ points2d[i], points2d[i + 1] } },
                    geode::InfiniteLine2D{ geode::Segment2D{
                        points2d[i + 2], points2d[i + 3] } } );
                return result.has_intersection() ? 1. : 0.;
            } );

        const auto points = random_points< 3 >( NB_INPUTS + 3 );
        std::vector< geode::Point3D > coplanar;
        std::vector< geode::Point3D > near_parallel0;
        std::vector< geode::Point3D > near_parallel1;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            const geode::Vector3D u{ points[i], points[i + 1] };
            const geode::Vector3D v{ points[i], points[i + 2] };
            coplanar.emplace_back( points[i] + u * 0.3 + v * 0.4 );
            near_parallel0.emplace_back( points[i] + random_vector< 3 >( 1 ) );
            near_parallel1.emplace_back(
                near_parallel0.back() + u + random_vector< 3 >( 1e-6 ) );
        }
        bench( "segment_segment_distance(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::segment_segment_distance(
                    geode::Segment3D{ points[i], points[i + 1] },
                    geode::Segment3D{ points[i + 2], points[i + 3] } ) );
            } );
        bench( "segment_segment_distance(3D)", "near parallel", 2000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::segment_segment_distance(
                    geode::Segment3D{ points[i], points[i + 1] },
                    geode::Segment3D{
                        near_parallel0[i], near_parallel1[i] } ) );
            } );
        bench( "segment_segment_intersection_detection(3D)", "coplanar",
            2000000, [&]( geode::index_t i ) {
                const auto result =
                    geode::segment_segment_intersection_detection(
                        geode::Segment3D{ points[i], points[i + 1] },
                        geode::Segment3D{ points[i + 2], coplanar[i] } );
                return static_cast< double >( result.first );
            } );
        bench( "segment_segment_intersection_detection(3D)", "random", 2000000,
            [&]( geode::index_t i ) {
                const auto result =
                    geode::segment_segment_intersection_detection(
                        geode::Segment3D{ points[i], points[i + 1] },
                        geode::Segment3D{ points[i + 2], points[i + 3] } );
                return static_cast< double >( result.first );
            } );
    }

    void bench_polygons()
    {
        constexpr geode::index_t nb_vertices{ 8 };
        std::vector< std::vector< geode::Point2D > > polygons2d;
        std::vector< geode::OwnerPolygon3D > polygons3d;
        for( const auto unused : geode::Range{ NB_INPUTS } )
        {
            geode_unused( unused );
            std::vector< geode::Point2D > vertices2d;
            std::vector< geode::Point3D > vertices3d;
            const auto center = random_point< 2 >();
            for( const auto v : geode::Range{ nb_vertices } )
            {
                const auto angle = 2. * M_PI * v / nb_vertices;
                const auto radius =
                    v % 2 == 0 ? 1.
                               : 0.3 + 0.2 * unit_distribution( generator );
                vertices2d.emplace_back( geode::Point2D{
                    { center.value( 0 ) + radius * std::cos( angle ),
                        center.value( 1 ) + radius * std::sin( angle ) } } );
                vertices3d.emplace_back(
                    geode::Point3D{ { vertices2d.back().value( 0 ),
                        vertices2d.back().value( 1 ),
                        0.1 * unit_distribution( generator ) } } );
            }
            polygons2d.emplace_back( std::move( vertices2d ) );
            polygons3d.emplace_back( std::move( vertices3d ) );
        }
        bench( "polygon_area_sign", "concave stars (8 vertices)", 1000000,
            [&]( geode::index_t i ) {
                const auto& vertices = polygons2d[i];
                const geode::Polygon2D polygon{ { vertices[0], vertices[1],
                    vertices[2], vertices[3], vertices[4], vertices[5],
                    vertices[6], vertices[7] } };
                return static_cast< double >(
                    geode::polygon_area_sign( polygon ) );
            } );
        bench( "OwnerPolygon3D::normal", "concave stars (8 vertices)", 1000000,
            [&]( geode::index_t i ) {
                return polygons3d[i]
                    .normal()
                    .value_or( geode::Vector3D{} )
                    .value( 2 );
            } );
        bench( "OwnerPolygon3D::triangulate", "concave stars (8 vertices)",
            200000, [&]( geode::index_t i ) {
                return static_cast< double >(
                    polygons3d[i].triangulate().size() );
            } );
    }

    void bench_volumes_and_distances()
    {
        const auto points = random_points< 3 >( NB_INPUTS + 4 );
        std::vector< geode::Tetrahedron > tetrahedra;
        std::vector< geode::Point3D > flat_tops;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            flat_tops.emplace_back(
                ( points[i] + points[i + 1] + points[i + 2] ) / 3.
                + random_vector< 3 >( 1e-3 ) );
        }
        std::vector< geode::Tetrahedron > flat_tetrahedra;
        for( const auto i : geode::Range{ NB_INPUTS } )
        {
            tetrahedra.emplace_back(
                points[i], points[i + 1], points[i + 2], points[i + 3] );
            flat_tetrahedra.emplace_back(
                points[i], points[i + 1], points[i + 2], flat_tops[i] );
        }
        bench( "tetrahedron_signed_volume", "random", 2000000,
            [&]( geode::index_t i ) {
                return geode::tetrahedron_signed_volume( tetrahedra[i] );
            } );
        bench( "tetrahedron_signed_volume", "flat (h~1e-3)", 2000000,
            [&]( geode::index_t i ) {
                return geode::tetrahedron_signed_volume( flat_tetrahedra[i] );
            } );
        const auto queries = random_points< 3 >( NB_INPUTS );
        bench( "point_tetrahedron_distance", "random", 1000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >( geode::point_tetrahedron_distance(
                    queries[i], tetrahedra[i] ) );
            } );

        const geode::Point3D center{ { 0, 0, 10 } };
        const geode::Vector3D normal{ { 0.2, 0.1, 1 } };
        const geode::Plane plane{ normal.normalize(), center };
        const geode::Circle circle{ plane, 3 };
        bench( "point_circle_signed_distance", "random", 2000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >(
                    geode::point_circle_signed_distance( queries[i], circle ) );
            } );

        const geode::Frame2D frame2d{ { geode::Vector2D{ { 1, 2 } },
            geode::Vector2D{ { -6, 3 } } } };
        const geode::Ellipse2D ellipse2d{ geode::Point2D{ { 1, -1 } },
            frame2d };
        const auto queries2d = random_points< 2 >( NB_INPUTS );
        bench( "point_ellipse_distance(2D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >(
                    geode::point_ellipse_distance( queries2d[i], ellipse2d ) );
            } );
        const geode::Frame3D frame3d{ { geode::Vector3D{ { 0, 0, 1 } },
            geode::Vector3D{ { 0, 4, 0 } }, geode::Vector3D{ { 2, 0, 0 } } } };
        const geode::Ellipse3D ellipse3d{ geode::Point3D{ { 1, -1, 2 } },
            frame3d };
        bench( "point_ellipse_distance(3D)", "random", 1000000,
            [&]( geode::index_t i ) {
                return std::get< 0 >(
                    geode::point_ellipse_distance( queries[i], ellipse3d ) );
            } );
    }
} // namespace

void test()
{
    bench_coordinate_system();
    bench_frame();
    bench_bounding_box();
    bench_triangle();
    bench_lines_and_segments();
    bench_polygons();
    bench_volumes_and_distances();
}

OPENGEODE_TEST( "benchmark-tier1-robustness" )
