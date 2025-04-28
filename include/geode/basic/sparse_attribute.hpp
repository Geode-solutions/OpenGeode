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

#include <memory>
#include <string_view>
#include <typeinfo>

#include <absl/container/flat_hash_map.h>

#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/vector.h>
#include <bitsery/ext/inheritance.h>
#include <bitsery/ext/std_map.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_utils.hpp>
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
     * Read and write interface for sparse attribute storage.
     * Only store what you use!
     */
    template < typename T >
    class SparseAttribute : public ReadOnlyAttribute< T >
    {
        friend class bitsery::Access;

    public:
        SparseAttribute( T default_value,
            AttributeProperties properties,
            AttributeBase::AttributeKey )
            : SparseAttribute(
                  std::move( default_value ), std::move( properties ) )
        {
        }

        [[nodiscard]] const T& value( index_t element ) const override
        {
            const auto it = values_.find( element );
            if( it != values_.end() )
            {
                return it->second;
            }
            return default_value_;
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
            const auto it = values_.find( element );
            if( it == values_.end() )
            {
                values_.emplace( element, default_value_ );
            }
            modifier( values_[element] );
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

    private:
        SparseAttribute( T default_value, AttributeProperties properties )
            : ReadOnlyAttribute< T >( std::move( properties ) ),
              default_value_( std::move( default_value ) )
        {
            values_.reserve( 10 );
        }

        SparseAttribute() : ReadOnlyAttribute< T >( AttributeProperties{} ) {};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, SparseAttribute< T > >{
                    { []( Archive& a, SparseAttribute< T >& attribute ) {
                        a.ext( attribute, bitsery::ext::BaseClass<
                                              ReadOnlyAttribute< T > >{} );
                        a( attribute.default_value_ );
                        a.ext( attribute.values_,
                            bitsery::ext::StdMap{
                                attribute.values_.max_size() },
                            []( Archive& a2, index_t& i, T& item ) {
                                a2.value4b( i );
                                a2( item );
                            } );
                    } } } );
            values_.reserve( 10 );
        }

        void resize( index_t /*unused*/, AttributeBase::AttributeKey ) override
        {
        }

        void reserve( index_t capacity, AttributeBase::AttributeKey ) override
        {
            values_.reserve( capacity );
        }

        void delete_elements( const std::vector< bool >& to_delete,
            AttributeBase::AttributeKey ) override
        {
            const auto old2new = detail::mapping_after_deletion( to_delete );
            auto old_values = std::move( values_ );
            values_ = decltype( values_ )();
            values_.reserve( old_values.size() );
            for( auto& [index, value] : old_values )
            {
                if( !to_delete[index] && value != default_value_ )
                {
                    values_.emplace( old2new[index], std::move( value ) );
                }
            }
        }

        void permute_elements( absl::Span< const index_t > permutation,
            AttributeBase::AttributeKey ) override
        {
            auto old_values = std::move( values_ );
            values_ = decltype( values_ )();
            values_.reserve( old_values.size() );
            for( auto& [index, value] : old_values )
            {
                values_.emplace( permutation[index], std::move( value ) );
            }
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > clone(
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< SparseAttribute< T > > attribute{
                new SparseAttribute< T >{ default_value_, this->properties() }
            };
            attribute->values_ = values_;
            return attribute;
        }

        void copy( const AttributeBase& attribute,
            index_t nb_elements,
            AttributeBase::AttributeKey ) override
        {
            const auto& typed_attribute =
                dynamic_cast< const SparseAttribute< T >& >( attribute );
            default_value_ = typed_attribute.default_value_;
            if( nb_elements != 0 )
            {
                for( const auto i : Range{ nb_elements } )
                {
                    if( typed_attribute.value( i ) != default_value_ )
                    {
                        values_[i] = typed_attribute.value( i );
                    }
                }
            }
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > extract(
            absl::Span< const index_t > old2new,
            index_t nb_elements,
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< SparseAttribute< T > > attribute{
                new SparseAttribute< T >{ default_value_, this->properties() }
            };
            for( const auto i : Indices{ old2new } )
            {
                const auto new_index = old2new[i];
                if( value( i ) != default_value_ && new_index != NO_ID )
                {
                    OPENGEODE_EXCEPTION( new_index < nb_elements,
                        "[SparseAttribute::extract] The given mapping "
                        "contains values that go beyond the given number of "
                        "elements." );
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
            std::shared_ptr< SparseAttribute< T > > attribute{
                new SparseAttribute< T >{ default_value_, this->properties() }
            };
            for( const auto& [in, outs] : old2new_mapping.in2out_map() )
            {
                if( value( in ) != default_value_ )
                {
                    for( const auto new_index : outs )
                    {
                        OPENGEODE_EXCEPTION( new_index < nb_elements,
                            "[SparseAttribute::extract] The given mapping "
                            "contains values that go beyond the given number "
                            "of elements." );
                        attribute->set_value( new_index, value( in ) );
                    }
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
                if( from.value( i ) != default_value_ && new_index != NO_ID )
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
                if( from.value( in ) != default_value_ )
                {
                    for( const auto new_index : outs )
                    {
                        this->set_value( new_index, from.value( in ) );
                    }
                }
            }
        }

    private:
        T default_value_;
        absl::flat_hash_map< index_t, T > values_{};
    };
} // namespace geode
