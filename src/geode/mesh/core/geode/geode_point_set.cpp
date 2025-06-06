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

#include <geode/mesh/core/geode/geode_point_set.hpp>

#include <fstream>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/internal/points_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodePointSet< dimension >::Impl
        : public internal::PointsImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        explicit Impl( OpenGeodePointSet< dimension >& mesh )
            : internal::PointsImpl< dimension >( mesh )
        {
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl, bitsery::ext::BaseClass<
                                     internal::PointsImpl< dimension > >{} );
                } } } );
        }
    };

    template < index_t dimension >
    OpenGeodePointSet< dimension >::OpenGeodePointSet() : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodePointSet< dimension >::OpenGeodePointSet(
        OpenGeodePointSet&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodePointSet< dimension >& OpenGeodePointSet< dimension >::operator=(
        OpenGeodePointSet&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodePointSet< dimension >::~OpenGeodePointSet() = default;

    template < index_t dimension >
    void OpenGeodePointSet< dimension >::set_vertex(
        index_t vertex_id, Point< dimension > point, OGPointSetKey )
    {
        impl_->set_point( vertex_id, std::move( point ) );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodePointSet< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OpenGeodePointSet >{
                { []( Archive& a, OpenGeodePointSet& point_set ) {
                     a.ext( point_set,
                         bitsery::ext::BaseClass< PointSet< dimension > >{} );
                     a.object( point_set.impl_ );
                     point_set.impl_->initialize_crs( point_set );
                 },
                    []( Archive& a, OpenGeodePointSet& point_set ) {
                        a.ext( point_set, bitsery::ext::BaseClass<
                                              PointSet< dimension > >{} );
                        a.object( point_set.impl_ );
                    } } } );
    }

    template class opengeode_mesh_api OpenGeodePointSet< 1 >;
    template class opengeode_mesh_api OpenGeodePointSet< 2 >;
    template class opengeode_mesh_api OpenGeodePointSet< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodePointSet< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodePointSet< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodePointSet< 3 > );
} // namespace geode
