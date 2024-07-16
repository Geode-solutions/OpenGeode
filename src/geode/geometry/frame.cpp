/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <optional>

#include <geode/geometry/frame.hpp>

#include <geode/geometry/vector.hpp>

namespace
{
    template < geode::index_t dimension >
    std::optional< geode::Frame< dimension > > compute_inverse(
        geode::Frame< dimension > frame )
    {
        std::optional< geode::Frame< dimension > > result{ std::in_place };
        for( const auto i : geode::LRange{ dimension } )
        {
            auto value = frame.direction( i ).value( i );
            auto index = i;
            for( const auto j : geode::LRange{ i + 1, dimension } )
            {
                const auto& cur_value = frame.direction( j ).value( i );
                if( std::fabs( cur_value ) > std::fabs( value ) )
                {
                    index = j;
                    value = cur_value;
                }
            }

            if( value == 0 )
            {
                return std::nullopt;
            }

            if( index != i )
            {
                auto temp_result = result->direction( i );
                result->set_direction( i, result->direction( index ) );
                result->set_direction( index, std::move( temp_result ) );
                auto temp_frame = frame.direction( i );
                frame.set_direction( i, frame.direction( index ) );
                frame.set_direction( index, std::move( temp_frame ) );
            }

            frame.set_direction( i, frame.direction( i ) / value );
            result->set_direction( i, result->direction( i ) / value );

            for( const auto j : geode::LRange{ dimension } )
            {
                if( j == i )
                {
                    continue;
                }
                const auto scale = frame.direction( j ).value( i );
                frame.set_direction(
                    j, frame.direction( j ) - frame.direction( i ) * scale );
                result->set_direction( j,
                    result->direction( j ) - result->direction( i ) * scale );
            }
        }

        return result;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    Frame< dimension >::Frame()
    {
        for( const auto d : LRange{ dimension } )
        {
            Vector< dimension > direction;
            direction.set_value( d, 1 );
            set_direction( d, std::move( direction ) );
        }
    }

    template < index_t dimension >
    Frame< dimension >::Frame(
        std::array< Vector< dimension >, dimension > directions )
        : frame_( std::move( directions ) )
    {
    }

    template < index_t dimension >
    const Vector< dimension >& Frame< dimension >::direction(
        local_index_t index ) const
    {
        return frame_.at( index );
    }

    template < index_t dimension >
    void Frame< dimension >::set_direction(
        local_index_t index, Vector< dimension > direction )
    {
        frame_.at( index ) = std::move( direction );
    }

    template < index_t dimension >
    Frame< dimension > Frame< dimension >::inverse() const
    {
        auto result = compute_inverse( *this );
        OPENGEODE_EXCEPTION( result.has_value(),
            "[Frame<dimension>::inverse] Failed to inverse" );
        return result.value();
    }

    template < index_t dimension >
    Frame< dimension > Frame< dimension >::operator+(
        const Frame< dimension >& rhs ) const
    {
        Frame result;
        for( const auto d : geode::LRange{ dimension } )
        {
            result.set_direction( d, direction( d ) + rhs.direction( d ) );
        }
        return result;
    }

    template < index_t dimension >
    Vector< dimension > Frame< dimension >::operator*(
        const Vector< dimension >& rhs ) const
    {
        Vector< dimension > result;
        for( const auto d : geode::LRange{ dimension } )
        {
            result.set_value( d, direction( d ).dot( rhs ) );
        }
        return result;
    }

    template < index_t dimension >
    Frame< dimension > Frame< dimension >::rescale( double length ) const
    {
        Frame result;
        for( const auto d : geode::LRange{ dimension } )
        {
            result.set_direction( d, direction( d ) * length );
        }
        return result;
    }

    template < index_t dimension >
    std::string Frame< dimension >::string() const
    {
        auto output = absl::StrCat( "(", frame_[0].string() );
        for( const auto f : LRange{ dimension - 1 } )
        {
            absl::StrAppend( &output, " -- ", frame_[f].string() );
        }
        absl::StrAppend( &output, ")" );
        return output;
    }

    template < index_t dimension >
    template < typename Archive >
    void Frame< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, Frame >{ { []( Archive& a, Frame& frame ) {
                a.container( frame.frame_ );
            } } } );
    }

    template class opengeode_geometry_api Frame< 1 >;
    template class opengeode_geometry_api Frame< 2 >;
    template class opengeode_geometry_api Frame< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Frame< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Frame< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_geometry_api, Frame< 3 > );
} // namespace geode
