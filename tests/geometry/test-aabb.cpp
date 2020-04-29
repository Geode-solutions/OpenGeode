/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/logger.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <geode/tests/common.h>

#include <absl/container/flat_hash_set.h>

using namespace geode;

template < index_t dimension >
BoundingBox< dimension > create_bounding_box(
    const Point< dimension >& center, double box_size )
{
    Point< dimension > temp;
    for( const index_t c : Range( dimension ) )
    {
        temp.set_value( c, box_size );
    }
    BoundingBox< dimension > box;
    box.add_point( center - temp );
    box.add_point( center + temp );

    return box;
}

template < index_t dimension >
std::vector< BoundingBox< dimension > > create_box_vector(
    index_t nb_box_range, double box_size )
{
    std::vector< BoundingBox< dimension > > box_vector(
        nb_box_range * nb_box_range );
    for( const index_t i : Range( nb_box_range ) )
    {
        for( const index_t j : Range( nb_box_range ) )
        {
            Point< dimension > point;
            point.set_value( 0, i );
            point.set_value( 1, j );
            box_vector[nb_box_range * i + j] =
                create_bounding_box( point, box_size );
        }
    }
    return box_vector;
}

index_t global_box_index( index_t i, index_t j, index_t size )
{
    return size * i + j;
}

template < index_t dimension >
void test_build_aabb()
{
    const index_t nb_boxes{ 100 };
    const double box_size{ 0.25 };

    // Create a grid of non overlapping boxes
    const auto box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    AABBTree< dimension > aabb( box_vector );

    OPENGEODE_EXCEPTION( aabb.nb_bboxes() == box_vector.size(),
        "[Test] Wrong number of boxes in the AABB tree" );
    return;
}

template < index_t dimension >
class BoxAABBEvalDistance
{
public:
    BoxAABBEvalDistance(
        const std::vector< BoundingBox< dimension > >& bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }
    ~BoxAABBEvalDistance() = default;

    std::tuple< double, Point< dimension > > operator()(
        const Point< dimension >& query, index_t current_element_box ) const
    {
        const auto box_center =
            ( bounding_boxes_[current_element_box].min()
                + bounding_boxes_[current_element_box].max() )
            / 2.;
        const Vector< dimension > vec{ box_center, query };
        return std::tuple< double, Point< dimension > >{ vec.length(),
            box_center };
    }

private:
    const std::vector< BoundingBox< dimension > >& bounding_boxes_;
};

template < index_t dimension >
void test_nearest_neighbor_search()
{
    const index_t nb_boxes{ 10 };
    const double box_size{ 0.75 };
    const auto box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    AABBTree< dimension > aabb( box_vector );

    BoxAABBEvalDistance< dimension > disteval =
        BoxAABBEvalDistance< dimension >( box_vector );

    for( const index_t i : Range( nb_boxes ) )
    {
        for( const index_t j : Range( nb_boxes ) )
        {
            Point< dimension > box_center;
            box_center.set_value( 0, i );
            box_center.set_value( 1, j );

            Point< dimension > query;
            // query point is in the up right corner of each box.
            query.set_value( 0, i + box_size / 2. );
            query.set_value( 1, j + box_size / 2. );
            index_t box_id;
            Point< dimension > nearest_point;
            double distance;
            std::tie( box_id, nearest_point, distance ) =
                aabb.closest_element_box( query, disteval );

            OPENGEODE_EXCEPTION( box_id == global_box_index( i, j, nb_boxes ),
                "[Test] Wrong nearest box index" );
            OPENGEODE_EXCEPTION( nearest_point == box_center,
                "[Test] Wrong nearest box center " );
            const Vector< dimension > vec{ box_center, query };
            OPENGEODE_EXCEPTION( distance == vec.length(),
                "[Test] Wrong distance to nearest box center" );
        }
    }
}

template < index_t dimension >
class BoxAABBIntersection
{
public:
    BoxAABBIntersection(
        const std::vector< BoundingBox< dimension > >& bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }
    ~BoxAABBIntersection() = default;

    void operator()( index_t cur_box )
    {
        box_intersections_.emplace( cur_box );
    }

