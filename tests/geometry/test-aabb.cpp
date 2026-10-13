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

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

#include <absl/algorithm/container.h>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <algorithm>
#include <random>

template < geode::index_t dimension >
geode::BoundingBox< dimension > create_bounding_box(
    const geode::Point< dimension >& center, double box_size )
{
    geode::Point< dimension > temp;
    for( const auto c : geode::LRange{ dimension } )
    {
        temp.set_value( c, box_size );
    }
    geode::BoundingBox< dimension > box;
    box.add_point( center - temp );
    box.add_point( center + temp );
    return box;
}

template < geode::index_t dimension >
std::vector< geode::BoundingBox< dimension > > create_box_vector(
    geode::index_t nb_box_range, double box_size )
{
    std::vector< geode::BoundingBox< dimension > > box_vector(
        nb_box_range * nb_box_range );
    for( const auto i : geode::Range{ nb_box_range } )
    {
        for( const auto j : geode::Range{ nb_box_range } )
        {
            geode::Point< dimension > point;
            point.set_value( 0, i );
            point.set_value( 1, j );
            box_vector[nb_box_range * i + j] =
                create_bounding_box( point, box_size );
        }
    }
    return box_vector;
}

geode::index_t global_box_index(
    geode::index_t i, geode::index_t j, geode::index_t size )
{
    return size * i + j;
}

template < geode::index_t dimension >
void test_build_aabb()
{
    geode::Logger::info( "TEST", "Build AABB ", dimension, "D" );
    const geode::index_t nb_boxes{ 100 };
    const double box_size{ 0.25 };

    // Create a grid of non overlapping boxes
    const auto box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    const geode::AABBTree< dimension > aabb{ box_vector };

    geode::OpenGeodeGeometryException::test(
        aabb.nb_bboxes() == box_vector.size(),
        "Build AABB - Wrong number of boxes in the tree" );
}

template < geode::index_t dimension >
class BoxAABBEvalDistance
{
public:
    BoxAABBEvalDistance(
        absl::Span< const geode::BoundingBox< dimension > > bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }
    ~BoxAABBEvalDistance() = default;

    double operator()( const geode::Point< dimension >& query,
        geode::index_t current_element_box ) const
    {
        const auto box_center =
            ( bounding_boxes_[current_element_box].min()
                + bounding_boxes_[current_element_box].max() )
            / 2.;
        return geode::point_point_distance( box_center, query );
    }

private:
    absl::Span< const geode::BoundingBox< dimension > > bounding_boxes_;
};

template < geode::index_t dimension >
void test_nearest_neighbor_search()
{
    geode::Logger::info(
        "TEST", " Nearest box to point AABB ", dimension, "D" );
    const geode::index_t nb_boxes{ 10 };
    const double box_size{ 0.75 };
    const auto box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    const geode::AABBTree< dimension > aabb{ box_vector };

    const BoxAABBEvalDistance< dimension > disteval{ box_vector };

    for( const auto i : geode::Range{ nb_boxes } )
    {
        for( const auto j : geode::Range{ nb_boxes } )
        {
            geode::Point< dimension > box_center;
            box_center.set_value( 0, i );
            box_center.set_value( 1, j );

            geode::Point< dimension > query;
            // query point is in the up right corner of each box.
            query.set_value( 0, i + box_size / 2. );
            query.set_value( 1, j + box_size / 2. );
            geode::index_t box_id;
            double distance;
            std::tie( box_id, distance ) =
                aabb.closest_element_box( query, disteval );

            geode::OpenGeodeGeometryException::test(
                box_id == global_box_index( i, j, nb_boxes ),
                " Nearest box to point AABB - Wrong nearest box index" );
            geode::OpenGeodeGeometryException::test(
                distance == geode::point_point_distance( box_center, query ),
                " Nearest box to point AABB - Wrong distance to nearest "
                "box center" );

            const auto boxes = aabb.containing_boxes( box_center );
            geode::OpenGeodeGeometryException::test( boxes.size() == 1,
                "Containing box AABB - Wrong number of boxes" );
            geode::OpenGeodeGeometryException::test(
                boxes[0] == box_id, "Containing box AABB - Wrong box index" );
        }
    }
}

