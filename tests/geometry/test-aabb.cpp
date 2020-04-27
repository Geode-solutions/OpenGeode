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
        const Point< dimension >& query, index_t cur_box ) const
    {
        Vector vec( box_center, query );
        Point< dimension > box_center = bounding_boxes_[cur_box].center();
        return std::tuple< double, Point< dimension > >{ vec.length,
            box_center };
    }
    double operator()(
        const Point< dimension >& pt1, const Point< dimension >& pt2 ) const
    {
        Vector vec( pt1, pt2 );
        return vec.length();
    }

private:
    const std::vector< geode::BoundingBox< dimension > >& bounding_boxes_;
};

template < index_t dimension >
class BoxAABBEvalBoxIntersection
{
public:
    // EvalInteserction
    void operator()( index_t cur_box ){ intersections.push_back( cur_box ) }

    std::set< index_t > get_intersected_box()
    {
        return intersections;
    }

    // EvalInteserction
    void operator()( index_t box1, index_t box2 ) {}

private:
    std::set< index_t > intersections;
};

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

    // EvalInteserction
    void operator()( index_t box1, index_t box2 )
    {
        bounding_boxes_[box1].min();
        bounding_boxes_[box2].max();
    }

private:
    const std::vector< geode::BoundingBox< dimension > >& bounding_boxes_;
    std::set< index_t > intersections;
};

template < index_t dimension >
BoundingBox< dimension > create_bounding_box(
    const Point< dimension >& center, double box_size )
{
    Point< dimension > temp;
    for( index_t c : Range( dimension ) )
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
            for( auto c : Range( dimension ) )
            {
                point.set_value( 0, (double) i );
                point.set_value( 1, (double) j );
            }
            box_vector.push_back( create_bounding_box( point, box_size ) );
        }
    }
    return box_vector;
}

template < index_t dimension >
void do_test()
{
    std::vector< std::pair< index_t, double > > param_to_test(
        { { 10, 0.25 }, { 10, 0.5 }, { 10, 0.75 } } );
    for( auto param : param_to_test )
    {
        // build aabb
        std::vector< geode::BoundingBox< dimension > > box_vector =
            create_box_vector< dimension >( param.first, param.second );
        geode::AABBTree< dimension > aabb( box_vector );

        // build aabb
        OPENGEODE_EXCEPTION( aabb.nb_bboxes() == box_vector.size(),
            "[Test] Error ... wrong number of box in the aabb tree" );
        BoxAABBEvalDistance< dimension > disteval =
            BoxAABBEvalDistance< dimension >( box_vector );
        Point< dimension > query;
        for( auto c : Range( dimension ) )
        {
            query.set_value( c, 0.1 );
        }
        aabb.closest_element_box( query, disteval );
    }
}

void test()
{
    do_test< 2 >();
    do_test< 3 >();
}

/*aabb.closest_element_box();
aabb.compute_bbox_element_bbox_intersections();
aabb.compute_self_element_bbox_intersections();

  OPENGEODE_EXCEPTION( search.closest_neighbor( { { 0, 0 } } ) == 0,
      "[Test] Error in closest neighbor" );
  OPENGEODE_EXCEPTION( search.closest_neighbor( { { 1, -4 } } ) == 2,
      "[Test] Error in closest neighbor" );

  const std::vector< geode::index_t > answer_radius{ 0, 2 };
  OPENGEODE_EXCEPTION(
      search.radius_neighbors( { { 0, 0 } }, 5.4 ) == answer_radius,
      "[Test] Error in radius neighbors" );

  const std::vector< geode::index_t > answer_neighbors{ 2, 0 };
  OPENGEODE_EXCEPTION(
      search.neighbors( { { -1, -1 } }, 2 ) == answer_neighbors,
      "[Test] Error in neighbors" );

  const geode::Point3D p0{ { 0.1, 2.9, 5.4 } };
  const geode::Point3D p1{ { 2.4, 8.1, 7.6 } };
  const geode::Point3D p2{ { 8.1, 4.2, 3.8 } };
  const geode::Point3D p3{ { 3.1, 9.4, 9.7 } };
  const geode::NNSearch3D colocator( { p0, p0, p1, p0, p2, p1, p3 } );

  const auto colocated_info = colocator.colocated_index_mapping( 1e-8 );
  OPENGEODE_EXCEPTION( colocated_info.nb_colocated_points() == 3,
      "[Test] Should be 3 colocated points" );
  const absl::FixedArray< geode::index_t > mapping_answer{ 0, 0, 1, 0, 2, 1,
      3 };
  OPENGEODE_EXCEPTION( colocated_info.colocated_mapping == mapping_answer,
      "[Test] Error in colocated mapping" );
  const absl::FixedArray< geode::Point3D > points_answer{ p0, p1, p2, p3 };
  OPENGEODE_EXCEPTION( colocated_info.unique_points == points_answer,
      "[Test] Error in unique points" );*/

OPENGEODE_TEST( "aabb" )