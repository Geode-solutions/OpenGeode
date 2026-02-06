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
#include <geode/basic/attribute.hpp>
#include <geode/basic/common.hpp>
#include <geode/basic/detail/mapping_after_deletion.hpp>
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
} // namespace geode
