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

#include <geode/mesh/core/coordinate_reference_system_manager.hpp>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/coordinate_reference_system.hpp>
#include <geode/mesh/core/internal/coordinate_reference_system_manager_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class CoordinateReferenceSystemManager< dimension >::Impl
        : public internal::CoordinateReferenceSystemManagerImpl<
              CoordinateReferenceSystem< dimension > >
    {
        friend class bitsery::Access;

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
    CoordinateReferenceSystemManager<
        dimension >::CoordinateReferenceSystemManager() = default;

    template < index_t dimension >
    CoordinateReferenceSystemManager< dimension >::
        CoordinateReferenceSystemManager(
            CoordinateReferenceSystemManager&& ) noexcept = default;

    template < index_t dimension >
    CoordinateReferenceSystemManager<
        dimension >::~CoordinateReferenceSystemManager() = default;

    template < index_t dimension >
    index_t CoordinateReferenceSystemManager<
        dimension >::nb_coordinate_reference_systems() const
    {
        return impl_->nb_coordinate_reference_systems();
    }

    template < index_t dimension >
    const CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManager< dimension >::
            find_coordinate_reference_system( std::string_view name ) const
    {
        return impl_->find_coordinate_reference_system( name );
    }

    template < index_t dimension >
    const CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManager<
            dimension >::active_coordinate_reference_system() const
    {
        return impl_->active_coordinate_reference_system();
    }

    template < index_t dimension >
    std::string_view CoordinateReferenceSystemManager<
        dimension >::active_coordinate_reference_system_name() const
    {
        return impl_->active_coordinate_reference_system_name();
    }

    template < index_t dimension >
    absl::FixedArray< std::string > CoordinateReferenceSystemManager<
        dimension >::coordinate_reference_system_names() const
    {
        return impl_->coordinate_reference_system_names();
    }

    template < index_t dimension >
    bool CoordinateReferenceSystemManager< dimension >::
        coordinate_reference_system_exists( std::string_view name ) const
    {
        return impl_->coordinate_reference_system_exists( name );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManager< dimension >::
        register_coordinate_reference_system( std::string_view name,
            std::shared_ptr< CoordinateReferenceSystem< dimension > >&& crs,
            CRSManagerKey /*key*/ )
    {
        impl_->register_coordinate_reference_system( name, std::move( crs ) );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManager<
        dimension >::delete_coordinate_reference_system( std::string_view name,
        CRSManagerKey /*key*/ )
    {
        impl_->delete_coordinate_reference_system( name );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManager< dimension >::
        set_active_coordinate_reference_system(
            std::string_view name, CRSManagerKey /*key*/ )
    {
        impl_->set_active_coordinate_reference_system( name );
    }

    template < index_t dimension >
    CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManager< dimension >::
            modifiable_active_coordinate_reference_system(
                CRSManagerKey /*key*/ )
    {
        return impl_->modifiable_active_coordinate_reference_system();
    }

    template < index_t dimension >
    CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManager< dimension >::
            modifiable_coordinate_reference_system(
                std::string_view name, CRSManagerKey /*key*/ )
    {
        return impl_->modifiable_coordinate_reference_system( name );
    }

    template < index_t dimension >
    template < typename Archive >
    void CoordinateReferenceSystemManager< dimension >::serialize(
        Archive& serializer )
    {
        serializer.ext(
            *this, Growable< Archive, CoordinateReferenceSystemManager >{
                       { []( Archive& archive,
                             CoordinateReferenceSystemManager& manager ) {
                           archive.object( manager.impl_ );
                       } } } );
    }

    template class opengeode_mesh_api CoordinateReferenceSystemManager< 1 >;
    template class opengeode_mesh_api CoordinateReferenceSystemManager< 2 >;
    template class opengeode_mesh_api CoordinateReferenceSystemManager< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, CoordinateReferenceSystemManager< 1 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, CoordinateReferenceSystemManager< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, CoordinateReferenceSystemManager< 3 > );
} // namespace geode