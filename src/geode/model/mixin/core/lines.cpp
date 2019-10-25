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

#include <geode/model/mixin/core/lines.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/model/mixin/core/detail/components_storage.h>
#include <geode/model/mixin/core/line.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/io/edged_curve_input.h>
#include <geode/mesh/io/edged_curve_output.h>

namespace geode
{
    template < index_t dimension >
    class Lines< dimension >::Impl
        : public detail::ComponentsStorage< Line< dimension > >
    {
    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{},
            []( Archive &archive, Impl & impl ) {
            archive.ext( impl, bitsery::ext::BaseClass< detail::ComponentsStorage< Line< dimension > > >{} );
            } );
        }
    };

    template < index_t dimension >
    Lines< dimension >::Lines() // NOLINT
    {
    }

    template < index_t dimension >
    Lines< dimension >::~Lines() // NOLINT
    {
    }

    template < index_t dimension >
    index_t Lines< dimension >::nb_lines() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const Line< dimension >& Lines< dimension >::line( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Line< dimension >& Lines< dimension >::modifiable_line( const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    std::vector< std::string > Lines< dimension >::save_lines(
        const std::string& directory ) const
    {
        std::vector< std::string > files;
        for( const auto& line : lines() )
        {
            const auto& mesh = line.mesh();
            files.emplace_back( directory + "/" + line.component_type().get()
                                + line.id().string() + "."
                                + mesh.native_extension() );
            save_edged_curve( mesh, files.back() );
        }
        files.emplace_back( directory + "/lines" );
        impl_->save_components( files.back() );
        return files;
    }

    template < index_t dimension >
    void Lines< dimension >::load_lines( const std::string& directory )
    {
        impl_->load_components( directory + "/lines" );
        for( auto& line : modifiable_lines() )
        {
            line.ensure_mesh_type();
            auto& mesh = line.modifiable_mesh();
            load_edged_curve( mesh,
                directory + "/" + line.component_type().get()
                    + line.id().string() + "." + mesh.native_extension() );
        }
    }

    template < index_t dimension >
    typename Lines< dimension >::LineRange Lines< dimension >::lines() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename Lines< dimension >::ModifiableLineRange
        Lines< dimension >::modifiable_lines()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Lines< dimension >::create_line()
    {
        typename Lines< dimension >::Impl::ComponentPtr line{
            new Line< dimension >
        };
        const auto& id = line->id();
        impl_->add_component( std::move( line ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Lines< dimension >::create_line( const MeshType& type )
    {
        typename Lines< dimension >::Impl::ComponentPtr line{
            new Line< dimension >{ type }
        };
        const auto& id = line->id();
        impl_->add_component( std::move( line ) );
        return id;
    }

    template < index_t dimension >
    void Lines< dimension >::delete_line( const Line< dimension >& line )
    {
        impl_->delete_component( line.id() );
    }

    template < index_t dimension >
    class Lines< dimension >::LineRangeBase::Impl
        : public BaseRange< typename Lines< dimension >::Impl::Iterator >
    {
        using Iterator = typename Lines< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        Line< dimension >& line() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    Lines< dimension >::LineRangeBase::LineRangeBase( const Lines& lines )
        : impl_( lines.impl_->begin(), lines.impl_->end() )
    {
    }

    template < index_t dimension >
    Lines< dimension >::LineRangeBase::LineRangeBase(
        LineRangeBase&& other ) noexcept
        : impl_( std::move( *other.impl_ ) )
    {
    }

    template < index_t dimension >
    Lines< dimension >::LineRangeBase::LineRangeBase(
        const LineRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Lines< dimension >::LineRangeBase::~LineRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool Lines< dimension >::LineRangeBase::operator!=(
        const LineRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Lines< dimension >::LineRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    Lines< dimension >::LineRange::LineRange( const Lines& lines )
        : LineRangeBase( lines )
    {
    }

    template < index_t dimension >
    const Line< dimension >& Lines< dimension >::LineRange::operator*() const
    {
        return this->impl_->line();
    }

    template < index_t dimension >
    Lines< dimension >::ModifiableLineRange::ModifiableLineRange(
        const Lines& lines )
        : LineRangeBase( lines )
    {
    }

    template < index_t dimension >
    Line< dimension >& Lines< dimension >::ModifiableLineRange::
        operator*() const
    {
        return this->impl_->line();
    }

    template class opengeode_model_api Lines< 2 >;
    template class opengeode_model_api Lines< 3 >;
} // namespace geode
