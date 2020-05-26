/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/model/mixin/core/corners.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/mesh/core/point_set.h>
#include <geode/mesh/io/point_set_input.h>
#include <geode/mesh/io/point_set_output.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/detail/components_storage.h>

namespace geode
{
    template < index_t dimension >
    class Corners< dimension >::Impl
        : public detail::ComponentsStorage< Corner< dimension > >
    {
    };

    template < index_t dimension >
    Corners< dimension >::Corners() // NOLINT
    {
    }

    template < index_t dimension >
    Corners< dimension >::Corners( Corners&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Corners< dimension >::~Corners() // NOLINT
    {
    }

    template < index_t dimension >
    index_t Corners< dimension >::nb_corners() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const Corner< dimension >& Corners< dimension >::corner(
        const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Corner< dimension >& Corners< dimension >::modifiable_corner(
        const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Corners< dimension >::save_corners( absl::string_view directory ) const
    {
        const auto prefix = absl::StrCat( directory, "/",
            Corner< dimension >::component_type_static().get() );
        for( const auto& corner : corners() )
        {
            const auto& mesh = corner.mesh();
            const auto file = absl::StrCat(
                prefix, corner.id().string(), ".", mesh.native_extension() );
            save_point_set( mesh, file );
        }
        impl_->save_components( absl::StrCat( directory, "/corners" ) );
    }

    template < index_t dimension >
    void Corners< dimension >::load_corners( absl::string_view directory )
    {
        impl_->load_components( absl::StrCat( directory, "/corners" ) );
        for( auto& corner : modifiable_corners() )
        {
            const auto file =
                impl_->find_file( directory, corner.component_id() );
            corner.set_mesh(
                load_point_set< dimension >( corner.mesh_type(), file ),
                typename Corner< dimension >::CornersKey{} );
        }
    }

    template < index_t dimension >
    typename Corners< dimension >::CornerRange
        Corners< dimension >::corners() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename Corners< dimension >::ModifiableCornerRange
        Corners< dimension >::modifiable_corners()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Corners< dimension >::create_corner()
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
    const uuid& Corners< dimension >::create_corner( const MeshType& type )
    {
        typename Corners< dimension >::Impl::ComponentPtr corner{
            new Corner< dimension >{
                type, typename Corner< dimension >::CornersKey{} }
        };
        const auto& id = corner->id();
        impl_->add_component( std::move( corner ) );
        return id;
    }

    template < index_t dimension >
    void Corners< dimension >::delete_corner(
        const Corner< dimension >& corner )
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
    };

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::CornerRangeBase(
        const Corners& corners )
        : impl_( corners.impl_->begin(), corners.impl_->end() )
    {
    }

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::CornerRangeBase(
        CornerRangeBase&& other ) noexcept
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::CornerRangeBase(
        const CornerRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Corners< dimension >::CornerRangeBase::~CornerRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool Corners< dimension >::CornerRangeBase::operator!=(
        const CornerRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Corners< dimension >::CornerRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    Corners< dimension >::CornerRange::CornerRange( const Corners& corners )
        : CornerRangeBase( corners ), BeginEnd< CornerRange >( *this )
    {
    }

    template < index_t dimension >
    const Corner< dimension >& Corners< dimension >::CornerRange::
        operator*() const
    {
        return this->impl_->corner();
    }

    template < index_t dimension >
    Corners< dimension >::ModifiableCornerRange::ModifiableCornerRange(
        const Corners& corners )
        : CornerRangeBase( corners ), BeginEnd< ModifiableCornerRange >( *this )
    {
    }

    template < index_t dimension >
    Corner< dimension >& Corners< dimension >::ModifiableCornerRange::
        operator*() const
    {
        return this->impl_->corner();
    }

    template class opengeode_model_api Corners< 2 >;
    template class opengeode_model_api Corners< 3 >;
} // namespace geode
