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

#include <algorithm>
#include <random>

#include <geode/geometry/distance.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/nn_search.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

void first_test()
{
    const geode::NNSearch2D search{ { geode::Point2D{ { 0.1, 4.2 } },
        geode::Point2D{ { 5.9, 7.3 } }, geode::Point2D{ { 1.8, -5 } },
        geode::Point2D{ { -7.3, -1.6 } } } };

    geode::OpenGeodeGeometryException::test(
        search.closest_neighbor( geode::Point2D{ { 0, 0 } } ) == 0,
        "Error in closest neighbor" );
    geode::OpenGeodeGeometryException::test(
        search.closest_neighbor( geode::Point2D{ { 1, -4 } } ) == 2,
        "Error in closest neighbor" );

    const std::vector< geode::index_t > answer_radius{ 0, 2 };
    geode::OpenGeodeGeometryException::test(
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, 5.4 )
            == answer_radius,
        "Error in radius neighbors" );

    const std::vector< geode::index_t > answer_neighbors{ 2, 0 };
    geode::OpenGeodeGeometryException::test(
        search.neighbors( geode::Point2D{ { -1, -1 } }, 2 ) == answer_neighbors,
        "Error in neighbors" );

    const geode::Point3D p0{ { 0.1, 2.9, 5.4 } };
    const geode::Point3D p1{ { 2.4, 8.1, 7.6 } };
    const geode::Point3D p2{ { 8.1, 4.2, 3.8 } };
    const geode::Point3D p3{ { 3.1, 9.4, 9.7 } };
    std::vector< geode::Point3D > points{ p0, p0, p1, p0, p2, p1, p3 };
    const geode::NNSearch3D colocator( points );

    const auto colocated_info =
        colocator.colocated_index_mapping( geode::GLOBAL_EPSILON );
    geode::OpenGeodeGeometryException::test(
        colocated_info.nb_colocated_points() == 3,
        "[Test 1] Should be 3 colocated points" );
    for( const auto p : geode::Indices{ points } )
    {
        geode::OpenGeodeGeometryException::test(
            colocated_info.colocated_mapping[p]
                < colocated_info.unique_points.size(),
            "[Test 1] Wrong value of colocated_mapping (bigger than unique "
            "points size)" );
        const auto& colocated_point =
            colocated_info.unique_points[colocated_info.colocated_mapping[p]];
        geode::OpenGeodeGeometryException::test(
            geode::point_point_distance( points[p], colocated_point )
                <= geode::GLOBAL_EPSILON,
            "[Test 1] Colocated point is not close enough to original point" );
    }
    for( const auto up0 : geode::Indices{ colocated_info.unique_points } )
    {
        for( const auto up1 : geode::Indices{ colocated_info.unique_points } )
        {
            if( up1 <= up0 )
            {
                continue;
            }
            geode::OpenGeodeGeometryException::test(
                geode::point_point_distance( colocated_info.unique_points[up0],
                    colocated_info.unique_points[up1] )
                    > geode::GLOBAL_EPSILON,
                "[Test 1] Colocated points are too close" );
        }
    }
}

