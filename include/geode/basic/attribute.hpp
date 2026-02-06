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
#include <typeinfo>

#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/ext/inheritance.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_utils.hpp>
#include <geode/basic/common.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/mapping.hpp>
#include <geode/basic/passkey.hpp>

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
} // namespace geode
