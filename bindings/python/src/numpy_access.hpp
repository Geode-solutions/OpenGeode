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

#pragma once

#include "common.hpp"

#include <cstdint>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include <pybind11/numpy.h>

/*!
 * Conversions between numpy arrays and C++ containers, shared by the bulk
 * access functions. Every function reading an array validates it (shape,
 * dtype, values) and copies it into a C++ container while the GIL is held, so
 * that the caller can then release the GIL and modify the mesh or attribute
 * without touching any Python object.
 * All the errors are raised before anything is modified: ValueError for the
 * shape and the dtype, IndexError for an index out of range.
 */
namespace geode
{
    /*!
     * Argument of the functions reading an array: a numpy array, or any
     * object numpy can convert to an array (list of lists...), which is
     * typed numpy.typing.ArrayLike in the signatures. Its dtype is checked by
     * the function.
     */
    class ArrayLike : public pybind11::array
    {
    public:
        ArrayLike() = default;
        explicit ArrayLike( pybind11::array array )
            : pybind11::array( std::move( array ) )
        {
        }
    };

    namespace numpy_detail
    {
        inline std::string shape_string( const pybind11::array& array )
        {
            std::string result{ "(" };
            for( pybind11::ssize_t i = 0; i < array.ndim(); ++i )
            {
                absl::StrAppend(
                    &result, i == 0 ? "" : ", ", array.shape( i ) );
            }
            if( array.ndim() == 1 )
            {
                result += ",";
            }
            return result + ")";
        }

        /*!
         * Return true if the value of an integer type is in the range of
         * Type, whatever the signedness of both types.
         */
        template < typename Type, typename Value >
        bool fits( Value value )
        {
            if constexpr( std::is_signed_v< Value > )
            {
                if( value < 0 )
                {
                    return std::numeric_limits< Type >::is_signed
                           && value >= static_cast< int64_t >(
                                  std::numeric_limits< Type >::min() );
                }
            }
            return static_cast< uint64_t >( value ) <= static_cast< uint64_t >(
                       std::numeric_limits< Type >::max() );
        }

        inline char dtype_kind( const pybind11::array& array )
        {
            return array.dtype().kind();
        }

        inline std::string dtype_string( const pybind11::array& array )
        {
            return pybind11::str( array.dtype() ).cast< std::string >();
        }
    } // namespace numpy_detail

    /*!
     * Raise a ValueError if the array is not of the given shape. A negative
     * extent in the expected shape matches any size.
     */
    inline void check_array_shape( std::string_view context,
        std::string_view name,
        const pybind11::array& array,
        std::initializer_list< pybind11::ssize_t > expected )
    {
        auto match =
            array.ndim() == static_cast< pybind11::ssize_t >( expected.size() );
        pybind11::ssize_t i{ 0 };
        for( const auto extent : expected )
        {
            match = match && ( extent < 0 || array.shape( i ) == extent );
            ++i;
        }
        if( match )
        {
            return;
        }
        std::string wanted{ "(" };
        i = 0;
        for( const auto extent : expected )
        {
            absl::StrAppend( &wanted, i == 0 ? "" : ", ",
                extent < 0 ? std::string{ "n" } : std::to_string( extent ) );
            ++i;
        }
        if( expected.size() == 1 )
        {
            wanted += ",";
        }
        wanted += ")";
        throw pybind11::value_error(
            absl::StrCat( "[", context, "] ", name, " must have shape ", wanted,
                ", got ", numpy_detail::shape_string( array ) ) );
    }

    /*!
     * Copy an array of integers of any dtype (signed or unsigned) into
     * indices of the given maximum. Raise a ValueError for another dtype
     * (floats, booleans, objects...) and an IndexError for a value that is
     * negative or not in [0, size[.
     * @param[in] what Kind of the indices (vertex...), for the error message.
     */
    inline std::vector< index_t > copy_indices( std::string_view context,
        std::string_view name,
        std::string_view what,
        const pybind11::array& array,
        index_t size )
    {
        // The position of a value in the error message: row and column of a
        // 2D array, index of a 1D array
        const auto nb_columns =
            array.ndim() == 2 ? static_cast< size_t >( array.shape( 1 ) ) : 0;
        const auto kind = numpy_detail::dtype_kind( array );
        if( kind != 'i' && kind != 'u' )
        {
            throw pybind11::value_error( absl::StrCat( "[", context, "] ", name,
                " must be an array of integers, got dtype ",
                numpy_detail::dtype_string( array ) ) );
        }
        const auto nb = static_cast< size_t >( array.size() );
        std::vector< index_t > result( nb );
        const auto fail = [&]( size_t position, const std::string& value ) {
            throw pybind11::index_error( absl::StrCat( "[", context, "] ", what,
                " ", value, " out of range [0, ", size, ") at ",
                nb_columns == 0 ? absl::StrCat( "position ", position )
                                : absl::StrCat( "row ", position / nb_columns,
                                      ", column ", position % nb_columns ),
                " of ", name ) );
        };
        if( kind == 'i' )
        {
            const auto values = pybind11::array_t< int64_t,
                pybind11::array::c_style
                    | pybind11::array::forcecast >::ensure( array );
            const auto* data = values.data();
            for( size_t i = 0; i < nb; ++i )
            {
                if( data[i] < 0 || data[i] >= static_cast< int64_t >( size ) )
                {
                    fail( i, std::to_string( data[i] ) );
                }
                result[i] = static_cast< index_t >( data[i] );
            }
        }
        else
        {
            const auto values = pybind11::array_t< uint64_t,
                pybind11::array::c_style
                    | pybind11::array::forcecast >::ensure( array );
            const auto* data = values.data();
            for( size_t i = 0; i < nb; ++i )
            {
                if( data[i] >= size )
                {
                    fail( i, std::to_string( data[i] ) );
                }
                result[i] = static_cast< index_t >( data[i] );
            }
        }
        return result;
    }

