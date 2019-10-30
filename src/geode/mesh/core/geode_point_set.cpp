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

#include <geode/mesh/core/geode_point_set.h>

#include <fstream>

#include <geode/basic/attribute.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/detail/points_impl.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodePointSet< dimension >::Impl
        : public detail::PointsImpl< dimension >
    {
    public:
        explicit Impl( OpenGeodePointSet< dimension >& mesh )
            : detail::PointsImpl< dimension >( mesh )
        {
        }

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.ext(
                        impl, bitsery::ext::BaseClass<
                                  detail::PointsImpl< dimension > >{} );
                } );
        }
    };

    template < index_t dimension >
    OpenGeodePointSet< dimension >::OpenGeodePointSet() : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodePointSet< dimension >::~OpenGeodePointSet() // NOLINT
    {
    }

    template < index_t dimension >
    const Point< dimension >& OpenGeodePointSet< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    void OpenGeodePointSet< dimension >::set_vertex(
        index_t vertex_id, const Point< dimension >& point )
    {
        impl_->set_point( vertex_id, point );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodePointSet< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, OpenGeodePointSet >{},
            []( Archive& archive, OpenGeodePointSet& point_set ) {
                archive.ext( point_set,
                    bitsery::ext::BaseClass< PointSet< dimension > >{} );
                archive.object( point_set.impl_ );
            } );
    }

    template class opengeode_mesh_api OpenGeodePointSet< 2 >;
    template class opengeode_mesh_api OpenGeodePointSet< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodePointSet< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodePointSet< 3 > );
} // namespace geode
