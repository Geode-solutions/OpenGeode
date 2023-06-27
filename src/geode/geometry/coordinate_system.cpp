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
#include <geode/geometry/distance.h>
#include <geode/geometry/information.h>
#include <geode/geometry/position.h>
#include <geode/geometry/square_matrix.h>

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

    template < index_t dimension >
    void CoordinateSystem< dimension >::set_directions(
        std::array< Vector< dimension >, dimension > directions )
    {
        for( const auto d : LRange{ dimension } )
        {
            this->set_direction( d, directions[d] );
        }
    }

    template <>
    Point< 2 > opengeode_geometry_api CoordinateSystem< 2 >::coordinates(
        const Point< 2 >& global_coordinates ) const
    {
        const auto A = origin_ + this->direction( 0 );
        const auto B = origin_ + this->direction( 1 );
        const Vector2D OP{ origin_, global_coordinates };
        const InfiniteLine2D OP_line{ OP, origin_ };
        const auto distance_A = point_line_distance( A, OP_line );
        const auto distance_B = point_line_distance( B, OP_line );
        const auto ratio = distance_A / ( distance_A + distance_B );
        const InfiniteLine2D OA_line{ this->direction( 0 ), origin_ };
        const InfiniteLine2D OB_line{ this->direction( 1 ), origin_ };
        const auto sideB = point_side_to_line( global_coordinates, OA_line );
        const auto sideA = point_side_to_line( global_coordinates, OB_line );
        const auto sign_A = sideA == Side::positive ? -1. : 1.;
        const auto sign_B = sideB == Side::negative ? -1. : 1.;
        const auto direction = this->direction( 1 ) * ratio * sign_A
                               + this->direction( 0 ) * ( 1. - ratio ) * sign_B;
        const auto scale = OP.length() / direction.length();
        return { { ( 1. - ratio ) * scale * sign_A, ratio * scale * sign_B } };
    }

    template <>
    Point< 3 > opengeode_geometry_api CoordinateSystem< 3 >::coordinates(
        const Point< 3 >& global_coordinates ) const
    {
        const auto OA = this->direction( 0 );
        const auto OB = this->direction( 1 );
        const auto OC = this->direction( 2 );
        const SquareMatrix< 3 > system_matrix{
            { Vector3D{ { OA.value( 0 ), OB.value( 0 ), OC.value( 0 ) } },
                Vector3D{ { OA.value( 1 ), OB.value( 1 ), OC.value( 1 ) } },
                Vector3D{ { OA.value( 2 ), OB.value( 2 ), OC.value( 2 ) } } }
        };
        const auto inverse_matrix = system_matrix.inverse();
        const auto OP = global_coordinates - origin_;
        const auto result = inverse_matrix * OP;
        return result;
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

    template < index_t dimension >
    template < typename Archive >
    void CoordinateSystem< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, CoordinateSystem >{
                { []( Archive& a, CoordinateSystem& coord_system ) {
                    a.ext( coord_system,
                        bitsery::ext::BaseClass< Frame< dimension > >{} );
                    a.object( coord_system.origin_ );
                } } } );
    }

    template class opengeode_geometry_api CoordinateSystem< 2 >;
    template class opengeode_geometry_api CoordinateSystem< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, CoordinateSystem< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, CoordinateSystem< 3 > );
} // namespace geode
