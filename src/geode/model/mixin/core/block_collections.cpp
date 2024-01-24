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

#include <geode/model/mixin/core/block_collections.h>

#include <geode/basic/identifier_builder.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/model/mixin/core/block_collection.h>
#include <geode/model/mixin/core/detail/components_storage.h>

namespace geode
{
    template < index_t dimension >
    class BlockCollections< dimension >::Impl
        : public detail::ComponentsStorage< BlockCollection< dimension > >
    {
    };

    template < index_t dimension >
    BlockCollections< dimension >::BlockCollections() = default;

    template < index_t dimension >
    BlockCollections< dimension >::BlockCollections(
        BlockCollections&& ) noexcept = default;

    template < index_t dimension >
    BlockCollections< dimension >& BlockCollections< dimension >::operator=(
        BlockCollections&& ) noexcept = default;

    template < index_t dimension >
    BlockCollections< dimension >::~BlockCollections() = default;

    template < index_t dimension >
    index_t BlockCollections< dimension >::nb_block_collections() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const BlockCollection< dimension >&
        BlockCollections< dimension >::block_collection( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    BlockCollection< dimension >&
        BlockCollections< dimension >::modifiable_block_collection(
            const uuid& id, BlockCollectionsBuilderKey )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void BlockCollections< dimension >::save_block_collections(
        absl::string_view directory ) const
    {
        impl_->save_components(
            absl::StrCat( directory, "/block_collections" ) );
    }

    template < index_t dimension >
    void BlockCollections< dimension >::load_block_collections(
        absl::string_view directory, BlockCollectionsBuilderKey )
    {
        impl_->load_components(
            absl::StrCat( directory, "/block_collections" ) );
    }

    template < index_t dimension >
    typename BlockCollections< dimension >::BlockCollectionRange
        BlockCollections< dimension >::block_collections() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename BlockCollections< dimension >::ModifiableBlockCollectionRange
        BlockCollections< dimension >::modifiable_block_collections(
            BlockCollectionsBuilderKey )
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& BlockCollections< dimension >::create_block_collection(
        BlockCollectionsBuilderKey )
    {
        typename BlockCollections< dimension >::Impl::ComponentPtr collection{
            new BlockCollection< dimension >{
                typename BlockCollection< dimension >::BlockCollectionsKey{} }
        };
        const auto& id = collection->id();
        impl_->add_component( std::move( collection ) );
        return id;
    }

    template < index_t dimension >
    void BlockCollections< dimension >::create_block_collection(
        uuid block_collection_id, BlockCollectionsBuilderKey )
    {
        typename BlockCollections<
            dimension >::Impl::ComponentPtr block_collection{
            new BlockCollection< dimension >{
                typename BlockCollection< dimension >::BlockCollectionsKey{} }
        };
        IdentifierBuilder{ *block_collection }.set_id(
            std::move( block_collection_id ) );
        impl_->add_component( std::move( block_collection ) );
    }

    template < index_t dimension >
    void BlockCollections< dimension >::delete_block_collection(
        const BlockCollection< dimension >& collection,
        BlockCollectionsBuilderKey )
    {
        impl_->delete_component( collection.id() );
    }

    template < index_t dimension >
    class BlockCollections< dimension >::BlockCollectionRangeBase::Impl
        : public BaseRange<
              typename BlockCollections< dimension >::Impl::Iterator >
    {
        using Iterator = typename BlockCollections< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Impl::Iterator >( begin, end )
        {
        }

        BlockCollection< dimension >& block_collection() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    BlockCollections< dimension >::BlockCollectionRangeBase::
        BlockCollectionRangeBase( const BlockCollections& collections )
        : impl_( collections.impl_->begin(), collections.impl_->end() )
    {
    }

    template < index_t dimension >
    BlockCollections< dimension >::BlockCollectionRangeBase::
        BlockCollectionRangeBase(
            BlockCollectionRangeBase&& ) noexcept = default;

    template < index_t dimension >
    BlockCollections< dimension >::BlockCollectionRangeBase::
        BlockCollectionRangeBase( const BlockCollectionRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    BlockCollections<
        dimension >::BlockCollectionRangeBase::~BlockCollectionRangeBase() =
        default;

    template < index_t dimension >
    bool BlockCollections< dimension >::BlockCollectionRangeBase::operator!=(
        const BlockCollectionRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void BlockCollections< dimension >::BlockCollectionRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    BlockCollections< dimension >::BlockCollectionRange::BlockCollectionRange(
        const BlockCollections& collections )
        : BlockCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    BlockCollections< dimension >::BlockCollectionRange::BlockCollectionRange(
        const BlockCollectionRange& range )
        : BlockCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    BlockCollections<
        dimension >::BlockCollectionRange::~BlockCollectionRange() = default;

    template < index_t dimension >
    auto BlockCollections< dimension >::BlockCollectionRange::begin() const
        -> const BlockCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto BlockCollections< dimension >::BlockCollectionRange::end() const
        -> const BlockCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    const BlockCollection< dimension >&
        BlockCollections< dimension >::BlockCollectionRange::operator*() const
    {
        return this->impl_->block_collection();
    }

    template < index_t dimension >
    BlockCollections< dimension >::ModifiableBlockCollectionRange::
        ModifiableBlockCollectionRange( const BlockCollections& collections )
        : BlockCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    BlockCollections< dimension >::ModifiableBlockCollectionRange::
        ModifiableBlockCollectionRange(
            const ModifiableBlockCollectionRange& range )
        : BlockCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    BlockCollections< dimension >::ModifiableBlockCollectionRange::
        ~ModifiableBlockCollectionRange()
    {
    }

    template < index_t dimension >
    auto BlockCollections< dimension >::ModifiableBlockCollectionRange::begin()
        const -> const ModifiableBlockCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto BlockCollections< dimension >::ModifiableBlockCollectionRange::end()
        const -> const ModifiableBlockCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    BlockCollection< dimension >& BlockCollections<
        dimension >::ModifiableBlockCollectionRange::operator*() const
    {
        return this->impl_->block_collection();
    }

    template class opengeode_model_api BlockCollections< 2 >;
    template class opengeode_model_api BlockCollections< 3 >;
} // namespace geode