template < geode::index_t dimension >
class BoxAABBIntersection
{
public:
    BoxAABBIntersection(
        absl::Span< const geode::BoundingBox< dimension > > bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }
    ~BoxAABBIntersection() = default;

    bool operator()( geode::index_t cur_box )
    {
        std::lock_guard< std::mutex > lock( mutex_ );
        box_intersections_.emplace( cur_box );
        return false;
    }

public:
    std::mutex mutex_;
    absl::flat_hash_set< geode::index_t > box_intersections_;

private:
    absl::Span< const geode::BoundingBox< dimension > > bounding_boxes_;
};

template < geode::index_t dimension >
class BoxAABBInclusion
{
public:
    explicit BoxAABBInclusion(
        absl::Span< const geode::BoundingBox< dimension > > bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }

    [[nodiscard]] bool box_contains_box(
        geode::index_t box1, geode::index_t box2 ) const
    {
        return bounding_boxes_[box1].contains( bounding_boxes_[box2].min() )
               && bounding_boxes_[box1].contains( bounding_boxes_[box2].max() );
    }

    [[nodiscard]] bool operator()(
        geode::index_t box1, geode::index_t box2 ) const
    {
        return box_contains_box( box1, box2 ) || box_contains_box( box2, box1 );
    }

private:
    absl::Span< const geode::BoundingBox< dimension > > bounding_boxes_;
};

template < geode::index_t dimension >
void test_intersections_with_query_box()
{
    geode::Logger::info(
        "TEST", " Box-Box intersection AABB ", dimension, "D " );
    const geode::index_t nb_boxes{ 10 };
    const double box_size{ 0.5 };
    const auto box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    geode::AABBTree< dimension > aabb{ box_vector };
    BoxAABBIntersection< dimension > eval_intersection{ box_vector };
    for( const auto i : geode::Range{ nb_boxes - 1 } )
    {
        for( const auto j : geode::Range{ nb_boxes - 1 } )
        {
            geode::Point< dimension > query;
            // query boxes will be at internal corner grid
            query.set_value( 0, i + box_size );
            query.set_value( 1, j + box_size );
            const auto box_query = create_bounding_box( query, box_size );

            eval_intersection.box_intersections_.clear();
            aabb.compute_bbox_element_bbox_intersections(
                box_query, eval_intersection );

            geode::OpenGeodeGeometryException::test(
                eval_intersection.box_intersections_.size() == 4,
                " Box-Box intersection - Wrong number of intersected "
                "boxes" );

            absl::flat_hash_set< geode::index_t > expected_set;
            expected_set.emplace( global_box_index( i, j, nb_boxes ) );
            expected_set.emplace( global_box_index( i, j + 1, nb_boxes ) );
            expected_set.emplace( global_box_index( i + 1, j, nb_boxes ) );
            expected_set.emplace( global_box_index( i + 1, j + 1, nb_boxes ) );
            geode::OpenGeodeGeometryException::test(
                eval_intersection.box_intersections_ == expected_set,
                "Box-Box intersection - Wrong set of boxes" );
        }
    }
    geode::Point< dimension > query;
    // query box will be at top right corner grid
    query.set_value( 0, nb_boxes - 1 + box_size );
    query.set_value( 1, nb_boxes - 1 + box_size );
    const auto box_query = create_bounding_box( query, box_size );

    eval_intersection.box_intersections_.clear();
    aabb.compute_bbox_element_bbox_intersections(
        box_query, eval_intersection );

    geode::OpenGeodeGeometryException::test(
        eval_intersection.box_intersections_.size() == 1,
        "Box-Box intersection - Wrong number of intersected boxes" );

    absl::flat_hash_set< geode::index_t > expected_set;
    expected_set.emplace(
        global_box_index( nb_boxes - 1, nb_boxes - 1, nb_boxes ) );
    geode::OpenGeodeGeometryException::test(
        eval_intersection.box_intersections_ == expected_set,
        "Box-Box intersection - Wrong set of boxes" );
}

template < geode::index_t dimension >
class RayAABBIntersection
{
public:
    RayAABBIntersection(
        absl::Span< const geode::BoundingBox< dimension > > bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }

