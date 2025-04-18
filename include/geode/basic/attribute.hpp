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
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/common.hpp>
#include <geode/basic/detail/mapping_after_deletion.hpp>
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
     * Base class defining the virtual API used by the AttributeManager.
     */
    class AttributeBase
    {
        OPENGEODE_DISABLE_COPY( AttributeBase );
        friend class bitsery::Access;

    public:
        PASSKEY( AttributeManager, AttributeKey );

        virtual ~AttributeBase() = default;

        [[nodiscard]] virtual float generic_value( index_t element ) const = 0;

        [[nodiscard]] virtual float generic_item_value(
            index_t element, local_index_t item ) const = 0;

        [[nodiscard]] virtual bool is_genericable() const = 0;

        [[nodiscard]] virtual local_index_t nb_items() const = 0;

        [[nodiscard]] virtual std::string_view type() = 0;

        [[nodiscard]] std::string_view name() const
        {
            return name_;
        }

        [[nodiscard]] const AttributeProperties& properties() const
        {
            return properties_;
        }

        void set_properties( AttributeProperties new_properties )
        {
            properties_ = std::move( new_properties );
        }

    public:
        void set_name( std::string_view name, AttributeKey )
        {
            name_ = to_string( name );
        }

        [[nodiscard]] virtual std::shared_ptr< AttributeBase > clone(
            AttributeKey ) const = 0;

        virtual void copy( const AttributeBase& attribute,
            index_t nb_elements,
            AttributeKey ) = 0;

        [[nodiscard]] virtual std::shared_ptr< AttributeBase > extract(
            absl::Span< const index_t > old2new,
            index_t nb_elements,
            AttributeKey ) const = 0;

        [[nodiscard]] virtual std::shared_ptr< AttributeBase > extract(
            const GenericMapping< index_t >& old2new_mapping,
            index_t nb_elements,
            AttributeKey ) const = 0;

        virtual void import( absl::Span< const index_t > old2new,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) = 0;

        virtual void import( const GenericMapping< index_t >& old2new_mapping,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) = 0;

        virtual void resize( index_t size, AttributeKey ) = 0;

        virtual void reserve( index_t capacity, AttributeKey ) = 0;

        virtual void delete_elements(
            const std::vector< bool >& to_delete, AttributeKey ) = 0;

        virtual void permute_elements(
            absl::Span< const index_t > permutation, AttributeKey ) = 0;

        virtual void compute_value(
            index_t from_element, index_t to_element, AttributeKey ) = 0;

        virtual void compute_value(
            const AttributeLinearInterpolation& interpolation,
            index_t to_element,
            AttributeKey ) = 0;

    private:
        AttributeBase() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext(
                *this, Growable< Archive, AttributeBase >{
                           { []( Archive& a, AttributeBase& attribute ) {
                                a.object( attribute.properties_ );
                            },
                               []( Archive& a, AttributeBase& attribute ) {
                                   a.object( attribute.properties_ );
                                   a.text1b( attribute.name_,
                                       attribute.name_.max_size() );
                               } } } );
        }

    protected:
        AttributeBase( AttributeProperties properties )
            : properties_( std::move( properties ) )
        {
        }

    private:
        AttributeProperties properties_;
        std::string name_;
    };

    /*!
     * Read only attribute interface that does not need to know
     * the underlying attribute storage type.
     */
    template < typename T >
    class ReadOnlyAttribute : public AttributeBase
    {
        friend class bitsery::Access;

    public:
        [[nodiscard]] virtual const T& value( index_t element ) const = 0;

        [[nodiscard]] std::string_view type() final
        {
            return typeid( T ).name();
        }

        [[nodiscard]] float generic_value( index_t element ) const final
        {
            return GenericAttributeConversion< T >::converted_value(
                value( element ) );
        }

        [[nodiscard]] float generic_item_value(
            index_t element, local_index_t item ) const final
        {
            return GenericAttributeConversion< T >::converted_item_value(
                value( element ), item );
        }

        [[nodiscard]] bool is_genericable() const final
        {
            return GenericAttributeConversion< T >::is_genericable();
        }

        [[nodiscard]] local_index_t nb_items() const final
        {
            return GenericAttributeConversion< T >::nb_items();
        }

    protected:
        ReadOnlyAttribute( AttributeProperties properties )
            : AttributeBase( std::move( properties ) )
        {
        }

    private:
        ReadOnlyAttribute() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, ReadOnlyAttribute< T > >{
                    { []( Archive& a, ReadOnlyAttribute< T >& attribute ) {
                        a.ext( attribute,
                            bitsery::ext::BaseClass< AttributeBase >{} );
                    } } } );
        }
    };

    /*!
     * Read and write interface for constant attribute storage
     */
    template < typename T >
    class ConstantAttribute : public ReadOnlyAttribute< T >
    {
        friend class bitsery::Access;

    public:
        ConstantAttribute( T value,
            AttributeProperties properties,
            AttributeBase::AttributeKey )
            : ConstantAttribute( std::move( value ), std::move( properties ) )
        {
        }

        [[nodiscard]] const T& value( index_t /*unused*/ ) const override
        {
            return value_;
        }

        [[nodiscard]] const T& value() const
        {
            return value_;
        }

        void set_value( T value )
        {
            value_ = std::move( value );
        }

        [[nodiscard]] const T& default_value() const
        {
            return value();
        }

        template < typename Modifier >
        void modify_value( Modifier&& modifier )
        {
            modifier( value_ );
        }

    public:
        void compute_value( index_t /*unused*/,
            index_t /*unused*/,
            AttributeBase::AttributeKey ) override
        {
        }

        void compute_value( const AttributeLinearInterpolation& /*unused*/,
            index_t /*unused*/,
            AttributeBase::AttributeKey ) override
        {
        }

    private:
        ConstantAttribute( T value, AttributeProperties properties )
            : ReadOnlyAttribute< T >( std::move( properties ) )
        {
            set_value( std::move( value ) );
        }

        ConstantAttribute()
            : ReadOnlyAttribute< T >( AttributeProperties{} ) {};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, ConstantAttribute< T > >{
                    { []( Archive& a, ConstantAttribute< T >& attribute ) {
                        a.ext( attribute, bitsery::ext::BaseClass<
                                              ReadOnlyAttribute< T > >{} );
                        a( attribute.value_ );
                    } } } );
        }

    public:
        void resize( index_t /*unused*/, AttributeBase::AttributeKey ) override
        {
        }

        void reserve( index_t /*unused*/, AttributeBase::AttributeKey ) override
        {
        }

        void delete_elements( const std::vector< bool >& /*unused*/,
            AttributeBase::AttributeKey ) override
        {
        }

        void permute_elements( absl::Span< const index_t > /*unused*/,
            AttributeBase::AttributeKey ) override
        {
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > clone(
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< ConstantAttribute< T > > attribute{
                new ConstantAttribute< T >{ value_, this->properties() }
            };
            return attribute;
        }

        void copy( const AttributeBase& attribute,
            index_t /*unused*/,
            AttributeBase::AttributeKey ) override
        {
            value_ = dynamic_cast< const ConstantAttribute< T >& >( attribute )
                         .value();
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > extract(
            absl::Span< const index_t > /* unused */,
            index_t /* unused */,
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< ConstantAttribute< T > > attribute{
                new ConstantAttribute< T >{ value_, this->properties() }
            };
            return attribute;
        }

        [[nodiscard]] std::shared_ptr< AttributeBase > extract(
            const GenericMapping< index_t >& /* unused */,
            index_t /* unused */,
            AttributeBase::AttributeKey ) const override
        {
            std::shared_ptr< ConstantAttribute< T > > attribute{
                new ConstantAttribute< T >{ value_, this->properties() }
            };
            return attribute;
        }

        void import( absl::Span< const index_t > /* unused */,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) override
        {
            import( dynamic_cast< const ReadOnlyAttribute< T >& >( *from ) );
        }

        void import( const GenericMapping< index_t >& /* unused */,
            const std::shared_ptr< AttributeBase >& from,
            AttributeBase::AttributeKey ) override
        {
            import( dynamic_cast< const ReadOnlyAttribute< T >& >( *from ) );
        }

        void import( const ReadOnlyAttribute< T >& from )
        {
            this->set_value( from.value( 0 ) );
        }

    private:
        T value_;
    };

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
