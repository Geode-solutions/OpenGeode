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

#include <geode/model/mixin/core/component.hpp>

#include <algorithm>

#include <bitsery/traits/string.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    template < index_t dimension >
    class Component< dimension >::Impl
    {
    public:
        std::string_view backward_compatible_name() const
        {
            return backward_compatible_name_;
        }

        const uuid& backward_compatible_id() const
        {
            return backward_compatible_id_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.text1b( impl.backward_compatible_name_,
                        impl.backward_compatible_name_.max_size() );
                    a.object( impl.backward_compatible_id_ );
                } } } );
        }

    private:
        std::string backward_compatible_name_ = std::string{ "unknown" };
        uuid backward_compatible_id_;
    };

    template < index_t dimension >
    Component< dimension >::~Component() = default;

    template < index_t dimension >
    Component< dimension >::Component() = default;

    template < index_t dimension >
    Component< dimension >::Component( Component&& ) noexcept = default;

    template < index_t dimension >
    ComponentID Component< dimension >::component_id() const
    {
        return { this->component_type(), this->id() };
    }

    template < index_t dimension >
    template < typename Archive >
    void Component< dimension >::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, Component >{
                       { []( Archive& a, Component& component ) {
                            a.object( component.impl_ );
                            component.set_id(
                                component.impl_->backward_compatible_id() );
                            component.set_name(
                                component.impl_->backward_compatible_name() );
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
