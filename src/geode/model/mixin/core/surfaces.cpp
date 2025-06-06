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

#include <geode/model/mixin/core/surfaces.hpp>

#include <async++.h>

#include <geode/basic/detail/count_range_elements.hpp>
#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/polygonal_surface_input.hpp>
#include <geode/mesh/io/polygonal_surface_output.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/model/mixin/core/detail/components_storage.hpp>
#include <geode/model/mixin/core/surface.hpp>

namespace geode
{
    template < index_t dimension >
    class Surfaces< dimension >::Impl
        : public detail::ComponentsStorage< Surface< dimension > >
    {
    };

    template < index_t dimension >
    Surfaces< dimension >::Surfaces() = default;

    template < index_t dimension >
    Surfaces< dimension >::Surfaces( Surfaces&& ) noexcept = default;

    template < index_t dimension >
    Surfaces< dimension >::~Surfaces() = default;

    template < index_t dimension >
    Surfaces< dimension >& Surfaces< dimension >::operator=(
        Surfaces< dimension >&& ) noexcept = default;

    template < index_t dimension >
    index_t Surfaces< dimension >::nb_surfaces() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    index_t Surfaces< dimension >::nb_active_surfaces() const
    {
        return detail::count_range_elements( active_surfaces() );
    }

    template < index_t dimension >
    bool Surfaces< dimension >::has_surface( const uuid& id ) const
    {
        return impl_->has_component( id );
    }

