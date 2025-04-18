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

#pragma once

#include <absl/types/span.h>

#include <geode/basic/common.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/types.hpp>

namespace geode
{
    struct AttributeLinearInterpolation;
} // namespace geode

namespace geode
{
    /*!
     * Struct holding Attribute flags used by the AttributeManager for
     * updating/computing attribute values
     */
    struct AttributeProperties
    {
        AttributeProperties() = default;

        AttributeProperties( bool is_assignable, bool is_interpolable )
            : assignable( is_assignable ), interpolable( is_interpolable )
        {
        }

        AttributeProperties(
            bool is_assignable, bool is_interpolable, bool is_transferable )
            : assignable( is_assignable ),
              interpolable( is_interpolable ),
              transferable( is_transferable )
        {
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, AttributeProperties >{
                    { []( Archive& a, AttributeProperties& properties ) {
                         a.value1b( properties.assignable );
                         a.value1b( properties.interpolable );
                     },
                        []( Archive& a, AttributeProperties& properties ) {
                            a.value1b( properties.assignable );
                            a.value1b( properties.interpolable );
                            a.value1b( properties.transferable );
                        } } } );
        }

        bool assignable{ false };
        bool interpolable{ false };
        bool transferable{ true };
    };

    /*!
     * Helper struct to interpolate an Attribute value.
     * This struct may be customized for a given type.
     * Example:
     * template <>
     * struct AttributeLinearInterpolationImpl< MyType >
     * {
     *     template < template < typename > class Attribute >
     *     static MyType compute(
     *         const AttributeLinearInterpolation& interpolator,
     *         const Attribute< MyType >& attribute )
     *     {
     *         // Interpolate using data from the interpolator
     *         return interpolated_value;
     *     }
     * };
     */
    template < typename AttributeType >
    struct AttributeLinearInterpolationImpl
    {
        template < template < typename > class Attribute >
        static AttributeType compute(
            const AttributeLinearInterpolation& /*unused*/,
            const Attribute< AttributeType >& attribute )
        {
            return attribute.default_value();
        }
    };

    struct AttributeLinearInterpolation
    {
        AttributeLinearInterpolation( absl::Span< const index_t > indices,
            absl::Span< const double > lambdas )
            : indices_( indices.size() ), lambdas_( lambdas.size() )
        {
            OPENGEODE_EXCEPTION( indices_.size() == lambdas_.size(),
                "[AttributeLinearInterpolation] Both arrays should have the "
                "same size" );
            for( const auto index : Indices{ indices } )
            {
                indices_[index] = indices[index];
                lambdas_[index] = lambdas[index];
            }
        }

        template < template < typename > class Attribute, typename T >
        [[nodiscard]] T compute_value( const Attribute< T >& attribute ) const
        {
            return AttributeLinearInterpolationImpl< T >::compute(
                *this, attribute );
        }

        absl::FixedArray< index_t > indices_;
        absl::FixedArray< double > lambdas_;
    };

#define IMPLICIT_ATTRIBUTE_LINEAR_INTERPOLATION( Type )                        \
    template <>                                                                \
    struct AttributeLinearInterpolationImpl< Type >                            \
    {                                                                          \
        template < template < typename > class Attribute >                     \
        [[nodiscard]] static Type compute(                                     \
            const AttributeLinearInterpolation& interpolator,                  \
            const Attribute< Type >& attribute )                               \
        {                                                                      \
            Type result{ 0 };                                                  \
            bool is_same{ true };                                              \
            const auto& first_value =                                          \
                attribute.value( interpolator.indices_[0] );                   \
            for( auto i : Indices{ interpolator.indices_ } )                   \
            {                                                                  \
                const auto& value =                                            \
                    attribute.value( interpolator.indices_[i] );               \
                if( is_same )                                                  \
                {                                                              \
                    is_same = value == first_value;                            \
                }                                                              \
                result += interpolator.lambdas_[i] * value;                    \
            }                                                                  \
            if( is_same )                                                      \
            {                                                                  \
                return first_value;                                            \
            }                                                                  \
            return result;                                                     \
        }                                                                      \
    }

    IMPLICIT_ATTRIBUTE_LINEAR_INTERPOLATION( float );
    IMPLICIT_ATTRIBUTE_LINEAR_INTERPOLATION( double );

