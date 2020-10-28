/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <array>

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurfaceViewBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    /*!
     * A mesh view of a TriangulatedSurface meaning a subset of a
     * TriangulatedSurface without copying any geometry or microtopology, it
     * only "views" the selected subeset and acts like it is a complete mesh
     * with all the API from vertices to triangles, attributes included.
     * @warning Modification on the TriangulatedSurface will not be reproduced
     on

     * the corresponding view.
     */
    template < index_t dimension >
    class TriangulatedSurfaceView : public TriangulatedSurface< dimension >
    {
        OPENGEODE_DISABLE_COPY( TriangulatedSurfaceView );
        PASSKEY( TriangulatedSurfaceViewBuilder< dimension >,
            TriangulatedSurfaceViewKey );

    public:
        TriangulatedSurfaceView(
            const TriangulatedSurface< dimension >& surface );
        TriangulatedSurfaceView( TriangulatedSurfaceView&& other );
        ~TriangulatedSurfaceView();

        static MeshImpl impl_name_static()
        {
            return MeshImpl{ absl::StrCat(
                "TriangulatedSurfaceView", dimension, "D" ) };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return TriangulatedSurface< dimension >::type_name_static();
        }

        absl::string_view native_extension() const override
        {
            throw OpenGeodeException(
                "TriangulatedSurfaceView cannot be saved" );
            return "";
        }

        index_t viewed_vertex( index_t vertex_id ) const;

        index_t add_viewed_vertex(
            index_t vertex_id, TriangulatedSurfaceViewKey );

        index_t viewed_triangle( index_t triangle_id ) const;

        index_t add_viewed_triangle(
            index_t triangle_id, TriangulatedSurfaceViewKey );

    private:
        const Point< dimension >& get_point( index_t vertex_id ) const override;

        index_t get_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const override;

        absl::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const override;

        absl::optional< PolygonVertex > get_polygon_around_vertex(
            index_t vertex_id ) const override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( TriangulatedSurfaceView );
} // namespace geode