    template < index_t dimension >
    const Surface< dimension >& Surfaces< dimension >::surface(
        const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Surface< dimension >& Surfaces< dimension >::modifiable_surface(
        const uuid& id, SurfacesBuilderKey )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Surfaces< dimension >::save_surfaces(
        std::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/surfaces" ) );
        const auto prefix = absl::StrCat( directory, "/",
            Surface< dimension >::component_type_static().get() );
        absl::FixedArray< async::task< void > > tasks( nb_surfaces() );
        index_t count{ 0 };
        for( const auto& surface : surfaces() )
        {
            tasks[count++] = async::spawn( [&surface, &prefix] {
                const auto& mesh = surface.mesh();
                const auto file = absl::StrCat( prefix, surface.id().string(),
                    ".", mesh.native_extension() );
                if( const auto* triangulated =
                        dynamic_cast< const TriangulatedSurface< dimension >* >(
                            &mesh ) )
                {
                    save_triangulated_surface( *triangulated, file );
                }
                else if( const auto* polygonal = dynamic_cast<
                             const PolygonalSurface< dimension >* >( &mesh ) )
                {
                    save_polygonal_surface( *polygonal, file );
                }
                else
                {
                    throw OpenGeodeException( "[Surfaces::save_surfaces] "
                                              "Cannot find the explicit "
                                              "SurfaceMesh type" );
                }
            } );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
    }

    template < index_t dimension >
    void Surfaces< dimension >::load_surfaces(
        std::string_view directory, SurfacesBuilderKey )
    {
        impl_->load_components( absl::StrCat( directory, "/surfaces" ) );
        const auto mapping = impl_->file_mapping( directory );
        absl::FixedArray< async::task< void > > tasks( nb_surfaces() );
        index_t count{ 0 };
        for( auto& surface : modifiable_surfaces( {} ) )
        {
            tasks[count++] = async::spawn( [&surface, &mapping] {
                const auto file = mapping.at( surface.id().string() );
                if( MeshFactory::type( surface.mesh_type() )
                    == TriangulatedSurface< dimension >::type_name_static() )
                {
                    surface.set_mesh( load_triangulated_surface< dimension >(
                                          surface.mesh_type(), file ),
                        typename Surface< dimension >::SurfacesKey{} );
                }
                else
                {
                    surface.set_mesh( load_polygonal_surface< dimension >(
                                          surface.mesh_type(), file ),
                        typename Surface< dimension >::SurfacesKey{} );
                }
            } );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
    }

    template < index_t dimension >
    auto Surfaces< dimension >::surfaces() const -> SurfaceRange
    {
        return { *this };
    }

    template < index_t dimension >
    auto Surfaces< dimension >::active_surfaces() const -> SurfaceRange
    {
        SurfaceRange range{ *this };
        range.set_active_only();
        return range;
    }

    template < index_t dimension >
    auto Surfaces< dimension >::modifiable_surfaces( SurfacesBuilderKey )
        -> ModifiableSurfaceRange
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Surfaces< dimension >::create_surface( SurfacesBuilderKey )
    {
        typename Surfaces< dimension >::Impl::ComponentPtr surface{
            new Surface< dimension >{
                typename Surface< dimension >::SurfacesKey{} }
        };
        const auto& id = surface->id();
        impl_->add_component( std::move( surface ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Surfaces< dimension >::create_surface(
        const MeshImpl& impl, SurfacesBuilderKey )
    {
        typename Surfaces< dimension >::Impl::ComponentPtr surface{
            new Surface< dimension >{
                impl, typename Surface< dimension >::SurfacesKey{} }
        };
        const auto& id = surface->id();
        impl_->add_component( std::move( surface ) );
        return id;
    }

    template < index_t dimension >
    void Surfaces< dimension >::create_surface(
        uuid surface_id, SurfacesBuilderKey )
    {
        typename Surfaces< dimension >::Impl::ComponentPtr surface{
            new Surface< dimension >{
                typename Surface< dimension >::SurfacesKey{} }
        };
        IdentifierBuilder{ *surface }.set_id( std::move( surface_id ) );
        impl_->add_component( std::move( surface ) );
    }

    template < index_t dimension >
    void Surfaces< dimension >::create_surface(
        uuid surface_id, const MeshImpl& impl, SurfacesBuilderKey )
    {
        typename Surfaces< dimension >::Impl::ComponentPtr surface{
            new Surface< dimension >{ impl, {} }
        };
        IdentifierBuilder{ *surface }.set_id( std::move( surface_id ) );
        impl_->add_component( std::move( surface ) );
    }

    template < index_t dimension >
    void Surfaces< dimension >::delete_surface(
        const Surface< dimension >& surface, SurfacesBuilderKey )
    {
        impl_->delete_component( surface.id() );
    }

    template < index_t dimension >
    class Surfaces< dimension >::SurfaceRangeBase::Impl
        : public BaseRange< typename Surfaces< dimension >::Impl::Iterator >
    {
        using Iterator = typename Surfaces< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Impl::Iterator >( begin, end )
        {
        }

        Surface< dimension >& surface() const
        {
            return *this->current()->second;
        }

        void set_active_only()
        {
            active_only_ = true;
            next_surface();
        }

        void next()
        {
            this->operator++();
            next_surface();
        }

    private:
        void next_surface()
        {
            while( this->operator!=( *this )
                   && ( active_only_ && !surface().is_active() ) )
            {
                this->operator++();
            }
        }

    private:
        bool active_only_{ false };
    };

    template < index_t dimension >
    Surfaces< dimension >::SurfaceRangeBase::SurfaceRangeBase(
        const Surfaces& surfaces )
        : impl_( surfaces.impl_->begin(), surfaces.impl_->end() )
    {
    }

    template < index_t dimension >
    Surfaces< dimension >::SurfaceRangeBase::SurfaceRangeBase(
        SurfaceRangeBase&& ) noexcept = default;

    template < index_t dimension >
    Surfaces< dimension >::SurfaceRangeBase::SurfaceRangeBase(
        const SurfaceRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Surfaces< dimension >::SurfaceRangeBase::~SurfaceRangeBase() = default;

    template < index_t dimension >
    bool Surfaces< dimension >::SurfaceRangeBase::operator!=(
        const SurfaceRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Surfaces< dimension >::SurfaceRangeBase::set_active_only()
    {
        impl_->set_active_only();
    }

    template < index_t dimension >
    void Surfaces< dimension >::SurfaceRangeBase::operator++()
    {
        return impl_->next();
    }

    template < index_t dimension >
    Surfaces< dimension >::SurfaceRange::SurfaceRange(
        const Surfaces& surfaces )
        : SurfaceRangeBase( surfaces )
    {
    }

    template < index_t dimension >
    Surfaces< dimension >::SurfaceRange::SurfaceRange(
        const SurfaceRange& range )
        : SurfaceRangeBase{ range }
    {
    }

    template < index_t dimension >
    Surfaces< dimension >::SurfaceRange::~SurfaceRange() = default;

    template < index_t dimension >
    auto Surfaces< dimension >::SurfaceRange::begin() const
        -> const SurfaceRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Surfaces< dimension >::SurfaceRange::end() const -> const SurfaceRange&
    {
        return *this;
    }

    template < index_t dimension >
    const Surface< dimension >&
        Surfaces< dimension >::SurfaceRange::operator*() const
    {
        return this->impl_->surface();
    }

    template < index_t dimension >
    Surfaces< dimension >::ModifiableSurfaceRange::ModifiableSurfaceRange(
        const Surfaces& surfaces )
        : SurfaceRangeBase( surfaces )
    {
    }

    template < index_t dimension >
    Surfaces< dimension >::ModifiableSurfaceRange::ModifiableSurfaceRange(
        const ModifiableSurfaceRange& range )
        : SurfaceRangeBase{ range }
    {
    }

    template < index_t dimension >
    Surfaces< dimension >::ModifiableSurfaceRange::~ModifiableSurfaceRange() =
        default;

    template < index_t dimension >
    auto Surfaces< dimension >::ModifiableSurfaceRange::begin() const
        -> const ModifiableSurfaceRange&
    {
        return *this;
    }

    template < index_t dimension >
    auto Surfaces< dimension >::ModifiableSurfaceRange::end() const
        -> const ModifiableSurfaceRange&
    {
        return *this;
    }

    template < index_t dimension >
    Surface< dimension >&
        Surfaces< dimension >::ModifiableSurfaceRange::operator*() const
    {
        return this->impl_->surface();
    }

    template class opengeode_model_api Surfaces< 2 >;
    template class opengeode_model_api Surfaces< 3 >;
} // namespace geode
