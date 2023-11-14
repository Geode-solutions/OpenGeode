/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/core/coordinate_reference_system_managers.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>
#include <geode/mesh/core/coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system_manager.h>

namespace geode
{
    template < index_t dimension >
    class CoordinateReferenceSystemManagers< dimension >::Impl
    {
        friend class bitsery::Access;

    public:
        const CoordinateReferenceSystemManager1D&
            coordinate_reference_system_manager1D() const
        {
            return crs_manager1D_;
        }

        const CoordinateReferenceSystemManager2D&
            coordinate_reference_system_manager2D() const
        {
            return crs_manager2D_;
        }

        const CoordinateReferenceSystemManager3D&
            coordinate_reference_system_manager3D() const
        {
            return crs_manager3D_;
        }

        const CoordinateReferenceSystemManager< dimension >&
            main_coordinate_reference_system_manager() const;

        const Point< dimension >& point( index_t vertex ) const
        {
            return main_coordinate_reference_system_manager()
                .active_coordinate_reference_system()
                .point( vertex );
        }

        CoordinateReferenceSystemManager1D&
            coordinate_reference_system_manager1D()
        {
            return crs_manager1D_;
        }

        CoordinateReferenceSystemManager2D&
            coordinate_reference_system_manager2D()
        {
            return crs_manager2D_;
        }

        CoordinateReferenceSystemManager3D&
            coordinate_reference_system_manager3D()
        {
            return crs_manager3D_;
        }

        CoordinateReferenceSystemManager< dimension >&
            main_coordinate_reference_system_manager();

        void set_point( index_t vertex, Point< dimension > point )
        {
            CoordinateReferenceSystemManagerBuilder< dimension >{
                main_coordinate_reference_system_manager()
            }
                .active_coordinate_reference_system()
                .set_point( vertex, std::move( point ) );
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.object( impl.crs_manager1D_ );
                    a.object( impl.crs_manager2D_ );
                    a.object( impl.crs_manager3D_ );
                } } } );
        }

    private:
        CoordinateReferenceSystemManager1D crs_manager1D_;
        CoordinateReferenceSystemManager2D crs_manager2D_;
        CoordinateReferenceSystemManager3D crs_manager3D_;
    };

    template <>
    const CoordinateReferenceSystemManager< 3 >&
        CoordinateReferenceSystemManagers<
            3 >::Impl::main_coordinate_reference_system_manager() const
    {
        return coordinate_reference_system_manager3D();
    }

    template <>
    const CoordinateReferenceSystemManager< 2 >&
        CoordinateReferenceSystemManagers<
            2 >::Impl::main_coordinate_reference_system_manager() const
    {
        return coordinate_reference_system_manager2D();
    }

    template <>
    CoordinateReferenceSystemManager< 3 >& CoordinateReferenceSystemManagers<
        3 >::Impl::main_coordinate_reference_system_manager()
    {
        return coordinate_reference_system_manager3D();
    }

    template <>
    CoordinateReferenceSystemManager< 2 >& CoordinateReferenceSystemManagers<
        2 >::Impl::main_coordinate_reference_system_manager()
    {
        return coordinate_reference_system_manager2D();
    }

    template < index_t dimension >
    CoordinateReferenceSystemManagers<
        dimension >::CoordinateReferenceSystemManagers() = default;

    template < index_t dimension >
    CoordinateReferenceSystemManagers< dimension >::
        CoordinateReferenceSystemManagers(
            CoordinateReferenceSystemManagers&& ) noexcept = default;

    template < index_t dimension >
    CoordinateReferenceSystemManagers< dimension >&
        CoordinateReferenceSystemManagers< dimension >::operator=(
            CoordinateReferenceSystemManagers&& ) noexcept = default;

    template < index_t dimension >
    CoordinateReferenceSystemManagers<
        dimension >::~CoordinateReferenceSystemManagers() = default;

    template < index_t dimension >
    const CoordinateReferenceSystemManager1D& CoordinateReferenceSystemManagers<
        dimension >::coordinate_reference_system_manager1D() const
    {
        return impl_->coordinate_reference_system_manager1D();
    }

    template < index_t dimension >
    const CoordinateReferenceSystemManager2D& CoordinateReferenceSystemManagers<
        dimension >::coordinate_reference_system_manager2D() const
    {
        return impl_->coordinate_reference_system_manager2D();
    }

    template < index_t dimension >
    const CoordinateReferenceSystemManager3D& CoordinateReferenceSystemManagers<
        dimension >::coordinate_reference_system_manager3D() const
    {
        return impl_->coordinate_reference_system_manager3D();
    }

    template < index_t dimension >
    const CoordinateReferenceSystemManager< dimension >&
        CoordinateReferenceSystemManagers<
            dimension >::main_coordinate_reference_system_manager() const
    {
        return impl_->main_coordinate_reference_system_manager();
    }

    template < index_t dimension >
    const Point< dimension >&
        CoordinateReferenceSystemManagers< dimension >::point(
            index_t vertex ) const
    {
        return impl_->point( vertex );
    }

    template < index_t dimension >
    CoordinateReferenceSystemManager1D& CoordinateReferenceSystemManagers<
        dimension >::coordinate_reference_system_manager1D( CRSManagersKey )
    {
        return impl_->coordinate_reference_system_manager1D();
    }

    template < index_t dimension >
    CoordinateReferenceSystemManager2D& CoordinateReferenceSystemManagers<
        dimension >::coordinate_reference_system_manager2D( CRSManagersKey )
    {
        return impl_->coordinate_reference_system_manager2D();
    }

    template < index_t dimension >
    CoordinateReferenceSystemManager3D& CoordinateReferenceSystemManagers<
        dimension >::coordinate_reference_system_manager3D( CRSManagersKey )
    {
        return impl_->coordinate_reference_system_manager3D();
    }

    template < index_t dimension >
    CoordinateReferenceSystemManager< dimension >&
        CoordinateReferenceSystemManagers< dimension >::
            main_coordinate_reference_system_manager( CRSManagersKey )
    {
        return impl_->main_coordinate_reference_system_manager();
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManagers< dimension >::set_point(
        index_t vertex, Point< dimension > point, CRSManagersKey )
    {
        impl_->set_point( vertex, std::move( point ) );
    }

    template < index_t dimension >
    template < typename Archive >
    void CoordinateReferenceSystemManagers< dimension >::serialize(
        Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, CoordinateReferenceSystemManagers >{
                       { []( Archive& a,
                             CoordinateReferenceSystemManagers& managers ) {
                           a.object( managers.impl_ );
                       } } } );
    }

    template class opengeode_mesh_api CoordinateReferenceSystemManagers< 2 >;
    template class opengeode_mesh_api CoordinateReferenceSystemManagers< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, CoordinateReferenceSystemManagers< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, CoordinateReferenceSystemManagers< 3 > );
} // namespace geode