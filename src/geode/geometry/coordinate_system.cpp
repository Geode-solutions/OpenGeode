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

#include <geode/geometry/perpendicular.h>

namespace
{
    void check_frame( std::array< geode::Vector2D, 2 > directions )
    {
        const auto dot = geode::dot_perpendicular(
            directions[0].normalize(), directions[1].normalize() );
        OPENGEODE_EXCEPTION( std::fabs( dot ) > geode::global_angular_epsilon,
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

    geode::SquareMatrix< 2 > frame_inverse_matrix(
        const geode::CoordinateSystem2D& frame )
    {
        const auto OA = frame.direction( 0 );
        const auto OB = frame.direction( 1 );
        const geode::SquareMatrix< 2 > system_matrix{
            { geode::Vector2D{ { OA.value( 0 ), OB.value( 0 ) } },
                geode::Vector2D{ { OA.value( 1 ), OB.value( 1 ) } } }
        };
        return system_matrix.inverse();
    }

    geode::SquareMatrix< 3 > frame_inverse_matrix(
        const geode::CoordinateSystem3D& frame )
    {
        const auto OA = frame.direction( 0 );
        const auto OB = frame.direction( 1 );
        const auto OC = frame.direction( 2 );
        const geode::SquareMatrix< 3 > system_matrix{
            { geode::Vector3D{
                  { OA.value( 0 ), OB.value( 0 ), OC.value( 0 ) } },
                geode::Vector3D{
                    { OA.value( 1 ), OB.value( 1 ), OC.value( 1 ) } },
                geode::Vector3D{
                    { OA.value( 2 ), OB.value( 2 ), OC.value( 2 ) } } }
        };
        return system_matrix.inverse();
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

    template < index_t dimension >
    void CoordinateSystem< dimension >::set_directions(
        std::array< Vector< dimension >, dimension > directions )
    {
        for( const auto d : LRange{ dimension } )
        {
            this->set_direction( d, directions[d] );
        }
    }

    template < index_t dimension >
    Point< dimension > CoordinateSystem< dimension >::coordinates(
        const Point< dimension >& global_coordinates ) const
    {
        const auto OP = global_coordinates - origin_;
        return global_to_local_matrix_( frame_inverse_matrix, *this ) * OP;
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
