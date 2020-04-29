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
std::vector< geode::BoundingBox< dimension > > create_box_vector(
    index_t nb_box_range, double box_size )
{
    std::vector< geode::BoundingBox< dimension > > box_vector;
    for( geode::index_t i : Range( nb_box_range ) )
    {
        for( index_t j : Range( nb_box_range ) )
        {
            Point< dimension > point;
            point.set_value( 0, (double) i );
            point.set_value( 1, (double) j );
            box_vector.push_back( create_bounding_box( point, box_size ) );
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
    index_t nb_boxes = 100;
    double box_size = 0.25;
    // create a grid of separate boxes
    std::vector< geode::BoundingBox< dimension > > box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    AABBTree< dimension > aabb( box_vector );

    // check number of box
    OPENGEODE_EXCEPTION( aabb.nb_bboxes() == box_vector.size(),
        "[Test] Error ... wrong number of box in the aabb tree" );
    return;
}

template < index_t dimension >
class BoxAABBEvalDistance
{
public:
    BoxAABBEvalDistance(
        const std::vector< geode::BoundingBox< dimension > >& bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }
    ~BoxAABBEvalDistance() = default;

    // EvalDistance
    std::tuple< double, Point< dimension > > operator()(
        const Point< dimension >& query, index_t curent_element_box ) const
    {
        Point< dimension > box_center =
            ( bounding_boxes_[curent_element_box].min()
                + bounding_boxes_[curent_element_box].max() )
            / 2.;
        Vector< dimension > vec( box_center, query );
        return std::tuple< double, Point< dimension > >{ vec.length(),
            box_center };
    }

private:
    const std::vector< geode::BoundingBox< dimension > >& bounding_boxes_;
};

template < index_t dimension >
void test_nearest_neighbor_search()
{
    index_t nb_boxes = 10;
    double box_size = 0.75;
    // create a grid of boxes that intersect
    std::vector< geode::BoundingBox< dimension > > box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    AABBTree< dimension > aabb( box_vector );

    BoxAABBEvalDistance< dimension > disteval =
        BoxAABBEvalDistance< dimension >( box_vector );

    for( geode::index_t i : Range( nb_boxes ) )
    {
        for( index_t j : Range( nb_boxes ) )
        {
            Point< dimension > box_center;
            box_center.set_value( 0, (double) i );
            box_center.set_value( 1, (double) j );

            Point< dimension > query;
            // query point is in the up right corner of each box.
            query.set_value( 0, (double) i + box_size / 2. );
            query.set_value( 1, (double) j + box_size / 2. );
            index_t box_id;
            Point< dimension > neatest_point;
            double distance;
            std::tie( box_id, neatest_point, distance ) =
                aabb.closest_element_box( query, disteval );

            OPENGEODE_EXCEPTION( box_id == global_box_index( i, j, nb_boxes ),
                "[Test] Error ... wrong nearest box index" );
            OPENGEODE_EXCEPTION( neatest_point == box_center,
                "[Test] Error ... wrong nearest point " );
            Vector< dimension > vec( box_center, query );
            OPENGEODE_EXCEPTION(
                distance == vec.length(), "[Test] Error ... wrong distance " );
        }
    }
}

template < index_t dimension >
class BoxAABBEvalIntersection
{
public:
    BoxAABBEvalIntersection(
        const std::vector< geode::BoundingBox< dimension > >& bounding_boxes )
        : bounding_boxes_( bounding_boxes )
    {
    }
    ~BoxAABBEvalIntersection() = default;

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
        if( box_contains_box( box2, box1 ) )
        {
            included_box_.push_back( { box2, box1 } );
        }
    }

public:
    std::set< index_t > box_intersections_;
    std::vector< std::pair< index_t, index_t > > included_box_;

private:
    const std::vector< geode::BoundingBox< dimension > >& bounding_boxes_;
};

template < index_t dimension >
void test_intersections_with_query_box()
{
    index_t nb_boxes = 10;
    double box_size = 0.5;
    // create a grid of tangent boxes
    std::vector< BoundingBox< dimension > > box_vector =
        create_box_vector< dimension >( nb_boxes, box_size );
    AABBTree< dimension > aabb( box_vector );

    BoxAABBEvalIntersection< dimension > eval_intersection =
        BoxAABBEvalIntersection< dimension >( box_vector );

    for( geode::index_t i : Range( nb_boxes - 1 ) )
    {
        for( index_t j : Range( nb_boxes - 1 ) )
        {
            Point< dimension > query;
            // query boxes will be at internal corner grid
            query.set_value( 0, (double) i + box_size );
            query.set_value( 1, (double) j + box_size );
            BoundingBox< dimension > box_query =
                create_bounding_box( query, box_size );

            eval_intersection.box_intersections_.clear();
            aabb.compute_bbox_element_bbox_intersections(
                box_query, eval_intersection );

            OPENGEODE_EXCEPTION(
                eval_intersection.box_intersections_.size() == 4,
                "[Test] Error ... wrong number of intersected boxes" );

            std::set< index_t > expected_set;
            expected_set.emplace( global_box_index( i, j, nb_boxes ) );
            expected_set.emplace( global_box_index( i, j + 1, nb_boxes ) );
            expected_set.emplace( global_box_index( i + 1, j, nb_boxes ) );
            expected_set.emplace( global_box_index( i + 1, j + 1, nb_boxes ) );
            OPENGEODE_EXCEPTION(
                eval_intersection.box_intersections_ == expected_set,
                "[Test] Error ... wrong intersected set of box" );
        }
    }
    Point< dimension > query;
    // query box will be at top right corner grid
    query.set_value( 0, (double) nb_boxes - 1 + box_size );
    query.set_value( 1, (double) nb_boxes - 1 + box_size );
    BoundingBox< dimension > box_query = create_bounding_box( query, box_size );

    eval_intersection.box_intersections_.clear();
    aabb.compute_bbox_element_bbox_intersections(
        box_query, eval_intersection );

    OPENGEODE_EXCEPTION( eval_intersection.box_intersections_.size() == 1,
        "[Test] Error ... wrong number of intersected boxes" );

    std::set< index_t > expected_set;
    expected_set.emplace(
        global_box_index( nb_boxes - 1, nb_boxes - 1, nb_boxes ) );
    OPENGEODE_EXCEPTION( eval_intersection.box_intersections_ == expected_set,
        "[Test] Error ... wrong intersected set of box" );
}

template < index_t dimension >
void test_self_intersections()
{
    index_t nb_boxes = 10;
    // create a grid of intersected boxes
    std::vector< BoundingBox< dimension > > box_vector =
        create_box_vector< dimension >( nb_boxes, 0.75 );
    // create a grid of tangent boxes included in previous boxes
    std::vector< BoundingBox< dimension > > box_vector2 =
        create_box_vector< dimension >( nb_boxes, 0.50 );
    box_vector.insert(
        box_vector.end(), box_vector2.begin(), box_vector2.end() );

    AABBTree< dimension > aabb( box_vector );
    BoxAABBEvalIntersection< dimension > eval_intersection =
        BoxAABBEvalIntersection< dimension >( box_vector );
    // investigate box inclusions
    eval_intersection.included_box_.clear();
    aabb.compute_self_element_bbox_intersections( eval_intersection );

    OPENGEODE_EXCEPTION(
        eval_intersection.included_box_.size() == nb_boxes * nb_boxes,
        "[Test] Error ... every box should have on box inside" );

    for( geode::index_t i : Range( eval_intersection.included_box_.size() ) )
    {
        OPENGEODE_EXCEPTION( eval_intersection.included_box_[i].first
                                 == eval_intersection.included_box_[i].second
                                        - ( nb_boxes * nb_boxes ),
            "[Test] Error ... wrong box inclusion" );
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
