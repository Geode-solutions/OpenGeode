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

#include <geode/mesh/core/geode/geode_edged_curve.hpp>

#include <array>
#include <fstream>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/internal/edges_impl.hpp>
#include <geode/mesh/core/internal/points_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeEdgedCurve< dimension >::Impl : public internal::EdgesImpl
    {
        friend class bitsery::Access;

    public:
        explicit Impl( OpenGeodeEdgedCurve< dimension >& mesh )
            : internal::EdgesImpl( mesh )
        {
            detail::template initialize_crs< OpenGeodeEdgedCurve< dimension > >(
                mesh );
        }

        Impl() = default;

    private:
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                        archive.ext( impl,
                            bitsery::ext::BaseClass< internal::EdgesImpl >{} );
                        internal::PointsImpl< dimension > temp;
                        archive.object( temp );
                    } } } );
        }
    };

    template < index_t dimension >
    OpenGeodeEdgedCurve< dimension >::OpenGeodeEdgedCurve() : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodeEdgedCurve< dimension >::OpenGeodeEdgedCurve( BITSERY bitsery )
        : EdgedCurve< dimension >{ bitsery }
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
    index_t OpenGeodeEdgedCurve< dimension >::get_edge_vertex(
        const EdgeVertex& edge_vertex ) const
    {
        return impl_->get_edge_vertex( edge_vertex );
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurve< dimension >::set_edge_vertex(
        const EdgeVertex& edge_vertex,
        index_t vertex_id,
        OGEdgedCurveKey /*key*/ )
    {
        impl_->set_edge_vertex( edge_vertex, vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodeEdgedCurve< dimension >::serialize( Archive& serializer )
    {
        serializer.ext( *this,
            Growable< Archive, OpenGeodeEdgedCurve >{
                { []( Archive& archive, OpenGeodeEdgedCurve& edged_curve ) {
                     archive.ext( edged_curve,
                         bitsery::ext::BaseClass< EdgedCurve< dimension > >{} );
                     archive.object( edged_curve.impl_ );
                     const auto new_point_attribute_id =
                         edged_curve.vertex_attribute_manager()
                             .attribute_ids_with_name( internal::PointsImpl<
                                 dimension >::POINTS_NAME )
                             .value()
                             .at( 0 );
                     detail::template initialize_crs<
                         OpenGeodeEdgedCurve< dimension > >(
                         edged_curve, new_point_attribute_id );
                 },
                    []( Archive& archive, OpenGeodeEdgedCurve& edged_curve ) {
                        archive.ext(
                            edged_curve, bitsery::ext::BaseClass<
                                             EdgedCurve< dimension > >{} );
                        archive.object( edged_curve.impl_ );
                        const auto new_point_attribute_id =
                            edged_curve.vertex_attribute_manager()
                                .attribute_ids_with_name( internal::PointsImpl<
                                    dimension >::POINTS_NAME )
                                .value()
                                .at( 0 );
                        detail::template initialize_crs<
                            OpenGeodeEdgedCurve< dimension > >(
                            edged_curve, new_point_attribute_id );
                    },
                    []( Archive& archive, OpenGeodeEdgedCurve& edged_curve ) {
                        archive.ext(
                            edged_curve, bitsery::ext::BaseClass<
                                             EdgedCurve< dimension > >{} );
                        archive.object( edged_curve.impl_ );
                    } } } );
    }

    template class opengeode_mesh_api OpenGeodeEdgedCurve< 1 >;
    template class opengeode_mesh_api OpenGeodeEdgedCurve< 2 >;
    template class opengeode_mesh_api OpenGeodeEdgedCurve< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeEdgedCurve< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeEdgedCurve< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeEdgedCurve< 3 > );
} // namespace geode
