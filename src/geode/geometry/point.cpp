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

#include <geode/geometry/point.hpp>

#include <array>
#include <limits>
#include <sstream>

#include <absl/hash/hash.h>

#include <bitsery/bitsery.h>

#include <geode/basic/attribute_utils.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
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
        OpenGeodeGeometryException::check_assertion(
            axis_to_remove < dimension, "[Point] Invalid axis to remove" );
        OpenGeodeGeometryException::check_assertion(
            dimension > 1, "[Point] Invalid dimension to reduce" );
        Point< dimension - 1 > projected_point;
        geode::index_t dim_id{ 0 };
        for( const auto i : LRange{ dimension } )
        {
            if( i != axis_to_remove )
            {
                projected_point.set_value( dim_id, this->value( i ) );
                dim_id++;
            }
        }
        return projected_point;
    }

    template < index_t dimension >
    template < typename Archive >
    void Point< dimension >::serialize( Archive &serializer )
    {
        serializer.ext( *this,
            Growable< Archive, Point >{ { []( Archive &archive, Point &point ) {
                archive.container8b( point.values_ );
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
