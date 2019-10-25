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

#pragma once

#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    /*!
     * Interface class for Surface of arbitrary dimension made up with triangles
     * only.
     */
    template < index_t dimension >
    class TriangulatedSurface : public PolygonalSurface< dimension >
    {
    public:
        /*!
         * Create a new TriangulatedSurface using default data structure.
         */
        static std::unique_ptr< TriangulatedSurface< dimension > > create();

        /*!
         * Create a new TriangulatedSurface using a specified data structure.
         * @param[in] type Data structure type
         */
        static std::unique_ptr< TriangulatedSurface< dimension > > create(
            const MeshType& type );

    protected:
        TriangulatedSurface() = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
        archive.ext( *this, geode::Growable< Archive, TriangulatedSurface >{},
            []( Archive &archive, TriangulatedSurface &triangulated_surface ) {
            archive.ext( triangulated_surface,
                bitsery::ext::BaseClass< PolygonalSurface< dimension > >{} );
            });
        }

        index_t get_nb_polygon_vertices( index_t /*unused*/ ) const final
        {
            return 3;
        }
    };
    ALIAS_2D_AND_3D( TriangulatedSurface );

    template < index_t dimension >
    using TriangulatedSurfaceFactory =
        Factory< MeshType, TriangulatedSurface< dimension > >;
    ALIAS_2D_AND_3D( TriangulatedSurfaceFactory );
} // namespace geode
