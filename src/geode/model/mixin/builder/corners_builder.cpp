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

#include <geode/model/mixin/builder/corners_builder.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/corners.h>

#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/core/point_set.h>

namespace geode
{
    template < index_t dimension >
    const uuid& CornersBuilder< dimension >::create_corner()
    {
        return corners_.create_corner();
    }

    template < index_t dimension >
    const uuid& CornersBuilder< dimension >::create_corner(
        const MeshType& type )
    {
        return corners_.create_corner( type );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::delete_corner(
        const Corner< dimension >& corner )
    {
        corners_.delete_corner( corner );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::load_corners(
        const std::string& directory )
    {
        return corners_.load_corners( directory );
    }

    template < index_t dimension >
    std::unique_ptr< PointSetBuilder< dimension > >
        CornersBuilder< dimension >::corner_mesh_builder( const uuid& id )
    {
        return PointSetBuilder< dimension >::create(
            corners_.modifiable_corner( id ).modifiable_mesh() );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::set_corner_name(
        const uuid& id, std::string name )
    {
        corners_.modifiable_corner( id ).set_corner_name( std::move( name ) );
    }

    template < index_t dimension >
    void CornersBuilder< dimension >::set_corner_mesh(
        const uuid& id, std::unique_ptr< PointSet< dimension > > mesh )
    {
        corners_.modifiable_corner( id ).set_mesh( std::move( mesh ) );
    }

    template class opengeode_model_api CornersBuilder< 2 >;
    template class opengeode_model_api CornersBuilder< 3 >;
} // namespace geode
