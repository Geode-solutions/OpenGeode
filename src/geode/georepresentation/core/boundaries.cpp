/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/georepresentation/core/boundaries.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/georepresentation/core/detail/components_storage.h>
#include <geode/georepresentation/core/boundary.h>

namespace geode
{
    template < index_t dimension >
    class Boundaries< dimension >::Impl
        : public detail::ComponentsStorage< Boundary< dimension > >
    {
    };

    template < index_t dimension >
    Boundaries< dimension >::Boundaries() // NOLINT
    {
    }

    template < index_t dimension >
    Boundaries< dimension >::~Boundaries() // NOLINT
    {
    }

    template < index_t dimension >
    index_t Boundaries< dimension >::nb_boundaries() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const Boundary< dimension >& Boundaries< dimension >::boundary(
        const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Boundary< dimension >& Boundaries< dimension >::modifiable_boundary(
        const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    std::vector< std::string > Boundaries< dimension >::save_boundaries(
        const std::string& directory ) const
    {
        std::vector< std::string > files;
        files.emplace_back( directory + "/boundaries" );
        impl_->save_components( files.back() );
        return files;
    }

    template < index_t dimension >
    void Boundaries< dimension >::load_boundaries( const std::string& directory )
    {
        impl_->load_components( directory + "/boundaries" );
    }

    template < index_t dimension >
    typename Boundaries< dimension >::BoundaryRange
        Boundaries< dimension >::boundaries() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename Boundaries< dimension >::ModifiableBoundaryRange
        Boundaries< dimension >::modifiable_boundaries()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Boundaries< dimension >::create_boundary()
    {
        typename Boundaries< dimension >::Impl::ComponentPtr boundary{
            new Boundary< dimension >
        };
        const auto& id = boundary->id();
        impl_->add_component( std::move( boundary ) );
        return id;
    }

    template < index_t dimension >
    void Boundaries< dimension >::delete_boundary(
        const Boundary< dimension >& boundary )
    {
        impl_->delete_component( boundary.id() );
    }

    template < index_t dimension >
    class Boundaries< dimension >::BoundaryRangeBase::Impl
        : public BaseRange< typename Boundaries< dimension >::Impl::Iterator >
    {
        using Iterator = typename Boundaries< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Impl::Iterator >( begin, end )
        {
        }

        Boundary< dimension >& boundary() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    Boundaries< dimension >::BoundaryRangeBase::BoundaryRangeBase(
        const Boundaries& boundaries )
        : impl_( boundaries.impl_->begin(), boundaries.impl_->end() )
    {
    }

    template < index_t dimension >
    Boundaries< dimension >::BoundaryRangeBase::BoundaryRangeBase(
        BoundaryRangeBase&& other ) noexcept
        : impl_( std::move( *other.impl_ ) )
    {
    }

    template < index_t dimension >
    Boundaries< dimension >::BoundaryRangeBase::BoundaryRangeBase(
        const BoundaryRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Boundaries< dimension >::BoundaryRangeBase::~BoundaryRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool Boundaries< dimension >::BoundaryRangeBase::operator!=(
        const BoundaryRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Boundaries< dimension >::BoundaryRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    Boundaries< dimension >::BoundaryRange::BoundaryRange(
        const Boundaries& boundaries )
        : BoundaryRangeBase( boundaries )
    {
    }

    template < index_t dimension >
    const Boundary< dimension >& Boundaries< dimension >::BoundaryRange::
        operator*() const
    {
        return this->impl_->boundary();
    }

    template < index_t dimension >
    Boundaries< dimension >::ModifiableBoundaryRange::ModifiableBoundaryRange(
        const Boundaries& boundaries )
        : BoundaryRangeBase( boundaries )
    {
    }

    template < index_t dimension >
    Boundary< dimension >& Boundaries< dimension >::ModifiableBoundaryRange::
        operator*() const
    {
        return this->impl_->boundary();
    }

    template class opengeode_georepresentation_api Boundaries< 2 >;
    template class opengeode_georepresentation_api Boundaries< 3 >;
} // namespace geode
