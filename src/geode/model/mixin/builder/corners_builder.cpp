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

#include <geode/model/mixin/builder/corners_builder.hpp>

#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/core/point_set.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corners.hpp>

namespace geode
{
    template < index_t dimension >
    const uuid& CornersBuilder< dimension >::create_corner()
    {
        return corners_.create_corner(
            typename Corners< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    const uuid& CornersBuilder< dimension >::create_corner(
        const MeshImpl& impl )
    {
        return corners_.create_corner(
            impl, typename Corners< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::create_corner( uuid corner_id )
    {
        corners_.create_corner( std::move( corner_id ),
            typename Corners< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::create_corner(
        uuid corner_id, const MeshImpl& impl )
    {
        corners_.create_corner( std::move( corner_id ), impl,
            typename Corners< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::delete_corner(
        const Corner< dimension >& corner )
    {
        corners_.delete_corner(
            corner, typename Corners< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::load_corners( std::string_view directory )
    {
        return corners_.load_corners(
            directory, typename Corners< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    std::unique_ptr< PointSetBuilder< dimension > >
        CornersBuilder< dimension >::corner_mesh_builder(
            const Corner< dimension >& corner )
    {
        return PointSetBuilder< dimension >::create( corners_
                .modifiable_corner( corner.id(),
                    typename Corners< dimension >::CornersBuilderKey{} )
                .modifiable_mesh(
                    typename Corner< dimension >::CornersBuilderKey{} ) );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::set_corner_name(
        const Corner< dimension >& corner, std::string_view name )
    {
        corners_
            .modifiable_corner( corner.id(),
                typename Corners< dimension >::CornersBuilderKey{} )
            .set_corner_name(
                name, typename Corners< dimension >::CornersBuilderKey{} );
        corner_mesh_builder( corner )->set_name( name );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::set_corner_active(
        const Corner< dimension >& corner, bool active )
    {
        corners_
            .modifiable_corner( corner.id(),
                typename Corners< dimension >::CornersBuilderKey{} )
            .set_corner_active(
                active, typename Corners< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::set_corner_mesh(
        const uuid& id, std::unique_ptr< PointSet< dimension > > mesh )
    {
        corners_
            .modifiable_corner(
                id, typename Corners< dimension >::CornersBuilderKey{} )
            .set_mesh( std::move( mesh ),
                typename Corner< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    PointSet< dimension >& CornersBuilder< dimension >::modifiable_corner_mesh(
        const uuid& id )
    {
        return corners_
            .modifiable_corner(
                id, typename Corner< dimension >::CornersBuilderKey{} )
            .modifiable_mesh(
                typename Corner< dimension >::CornersBuilderKey{} );
    }

    template < index_t dimension >
    std::unique_ptr< PointSet< dimension > >
        CornersBuilder< dimension >::steal_corner_mesh(
            const Corner< dimension >& corner )
    {
        return corners_
            .modifiable_corner(
                corner.id(), typename Corner< dimension >::CornersBuilderKey{} )
            .steal_mesh( typename Corner< dimension >::CornersBuilderKey{} );
    }

    template class opengeode_model_api CornersBuilder< 2 >;
    template class opengeode_model_api CornersBuilder< 3 >;
} // namespace geode