void second_test()
{
    geode::Logger::set_level( geode::Logger::LEVEL::trace );
    std::vector< geode::Point2D > points{
        geode::Point2D{ { 0, 0.2 } },
        geode::Point2D{ { 0.25, 0 } },
        geode::Point2D{ { -0.25, 0 } },
        geode::Point2D{ { 0, 0.8 } },
        geode::Point2D{ { 0.25, 1 } },
        geode::Point2D{ { -0.25, 1 } },
    };
    const geode::NNSearch2D colocator{ points };
    const double DISTANCE{ 0.8 };

    const auto colocated_info = colocator.colocated_index_mapping( DISTANCE );
    DEBUG( colocated_info.nb_unique_points() );
    for( const auto v : geode::Range{ 6 } )
    {
        geode::Logger::trace( v, " : ",
            colocated_info.colocated_input_points[v], " / ",
            colocated_info.colocated_mapping[v] );
    }
    for( const auto p : geode::Indices{ points } )
    {
        geode::OpenGeodeGeometryException::test(
            colocated_info.colocated_mapping[p]
                < colocated_info.unique_points.size(),
            "[Test 2] Wrong value of colocated_mapping (bigger than unique "
            "points size)" );
        const auto& colocated_point =
            colocated_info.unique_points[colocated_info.colocated_mapping[p]];
        geode::OpenGeodeGeometryException::test(
            geode::point_point_distance( points[p], colocated_point )
                <= DISTANCE,
            "[Test 2] Colocated point is not close enough to original point" );
    }
    for( const auto up0 : geode::Indices{ colocated_info.unique_points } )
    {
        for( const auto up1 : geode::Indices{ colocated_info.unique_points } )
        {
            if( up1 <= up0 )
            {
                continue;
            }
            geode::OpenGeodeGeometryException::test(
                geode::point_point_distance( colocated_info.unique_points[up0],
                    colocated_info.unique_points[up1] )
                    > DISTANCE,
                "[Test 2] Colocated points are too close" );
        }
    }
    geode::OpenGeodeGeometryException::test(
        colocated_info.nb_unique_points() == 2,
        "[Test 2] Should be 2 unique points" );
}

template < typename Neighbors >
std::vector< geode::index_t > reference_mapping(
    geode::index_t nb_points, const Neighbors& neighbors )
{
    std::vector< geode::index_t > mapping( nb_points, geode::NO_ID );
    for( const auto point_id : geode::Range{ nb_points } )
    {
        if( mapping[point_id] != geode::NO_ID )
        {
            continue;
        }
        for( const auto vertex_id : neighbors( point_id ) )
        {
            if( mapping[vertex_id] == geode::NO_ID )
            {
                mapping[vertex_id] = point_id;
            }
        }
    }
    return mapping;
}

void random_colocation_test()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ 0, 1 };
    std::uniform_real_distribution< double > noise{ -1e-3, 1e-3 };
    std::vector< geode::Point3D > points;
    for( const auto unused : geode::Range{ 3000 } )
    {
        geode_unused( unused );
        points.emplace_back( geode::Point3D{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } } );
    }
    for( const auto p : geode::Range{ 1500 } )
    {
        points.push_back( points[2 * p] );
        points.push_back( points[2 * p]
                          + geode::Point3D{ { noise( generator ),
                              noise( generator ), noise( generator ) } } );
    }
    std::shuffle( points.begin(), points.end(), generator );
    const geode::NNSearch3D search{ points };
    constexpr double epsilon{ 1e-3 };
    const auto info = search.colocated_index_mapping( epsilon );
    const auto expected = reference_mapping(
        search.nb_points(), [&search]( geode::index_t point_id ) {
            return search.radius_neighbors( search.point( point_id ), epsilon );
        } );
    geode::OpenGeodeGeometryException::test(
        info.colocated_input_points == expected,
        "[Test] Wrong random colocated mapping" );

    geode::Frame3D frame;
    frame.set_direction( 0, geode::Vector3D{ { 1e-3, 0, 0 } } );
    frame.set_direction( 1, geode::Vector3D{ { 0, 2e-3, 0 } } );
    frame.set_direction( 2, geode::Vector3D{ { 0, 0, 5e-4 } } );
    const auto frame_info = search.colocated_index_mapping( frame );
    const auto frame_expected = reference_mapping(
        search.nb_points(), [&search, &frame]( geode::index_t point_id ) {
            return search.frame_neighbors( search.point( point_id ), frame );
        } );
    geode::OpenGeodeGeometryException::test(
        frame_info.colocated_input_points == frame_expected,
        "[Test] Wrong random frame colocated mapping" );
    geode::OpenGeodeGeometryException::test(
        info.nb_unique_points() < points.size()
            && frame_info.nb_unique_points() < points.size(),
        "[Test] Random points should have colocated points" );
}

void test()
{
    first_test();
    second_test();
    random_colocation_test();
}

OPENGEODE_TEST( "nnsearch" )