    // test box strict inclusion
    bool box_contains_box( index_t box1, index_t box2 )
    {
        return (
            bounding_boxes_[box1].contains( bounding_boxes_[box2].min() )
            && bounding_boxes_[box1].contains( bounding_boxes_[box2].max() ) );
    }
    void operator()( index_t box1, index_t box2 )
    {
        if( box_contains_box( box1, box2 ) )
        {
            included_box_.push_back( { box1, box2 } );
        }
        else if( box_contains_box( box2, box1 ) )
        {
            included_box_.push_back( { box2, box1 } );
        }
    }

public:
    absl::flat_hash_set< index_t > box_intersections_;
    std::vector< std::pair< index_t, index_t > > included_box_;

private:
    const std::vector< BoundingBox< dimension > >& bounding_boxes_;
};

template < index_t dimension >
void test_intersections_with_query_box()
{
    const index_t nb_boxes{ 10 };
    const double box_size{ 0.5 };
    const auto box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    AABBTree< dimension > aabb( box_vector );

    BoxAABBIntersection< dimension > eval_intersection =
        BoxAABBIntersection< dimension >( box_vector );

    for( const index_t i : Range( nb_boxes - 1 ) )
    {
        for( const index_t j : Range( nb_boxes - 1 ) )
        {
            Point< dimension > query;
            // query boxes will be at internal corner grid
            query.set_value( 0, i + box_size );
            query.set_value( 1, j + box_size );
            const auto box_query = create_bounding_box( query, box_size );

            eval_intersection.box_intersections_.clear();
            aabb.compute_bbox_element_bbox_intersections(
                box_query, eval_intersection );

            OPENGEODE_EXCEPTION(
                eval_intersection.box_intersections_.size() == 4,
                "[Test] Wrong number of intersected boxes" );

            absl::flat_hash_set< index_t > expected_set;
            expected_set.emplace( global_box_index( i, j, nb_boxes ) );
            expected_set.emplace( global_box_index( i, j + 1, nb_boxes ) );
            expected_set.emplace( global_box_index( i + 1, j, nb_boxes ) );
            expected_set.emplace( global_box_index( i + 1, j + 1, nb_boxes ) );
            OPENGEODE_EXCEPTION(
                eval_intersection.box_intersections_ == expected_set,
                "[Test] Wrong intersected set of boxes" );
        }
    }
    Point< dimension > query;
    // query box will be at top right corner grid
    query.set_value( 0, nb_boxes - 1 + box_size );
    query.set_value( 1, nb_boxes - 1 + box_size );
    const auto box_query = create_bounding_box( query, box_size );

    eval_intersection.box_intersections_.clear();
    aabb.compute_bbox_element_bbox_intersections(
        box_query, eval_intersection );

    OPENGEODE_EXCEPTION( eval_intersection.box_intersections_.size() == 1,
        "[Test] Wrong number of intersected boxes" );

    absl::flat_hash_set< index_t > expected_set;
    expected_set.emplace(
        global_box_index( nb_boxes - 1, nb_boxes - 1, nb_boxes ) );
    OPENGEODE_EXCEPTION( eval_intersection.box_intersections_ == expected_set,
        "[Test] Wrong intersected set of boxes" );
}

template < index_t dimension >
void test_self_intersections()
{
    const index_t nb_boxes{ 10 };
    // Create a grid of intersecting boxes
    auto box_vector = create_box_vector< dimension >( nb_boxes, 0.75 );
    // Create a grid of tangent boxes included in previous boxes
    const auto box_vector2 = create_box_vector< dimension >( nb_boxes, 0.50 );
    box_vector.insert(
        box_vector.end(), box_vector2.begin(), box_vector2.end() );

    AABBTree< dimension > aabb( box_vector );
    BoxAABBIntersection< dimension > eval_intersection{ box_vector };
    // investigate box inclusions
    eval_intersection.included_box_.clear();
    aabb.compute_self_element_bbox_intersections( eval_intersection );

    OPENGEODE_EXCEPTION(
        eval_intersection.included_box_.size() == nb_boxes * nb_boxes,
        "[Test] Every box should have one box inside" );

    for( index_t i : Range( eval_intersection.included_box_.size() ) )
    {
        OPENGEODE_EXCEPTION( eval_intersection.included_box_[i].first
                                 == eval_intersection.included_box_[i].second
                                        - ( nb_boxes * nb_boxes ),
            "[Test] Wrong box inclusion result" );
    }
}

template < index_t dimension >
void do_test()
{
    test_build_aabb< dimension >();
    test_nearest_neighbor_search< dimension >();
    test_intersections_with_query_box< dimension >();
    test_self_intersections< dimension >();
}

void test()
{
    do_test< 2 >();
    do_test< 3 >();
}

OPENGEODE_TEST( "aabb" )
