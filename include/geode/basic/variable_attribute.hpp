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

#include <memory>
#include <string_view>

#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/vector.h>
#include <bitsery/ext/inheritance.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute.hpp>
#include <geode/basic/common.hpp>
#include <geode/basic/detail/mapping_after_deletion.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/mapping.hpp>
#include <geode/basic/passkey.hpp>
#include <geode/basic/permutation.hpp>

namespace geode
{
    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * Read and write interface for variable attribute storage
     */
    template < typename T >
    class VariableAttribute : public ReadOnlyAttribute< T >
    {
        friend class bitsery::Access;

    public:
        VariableAttribute( T default_value,
            AttributeProperties properties,
            AttributeBase::AttributeKey )
            : VariableAttribute(
                  std::move( default_value ), std::move( properties ) )
        {
        }

        [[nodiscard]] const T& value( index_t element ) const override
        {
            return values_[element];
        }

        void set_value( index_t element, T value )
        {
            values_[element] = std::move( value );
        }

        [[nodiscard]] const T& default_value() const
        {
            return default_value_;
        }

        template < typename Modifier >
        void modify_value( index_t element, Modifier&& modifier )
        {
            modifier( values_[element] );
        }

        [[nodiscard]] index_t size() const
        {
            return values_.size();
        }

    public:
        void compute_value( index_t from_element,
            index_t to_element,
            AttributeBase::AttributeKey ) override
        {
            set_value( to_element, value( from_element ) );
        }

        void compute_value( const AttributeLinearInterpolation& interpolation,
            index_t to_element,
            AttributeBase::AttributeKey ) override
        {
            set_value( to_element, interpolation.compute_value( *this ) );
        }

    protected:
        VariableAttribute( T default_value, AttributeProperties properties )
            : ReadOnlyAttribute< T >( std::move( properties ) ),
              default_value_( std::move( default_value ) )
        {
            values_.reserve( 10 );
        }