    bool operator()( geode::index_t cur_box )
    {
        std::lock_guard< std::mutex > lock( mutex_ );
        box_intersections_.emplace( cur_box );
        return false;
    }

public:
    std::mutex mutex_;
    absl::flat_hash_set< geode::index_t > box_intersections_;

private:
    absl::Span< const geode::BoundingBox< dimension > > bounding_boxes_;
};

template < geode::index_t dimension >
void test_intersections_with_ray_trace()
{
    geode::Logger::info(
        "TEST", " Box-Ray intersection AABB ", dimension, "D " );

    const geode::index_t nb_boxes{ 10 };
    const double box_size{ 0.5 };
    const auto box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    geode::AABBTree< dimension > aabb{ box_vector };
    RayAABBIntersection< dimension > eval_intersection{ box_vector };
    geode::Vector< dimension > ray_direction;
    ray_direction.set_value( 1, 1.0 );

    for( const auto i : geode::Range{ nb_boxes } )
    {
        geode::Point< dimension > ray_origin;
        ray_origin.set_value( 0, i );
        ray_origin.set_value( 1, i );

        geode::Ray< dimension > query{ ray_direction, ray_origin };

        eval_intersection.box_intersections_.clear();
        aabb.compute_ray_element_bbox_intersections( query, eval_intersection );

        geode::OpenGeodeGeometryException::test(
            eval_intersection.box_intersections_.size() == nb_boxes - i,
            "Box-Ray intersection - Wrong number of boxes intersected "
            "by the ray " );

        absl::flat_hash_set< geode::index_t > expected_set;
        for( const auto c : geode::Range{ nb_boxes - i } )
        {
            expected_set.emplace( global_box_index( i, c + i, nb_boxes ) );
        }

        geode::OpenGeodeGeometryException::test(
            eval_intersection.box_intersections_ == expected_set,
            "Box-Ray intersection - Wrong set of boxes" );
    }
    // ray between two boxes
    geode::Point< dimension > ray_origin;
    ray_origin.set_value( 0, box_size );
    ray_origin.set_value( 1, box_size );

    geode::Ray< dimension > query{ ray_direction, ray_origin };

    eval_intersection.box_intersections_.clear();
    aabb.compute_ray_element_bbox_intersections( query, eval_intersection );

    geode::OpenGeodeGeometryException::test(
        eval_intersection.box_intersections_.size() == 2 * nb_boxes,
        "Box-Ray intersection - Wrong number of boxes intersected by "
        "the ray " );
    absl::flat_hash_set< geode::index_t > expected_set;
    for( const auto c : geode::Range{ nb_boxes } )
    {
        expected_set.emplace( global_box_index( 0, c, nb_boxes ) );
        expected_set.emplace( global_box_index( 1, c, nb_boxes ) );
    }
    geode::OpenGeodeGeometryException::test(
        eval_intersection.box_intersections_ == expected_set,
        "Box-Ray intersection - Wrong set of boxes" );

    // oblique ray
    ray_direction.set_value( 0, 1.0 );
    ray_direction.set_value( 1, 1.0 );

    ray_origin.set_value( 0, box_size );
    ray_origin.set_value( 1, box_size );

    geode::Ray< dimension > query2{ ray_direction, ray_origin };

    eval_intersection.box_intersections_.clear();
    aabb.compute_ray_element_bbox_intersections( query2, eval_intersection );

    geode::OpenGeodeGeometryException::test(
        eval_intersection.box_intersections_.size() == 3 * ( nb_boxes - 1 ) + 1,
        "Box-Ray intersection - Wrong number of boxes intersected by "
        "the ray " );

    expected_set.clear();
    expected_set.emplace( global_box_index( 0, 0, nb_boxes ) );
    expected_set.emplace( global_box_index( 1, 0, nb_boxes ) );
    for( const auto c : geode::Range{ nb_boxes - 2 } )
    {
        expected_set.emplace( global_box_index( c, c + 1, nb_boxes ) );
        expected_set.emplace( global_box_index( c + 1, c + 1, nb_boxes ) );
        expected_set.emplace( global_box_index( c + 2, c + 1, nb_boxes ) );
    }
    expected_set.emplace(
        global_box_index( nb_boxes - 2, nb_boxes - 1, nb_boxes ) );
    expected_set.emplace(
        global_box_index( nb_boxes - 1, nb_boxes - 1, nb_boxes ) );
    geode::OpenGeodeGeometryException::test(
        eval_intersection.box_intersections_ == expected_set,
        "Box-Ray intersection - Wrong set of boxes" );
}

