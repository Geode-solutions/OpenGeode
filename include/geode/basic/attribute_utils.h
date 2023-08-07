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

#pragma once

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/common.h>

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

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext(
                *this, Growable< Archive, AttributeProperties >{
                           { []( Archive& a, AttributeProperties& properties ) {
                               a.value1b( properties.assignable );
                               a.value1b( properties.interpolable );
                           } } } );
        }

        bool assignable{ true };
        bool interpolable{ false };
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
        AttributeLinearInterpolation( absl::FixedArray< index_t > indices,
            absl::FixedArray< double > lambdas )
            : indices_{ std::move( indices ) }, lambdas_{ std::move( lambdas ) }
        {
            OPENGEODE_EXCEPTION( indices_.size() == lambdas_.size(),
                "[AttributeLinearInterpolation] Both arrays should have the "
                "same size" );
        }

        template < template < typename > class Attribute, typename T >
        T compute_value( const Attribute< T >& attribute ) const
        {
            return AttributeLinearInterpolationImpl< T >::compute(
                *this, attribute );
        }

        const absl::FixedArray< index_t > indices_;
        const absl::FixedArray< double > lambdas_;
    };

#define IMPLICIT_ATTRIBUTE_LINEAR_INTERPOLATION( Type )                        \
    template <>                                                                \
    struct AttributeLinearInterpolationImpl< Type >                            \
    {                                                                          \
        template < template < typename > class Attribute >                     \
        static Type compute( const AttributeLinearInterpolation& interpolator, \
            const Attribute< Type >& attribute )                               \
        {                                                                      \
            Type result{ 0 };                                                  \
            for( auto i : Indices{ interpolator.indices_ } )                   \
            {                                                                  \
                result += interpolator.lambdas_[i]                             \
                          * attribute.value( interpolator.indices_[i] );       \
            }                                                                  \
            return result;                                                     \
        }                                                                      \
    }

    IMPLICIT_ATTRIBUTE_LINEAR_INTERPOLATION( float );
    IMPLICIT_ATTRIBUTE_LINEAR_INTERPOLATION( double );

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
        static float converted_value( const AttributeType& /*unused*/ )
        {
            return 0.;
        }

        static float converted_item_value(
            const AttributeType& /*unused*/, local_index_t /*unused*/ )
        {
            return 0.;
        }

        static bool is_genericable()
        {
            return false;
        }

        static local_index_t nb_items()
        {
            return 1;
        }
    };

#define IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( Type )                          \
    template <>                                                                \
    struct GenericAttributeConversion< Type >                                  \
    {                                                                          \
        static float converted_value( const Type& value )                      \
        {                                                                      \
            return static_cast< float >( value );                              \
        }                                                                      \
                                                                               \
        static float converted_item_value(                                     \
            const Type& value, local_index_t /*unused*/ )                      \
        {                                                                      \
            return converted_value( value );                                   \
        }                                                                      \
                                                                               \
        static bool is_genericable()                                           \
        {                                                                      \
            return true;                                                       \
        }                                                                      \
                                                                               \
        static local_index_t nb_items()                                        \
        {                                                                      \
            return 1;                                                          \
        }                                                                      \
    }

    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( bool );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( int );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( unsigned int );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( float );
    IMPLICIT_GENERIC_ATTRIBUTE_CONVERSION( double );

#define IMPLICIT_ARRAY_GENERIC_ATTRIBUTE_CONVERSION( Type )                    \
    template < size_t size >                                                   \
    struct GenericAttributeConversion< std::array< Type, size > >              \
    {                                                                          \
        using Container = std::array< Type, size >;                            \
        static float converted_value( const Container& value )                 \
        {                                                                      \
            return converted_item_value( value, 0 );                           \
        }                                                                      \
                                                                               \
        static float converted_item_value(                                     \
            const Container& value, local_index_t item )                       \
        {                                                                      \
            OPENGEODE_ASSERT( item < nb_items(),                               \
                "[GenericAttributeConversion] Accessing "                      \
                "incorrect item value" );                                      \
            return static_cast< float >( value[item] );                        \
        }                                                                      \
                                                                               \
        static bool is_genericable()                                           \
        {                                                                      \
            return true;                                                       \
        }                                                                      \
                                                                               \
        static local_index_t nb_items()                                        \
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
