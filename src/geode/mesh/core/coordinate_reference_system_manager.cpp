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

#include <geode/mesh/core/coordinate_reference_system_manager.h>

#include <bitsery/ext/std_map.h>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/coordinate_reference_system.h>

namespace geode
{
    template < index_t dimension >
    class CoordinateReferenceSystemManager< dimension >::Impl
    {
        friend class bitsery::Access;

    public:
        index_t nb_coordinate_reference_systems() const
        {
            return crss_.size();
        }

        const CoordinateReferenceSystem< dimension >&
            find_coordinate_reference_system( absl::string_view name ) const
        {
            const auto it = crss_.find( name );
            OPENGEODE_EXCEPTION( it != crss_.end(),
                "[CoordinateReferenceSystemManager::find_coordinate_reference_"
                "system] Unknown CRS :",
                name );
            return *it->second;
        }

        const CoordinateReferenceSystem< dimension >&
            active_coordinate_reference_system() const
        {
            OPENGEODE_EXCEPTION( active_crs_.use_count() > 1,
                "[CoordinateReferenceSystemManager::active_coordinate_"
                "reference_system] Active CRS not defined" );
            return *active_crs_;
        }

        absl::string_view active_coordinate_reference_system_name() const
        {
            return active_crs_name_;
        }

        absl::FixedArray< absl::string_view >
            coordinate_reference_system_names() const
        {
            absl::FixedArray< absl::string_view > names( crss_.size() );
            index_t count{ 0 };
            for( const auto& it : crss_ )
            {
                names[count++] = it.first;
            }
            return names;
        }

        bool coordinate_reference_system_exists( absl::string_view name ) const
        {
            return crss_.find( name ) != crss_.end();
        }

        void register_coordinate_reference_system( absl::string_view name,
            std::shared_ptr< CoordinateReferenceSystem< dimension > >&& crs )
        {
            const auto status =
                crss_.emplace( to_string( name ), std::move( crs ) );
            OPENGEODE_EXCEPTION( status.second,
                "[CoordinateReferenceSystemManager::register_coordinate_"
                "reference_system] CRS named ",
                name, " already exists" );
        }

        void delete_coordinate_reference_system( absl::string_view name )
        {
            const auto it = crss_.find( name );
            if( it != crss_.end() )
            {
                crss_.erase( it );
            }
            if( name == active_crs_name_ )
            {
                active_crs_name_.clear();
                active_crs_.reset();
            }
        }

        void set_active_coordinate_reference_system( absl::string_view name )
        {
            const auto it = crss_.find( name );
            OPENGEODE_EXCEPTION( it != crss_.end(),
                "[CoordinateReferenceSystemManager::set_active_coordinate_"
                "reference_system] Unknown CRS :",
                name );
            active_crs_ = it->second;
            active_crs_name_ = to_string( name );
        }

        CoordinateReferenceSystem< dimension >&
            modifiable_active_coordinate_reference_system()
        {
            OPENGEODE_EXCEPTION( active_crs_.use_count() > 0,
                "[CoordinateReferenceSystemManager::modifiable_active_"
                "coordinate_reference_system] Active CRS not defined" );
            return *active_crs_;
        }

        CoordinateReferenceSystem< dimension >&
            modifiable_coordinate_reference_system( absl::string_view name )
        {
            const auto it = crss_.find( name );
            OPENGEODE_EXCEPTION( it != crss_.end(),
                "[CoordinateReferenceSystemManager::find_coordinate_reference_"
                "system] Unknown CRS :",
                name );
            return *it->second;
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{ { []( Archive& a,
                                                                 Impl& impl ) {
                a.ext( impl.crss_,
                    bitsery::ext::StdMap{ impl.crss_.max_size() },
                    []( Archive& a2, std::string& name,
                        std::shared_ptr<
                            CoordinateReferenceSystem< dimension > >& crs ) {
                        a2.text1b( name, name.max_size() );
                        a2.ext( crs, bitsery::ext::StdSmartPtr{} );
                    } );
                a.ext( impl.active_crs_, bitsery::ext::StdSmartPtr{} );
                a.text1b(
                    impl.active_crs_name_, impl.active_crs_name_.max_size() );
            } } } );
        }

    private:
        absl::flat_hash_map< std::string,
            std::shared_ptr< CoordinateReferenceSystem< dimension > > >
            crss_;
        std::shared_ptr< CoordinateReferenceSystem< dimension > > active_crs_;
        std::string active_crs_name_;
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
            find_coordinate_reference_system( absl::string_view name ) const
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
    absl::string_view CoordinateReferenceSystemManager<
        dimension >::active_coordinate_reference_system_name() const
    {
        return impl_->active_coordinate_reference_system_name();
    }

    template < index_t dimension >
    absl::FixedArray< absl::string_view > CoordinateReferenceSystemManager<
        dimension >::coordinate_reference_system_names() const
    {
        return impl_->coordinate_reference_system_names();
    }

    template < index_t dimension >
    bool CoordinateReferenceSystemManager< dimension >::
        coordinate_reference_system_exists( absl::string_view name ) const
    {
        return impl_->coordinate_reference_system_exists( name );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManager< dimension >::
        register_coordinate_reference_system( absl::string_view name,
            std::shared_ptr< CoordinateReferenceSystem< dimension > >&& crs,
            CRSManagerKey )
    {
        impl_->register_coordinate_reference_system( name, std::move( crs ) );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManager<
        dimension >::delete_coordinate_reference_system( absl::string_view name,
        CRSManagerKey )
    {
        impl_->delete_coordinate_reference_system( name );
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManager< dimension >::
        set_active_coordinate_reference_system(
            absl::string_view name, CRSManagerKey )
    {
        impl_->set_active_coordinate_reference_system( name );
    }

    template < index_t dimension >
    CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManager< dimension >::
            modifiable_active_coordinate_reference_system( CRSManagerKey )
    {
        return impl_->modifiable_active_coordinate_reference_system();
    }

    template < index_t dimension >
    CoordinateReferenceSystem< dimension >&
        CoordinateReferenceSystemManager< dimension >::
            modifiable_coordinate_reference_system(
                absl::string_view name, CRSManagerKey )
    {
        return impl_->modifiable_coordinate_reference_system( name );
    }

    template < index_t dimension >
    template < typename Archive >
    void CoordinateReferenceSystemManager< dimension >::serialize(
        Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, CoordinateReferenceSystemManager >{
                { []( Archive& a, CoordinateReferenceSystemManager& manager ) {
                    a.object( manager.impl_ );
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