template < geode::index_t dimension >
void test_self_intersections()
{
    geode::Logger::info(
        "TEST", " Box self intersection AABB ", dimension, "D " );

    const geode::index_t nb_boxes{ 10 };
    // Create a grid of intersecting boxes
    auto box_vector = create_box_vector< dimension >( nb_boxes, 0.75 );
    // Create a grid of tangent boxes included in previous boxes
    const auto box_vector2 = create_box_vector< dimension >( nb_boxes, 0.50 );
    box_vector.insert(
        box_vector.end(), box_vector2.begin(), box_vector2.end() );

    geode::AABBTree< dimension > aabb{ box_vector };

    const BoxAABBInclusion< dimension > box_inclusion{ box_vector };
    const auto included_boxes =
        aabb.compute_self_element_bbox_intersections( box_inclusion );

    geode::OpenGeodeGeometryException::test(
        included_boxes.size() == nb_boxes * nb_boxes,
        "Box self intersection - Every box should have one box "
        "inside" );

    for( const auto& [box1, box2] : included_boxes )
    {
        const auto container =
            box_inclusion.box_contains_box( box1, box2 ) ? box1 : box2;
        const auto contained = container == box1 ? box2 : box1;
        geode::OpenGeodeGeometryException::test(
            container == contained - ( nb_boxes * nb_boxes ),
            "Box self intersection - Wrong box inclusion result" );
    }
    geode::OpenGeodeGeometryException::test(
        aabb.compute_self_element_bbox_intersections( box_inclusion )
            == included_boxes,
        "Box self intersection - Result order should be deterministic" );
}

class OtherAABBIntersection
{
public:
    [[nodiscard]] bool operator()(
        geode::index_t /*box1*/, geode::index_t /*box2*/ ) const
    {
        return true;
    }
};

template < geode::index_t dimension >
void test_other_intersections()
{
    geode::Logger::info(
        "TEST", " Box other intersection AABB ", dimension, "D " );

    geode::AABBTree< dimension > aabb{ create_box_vector< dimension >(
        5, 0.2 ) };
    const geode::AABBTree< dimension > other{ create_box_vector< dimension >(
        2, 0.4 ) };
    const auto intersecting_boxes =
        aabb.compute_other_element_bbox_intersections(
            other, OtherAABBIntersection{} );

    absl::flat_hash_map< geode::index_t, geode::index_t > answer{ { 0, 0 },
        { 1, 1 }, { 5, 2 }, { 6, 3 } };
    for( const auto& [box, other_box] : intersecting_boxes )
    {
        geode::OpenGeodeGeometryException::test( answer.at( box ) == other_box,
            "Box other intersection - Wrong box result" );
    }
    geode::OpenGeodeGeometryException::test(
        aabb.compute_other_element_bbox_intersections(
            other, OtherAABBIntersection{} )
            == intersecting_boxes,
        "Box other intersection - Result order should be deterministic" );
}

void test_aabb_with_empty_box()
{
    geode::Logger::info( "TEST", "AABB with empty box" );
    std::vector< geode::BoundingBox3D > boxes( 3 );
    boxes[0].add_point( geode::Point3D{ { 0, 0, 0 } } );
    boxes[0].add_point( geode::Point3D{ { 1, 1, 1 } } );
    boxes[2].add_point( geode::Point3D{ { 2, 2, 2 } } );
    boxes[2].add_point( geode::Point3D{ { 3, 3, 3 } } );
    const geode::AABBTree3D aabb{ boxes };
    const auto& box = aabb.bounding_box();
    geode::OpenGeodeGeometryException::test(
        box.min() == geode::Point3D{ { 0, 0, 0 } }
            && box.max() == geode::Point3D{ { 3, 3, 3 } },
        "AABB with empty box - Wrong bounding box ", box.string() );
}

