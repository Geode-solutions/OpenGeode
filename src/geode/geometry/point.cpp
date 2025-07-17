/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geometry/point.hpp>

#include <array>
#include <limits>
#include <sstream>

#include <absl/hash/hash.h>

#include <bitsery/bitsery.h>

#include <geode/basic/attribute_utils.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/detail/point_operators.hpp>

namespace geode
{
    /*!
     * Description of a point in the given dimension with double coordinates
     */
    template < index_t dimension >
    Point< dimension >::Point()
    {
        values_.fill( 0 );
    }

    template < index_t dimension >
    Point< dimension >::Point( std::array< double, dimension > values )
        : values_( std::move( values ) )
    {
    }

    template < index_t dimension >
    double Point< dimension >::value( local_index_t index ) const
    {
        return values_.at(index);
    }

    template < index_t dimension >
    void Point< dimension >::set_value( local_index_t index, double coordinate )
    {
        values_.at(index) = coordinate;
    }

    template < index_t dimension >
    bool Point< dimension >::operator==( const Point &other ) const
    {
        for( const auto i : LRange{ dimension } )
        {
            if( value( i ) != other.value( i ) )
            {
                return false;
            }
        }
        return true;
    }

    template < index_t dimension >
    bool Point< dimension >::operator!=( const Point &other ) const
    {
        return !( *this == other );
    }

    template < index_t dimension >
    bool Point< dimension >::operator<( const Point &other ) const
    {
        for( const auto i : LRange{ dimension } )
        {
            if( value( i ) < other.value( i ) )
            {
                return true;
            }
            if( value( i ) > other.value( i ) )
            {
                return false;
            }
        }
        return false;
    }

    template < index_t dimension >
    bool Point< dimension >::operator<=( const Point &other ) const
    {
        return operator<( other ) || operator==( other );
    }

    template < index_t dimension >
    Point< dimension > Point< dimension >::operator*( double multiplier ) const
    {
        return detail::coords_multiply( *this, multiplier );
    }

    template < index_t dimension >
    Point< dimension > Point< dimension >::operator/( double divider ) const
    {
        return detail::coords_divide( *this, divider );
    }

    template < index_t dimension >
    Point< dimension > Point< dimension >::operator+( const Point &other ) const
    {
        return detail::coords_add( *this, other );
    }

    template < index_t dimension >
    Point< dimension > Point< dimension >::operator-( const Point &other ) const
    {
        return detail::coords_substract( *this, other );
    }

    template < index_t dimension >
    void Point< dimension >::operator*=( double multiplier )
    {
        detail::coords_multiply_equal( *this, multiplier );
    }

    template < index_t dimension >
    void Point< dimension >::operator/=( double divider )
    {
        detail::coords_divide_equal( *this, divider );
    }

    template < index_t dimension >
    void Point< dimension >::operator+=( const Point &other )
    {
        detail::coords_add_equal( *this, other );
    }

    template < index_t dimension >
    void Point< dimension >::operator-=( const Point &other )
    {
        detail::coords_substract_equal( *this, other );
    }

    template < index_t dimension >
    bool Point< dimension >::inexact_equal( const Point &other ) const
    {
        double square_length{ 0 };
        static constexpr auto SQR_EPSILON = GLOBAL_EPSILON * GLOBAL_EPSILON;
        for( const auto i : LRange{ dimension } )
        {
            const double diff{ other.value( i ) - this->value( i ) };
            square_length += diff * diff;
            if( square_length > SQR_EPSILON )
            {
                return false;
            }
        }
        return true;
    }

    template < index_t dimension >
    std::string Point< dimension >::string() const
    {
        std::ostringstream oss;
        oss.precision( std::numeric_limits< double >::digits10 );
        const auto *sep = "";
        for( const auto i : LRange{ dimension } )
        {
            oss << sep << value( i );
            sep = " ";
        }
        return oss.str();
    }

    template < index_t dimension >
    Point< dimension - 1 > Point< dimension >::project_point(
        geode::local_index_t axis_to_remove ) const
    {
        OPENGEODE_ASSERT( axis_to_remove < dimension && axis_to_remove >= 0,
            "[Point] Invalid axis to remove" );
        OPENGEODE_ASSERT(
            dimension > 1, "[Point] Invalid dimension to reduce" );
        Point< dimension - 1 > projected_point;
        geode::index_t dim{ 0 };
        for( const auto i : LRange{ dimension } )
        {
            if( i != axis_to_remove )
            {
                projected_point.set_value( dim, this->value( i ) );
                dim++;
            }
        }
        return projected_point;
    }

    template < index_t dimension >
    template < typename Archive >
    void Point< dimension >::serialize( Archive &archive )
    {
        archive.ext( *this,
            Growable< Archive, Point >{ { []( Archive &a, Point &point ) {
                a.container8b( point.values_ );
            } } } );
    }

    template class opengeode_geometry_api Point< 1 >;
    template class opengeode_geometry_api Point< 2 >;
    template class opengeode_geometry_api Point< 3 >;
    template class opengeode_geometry_api Point< 4 >;
    template class opengeode_geometry_api Point< 9 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Point< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Point< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Point< 3 > );
} // namespace geode

namespace std
{
    size_t hash< geode::Point1D >::operator()(
        const geode::Point1D &point ) const
    {
        return absl::Hash< double >()( point.value( 0 ) );
    }

    size_t hash< geode::Point2D >::operator()(
        const geode::Point2D &point ) const
    {
        return absl::Hash< double >()( point.value( 0 ) )
               ^ absl::Hash< double >()( point.value( 1 ) );
    }

    size_t hash< geode::Point3D >::operator()(
        const geode::Point3D &point ) const
    {
        return absl::Hash< double >()( point.value( 0 ) )
               ^ absl::Hash< double >()( point.value( 1 ) )
               ^ absl::Hash< double >()( point.value( 2 ) );
    }
} // namespace std
