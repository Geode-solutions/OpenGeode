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

        bool is_active() const
        {
            return is_active_;
        }

        void set_active( bool active )
        {
            is_active_ = active;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& local_archive, Impl& impl ) {
                         local_archive.text1b( impl.backward_compatible_name_,
                             impl.backward_compatible_name_.max_size() );
                         local_archive.object( impl.backward_compatible_id_ );
                     },
                        []( Archive& local_archive, Impl& impl ) {
                            local_archive.value1b( impl.is_active_ );
                        } } } );
        }

    private:
        std::string backward_compatible_name_ = std::string{ "unknown" };
        uuid backward_compatible_id_;
        bool is_active_{ true };
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
    bool Component< dimension >::is_active() const
    {
        return impl_->is_active();
    }

    template < index_t dimension >
    void Component< dimension >::set_active( bool active )
    {
        impl_->set_active( active );
    }

    template < index_t dimension >
    template < typename Archive >
    void Component< dimension >::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, Component >{
                       { []( Archive& local_archive, Component& component ) {
                            local_archive.object( component.impl_ );
                            component.set_id(
                                component.impl_->backward_compatible_id() );
                            component.set_name(
                                component.impl_->backward_compatible_name() );
                        },
                           []( Archive& local_archive, Component& component ) {
                               local_archive.ext( component,
                                   bitsery::ext::BaseClass< Identifier >{} );
                           },
                           []( Archive& local_archive, Component& component ) {
                               local_archive.ext( component,
                                   bitsery::ext::BaseClass< Identifier >{} );
                               local_archive.object( component.impl_ );
                           } } } );
    }

    template class opengeode_model_api Component< 2 >;
    template class opengeode_model_api Component< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Component< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Component< 3 > );
} // namespace geode