void test_closest_element_box_with_nan_hint()
{
    geode::Logger::info( "TEST", "Closest element box with NaN hint" );
    std::vector< geode::BoundingBox3D > boxes( 8 );
    for( const auto i : geode::Range{ 8 } )
    {
        boxes[i].add_point(
            geode::Point3D{ { static_cast< double >( i ), 0, 0 } } );
        boxes[i].add_point( geode::Point3D{ { i + 1., 1, 1 } } );
    }
    const geode::AABBTree3D aabb{ boxes };
    const auto action = [&boxes]( const geode::Point3D& query,
                            geode::index_t element ) {
        if( element == 0 )
        {
            return std::numeric_limits< double >::quiet_NaN();
        }
        return geode::point_point_distance( boxes[element].center(), query );
    };
    const auto [box_id, distance] =
        aabb.closest_element_box( boxes[0].center(), action );
    geode::OpenGeodeGeometryException::test(
        box_id == 1, "Closest element box with NaN hint - Wrong box ", box_id );
    geode::OpenGeodeGeometryException::test( std::isfinite( distance ),
        "Closest element box with NaN hint - Distance should be finite" );
}

void test_thin_strips()
{
    geode::Logger::info( "TEST", "AABB with long thin strips" );
    std::mt19937 generator{ 42 };
    constexpr geode::index_t NB_STRIPS{ 20000 };
    std::vector< geode::BoundingBox2D > strips( NB_STRIPS );
    for( const auto i : geode::Range{ NB_STRIPS } )
    {
        const auto y = 10. * i / NB_STRIPS;
        strips[i].add_point( geode::Point2D{ { 0, y } } );
        strips[i].add_point( geode::Point2D{ { 100, y + 0.0005 } } );
    }
    std::shuffle( strips.begin(), strips.end(), generator );
    const geode::AABBTree2D aabb{ strips };
    std::uniform_real_distribution< double > distribution{ -1, 101 };
    std::uniform_real_distribution< double > y_distribution{ -1, 11 };
    geode::index_t nb_visited_elements{ 0 };
    for( const auto unused : geode::Range{ 50 } )
    {
        geode_unused( unused );
        const geode::Point2D query{ { distribution( generator ),
            y_distribution( generator ) } };
        geode::BoundingBox2D query_box;
        query_box.add_point( query );
        query_box.add_point( query + geode::Point2D{ { 0.01, 0.01 } } );
        std::vector< geode::index_t > found;
        auto action = [&found]( geode::index_t box ) {
            found.push_back( box );
            return false;
        };
        aabb.compute_bbox_element_bbox_intersections( query_box, action );
        std::vector< geode::index_t > expected;
        for( const auto s : geode::Range{ NB_STRIPS } )
        {
            if( strips[s].epsilon_intersects( query_box ) )
            {
                expected.push_back( s );
            }
        }
        absl::c_sort( found );
        geode::OpenGeodeGeometryException::test(
            found == expected, "AABB with thin strips - Wrong box query" );

        const auto [closest, distance] = aabb.closest_element_box(
            query, [&strips, &nb_visited_elements](
                       const geode::Point2D& point, geode::index_t box ) {
                nb_visited_elements++;
                return strips[box].signed_distance( point );
            } );
        auto expected_distance = std::numeric_limits< double >::max();
        for( const auto& strip : strips )
        {
            expected_distance =
                std::min( expected_distance, strip.signed_distance( query ) );
        }
        geode::OpenGeodeGeometryException::test(
            distance == expected_distance
                && strips[closest].signed_distance( query ) == distance,
            "AABB with thin strips - Wrong closest box" );
    }
    // Splitting along strip extents (x) would evaluate most strips
    geode::Logger::info(
        "Thin strips closest element evaluations: ", nb_visited_elements );
    geode::OpenGeodeGeometryException::test( nb_visited_elements < 150,
        "AABB with thin strips - Too many visited elements: ",
        nb_visited_elements );
}

template < geode::index_t dimension >
void do_test()
{
    test_build_aabb< dimension >();
    test_nearest_neighbor_search< dimension >();
    test_intersections_with_query_box< dimension >();
    test_intersections_with_ray_trace< dimension >();
    test_self_intersections< dimension >();
    test_other_intersections< dimension >();
}

void test()
{
    do_test< 2 >();
    do_test< 3 >();
    test_aabb_with_empty_box();
    test_closest_element_box_with_nan_hint();
    test_thin_strips();
}

OPENGEODE_TEST( "aabb" )
