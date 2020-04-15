/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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
#include <typeinfo>

#include <absl/container/flat_hash_map.h>
#include <absl/strings/string_view.h>

#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/vector.h>
#include <bitsery/ext/inheritance.h>
#include <bitsery/ext/std_map.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/attribute_utils.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/common.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/passkey.h>

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

    public:
        virtual ~AttributeBase() = default;

        virtual std::shared_ptr< AttributeBase > clone() const = 0;

        virtual void copy(
            const AttributeBase& attribute, index_t nb_elements ) = 0;

        virtual float generic_value( index_t element ) const = 0;

        virtual absl::string_view type() = 0;

        virtual void resize( index_t size, AttributeKey ) = 0;

        virtual void reserve( index_t capacity, AttributeKey ) = 0;

        virtual void delete_elements(
            const std::vector< bool >& to_delete, AttributeKey ) = 0;

        virtual void compute_value(
            index_t from_element, index_t to_element, AttributeKey ) = 0;

        virtual void compute_value(
            const AttributeLinearInterpolation& interpolation,
            index_t to_element,
            AttributeKey ) = 0;

        const AttributeProperties& properties() const
        {
            return properties_;
        }

    private:
        AttributeBase() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, AttributeBase >{
                    { []( Archive& /*unused*/, AttributeBase& /*unused*/ ) {},
                        []( Archive& archive, AttributeBase& attribute ) {
                            archive.object( attribute.properties_ );
                        } } } );
        }

    protected:
        AttributeBase( AttributeProperties properties )
            : properties_( std::move( properties ) )
        {
        }

    private:
        AttributeProperties properties_;
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
        virtual const T& value( index_t element ) const = 0;

        absl::string_view type() final
        {
            return typeid( T ).name();
        }

        float generic_value( index_t element ) const final
        {
            return GenericAttributeConversion< T >::converted_value(
                value( element ) );
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
                DefaultGrowable< Archive, ReadOnlyAttribute< T > >{},
                []( Archive& archive, ReadOnlyAttribute< T >& attribute ) {
                    archive.ext(
                        attribute, bitsery::ext::BaseClass< AttributeBase >{} );
                } );
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

        const T& value( index_t /*unused*/ ) const override
        {
            return value_;
        }

        const T& value() const
        {
            return value_;
        }

        void set_value( T value )
        {
            value_ = std::move( value );
        }

        T default_value() const
        {
            return value();
        }

        template < typename Modifier >
        void modify_value( Modifier&& modifier )
        {
            modifier( value_ );
        }

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

        std::shared_ptr< AttributeBase > clone() const override
        {
            std::shared_ptr< ConstantAttribute< T > > attribute{
                new ConstantAttribute< T >{ value_, this->properties() }
            };
            return attribute;
        }

        void copy(
            const AttributeBase& attribute, index_t nb_elements ) override
        {
            if( nb_elements != 0 )
            {
                const auto& typed_attribute =
                    dynamic_cast< const ReadOnlyAttribute< T >& >( attribute );
                value_ = typed_attribute.value( 0 );
            }
        }

    private:
        ConstantAttribute( T value, AttributeProperties properties )
            : ReadOnlyAttribute< T >( std::move( properties ) )
        {
            set_value( std::move( value ) );
        }

        ConstantAttribute() : ReadOnlyAttribute< T >( AttributeProperties{} ){};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, ConstantAttribute< T > >{},
                []( Archive& archive, ConstantAttribute< T >& attribute ) {
                    archive.ext( attribute,
                        bitsery::ext::BaseClass< ReadOnlyAttribute< T > >{} );
                    archive( attribute.value_ );
                } );
        }

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

        const T& value( index_t element ) const override
        {
            return values_.at( element );
        }

        void set_value( index_t element, T value )
        {
            values_.at( element ) = std::move( value );
        }

        T default_value() const
        {
            return default_value_;
        }

        template < typename Modifier >
        void modify_value( index_t element, Modifier&& modifier )
        {
            modifier( values_.at( element ) );
        }

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

        std::shared_ptr< AttributeBase > clone() const override
        {
            std::shared_ptr< VariableAttribute< T > > attribute{
                new VariableAttribute< T >{ default_value_, this->properties() }
            };
            attribute->values_ = values_;
            return attribute;
        }

        void copy(
            const AttributeBase& attribute, index_t nb_elements ) override
        {
            const auto& typed_attribute =
                dynamic_cast< const VariableAttribute< T >& >( attribute );
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

    protected:
        VariableAttribute( T default_value, AttributeProperties properties )
            : ReadOnlyAttribute< T >( std::move( properties ) ),
              default_value_( std::move( default_value ) )
        {
            values_.reserve( 10 );
        }

        VariableAttribute() : ReadOnlyAttribute< T >( AttributeProperties{} ){};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, VariableAttribute< T > >{},
                []( Archive& archive, VariableAttribute< T >& attribute ) {
                    archive.ext( attribute,
                        bitsery::ext::BaseClass< ReadOnlyAttribute< T > >{} );
                    archive( attribute.default_value_ );
                    archive.container( attribute.values_,
                        attribute.values_.max_size(),
                        []( Archive& archive, T& item ) { archive( item ); } );
                } );
        }

        void resize( index_t size, AttributeBase::AttributeKey ) override
        {
            const auto capacity = values_.capacity();
            values_.reserve(
                static_cast< size_t >( std::ceil( size / capacity ) )
                * capacity );
            values_.resize( size, default_value_ );
        }

        void reserve( index_t capacity, AttributeBase::AttributeKey ) override
        {
            values_.reserve( capacity );
        }

        void delete_elements( const std::vector< bool >& to_delete,
            AttributeBase::AttributeKey ) override
        {
            std::vector< bool > to_keep( to_delete );
            to_keep.flip();
            values_ = extract_vector_elements( to_keep, values_ );
            values_.reserve( 10 );
        }

    private:
        T default_value_;
        std::vector< T > values_;
    };

    /*!
     * Read and write interface for boolean variable attribute storage.
     * This class removes the custom storage use by the STL for
     * std::vector<bool>.
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

        const bool& value( index_t element ) const override
        {
            return reinterpret_cast< const bool& >( values_.at( element ) );
        }

        void set_value( index_t element, bool value )
        {
            values_.at( element ) = std::move( value );
        }

        bool default_value() const
        {
            return default_value_;
        }

        template < typename Modifier >
        void modify_value( index_t element, Modifier&& modifier )
        {
            modifier( reinterpret_cast< bool& >( values_.at( element ) ) );
        }

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

        std::shared_ptr< AttributeBase > clone() const override
        {
            std::shared_ptr< VariableAttribute< bool > > attribute{
                new VariableAttribute< bool >{
                    static_cast< bool >( default_value_ ), this->properties() }
            };
            attribute->values_ = values_;
            return attribute;
        }

        void copy(
            const AttributeBase& attribute, index_t nb_elements ) override
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

    protected:
        VariableAttribute( bool default_value, AttributeProperties properties )
            : ReadOnlyAttribute< bool >( std::move( properties ) ),
              default_value_( default_value )
        {
            values_.reserve( 10 );
        }

        VariableAttribute()
            : ReadOnlyAttribute< bool >( AttributeProperties{} ){};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, VariableAttribute< bool > >{},
                []( Archive& archive, VariableAttribute< bool >& attribute ) {
                    archive.ext( attribute, bitsery::ext::BaseClass<
                                                ReadOnlyAttribute< bool > >{} );
                    archive.value1b( attribute.default_value_ );
                    archive.container1b(
                        attribute.values_, attribute.values_.max_size() );
                } );
        }

        void resize( index_t size, AttributeBase::AttributeKey ) override
        {
            const auto capacity = values_.capacity();
            values_.reserve(
                static_cast< size_t >( std::ceil( size / capacity ) )
                * capacity );
            values_.resize( size, default_value_ );
        }

        void reserve( index_t capacity, AttributeBase::AttributeKey ) override
        {
            values_.reserve( capacity );
        }

        void delete_elements( const std::vector< bool >& to_delete,
            AttributeBase::AttributeKey ) override
        {
            std::vector< bool > to_keep( to_delete );
            to_keep.flip();
            values_ = extract_vector_elements( to_keep, values_ );
            values_.reserve( 10 );
        }

    private:
        unsigned char default_value_;
        std::vector< unsigned char > values_;
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

        const T& value( index_t element ) const override
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

        T default_value() const
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

        std::shared_ptr< AttributeBase > clone() const override
        {
            std::shared_ptr< SparseAttribute< T > > attribute{
                new SparseAttribute< T >{ default_value_, this->properties() }
            };
            attribute->values_ = values_;
            return attribute;
        }

        void copy(
            const AttributeBase& attribute, index_t nb_elements ) override
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

    private:
        SparseAttribute( T default_value, AttributeProperties properties )
            : ReadOnlyAttribute< T >( std::move( properties ) ),
              default_value_( std::move( default_value ) )
        {
            values_.reserve( 10 );
        }

        SparseAttribute() : ReadOnlyAttribute< T >( AttributeProperties{} ){};

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, SparseAttribute< T > >{},
                []( Archive& archive, SparseAttribute< T >& attribute ) {
                    archive.ext( attribute,
                        bitsery::ext::BaseClass< ReadOnlyAttribute< T > >{} );
                    archive( attribute.default_value_ );
                    archive.ext( attribute.values_,
                        bitsery::ext::StdMap{ attribute.values_.max_size() },
                        []( Archive& archive, index_t& i, T& item ) {
                            archive.value4b( i );
                            archive( item );
                        } );
                } );
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
            const auto old_values = values_;
            values_.clear();
            values_.reserve( old_values.size() );
            for( const auto& value : old_values )
            {
                if( !to_delete[value.first] && value.second != default_value_ )
                {
                    values_.emplace( old2new[value.first], value.second );
                }
            }
        }

    private:
        T default_value_;
        absl::flat_hash_map< index_t, T > values_;
    };
} // namespace geode
