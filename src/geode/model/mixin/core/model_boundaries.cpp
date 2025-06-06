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

#include <geode/model/mixin/core/model_boundaries.hpp>

#include <geode/basic/detail/count_range_elements.hpp>
#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/detail/components_storage.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>

namespace geode
{
    template < index_t dimension >
    class ModelBoundaries< dimension >::Impl
        : public detail::ComponentsStorage< ModelBoundary< dimension > >
    {
    };

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaries() = default;

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaries(
        ModelBoundaries&& ) noexcept = default;

    template < index_t dimension >
    ModelBoundaries< dimension >& ModelBoundaries< dimension >::operator=(
        ModelBoundaries&& ) noexcept = default;

    template < index_t dimension >
    ModelBoundaries< dimension >::~ModelBoundaries() = default;

    template < index_t dimension >
    index_t ModelBoundaries< dimension >::nb_model_boundaries() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    index_t ModelBoundaries< dimension >::nb_active_model_boundaries() const
    {
        return detail::count_range_elements( active_model_boundaries() );
    }

    template < index_t dimension >
    const ModelBoundary< dimension >&
        ModelBoundaries< dimension >::model_boundary( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    ModelBoundary< dimension >&
        ModelBoundaries< dimension >::modifiable_model_boundary(
            const uuid& id, ModelBoundariesBuilderKey )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void ModelBoundaries< dimension >::save_model_boundaries(
        std::string_view directory ) const
    {
        impl_->save_components(
            absl::StrCat( directory, "/model_boundaries" ) );
    }

    template < index_t dimension >
    void ModelBoundaries< dimension >::load_model_boundaries(
        std::string_view directory, ModelBoundariesBuilderKey )
    {
        impl_->load_components(
            absl::StrCat( directory, "/model_boundaries" ) );
    }

    template < index_t dimension >
    auto ModelBoundaries< dimension >::model_boundaries() const
        -> ModelBoundaryRange
    {
        return { *this };
    }

    template < index_t dimension >
    auto ModelBoundaries< dimension >::active_model_boundaries() const
        -> ModelBoundaryRange
    {
        ModelBoundaryRange range{ *this };
        range.set_active_only();
        return range;
    }

    template < index_t dimension >
    auto ModelBoundaries< dimension >::modifiable_model_boundaries(
        ModelBoundariesBuilderKey ) -> ModifiableModelBoundaryRange
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& ModelBoundaries< dimension >::create_model_boundary(
        ModelBoundariesBuilderKey )
    {
        typename ModelBoundaries< dimension >::Impl::ComponentPtr boundary{
            new ModelBoundary< dimension >{
                typename ModelBoundary< dimension >::ModelBoundariesKey{} }
        };
        const auto& id = boundary->id();
        impl_->add_component( std::move( boundary ) );
        return id;
    }

    template < index_t dimension >
    void ModelBoundaries< dimension >::create_model_boundary(
        uuid model_boundary_id, ModelBoundariesBuilderKey )
    {
        typename ModelBoundaries< dimension >::Impl::ComponentPtr
            model_boundary{ new ModelBoundary< dimension >{
                typename ModelBoundary< dimension >::ModelBoundariesKey{} } };
        IdentifierBuilder{ *model_boundary }.set_id(
            std::move( model_boundary_id ) );
        impl_->add_component( std::move( model_boundary ) );
    }

    template < index_t dimension >
    void ModelBoundaries< dimension >::delete_model_boundary(
        const ModelBoundary< dimension >& boundary, ModelBoundariesBuilderKey )
    {
        impl_->delete_component( boundary.id() );
    }

    template < index_t dimension >
    class ModelBoundaries< dimension >::ModelBoundaryRangeBase::Impl
        : public BaseRange<
              typename ModelBoundaries< dimension >::Impl::Iterator >
    {
        using Iterator = typename ModelBoundaries< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Impl::Iterator >( begin, end )
        {
        }

        ModelBoundary< dimension >& model_boundary() const
        {
            return *this->current()->second;
        }

        void set_active_only()
        {
            active_only_ = true;
            next_model_boundary();
        }

        void next()
        {
            this->operator++();
            next_model_boundary();
        }

    private:
        void next_model_boundary()
        {
            while( this->operator!=( *this )
                   && ( active_only_ && !model_boundary().is_active() ) )
            {
                this->operator++();
            }
        }

    private:
        bool active_only_{ false };
    };

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaryRangeBase::
        ModelBoundaryRangeBase( const ModelBoundaries& boundaries )
        : impl_( boundaries.impl_->begin(), boundaries.impl_->end() )
    {
    }

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaryRangeBase::
        ModelBoundaryRangeBase( ModelBoundaryRangeBase&& ) noexcept = default;

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaryRangeBase::
        ModelBoundaryRangeBase( const ModelBoundaryRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    ModelBoundaries<
        dimension >::ModelBoundaryRangeBase::~ModelBoundaryRangeBase() =
        default;

    template < index_t dimension >
    bool ModelBoundaries< dimension >::ModelBoundaryRangeBase::operator!=(
        const ModelBoundaryRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void ModelBoundaries< dimension >::ModelBoundaryRangeBase::set_active_only()
    {
        impl_->set_active_only();
    }

    template < index_t dimension >
    void ModelBoundaries< dimension >::ModelBoundaryRangeBase::operator++()
    {
        return impl_->next();
    }

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaryRange::ModelBoundaryRange(
        const ModelBoundaries& boundaries )
        : ModelBoundaryRangeBase( boundaries )
    {
    }

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaryRange::ModelBoundaryRange(
        const ModelBoundaryRange& range )
        : ModelBoundaryRangeBase{ range }
    {
    }

    template < index_t dimension >
    ModelBoundaries< dimension >::ModelBoundaryRange::~ModelBoundaryRange() =
        default;

    template < index_t dimension >
    auto ModelBoundaries< dimension >::ModelBoundaryRange::begin() const
        -> const ModelBoundaryRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto ModelBoundaries< dimension >::ModelBoundaryRange::end() const
        -> const ModelBoundaryRange&
    {
        return *this;
    }

    template < index_t dimension >
    const ModelBoundary< dimension >&
        ModelBoundaries< dimension >::ModelBoundaryRange::operator*() const
    {
        return this->impl_->model_boundary();
    }

    template < index_t dimension >
    ModelBoundaries< dimension >::ModifiableModelBoundaryRange::
        ModifiableModelBoundaryRange( const ModelBoundaries& boundaries )
        : ModelBoundaryRangeBase( boundaries )
    {
    }

    template < index_t dimension >
    ModelBoundaries< dimension >::ModifiableModelBoundaryRange::
        ModifiableModelBoundaryRange(
            const ModifiableModelBoundaryRange& range )
        : ModelBoundaryRangeBase{ range }
    {
    }

    template < index_t dimension >
    ModelBoundaries< dimension >::ModifiableModelBoundaryRange::
        ~ModifiableModelBoundaryRange() = default;

    template < index_t dimension >
    auto ModelBoundaries< dimension >::ModifiableModelBoundaryRange::begin()
        const -> const ModifiableModelBoundaryRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto ModelBoundaries< dimension >::ModifiableModelBoundaryRange::end() const
        -> const ModifiableModelBoundaryRange&
    {
        return *this;
    }

    template < index_t dimension >
    ModelBoundary< dimension >&
        ModelBoundaries< dimension >::ModifiableModelBoundaryRange::operator*()
            const
    {
        return this->impl_->model_boundary();
    }

    template class opengeode_model_api ModelBoundaries< 2 >;
    template class opengeode_model_api ModelBoundaries< 3 >;
} // namespace geode