#define IMPLICIT_ARRAY_ATTRIBUTE_LINEAR_INTERPOLATION( Type )                  \
    template < size_t array_size >                                             \
    struct AttributeLinearInterpolationImpl< std::array< Type, array_size > >  \
    {                                                                          \
        template < template < typename > class Attribute >                     \
        [[nodiscard]] static std::array< Type, array_size > compute(           \
            const AttributeLinearInterpolation& interpolator,                  \
            const Attribute< std::array< Type, array_size > >& attribute )     \
        {                                                                      \
            std::array< Type, array_size > result;                             \
            result.fill( 0 );                                                  \
            bool is_same{ true };                                              \
            const auto& first_value =                                          \
                attribute.value( interpolator.indices_[0] );                   \
            for( const auto vertex_id : Indices{ interpolator.indices_ } )     \
            {                                                                  \
                const auto& array_value =                                      \
                    attribute.value( interpolator.indices_[vertex_id] );       \
                for( const auto position : Indices{ array_value } )            \
                {                                                              \
                    if( is_same )                                              \
                    {                                                          \
                        is_same =                                              \
                            array_value[position] == first_value[position];    \
                    }                                                          \
                    result[position] += interpolator.lambdas_[vertex_id]       \
                                        * array_value[position];               \
                }                                                              \
            }                                                                  \
            if( is_same )                                                      \
            {                                                                  \
                return first_value;                                            \
            }                                                                  \
            return result;                                                     \
        }                                                                      \
    }

    IMPLICIT_ARRAY_ATTRIBUTE_LINEAR_INTERPOLATION( float );
    IMPLICIT_ARRAY_ATTRIBUTE_LINEAR_INTERPOLATION( double );

    /*!
     * Helper struct to convert an Attribute value to generic float.
     * This struct may be customized for a given type.
     * Example:
     * template <>
     * struct GenericAttributeConversion< MyType >
     * {
     *      static float converted_value( const MyType& value )
     *      {
     *          return value.get_a_float();
     *      }
     * };
     */
    template < typename AttributeType >
    struct GenericAttributeConversion
    {
        [[nodiscard]] static float converted_value(
            const AttributeType& /*unused*/ )
        {
            return 0.;
        }

        [[nodiscard]] static float converted_item_value(
            const AttributeType& /*unused*/, local_index_t /*unused*/ )
        {
            return 0.;
        }

        [[nodiscard]] static bool is_genericable()
        {
            return false;
        }

        [[nodiscard]] static local_index_t nb_items()
        {
            return 1;
        }
    };

#define IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( Type )                          \
    template <>                                                                \
    struct GenericAttributeConversion< Type >                                  \
    {                                                                          \
        [[nodiscard]] static float converted_value( const Type& value )        \
        {                                                                      \
            return static_cast< float >( value );                              \
        }                                                                      \
                                                                               \
        [[nodiscard]] static float converted_item_value(                       \
            const Type& value, local_index_t /*unused*/ )                      \
        {                                                                      \
            return converted_value( value );                                   \
        }                                                                      \
                                                                               \
        [[nodiscard]] static bool is_genericable()                             \
        {                                                                      \
            return true;                                                       \
        }                                                                      \
                                                                               \
        [[nodiscard]] static local_index_t nb_items()                          \
        {                                                                      \
            return 1;                                                          \
        }                                                                      \
    }

    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( bool );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( signed char );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( unsigned char );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( short );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( unsigned short );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( int );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( unsigned int );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( float );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( double );

#define IMPLICIT_ARRAY_GENERIC_ATTRIBUTE_CONVERSION( Type )                    \
    template < size_t size >                                                   \
    struct GenericAttributeConversion< std::array< Type, size > >              \
    {                                                                          \
        using Container = std::array< Type, size >;                            \
        [[nodiscard]] static float converted_value( const Container& value )   \
        {                                                                      \
            return converted_item_value( value, 0 );                           \
        }                                                                      \
                                                                               \
        [[nodiscard]] static float converted_item_value(                       \
            const Container& value, local_index_t item )                       \
        {                                                                      \
            OPENGEODE_ASSERT( item < nb_items(),                               \
                "[GenericAttributeConversion] Accessing "                      \
                "incorrect item value" );                                      \
            return static_cast< float >( value[item] );                        \
        }                                                                      \
                                                                               \
        [[nodiscard]] static bool is_genericable()                             \
        {                                                                      \
            return true;                                                       \
        }                                                                      \
                                                                               \
        [[nodiscard]] static local_index_t nb_items()                          \
        {                                                                      \
            return size;                                                       \
        }                                                                      \
    }

    IMPLICIT_ARRAY_GENERIC_ATTRIBUTE_CONVERSION( bool );
    IMPLICIT_ARRAY_GENERIC_ATTRIBUTE_CONVERSION( int );
    IMPLICIT_ARRAY_GENERIC_ATTRIBUTE_CONVERSION( unsigned int );
    IMPLICIT_ARRAY_GENERIC_ATTRIBUTE_CONVERSION( float );
    IMPLICIT_ARRAY_GENERIC_ATTRIBUTE_CONVERSION( double );
} // namespace geode