    /*!
     * Copy an array of integers of any dtype into a vector of Type, which
     * must be an integer type. Raise a ValueError if the dtype is not an
     * integer one or a value does not fit in Type.
     */
    template < typename Type >
    std::vector< Type > copy_integers( std::string_view context,
        std::string_view name,
        const pybind11::array& array )
    {
        const auto kind = numpy_detail::dtype_kind( array );
        if( kind != 'i' && kind != 'u' )
        {
            throw pybind11::value_error( absl::StrCat( "[", context, "] ", name,
                " must be an array of integers, got dtype ",
                numpy_detail::dtype_string( array ) ) );
        }
        const auto nb = static_cast< size_t >( array.size() );
        std::vector< Type > result( nb );
        const auto convert = [&]( const auto& values ) {
            const auto* data = values.data();
            for( size_t i = 0; i < nb; ++i )
            {
                if( !numpy_detail::fits< Type >( data[i] ) )
                {
                    throw pybind11::value_error( absl::StrCat( "[", context,
                        "] value ", data[i], " at position ", i, " of ", name,
                        " does not fit in the ", sizeof( Type ) * 8, "-bit ",
                        std::numeric_limits< Type >::is_signed ? "signed"
                                                               : "unsigned",
                        " integer values" ) );
                }
                result[i] = static_cast< Type >( data[i] );
            }
        };
        if( kind == 'i' )
        {
            convert( pybind11::array_t< int64_t,
                pybind11::array::c_style
                    | pybind11::array::forcecast >::ensure( array ) );
        }
        else
        {
            convert( pybind11::array_t< uint64_t,
                pybind11::array::c_style
                    | pybind11::array::forcecast >::ensure( array ) );
        }
        return result;
    }

    /*!
     * Copy an array of reals (floats or integers of any dtype) into a vector
     * of Type, which must be a floating point type. Raise a ValueError for
     * another dtype (booleans, complex numbers, objects...).
     */
    template < typename Type >
    std::vector< Type > copy_reals( std::string_view context,
        std::string_view name,
        const pybind11::array& array )
    {
        const auto kind = numpy_detail::dtype_kind( array );
        if( kind != 'f' && kind != 'i' && kind != 'u' )
        {
            throw pybind11::value_error( absl::StrCat( "[", context, "] ", name,
                " must be an array of floats or integers, got dtype ",
                numpy_detail::dtype_string( array ) ) );
        }
        const auto values = pybind11::array_t< Type,
            pybind11::array::c_style
                | pybind11::array::forcecast >::ensure( array );
        return { values.data(), values.data() + values.size() };
    }

    /*!
     * Copy an array of booleans. Raise a ValueError for another dtype.
     */
    inline std::vector< bool > copy_booleans( std::string_view context,
        std::string_view name,
        const pybind11::array& array )
    {
        if( numpy_detail::dtype_kind( array ) != 'b' )
        {
            throw pybind11::value_error( absl::StrCat( "[", context, "] ", name,
                " must be an array of booleans, got dtype ",
                numpy_detail::dtype_string( array ) ) );
        }
        const auto values = pybind11::array_t< bool,
            pybind11::array::c_style
                | pybind11::array::forcecast >::ensure( array );
        std::vector< bool > result( static_cast< size_t >( values.size() ) );
        for( size_t i = 0; i < result.size(); ++i )
        {
            result[i] = values.data()[i];
        }
        return result;
    }

    /*!
     * Create a numpy array of Type of the given shape, filled by
     * fill( pointer ) with the GIL released.
     */
    template < typename Type, typename Fill >
    pybind11::array_t< Type > make_array(
        std::vector< pybind11::ssize_t > shape, Fill&& fill )
    {
        pybind11::array_t< Type > result( std::move( shape ) );
        auto* data = result.mutable_data();
        {
            pybind11::gil_scoped_release release;
            fill( data );
        }
        return result;
    }
} // namespace geode

namespace pybind11
{
    namespace detail
    {
        template <>
        struct type_caster< geode::ArrayLike >
        {
        public:
            PYBIND11_TYPE_CASTER(
                geode::ArrayLike, const_name( "numpy.typing.ArrayLike" ) );

            bool load( handle source, bool convert )
            {
                if( isinstance< array >( source ) )
                {
                    value = geode::ArrayLike{ reinterpret_borrow< array >(
                        source ) };
                    return true;
                }
                if( !convert )
                {
                    return false;
                }
                auto converted = array::ensure( source );
                if( !converted )
                {
                    PyErr_Clear();
                    return false;
                }
                value = geode::ArrayLike{ std::move( converted ) };
                return true;
            }

            static handle cast(
                const geode::ArrayLike& source, return_value_policy, handle )
            {
                return source.inc_ref();
            }
        };
    } // namespace detail
} // namespace pybind11