        VariableAttribute()
            : ReadOnlyAttribute< T >( AttributeProperties{} ) {};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, VariableAttribute< T > >{
                    { []( Archive& a, VariableAttribute< T >& attribute ) {
                        a.ext( attribute, bitsery::ext::BaseClass<
                                              ReadOnlyAttribute< T > >{} );
                        a( attribute.default_value_ );
                        a.container( attribute.values_,
                            attribute.values_.max_size(),
                            []( Archive& a2, T& item ) {
                                a2( item );
                            } );
                    } } } );
            values_.reserve( 10 );
        }

        void resize( index_t size, AttributeBase::AttributeKey ) override
        {
            const auto capacity = static_cast< index_t >( values_.capacity() );
            if( size > capacity )
            {
                const auto next_capacity = capacity * 2;
                values_.reserve( std::max( size, next_capacity ) );
            }
            values_.resize( size, default_value_ );
        }

        void reserve( index_t capacity, AttributeBase::AttributeKey ) override
        {
            values_.reserve( capacity );
        }

        void delete_elements( const std::vector< bool >& to_delete,
            AttributeBase::AttributeKey ) override
        {
            delete_vector_elements( to_delete, values_ );
        }

        void permute_elements( absl::Span< const index_t > permutation,
            AttributeBase::AttributeKey ) override
        {
            permute( values_, permutation );
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > clone(
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< VariableAttribute< T > > attribute{
                new VariableAttribute< T >{ default_value_, this->properties() }
            };
            attribute->values_ = values_;
            return attribute;
        }

        void copy( const AttributeBase& attribute,
            index_t nb_elements,
            AttributeBase::AttributeKey ) override
        {
            const auto& typed_attribute =
                dynamic_cast< const VariableAttribute< T >& >( attribute );
            default_value_ = typed_attribute.default_value_;
            if( nb_elements != 0 )
            {
                values_.resize( nb_elements, default_value_ );
                for( const auto i : Range{ nb_elements } )
                {
                    values_[i] = typed_attribute.value( i );
                }
            }
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > extract(
            absl::Span< const index_t > old2new,
            index_t nb_elements,
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< VariableAttribute< T > > attribute{
                new VariableAttribute< T >{ default_value_, this->properties() }
            };
            attribute->values_.resize( nb_elements, default_value_ );
            for( const auto i : Indices{ old2new } )
            {
                const auto new_index = old2new[i];
                if( new_index != NO_ID )
                {
                    OPENGEODE_EXCEPTION( new_index < nb_elements,
                        "[VariableAttribute::extract] The given mapping "
                        "contains values (",
                        new_index,
                        ") that go beyond the given number "
                        "of elements (",
                        nb_elements, ")." );
                    attribute->set_value( new_index, value( i ) );
                }
            }
            return attribute;
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > extract(
            const GenericMapping< index_t >& old2new_mapping,
            index_t nb_elements,
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< VariableAttribute< T > > attribute{
                new VariableAttribute< T >{ default_value_, this->properties() }
            };
            attribute->values_.resize( nb_elements, default_value_ );
            for( const auto& [in, outs] : old2new_mapping.in2out_map() )
            {
                for( const auto new_index : outs )
                {
                    OPENGEODE_EXCEPTION( new_index < nb_elements,
                        "[VariableAttribute::extract] The given mapping "
                        "contains values (",
                        new_index,
                        ") that go beyond the given number "
                        "of elements (",
                        nb_elements, ")." );
                    attribute->set_value( new_index, value( in ) );
                }
            }
            return attribute;
        }

        void import( absl::Span< const index_t > old2new,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) override
        {
            import( old2new,
                dynamic_cast< const ReadOnlyAttribute< T >& >( *from ) );
        }

        void import( const GenericMapping< index_t >& old2new_mapping,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) override
        {
            import( old2new_mapping,
                dynamic_cast< const ReadOnlyAttribute< T >& >( *from ) );
        }

        void import( absl::Span< const index_t > old2new,
            const ReadOnlyAttribute< T >& from )
        {
            for( const auto i : Indices{ old2new } )
            {
                const auto new_index = old2new[i];
                if( new_index != NO_ID )
                {
                    this->set_value( new_index, from.value( i ) );
                }
            }
        }

        void import( const GenericMapping< index_t >& old2new_mapping,
            const ReadOnlyAttribute< T >& from )
        {
            for( const auto& [in, outs] : old2new_mapping.in2out_map() )
            {
                for( const auto new_index : outs )
                {
                    this->set_value( new_index, from.value( in ) );
                }
            }
        }

    private:
        T default_value_;
        std::vector< T > values_{};
    };

    /*!
     * Read and write interface for boolean variable attribute storage.
     * This class removes the custom storage use by the STL for
     * std::vector of bool.
     */
    template <>
    class VariableAttribute< bool > : public ReadOnlyAttribute< bool >
    {
        friend class bitsery::Access;

    public:
        VariableAttribute( bool default_value,
            AttributeProperties properties,
            AttributeBase::AttributeKey )
            : VariableAttribute( default_value, std::move( properties ) )
        {
        }

        [[nodiscard]] const bool& value( index_t element ) const override
        {
            return reinterpret_cast< const bool& >( values_[element] );
        }

        void set_value( index_t element, bool value )
        {
            values_[element] = std::move( value );
        }

        [[nodiscard]] bool default_value() const
        {
            return default_value_;
        }

        template < typename Modifier >
        void modify_value( index_t element, Modifier&& modifier )
        {
            modifier( reinterpret_cast< bool& >( values_[element] ) );
        }

        [[nodiscard]] index_t size() const
        {
            return values_.size();
        }

    public:
        void compute_value( index_t from_element,
            index_t to_element,
            AttributeBase::AttributeKey ) override
        {
            set_value( to_element, value( from_element ) );
        }

        void compute_value( const AttributeLinearInterpolation& interpolation,
            index_t to_element,
            AttributeBase::AttributeKey ) override
        {
            set_value( to_element, interpolation.compute_value( *this ) );
        }

    protected:
        VariableAttribute( bool default_value, AttributeProperties properties )
            : ReadOnlyAttribute< bool >( std::move( properties ) ),
              default_value_( default_value )
        {
            values_.reserve( 10 );
        }

        VariableAttribute()
            : ReadOnlyAttribute< bool >( AttributeProperties{} ) {};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, VariableAttribute< bool > >{
                    { []( Archive& a, VariableAttribute< bool >& attribute ) {
                        a.ext( attribute, bitsery::ext::BaseClass<
                                              ReadOnlyAttribute< bool > >{} );
                        a.value1b( attribute.default_value_ );
                        a.container1b(
                            attribute.values_, attribute.values_.max_size() );
                    } } } );
            values_.reserve( 10 );
        }

        void resize( index_t size, AttributeBase::AttributeKey ) override
        {
            const auto capacity = static_cast< index_t >( values_.capacity() );
            if( size > capacity )
            {
                const auto next_capacity = capacity * 2;
                values_.reserve( std::max( size, next_capacity ) );
            }
            values_.resize( size, default_value_ );
        }

        void reserve( index_t capacity, AttributeBase::AttributeKey ) override
        {
            values_.reserve( capacity );
        }

        void delete_elements( const std::vector< bool >& to_delete,
            AttributeBase::AttributeKey ) override
        {
            delete_vector_elements( to_delete, values_ );
        }

        void permute_elements( absl::Span< const index_t > permutation,
            AttributeBase::AttributeKey ) override
        {
            permute( values_, permutation );
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > clone(
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< VariableAttribute< bool > > attribute{
                new VariableAttribute< bool >{
                    static_cast< bool >( default_value_ ), this->properties() }
            };
            attribute->values_ = values_;
            return attribute;
        }

        void copy( const AttributeBase& attribute,
            index_t nb_elements,
            AttributeBase::AttributeKey ) override
        {
            const auto& typed_attribute =
                dynamic_cast< const VariableAttribute< bool >& >( attribute );
            default_value_ = typed_attribute.default_value_;
            if( nb_elements != 0 )
            {
                values_.resize( nb_elements );
                for( const auto i : Range{ nb_elements } )
                {
                    values_[i] = typed_attribute.value( i );
                }
            }
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > extract(
            absl::Span< const index_t > old2new,
            index_t nb_elements,
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< VariableAttribute< bool > > attribute{
                new VariableAttribute< bool >{
                    static_cast< bool >( default_value_ ), this->properties() }
            };
            attribute->values_.resize( nb_elements, default_value_ );
            for( const auto i : Indices{ old2new } )
            {
                const auto new_index = old2new[i];
                if( new_index != NO_ID )
                {
                    OPENGEODE_EXCEPTION( new_index < nb_elements,
                        "[VariableAttribute::extract] The given mapping "
                        "contains values (",
                        new_index,
                        ") that go beyond the given number "
                        "of elements (",
                        nb_elements, ")." );
                    attribute->set_value( new_index, value( i ) );
                }
            }
            return attribute;
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > extract(
            const GenericMapping< index_t >& old2new_mapping,
            index_t nb_elements,
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< VariableAttribute< bool > > attribute{
                new VariableAttribute< bool >{
                    static_cast< bool >( default_value_ ), this->properties() }
            };
            attribute->values_.resize( nb_elements, default_value_ );
            for( const auto& [in, outs] : old2new_mapping.in2out_map() )
            {
                for( const auto new_index : outs )
                {
                    OPENGEODE_EXCEPTION( new_index < nb_elements,
                        "[VariableAttribute::extract] The given mapping "
                        "contains values (",
                        new_index,
                        ") that go beyond the given number "
                        "of elements (",
                        nb_elements, ")." );
                    attribute->set_value( new_index, value( in ) );
                }
            }
            return attribute;
        }

        void import( absl::Span< const index_t > old2new,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) override
        {
            import( old2new,
                dynamic_cast< const ReadOnlyAttribute< bool >& >( *from ) );
        }

        void import( const GenericMapping< index_t >& old2new_mapping,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) override
        {
            import( old2new_mapping,
                dynamic_cast< const ReadOnlyAttribute< bool >& >( *from ) );
        }

        void import( absl::Span< const index_t > old2new,
            const ReadOnlyAttribute< bool >& from )
        {
            for( const auto i : Indices{ old2new } )
            {
                const auto new_index = old2new[i];
                if( new_index != NO_ID )
                {
                    this->set_value( new_index, from.value( i ) );
                }
            }
        }

        void import( const GenericMapping< index_t >& old2new_mapping,
            const ReadOnlyAttribute< bool >& from )
        {
            for( const auto& [in, outs] : old2new_mapping.in2out_map() )
            {
                for( const auto new_index : outs )
                {
                    this->set_value( new_index, from.value( in ) );
                }
            }
        }

    private:
        unsigned char default_value_;
        std::vector< unsigned char > values_{};
    };
} // namespace geode
