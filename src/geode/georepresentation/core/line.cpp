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

#include <geode/georepresentation/core/line.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/georepresentation/core/detail/mesh_storage.h>

#include <geode/mesh/core/edged_curve.h>

namespace geode
{
    template < index_t dimension >
    class Line< dimension >::Impl
        : public detail::MeshStorage< EdgedCurve< dimension > >
    {
    };

    template < index_t dimension >
    Line< dimension >::~Line() // NOLINT
    {
    }

    template < index_t dimension >
    Line< dimension >::Line()
    {
        impl_->set_mesh( EdgedCurve< dimension >::create() );
    }

    template < index_t dimension >
    Line< dimension >::Line( const MeshType& type )
    {
        impl_->set_mesh( EdgedCurve< dimension >::create( type ) );
    }

    template < index_t dimension >
    const EdgedCurve< dimension >& Line< dimension >::mesh() const
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    EdgedCurve< dimension >& Line< dimension >::modifiable_mesh()
    {
        return impl_->modifiable_mesh();
    }

    template < index_t dimension >
    template < typename Archive >
    void Line< dimension >::serialize( Archive& archive )
    {
        archive.ext(
            *this, bitsery::ext::BaseClass< Component< dimension > >{} );
    }

    template class opengeode_georepresentation_api Line< 2 >;
    template class opengeode_georepresentation_api Line< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_georepresentation_api, Line< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_georepresentation_api, Line< 3 > );
} // namespace geode
