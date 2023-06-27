/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/coordinate_system.h>

#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/information.h>
#include <geode/geometry/perpendicular.h>
#include <geode/geometry/position.h>

namespace
{
    void check_frame( std::array< geode::Vector2D, 2 > directions )
    {
        const auto dot =
            geode::dot_perpendicular( directions[0], directions[1] );
        OPENGEODE_EXCEPTION( dot > geode::global_angular_epsilon,
            "[CoordinateSystem2D] Could not create a "
            "CoordinateSystem with given directions" );
    }

    void check_frame( std::array< geode::Vector3D, 3 > directions )
    {
        for( const auto d0 : geode::LRange{ 2 } )
        {
            for( const auto d1 : geode::LRange{ d0 + 1, 3 } )
            {
                const auto vector0 = directions[d0].normalize();
                const auto vector1 = directions[d1].normalize();

                const auto normal = vector0.cross( vector1 );
                const auto length = normal.length();
                OPENGEODE_EXCEPTION( length > geode::global_angular_epsilon,
                    "[CoordinateSystem3D] Could not create a "
                    "CoordinateSystem with given directions" );
            }
        }
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    CoordinateSystem< dimension >::CoordinateSystem()
    {
    }

    template < index_t dimension >
    CoordinateSystem< dimension >::CoordinateSystem(
        std::array< Vector< dimension >, dimension > directions,
        Point< dimension > origin )
        : Frame< dimension >( std::move( directions ) ),
          origin_( std::move( origin ) )
    {
        check_frame( directions );
    }

    template < index_t dimension >
    CoordinateSystem< dimension >::CoordinateSystem( Point< dimension > origin,
        const std::array< Point< dimension >, dimension >& other_points )
        : Frame< dimension >( [&other_points, &origin] {
              std::array< Vector< dimension >, dimension > directions;
              for( const auto d : LRange{ dimension } )
              {
                  directions[d] = { origin, other_points[d] };
              }
              return directions;
          }() ),
          origin_( std::move( origin ) )
    {
    }

    template < index_t dimension >
    const Point< dimension >& CoordinateSystem< dimension >::origin() const
    {
        return origin_;
    }

    template < index_t dimension >
    void CoordinateSystem< dimension >::set_origin( Point< dimension > origin )
    {
        origin_ = std::move( origin );
    }

    template <>
    Point< 3 > opengeode_geometry_api CoordinateSystem< 3 >::coordinates(
        const Point< 3 >& global_coordinates ) const
    {
        const auto A = origin_ + this->direction( 0 );
        const auto B = origin_ + this->direction( 1 );
        const auto C = origin_ + this->direction( 2 );
        const auto OAB_plane = Triangle3D{ origin_, A, B }.plane().value();
        const auto OBC_plane = Triangle3D{ origin_, B, C }.plane().value();
        const auto OCA_plane = Triangle3D{ origin_, C, A }.plane().value();
        const auto distance_A =
            std::get< 0 >( point_plane_distance( A, OBC_plane ) );
        const auto distance_AP = std::get< 0 >(
            point_plane_signed_distance( global_coordinates, OBC_plane ) );
        const auto distance_B =
            std::get< 0 >( point_plane_distance( B, OCA_plane ) );
        const auto distance_BP = std::get< 0 >(
            point_plane_signed_distance( global_coordinates, OCA_plane ) );
        const auto distance_C =
            std::get< 0 >( point_plane_distance( C, OAB_plane ) );
        const auto distance_CP = std::get< 0 >(
            point_plane_signed_distance( global_coordinates, OAB_plane ) );
        return { { distance_AP / distance_A, distance_BP / distance_B,
            distance_CP / distance_C } };
    }

    template <>
    Point< 2 > opengeode_geometry_api CoordinateSystem< 2 >::coordinates(
        const Point< 2 >& global_coordinates ) const
    {
        const auto A = origin_ + this->direction( 0 );
        const auto B = origin_ + this->direction( 1 );
        const Vector2D OP{ origin_, global_coordinates };
        const InfiniteLine2D OP_line{ OP, origin_ };
        const InfiniteLine2D OA_line{ this->direction( 0 ), origin_ };
        const InfiniteLine2D OB_line{ this->direction( 1 ), origin_ };
        const auto distance_A = point_line_distance( A, OB_line );
        const auto distance_AP =
            point_line_signed_distance( global_coordinates, OB_line );
        const auto distance_B = point_line_distance( B, OA_line );
        const auto distance_BP =
            point_line_signed_distance( global_coordinates, OA_line );
        return { { -distance_AP / distance_A, distance_BP / distance_B } };
    }

    template < index_t dimension >
    Point< dimension > CoordinateSystem< dimension >::global_coordinates(
        const Point< dimension >& coordinates ) const
    {
        auto global_point = origin_;
        for( const auto d : LRange{ dimension } )
        {
            global_point += this->direction( d ) * coordinates.value( d );
        }
        return global_point;
    }

    template < index_t dimension >
    std::string CoordinateSystem< dimension >::string() const
    {
        return absl::StrCat(
            "(", origin_.string(), " # ", Frame< dimension >::string(), ")" );
    }

    template class opengeode_geometry_api CoordinateSystem< 2 >;
    template class opengeode_geometry_api CoordinateSystem< 3 >;
} // namespace geode
