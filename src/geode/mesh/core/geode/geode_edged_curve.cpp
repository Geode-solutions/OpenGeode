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

#include <geode/mesh/core/geode/geode_edged_curve.h>

#include <array>
#include <fstream>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/private/edges_impl.h>
#include <geode/mesh/core/private/points_impl.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeEdgedCurve< dimension >::Impl
        : public detail::EdgesImpl,
          public detail::PointsImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        explicit Impl( OpenGeodeEdgedCurve< dimension >& mesh )
            : detail::EdgesImpl( mesh ), detail::PointsImpl< dimension >( mesh )
        {
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext(
                        impl, bitsery::ext::BaseClass< detail::EdgesImpl >{} );
                    a.ext( impl, bitsery::ext::BaseClass<
                                     detail::PointsImpl< dimension > >{} );
                } } } );
        }
    };

    template < index_t dimension >
    OpenGeodeEdgedCurve< dimension >::OpenGeodeEdgedCurve() : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodeEdgedCurve< dimension >::OpenGeodeEdgedCurve(
        OpenGeodeEdgedCurve&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeEdgedCurve< dimension >&
        OpenGeodeEdgedCurve< dimension >::operator=(
            OpenGeodeEdgedCurve&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeEdgedCurve< dimension >::~OpenGeodeEdgedCurve() = default;

    template < index_t dimension >
    void OpenGeodeEdgedCurve< dimension >::set_vertex(
        index_t vertex_id, Point< dimension > point, OGEdgedCurveKey )
    {
        impl_->set_point( vertex_id, std::move( point ) );
    }

    template < index_t dimension >
    index_t OpenGeodeEdgedCurve< dimension >::get_edge_vertex(
        const EdgeVertex& edge_vertex ) const
    {
        return impl_->get_edge_vertex( edge_vertex );
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurve< dimension >::set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id, OGEdgedCurveKey )
    {
        impl_->set_edge_vertex( edge_vertex, vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodeEdgedCurve< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OpenGeodeEdgedCurve >{
                { []( Archive& a, OpenGeodeEdgedCurve& edged_curve ) {
                     a.ext( edged_curve,
                         bitsery::ext::BaseClass< EdgedCurve< dimension > >{} );
                     a.object( edged_curve.impl_ );
                     edged_curve.impl_->initialize_crs( edged_curve );
                 },
                    []( Archive& a, OpenGeodeEdgedCurve& edged_curve ) {
                        a.ext( edged_curve, bitsery::ext::BaseClass<
                                                EdgedCurve< dimension > >{} );
                        a.object( edged_curve.impl_ );
                    } } } );
    }

    template class opengeode_mesh_api OpenGeodeEdgedCurve< 2 >;
    template class opengeode_mesh_api OpenGeodeEdgedCurve< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeEdgedCurve< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeEdgedCurve< 3 > );
} // namespace geode
