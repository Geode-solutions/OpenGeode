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

#include <geode/model/mixin/core/corners.hpp>

#include <async++.h>

#include <geode/basic/detail/count_range_elements.hpp>
#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/io/point_set_input.hpp>
#include <geode/mesh/io/point_set_output.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/detail/components_storage.hpp>

namespace geode
{
    template < index_t dimension >
    class Corners< dimension >::Impl
        : public detail::ComponentsStorage< Corner< dimension > >
    {
    };

    template < index_t dimension >
    Corners< dimension >::Corners() = default;

    template < index_t dimension >
    Corners< dimension >::Corners( Corners&& ) noexcept = default;

    template < index_t dimension >
    Corners< dimension >::~Corners() = default;

    template < index_t dimension >
    Corners< dimension >& Corners< dimension >::operator=(
        Corners< dimension >&& ) noexcept = default;

    template < index_t dimension >
    bool Corners< dimension >::has_corner( const uuid& id ) const
    {
        return impl_->has_component( id );
    }

    template < index_t dimension >
    index_t Corners< dimension >::nb_corners() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    index_t Corners< dimension >::nb_active_corners() const
    {
        return detail::count_range_elements( active_corners() );
    }

    template < index_t dimension >
    const Corner< dimension >& Corners< dimension >::corner(
        const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Corner< dimension >& Corners< dimension >::modifiable_corner(
        const uuid& id, CornersBuilderKey )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Corners< dimension >::save_corners( std::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/corners" ) );
        const auto prefix = absl::StrCat( directory, "/",
            Corner< dimension >::component_type_static().get() );
        absl::FixedArray< async::task< void > > tasks( nb_corners() );
        index_t count{ 0 };
        for( const auto& corner : corners() )
        {
            tasks[count++] = async::spawn( [&corner, &prefix] {
                const auto& mesh = corner.mesh();
                const auto file = absl::StrCat( prefix, corner.id().string(),
                    ".", mesh.native_extension() );
                save_point_set( mesh, file );
            } );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
    }

    template < index_t dimension >
    void Corners< dimension >::load_corners(
        std::string_view directory, CornersBuilderKey )
    {
        impl_->load_components( absl::StrCat( directory, "/corners" ) );
        const auto mapping = impl_->file_mapping( directory );
        absl::FixedArray< async::task< void > > tasks( nb_corners() );
        index_t count{ 0 };
        for( auto& corner : modifiable_corners( {} ) )
        {
            tasks[count++] = async::spawn( [&corner, &mapping] {
                const auto file = mapping.at( corner.id().string() );
                corner.set_mesh(
                    load_point_set< dimension >( corner.mesh_type(), file ),
                    typename Corner< dimension >::CornersKey{} );
            } );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
    }

    template < index_t dimension >
    auto Corners< dimension >::corners() const -> CornerRange
    {
        return { *this };
    }

    template < index_t dimension >
    auto Corners< dimension >::active_corners() const -> CornerRange
    {
        CornerRange range{ *this };
        range.set_active_only();
        return range;
    }

    template < index_t dimension >
    auto Corners< dimension >::modifiable_corners( CornersBuilderKey )
        -> ModifiableCornerRange
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Corners< dimension >::create_corner( CornersBuilderKey )
    {
        typename Corners< dimension >::Impl::ComponentPtr corner{
            new Corner< dimension >{
                typename Corner< dimension >::CornersKey{} }
        };
        const auto& id = corner->id();
        impl_->add_component( std::move( corner ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Corners< dimension >::create_corner(
        const MeshImpl& impl, CornersBuilderKey )
    {
        typename Corners< dimension >::Impl::ComponentPtr corner{
            new Corner< dimension >{
                impl, typename Corner< dimension >::CornersKey{} }
        };
        const auto& id = corner->id();
        impl_->add_component( std::move( corner ) );
        return id;
    }

    template < index_t dimension >
    void Corners< dimension >::create_corner(
        uuid corner_id, CornersBuilderKey )
    {
        typename Corners< dimension >::Impl::ComponentPtr corner{
            new Corner< dimension >{
                typename Corner< dimension >::CornersKey{} }
        };
        IdentifierBuilder{ *corner }.set_id( std::move( corner_id ) );
        impl_->add_component( std::move( corner ) );
    }

    template < index_t dimension >
    void Corners< dimension >::create_corner(
        uuid corner_id, const MeshImpl& impl, CornersBuilderKey )
    {
        typename Corners< dimension >::Impl::ComponentPtr corner{
            new Corner< dimension >{ impl, {} }
        };
        IdentifierBuilder{ *corner }.set_id( std::move( corner_id ) );
        impl_->add_component( std::move( corner ) );
    }

    template < index_t dimension >
    void Corners< dimension >::delete_corner(
        const Corner< dimension >& corner, CornersBuilderKey )
    {
        impl_->delete_component( corner.id() );
    }

    template < index_t dimension >
    class Corners< dimension >::CornerRangeBase::Impl
        : public BaseRange< typename Corners< dimension >::Impl::Iterator >
    {
        using Iterator = typename Corners< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        Corner< dimension >& corner() const
        {
            return *this->current()->second;
        }

        void set_active_only()
        {
            active_only_ = true;
            next_corner();
        }

        void next()
        {
            this->operator++();
            next_corner();
        }

    private:
        void next_corner()
        {
            while( this->operator!=( *this )
                   && ( active_only_ && !corner().is_active() ) )
            {
                this->operator++();
            }
        }

    private:
        bool active_only_{ false };
    };

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::CornerRangeBase(
        const Corners& corners )
        : impl_( corners.impl_->begin(), corners.impl_->end() )
    {
    }

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::CornerRangeBase(
        CornerRangeBase&& ) noexcept = default;

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::CornerRangeBase(
        const CornerRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::~CornerRangeBase() = default;

    template < index_t dimension >
    bool Corners< dimension >::CornerRangeBase::operator!=(
        const CornerRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Corners< dimension >::CornerRangeBase::set_active_only()
    {
        impl_->set_active_only();
    }

    template < index_t dimension >
    void Corners< dimension >::CornerRangeBase::operator++()
    {
        return impl_->next();
    }

    template < index_t dimension >
    Corners< dimension >::CornerRange::CornerRange( const Corners& corners )
        : CornerRangeBase( corners )
    {
    }

    template < index_t dimension >
    Corners< dimension >::CornerRange::CornerRange( const CornerRange& range )
        : CornerRangeBase{ range }
    {
    }

    template < index_t dimension >
    Corners< dimension >::CornerRange::~CornerRange() = default;

    template < index_t dimension >
    auto Corners< dimension >::CornerRange::begin() const -> const CornerRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Corners< dimension >::CornerRange::end() const -> const CornerRange&
    {
        return *this;
    }

    template < index_t dimension >
    const Corner< dimension >&
        Corners< dimension >::CornerRange::operator*() const
    {
        return this->impl_->corner();
    }

    template < index_t dimension >
    Corners< dimension >::ModifiableCornerRange::ModifiableCornerRange(
        const Corners& corners )
        : CornerRangeBase( corners )
    {
    }

    template < index_t dimension >
    Corners< dimension >::ModifiableCornerRange::ModifiableCornerRange(
        const ModifiableCornerRange& range )
        : CornerRangeBase{ range }
    {
    }

    template < index_t dimension >
    Corners< dimension >::ModifiableCornerRange::~ModifiableCornerRange() =
        default;

    template < index_t dimension >
    auto Corners< dimension >::ModifiableCornerRange::begin() const
        -> const ModifiableCornerRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Corners< dimension >::ModifiableCornerRange::end() const
        -> const ModifiableCornerRange&
    {
        return *this;
    }

    template < index_t dimension >
    Corner< dimension >&
        Corners< dimension >::ModifiableCornerRange::operator*() const
    {
        return this->impl_->corner();
    }

    template class opengeode_model_api Corners< 2 >;
    template class opengeode_model_api Corners< 3 >;
} // namespace geode
