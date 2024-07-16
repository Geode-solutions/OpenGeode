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

#include <geode/model/mixin/core/line_collections.hpp>

#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/detail/components_storage.hpp>
#include <geode/model/mixin/core/line_collection.hpp>

namespace geode
{
    template < index_t dimension >
    class LineCollections< dimension >::Impl
        : public detail::ComponentsStorage< LineCollection< dimension > >
    {
    };

    template < index_t dimension >
    LineCollections< dimension >::LineCollections() = default;

    template < index_t dimension >
    LineCollections< dimension >::LineCollections(
        LineCollections&& ) noexcept = default;

    template < index_t dimension >
    LineCollections< dimension >& LineCollections< dimension >::operator=(
        LineCollections&& ) noexcept = default;

    template < index_t dimension >
    LineCollections< dimension >::~LineCollections() = default;

    template < index_t dimension >
    index_t LineCollections< dimension >::nb_line_collections() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const LineCollection< dimension >&
        LineCollections< dimension >::line_collection( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    LineCollection< dimension >&
        LineCollections< dimension >::modifiable_line_collection(
            const uuid& id, LineCollectionsBuilderKey )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void LineCollections< dimension >::save_line_collections(
        std::string_view directory ) const
    {
        impl_->save_components(
            absl::StrCat( directory, "/line_collections" ) );
    }

    template < index_t dimension >
    void LineCollections< dimension >::load_line_collections(
        std::string_view directory, LineCollectionsBuilderKey )
    {
        impl_->load_components(
            absl::StrCat( directory, "/line_collections" ) );
    }

    template < index_t dimension >
    typename LineCollections< dimension >::LineCollectionRange
        LineCollections< dimension >::line_collections() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename LineCollections< dimension >::ModifiableLineCollectionRange
        LineCollections< dimension >::modifiable_line_collections(
            LineCollectionsBuilderKey )
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& LineCollections< dimension >::create_line_collection(
        LineCollectionsBuilderKey )
    {
        typename LineCollections< dimension >::Impl::ComponentPtr collection{
            new LineCollection< dimension >{
                typename LineCollection< dimension >::LineCollectionsKey{} }
        };
        const auto& id = collection->id();
        impl_->add_component( std::move( collection ) );
        return id;
    }

    template < index_t dimension >
    void LineCollections< dimension >::create_line_collection(
        uuid line_collection_id, LineCollectionsBuilderKey )
    {
        typename LineCollections< dimension >::Impl::ComponentPtr
            line_collection{ new LineCollection< dimension >{
                typename LineCollection< dimension >::LineCollectionsKey{} } };
        IdentifierBuilder{ *line_collection }.set_id(
            std::move( line_collection_id ) );
        impl_->add_component( std::move( line_collection ) );
    }

    template < index_t dimension >
    void LineCollections< dimension >::delete_line_collection(
        const LineCollection< dimension >& collection,
        LineCollectionsBuilderKey )
    {
        impl_->delete_component( collection.id() );
    }

    template < index_t dimension >
    class LineCollections< dimension >::LineCollectionRangeBase::Impl
        : public BaseRange<
              typename LineCollections< dimension >::Impl::Iterator >
    {
        using Iterator = typename LineCollections< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Impl::Iterator >( begin, end )
        {
        }

        LineCollection< dimension >& line_collection() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    LineCollections< dimension >::LineCollectionRangeBase::
        LineCollectionRangeBase( const LineCollections& collections )
        : impl_( collections.impl_->begin(), collections.impl_->end() )
    {
    }

    template < index_t dimension >
    LineCollections< dimension >::LineCollectionRangeBase::
        LineCollectionRangeBase( LineCollectionRangeBase&& ) noexcept = default;

    template < index_t dimension >
    LineCollections< dimension >::LineCollectionRangeBase::
        LineCollectionRangeBase( const LineCollectionRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    LineCollections<
        dimension >::LineCollectionRangeBase::~LineCollectionRangeBase() =
        default;

    template < index_t dimension >
    bool LineCollections< dimension >::LineCollectionRangeBase::operator!=(
        const LineCollectionRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void LineCollections< dimension >::LineCollectionRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    LineCollections< dimension >::LineCollectionRange::LineCollectionRange(
        const LineCollections& collections )
        : LineCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    LineCollections< dimension >::LineCollectionRange::LineCollectionRange(
        const LineCollectionRange& range )
        : LineCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    LineCollections< dimension >::LineCollectionRange::~LineCollectionRange() =
        default;

    template < index_t dimension >
    auto LineCollections< dimension >::LineCollectionRange::begin() const
        -> const LineCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto LineCollections< dimension >::LineCollectionRange::end() const
        -> const LineCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    const LineCollection< dimension >&
        LineCollections< dimension >::LineCollectionRange::operator*() const
    {
        return this->impl_->line_collection();
    }

    template < index_t dimension >
    LineCollections< dimension >::ModifiableLineCollectionRange::
        ModifiableLineCollectionRange( const LineCollections& collections )
        : LineCollectionRangeBase( collections )
    {
    }

    template < index_t dimension >
    LineCollections< dimension >::ModifiableLineCollectionRange::
        ModifiableLineCollectionRange(
            const ModifiableLineCollectionRange& range )
        : LineCollectionRangeBase{ range }
    {
    }

    template < index_t dimension >
    LineCollections< dimension >::ModifiableLineCollectionRange::
        ~ModifiableLineCollectionRange()
    {
    }

    template < index_t dimension >
    auto LineCollections< dimension >::ModifiableLineCollectionRange::begin()
        const -> const ModifiableLineCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto LineCollections< dimension >::ModifiableLineCollectionRange::end()
        const -> const ModifiableLineCollectionRange&
    {
        return *this;
    }

    template < index_t dimension >
    LineCollection< dimension >&
        LineCollections< dimension >::ModifiableLineCollectionRange::operator*()
            const
    {
        return this->impl_->line_collection();
    }

    template class opengeode_model_api LineCollections< 2 >;
    template class opengeode_model_api LineCollections< 3 >;
} // namespace geode
