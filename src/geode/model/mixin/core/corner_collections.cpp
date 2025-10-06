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

#include <geode/model/mixin/core/corner_collections.hpp>

#include <geode/basic/detail/count_range_elements.hpp>
#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/detail/components_storage.hpp>

namespace geode
{
    template < index_t dimension >
    class CornerCollections< dimension >::Impl
        : public detail::ComponentsStorage< CornerCollection< dimension > >
    {
    };

    template < index_t dimension >
    CornerCollections< dimension >::CornerCollections() = default;

    template < index_t dimension >
    CornerCollections< dimension >::CornerCollections(
        CornerCollections&& ) noexcept = default;

    template < index_t dimension >
    CornerCollections< dimension >& CornerCollections< dimension >::operator=(
        CornerCollections&& ) noexcept = default;

    template < index_t dimension >
    CornerCollections< dimension >::~CornerCollections() = default;

    template < index_t dimension >
    bool CornerCollections< dimension >::has_corner_collection(
        const uuid& id ) const
    {
        return impl_->has_component( id );
    }

    template < index_t dimension >
    index_t CornerCollections< dimension >::nb_corner_collections() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    index_t CornerCollections< dimension >::nb_active_corner_collections() const
    {
        return detail::count_range_elements( active_corner_collections() );
    }

    template < index_t dimension >
    const CornerCollection< dimension >&
        CornerCollections< dimension >::corner_collection(
            const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    CornerCollection< dimension >&
        CornerCollections< dimension >::modifiable_corner_collection(
            const uuid& id, CornerCollectionsBuilderKey )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void CornerCollections< dimension >::save_corner_collections(
        std::string_view directory ) const
    {
        impl_->save_components(
            absl::StrCat( directory, "/corner_collections" ) );
    }

    template < index_t dimension >
    void CornerCollections< dimension >::load_corner_collections(
        std::string_view directory, CornerCollectionsBuilderKey )
    {
        impl_->load_components(
            absl::StrCat( directory, "/corner_collections" ) );
    }

    template < index_t dimension >
    auto CornerCollections< dimension >::corner_collections() const
        -> CornerCollectionRange
    {
        return { *this };
    }

    template < index_t dimension >
    auto CornerCollections< dimension >::active_corner_collections() const
        -> CornerCollectionRange
    {
        CornerCollectionRange range{ *this };
        range.set_active_only();
        return range;
    }

    template < index_t dimension >
    auto CornerCollections< dimension >::modifiable_corner_collections(
        CornerCollectionsBuilderKey ) -> ModifiableCornerCollectionRange
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& CornerCollections< dimension >::create_corner_collection(
        CornerCollectionsBuilderKey )
    {
        typename CornerCollections< dimension >::Impl::ComponentPtr collection{
            new CornerCollection< dimension >{
                typename CornerCollection< dimension >::CornerCollectionsKey{} }
        };
        const auto& id = collection->id();
        impl_->add_component( std::move( collection ) );
        return id;
    }

    template < index_t dimension >
    void CornerCollections< dimension >::create_corner_collection(
        uuid corner_collection_id, CornerCollectionsBuilderKey )
    {
        typename CornerCollections<
            dimension >::Impl::ComponentPtr corner_collection{
            new CornerCollection< dimension >{
                typename CornerCollection< dimension >::CornerCollectionsKey{} }
        };
        IdentifierBuilder{ *corner_collection }.set_id(
            std::move( corner_collection_id ) );
        impl_->add_component( std::move( corner_collection ) );
    }

    template < index_t dimension >
    void CornerCollections< dimension >::delete_corner_collection(
        const CornerCollection< dimension >& collection,
        CornerCollectionsBuilderKey )
    {
        impl_->delete_component( collection.id() );
    }

    template < index_t dimension >
    class CornerCollections< dimension >::CornerCollectionRangeBase::Impl
        : public BaseRange<
              typename CornerCollections< dimension >::Impl::Iterator >
    {
        using Iterator =
            typename CornerCollections< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Impl::Iterator >( begin, end )
        {
        }

        CornerCollection< dimension >& corner_collection() const
        {
            return *this->current()->second;
        }

        void set_active_only()
        {
            active_only_ = true;
            next_corner_collection();
        }

        void next()
        {
            this->operator++();
            next_corner_collection();
        }

    private:
        void next_corner_collection()
        {
            while( this->operator!=( *this )
                   && ( active_only_ && !corner_collection().is_active() ) )
            {
                this->operator++();
            }
        }

    private:
        bool active_only_{ false };
    };

    template < index_t dimension >
    CornerCollections< dimension >::CornerCollectionRangeBase::
        CornerCollectionRangeBase( const CornerCollections& collections )
        : impl_( collections.impl_->begin(), collections.impl_->end() )
    {
    }

    template < index_t dimension >
    CornerCollections< dimension >::CornerCollectionRangeBase::
        CornerCollectionRangeBase(
            CornerCollectionRangeBase&& ) noexcept = default;

    template < index_t dimension >
    CornerCollections< dimension >::CornerCollectionRangeBase::
        CornerCollectionRangeBase( const CornerCollectionRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    CornerCollections<
        dimension >::CornerCollectionRangeBase::~CornerCollectionRangeBase() =
        default;

    template < index_t dimension >
    bool CornerCollections< dimension >::CornerCollectionRangeBase::operator!=(
        const CornerCollectionRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void CornerCollections<
        dimension >::CornerCollectionRangeBase::set_active_only()
    {
        impl_->set_active_only();
    }

    template < index_t dimension >
    void CornerCollections< dimension >::CornerCollectionRangeBase::operator++()
    {
        return impl_->next();
    }

    template < index_t dimension >
    CornerCollections< dimension >::CornerCollectionRange::
        CornerCollectionRange( const CornerCollections& collections )
        : CornerCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    CornerCollections< dimension >::CornerCollectionRange::
        CornerCollectionRange( const CornerCollectionRange& range )
        : CornerCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    CornerCollections<
        dimension >::CornerCollectionRange::~CornerCollectionRange() = default;

    template < index_t dimension >
    auto CornerCollections< dimension >::CornerCollectionRange::begin() const
        -> const CornerCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto CornerCollections< dimension >::CornerCollectionRange::end() const
        -> const CornerCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    const CornerCollection< dimension >&
        CornerCollections< dimension >::CornerCollectionRange::operator*() const
    {
        return this->impl_->corner_collection();
    }

    template < index_t dimension >
    CornerCollections< dimension >::ModifiableCornerCollectionRange::
        ModifiableCornerCollectionRange( const CornerCollections& collections )
        : CornerCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    CornerCollections< dimension >::ModifiableCornerCollectionRange::
        ModifiableCornerCollectionRange(
            const ModifiableCornerCollectionRange& range )
        : CornerCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    CornerCollections< dimension >::ModifiableCornerCollectionRange::
        ~ModifiableCornerCollectionRange() = default;

    template < index_t dimension >
    auto
        CornerCollections< dimension >::ModifiableCornerCollectionRange::begin()
            const -> const ModifiableCornerCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto CornerCollections< dimension >::ModifiableCornerCollectionRange::end()
        const -> const ModifiableCornerCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    CornerCollection< dimension >& CornerCollections<
        dimension >::ModifiableCornerCollectionRange::operator*() const
    {
        return this->impl_->corner_collection();
    }

    template class opengeode_model_api CornerCollections< 2 >;
    template class opengeode_model_api CornerCollections< 3 >;
} // namespace geode
