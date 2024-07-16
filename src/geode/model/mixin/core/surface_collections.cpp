/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/model/mixin/core/surface_collections.hpp>

#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/detail/components_storage.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>

namespace geode
{
    template < index_t dimension >
    class SurfaceCollections< dimension >::Impl
        : public detail::ComponentsStorage< SurfaceCollection< dimension > >
    {
    };

    template < index_t dimension >
    SurfaceCollections< dimension >::SurfaceCollections() = default;

    template < index_t dimension >
    SurfaceCollections< dimension >::SurfaceCollections(
        SurfaceCollections&& ) noexcept = default;

    template < index_t dimension >
    SurfaceCollections< dimension >& SurfaceCollections< dimension >::operator=(
        SurfaceCollections&& ) noexcept = default;

    template < index_t dimension >
    SurfaceCollections< dimension >::~SurfaceCollections() = default;

    template < index_t dimension >
    index_t SurfaceCollections< dimension >::nb_surface_collections() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const SurfaceCollection< dimension >&
        SurfaceCollections< dimension >::surface_collection(
            const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    SurfaceCollection< dimension >&
        SurfaceCollections< dimension >::modifiable_surface_collection(
            const uuid& id, SurfaceCollectionsBuilderKey )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void SurfaceCollections< dimension >::save_surface_collections(
        std::string_view directory ) const
    {
        impl_->save_components(
            absl::StrCat( directory, "/surface_collections" ) );
    }

    template < index_t dimension >
    void SurfaceCollections< dimension >::load_surface_collections(
        std::string_view directory, SurfaceCollectionsBuilderKey )
    {
        impl_->load_components(
            absl::StrCat( directory, "/surface_collections" ) );
    }

    template < index_t dimension >
    typename SurfaceCollections< dimension >::SurfaceCollectionRange
        SurfaceCollections< dimension >::surface_collections() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename SurfaceCollections< dimension >::ModifiableSurfaceCollectionRange
        SurfaceCollections< dimension >::modifiable_surface_collections(
            SurfaceCollectionsBuilderKey )
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& SurfaceCollections< dimension >::create_surface_collection(
        SurfaceCollectionsBuilderKey )
    {
        typename SurfaceCollections< dimension >::Impl::ComponentPtr collection{
            new SurfaceCollection< dimension >{ typename SurfaceCollection<
                dimension >::SurfaceCollectionsKey{} }
        };
        const auto& id = collection->id();
        impl_->add_component( std::move( collection ) );
        return id;
    }

    template < index_t dimension >
    void SurfaceCollections< dimension >::create_surface_collection(
        uuid surface_collection_id, SurfaceCollectionsBuilderKey )
    {
        typename SurfaceCollections< dimension >::Impl::ComponentPtr
            surface_collection{ new SurfaceCollection< dimension >{
                typename SurfaceCollection<
                    dimension >::SurfaceCollectionsKey{} } };
        IdentifierBuilder{ *surface_collection }.set_id(
            std::move( surface_collection_id ) );
        impl_->add_component( std::move( surface_collection ) );
    }

    template < index_t dimension >
    void SurfaceCollections< dimension >::delete_surface_collection(
        const SurfaceCollection< dimension >& collection,
        SurfaceCollectionsBuilderKey )
    {
        impl_->delete_component( collection.id() );
    }

    template < index_t dimension >
    class SurfaceCollections< dimension >::SurfaceCollectionRangeBase::Impl
        : public BaseRange<
              typename SurfaceCollections< dimension >::Impl::Iterator >
    {
        using Iterator =
            typename SurfaceCollections< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Impl::Iterator >( begin, end )
        {
        }

        SurfaceCollection< dimension >& surface_collection() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    SurfaceCollections< dimension >::SurfaceCollectionRangeBase::
        SurfaceCollectionRangeBase( const SurfaceCollections& collections )
        : impl_( collections.impl_->begin(), collections.impl_->end() )
    {
    }

    template < index_t dimension >
    SurfaceCollections< dimension >::SurfaceCollectionRangeBase::
        SurfaceCollectionRangeBase(
            SurfaceCollectionRangeBase&& ) noexcept = default;

    template < index_t dimension >
    SurfaceCollections< dimension >::SurfaceCollectionRangeBase::
        SurfaceCollectionRangeBase( const SurfaceCollectionRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    SurfaceCollections<
        dimension >::SurfaceCollectionRangeBase::~SurfaceCollectionRangeBase() =
        default;

    template < index_t dimension >
    bool
        SurfaceCollections< dimension >::SurfaceCollectionRangeBase::operator!=(
            const SurfaceCollectionRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void SurfaceCollections<
        dimension >::SurfaceCollectionRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    SurfaceCollections< dimension >::SurfaceCollectionRange::
        SurfaceCollectionRange( const SurfaceCollections& collections )
        : SurfaceCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    SurfaceCollections< dimension >::SurfaceCollectionRange::
        SurfaceCollectionRange( const SurfaceCollectionRange& range )
        : SurfaceCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    SurfaceCollections<
        dimension >::SurfaceCollectionRange::~SurfaceCollectionRange() =
        default;

    template < index_t dimension >
    auto SurfaceCollections< dimension >::SurfaceCollectionRange::begin() const
        -> const SurfaceCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto SurfaceCollections< dimension >::SurfaceCollectionRange::end() const
        -> const SurfaceCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    const SurfaceCollection< dimension >&
        SurfaceCollections< dimension >::SurfaceCollectionRange::operator*()
            const
    {
        return this->impl_->surface_collection();
    }

    template < index_t dimension >
    SurfaceCollections< dimension >::ModifiableSurfaceCollectionRange::
        ModifiableSurfaceCollectionRange(
            const SurfaceCollections& collections )
        : SurfaceCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    SurfaceCollections< dimension >::ModifiableSurfaceCollectionRange::
        ModifiableSurfaceCollectionRange(
            const ModifiableSurfaceCollectionRange& range )
        : SurfaceCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    SurfaceCollections< dimension >::ModifiableSurfaceCollectionRange::
        ~ModifiableSurfaceCollectionRange()
    {
    }

    template < index_t dimension >
    auto SurfaceCollections<
        dimension >::ModifiableSurfaceCollectionRange::begin() const
        -> const ModifiableSurfaceCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto
        SurfaceCollections< dimension >::ModifiableSurfaceCollectionRange::end()
            const -> const ModifiableSurfaceCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    SurfaceCollection< dimension >& SurfaceCollections<
        dimension >::ModifiableSurfaceCollectionRange::operator*() const
    {
        return this->impl_->surface_collection();
    }

    template class opengeode_model_api SurfaceCollections< 2 >;
    template class opengeode_model_api SurfaceCollections< 3 >;
} // namespace geode
