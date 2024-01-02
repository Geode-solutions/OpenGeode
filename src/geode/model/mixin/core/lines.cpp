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

#include <geode/model/mixin/core/lines.h>

#include <async++.h>

#include <geode/basic/identifier_builder.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/io/edged_curve_input.h>
#include <geode/mesh/io/edged_curve_output.h>

#include <geode/model/mixin/core/detail/components_storage.h>
#include <geode/model/mixin/core/line.h>

namespace geode
{
    template < index_t dimension >
    class Lines< dimension >::Impl
        : public detail::ComponentsStorage< Line< dimension > >
    {
    };

    template < index_t dimension >
    Lines< dimension >::Lines() = default;

    template < index_t dimension >
    Lines< dimension >::Lines( Lines&& ) noexcept = default;

    template < index_t dimension >
    Lines< dimension >::~Lines() = default;

    template < index_t dimension >
    Lines< dimension >& Lines< dimension >::operator=(
        Lines< dimension >&& ) noexcept = default;

    template < index_t dimension >
    index_t Lines< dimension >::nb_lines() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    bool Lines< dimension >::has_line( const uuid& id ) const
    {
        return impl_->has_component( id );
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
    void Lines< dimension >::save_lines( absl::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/lines" ) );
        const auto prefix = absl::StrCat(
            directory, "/", Line< dimension >::component_type_static().get() );
        const auto level = Logger::level();
        Logger::set_level( Logger::Level::warn );
        absl::FixedArray< async::task< void > > tasks( nb_lines() );
        index_t count{ 0 };
        for( const auto& line : lines() )
        {
            tasks[count++] = async::spawn( [&line, &prefix] {
                const auto& mesh = line.mesh();
                const auto file = absl::StrCat(
                    prefix, line.id().string(), ".", mesh.native_extension() );
                save_edged_curve( mesh, file );
            } );
        }
        auto all_tasks = async::when_all( tasks );
        all_tasks.wait();
        Logger::set_level( level );
        for( auto& task : all_tasks.get() )
        {
            task.get();
        }
    }

    template < index_t dimension >
    void Lines< dimension >::load_lines( absl::string_view directory )
    {
        impl_->load_components( absl::StrCat( directory, "/lines" ) );
        const auto mapping = impl_->file_mapping( directory );
        const auto level = Logger::level();
        Logger::set_level( Logger::Level::warn );
        absl::FixedArray< async::task< void > > tasks( nb_lines() );
        index_t count{ 0 };
        for( auto& line : modifiable_lines() )
        {
            tasks[count++] = async::spawn( [&line, &mapping] {
                const auto file = mapping.at( line.id().string() );
                line.set_mesh(
                    load_edged_curve< dimension >( line.mesh_type(), file ),
                    typename Line< dimension >::LinesKey{} );
            } );
        }
        auto all_tasks = async::when_all( tasks );
        all_tasks.wait();
        Logger::set_level( level );
        for( auto& task : all_tasks.get() )
        {
            task.get();
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
            new Line< dimension >{ typename Line< dimension >::LinesKey{} }
        };
        const auto& id = line->id();
        impl_->add_component( std::move( line ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Lines< dimension >::create_line( const MeshImpl& impl )
    {
        typename Lines< dimension >::Impl::ComponentPtr line{
            new Line< dimension >{
                impl, typename Line< dimension >::LinesKey{} }
        };
        const auto& id = line->id();
        impl_->add_component( std::move( line ) );
        return id;
    }

    template < index_t dimension >
    void Lines< dimension >::create_line( uuid line_id )
    {
        typename Lines< dimension >::Impl::ComponentPtr line{
            new Line< dimension >{ typename Line< dimension >::LinesKey{} }
        };
        IdentifierBuilder{ *line }.set_id( std::move( line_id ) );
        impl_->add_component( std::move( line ) );
    }

    template < index_t dimension >
    void Lines< dimension >::create_line( uuid line_id, const MeshImpl& impl )
    {
        typename Lines< dimension >::Impl::ComponentPtr line{
            new Line< dimension >{ impl, {} }
        };
        IdentifierBuilder{ *line }.set_id( std::move( line_id ) );
        impl_->add_component( std::move( line ) );
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
        LineRangeBase&& ) noexcept = default;

    template < index_t dimension >
    Lines< dimension >::LineRangeBase::LineRangeBase(
        const LineRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Lines< dimension >::LineRangeBase::~LineRangeBase() = default;

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
    Lines< dimension >::LineRange::LineRange( const LineRange& range )
        : LineRangeBase{ range }
    {
    }

    template < index_t dimension >
    Lines< dimension >::LineRange::~LineRange() = default;

    template < index_t dimension >
    auto Lines< dimension >::LineRange::begin() const -> const LineRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Lines< dimension >::LineRange::end() const -> const LineRange&
    {
        return *this;
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
    Lines< dimension >::ModifiableLineRange::ModifiableLineRange(
        const ModifiableLineRange& range )
        : LineRangeBase{ range }
    {
    }

    template < index_t dimension >
    Lines< dimension >::ModifiableLineRange::~ModifiableLineRange()
    {
    }

    template < index_t dimension >
    auto Lines< dimension >::ModifiableLineRange::begin() const
        -> const ModifiableLineRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Lines< dimension >::ModifiableLineRange::end() const
        -> const ModifiableLineRange&
    {
        return *this;
    }

    template < index_t dimension >
    Line< dimension >&
        Lines< dimension >::ModifiableLineRange::operator*() const
    {
        return this->impl_->line();
    }

    template class opengeode_model_api Lines< 2 >;
    template class opengeode_model_api Lines< 3 >;
} // namespace geode
