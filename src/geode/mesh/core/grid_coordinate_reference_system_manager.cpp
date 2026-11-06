/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/mesh/core/grid_coordinate_reference_system_manager.hpp>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/grid_coordinate_reference_system.hpp>
#include <geode/mesh/core/internal/coordinate_reference_system_manager_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class GridCoordinateReferenceSystemManager< dimension >::Impl
        : public internal::CoordinateReferenceSystemManagerImpl<
              GridCoordinateReferenceSystem< dimension > >
    {
        friend class bitsery::Access;

    public:
        void set_active_coordinate_system(
            const CoordinateSystem< dimension >& coordinate_system )
        {
            if( this->has_active_coordinate_reference_system() )
            {
                this->modifiable_active_coordinate_reference_system()
                    .set_coordinate_system( coordinate_system );
            }
        }

        void copy( const Impl& impl )
        {
            internal::CoordinateReferenceSystemManagerImpl<
                GridCoordinateReferenceSystem< dimension > >::copy( impl );
        }

    private:
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                        impl.serialize_coordinate_reference_systems( archive );
                    } } } );
        }
    };

    template < index_t dimension >
    GridCoordinateReferenceSystemManager<
        dimension >::GridCoordinateReferenceSystemManager() = default;

    template < index_t dimension >
    GridCoordinateReferenceSystemManager< dimension >::
        GridCoordinateReferenceSystemManager(
            GridCoordinateReferenceSystemManager&& ) noexcept = default;

    template < index_t dimension >
    auto GridCoordinateReferenceSystemManager< dimension >::operator=(
        GridCoordinateReferenceSystemManager&& ) noexcept
        -> GridCoordinateReferenceSystemManager& = default;

    template < index_t dimension >
    GridCoordinateReferenceSystemManager<
        dimension >::~GridCoordinateReferenceSystemManager() = default;

    template < index_t dimension >
    index_t GridCoordinateReferenceSystemManager<
        dimension >::nb_coordinate_reference_systems() const
    {
        return impl_->nb_coordinate_reference_systems();
    }

    template < index_t dimension >
    const GridCoordinateReferenceSystem< dimension >&
        GridCoordinateReferenceSystemManager< dimension >::
            find_coordinate_reference_system( std::string_view name ) const
    {
        return impl_->find_coordinate_reference_system( name );
    }

    template < index_t dimension >
    const GridCoordinateReferenceSystem< dimension >&
        GridCoordinateReferenceSystemManager<
            dimension >::active_coordinate_reference_system() const
    {
        return impl_->active_coordinate_reference_system();
    }

    template < index_t dimension >
    std::string_view GridCoordinateReferenceSystemManager<
        dimension >::active_coordinate_reference_system_name() const
    {
        return impl_->active_coordinate_reference_system_name();
    }

    template < index_t dimension >
    absl::FixedArray< std::string > GridCoordinateReferenceSystemManager<
        dimension >::coordinate_reference_system_names() const
    {
        return impl_->coordinate_reference_system_names();
    }

    template < index_t dimension >
    bool GridCoordinateReferenceSystemManager< dimension >::
        coordinate_reference_system_exists( std::string_view name ) const
    {
        return impl_->coordinate_reference_system_exists( name );
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystemManager< dimension >::
        register_coordinate_reference_system( std::string_view name,
            std::shared_ptr< GridCoordinateReferenceSystem< dimension > >&& crs,
            GridCRSManagerKey /*key*/ )
    {
        impl_->register_coordinate_reference_system( name, std::move( crs ) );
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystemManager<
        dimension >::delete_coordinate_reference_system( std::string_view name,
        GridCRSManagerKey /*key*/ )
    {
        impl_->delete_coordinate_reference_system( name );
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystemManager< dimension >::
        set_active_coordinate_reference_system(
            std::string_view name, GridCRSManagerKey /*key*/ )
    {
        impl_->set_active_coordinate_reference_system( name );
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystemManager< dimension >::
        set_active_coordinate_system(
            const CoordinateSystem< dimension >& coordinate_system,
            LightRegularGridKey /*key*/ )
    {
        impl_->set_active_coordinate_system( coordinate_system );
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystemManager< dimension >::copy(
        const GridCoordinateReferenceSystemManager< dimension >& crs_manager,
        LightRegularGridKey /*key*/ )
    {
        impl_->copy( *crs_manager.impl_ );
    }

    template < index_t dimension >
    template < typename Archive >
    void GridCoordinateReferenceSystemManager< dimension >::serialize(
        Archive& serializer )
    {
        serializer.ext(
            *this, Growable< Archive, GridCoordinateReferenceSystemManager >{
                       { []( Archive& archive,
                             GridCoordinateReferenceSystemManager& manager ) {
                           archive.object( manager.impl_ );
                       } } } );
    }

    template class opengeode_mesh_api GridCoordinateReferenceSystemManager< 2 >;
    template class opengeode_mesh_api GridCoordinateReferenceSystemManager< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, GridCoordinateReferenceSystemManager< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, GridCoordinateReferenceSystemManager< 3 > );
} // namespace geode
