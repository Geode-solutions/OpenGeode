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

#include <geode/model/mixin/core/component.h>

#include <algorithm>

#include <bitsery/traits/string.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

namespace geode
{
    template < index_t dimension >
    class Component< dimension >::Impl
    {
    public:
        absl::string_view name() const
        {
            return name_;
        }

        void set_name( absl::string_view name )
        {
            name_ = to_string( name );
        }

        const uuid& id() const
        {
            return id_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.text1b( impl.name_, impl.name_.max_size() );
                    a.object( impl.id_ );
                } } } );
        }

    private:
        std::string name_ = std::string{ "unknown" };
        uuid id_;
    };

    template < index_t dimension >
    Component< dimension >::~Component() = default;

    template < index_t dimension >
    Component< dimension >::Component() = default;

    template < index_t dimension >
    Component< dimension >::Component( Component&& other ) noexcept
        : Identifier{ std::move( other ) }, impl_{ std::move( other.impl_ ) }
    {
    }

    template < index_t dimension >
    template < typename Archive >
    void Component< dimension >::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, Component >{
                       { []( Archive& a, Component& component ) {
                            a.object( component.impl_ );
                            component.set_id( component.impl_->id() );
                            component.set_name( component.impl_->name() );
                            component.impl_.reset();
                        },
                           []( Archive& a, Component& component ) {
                               a.ext( component,
                                   bitsery::ext::BaseClass< Identifier >{} );
                           } } } );
    }

    template class opengeode_model_api Component< 2 >;
    template class opengeode_model_api Component< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Component< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Component< 3 > );
} // namespace geode
