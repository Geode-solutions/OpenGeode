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
#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurfaceViewBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class PolygonalSurfaceView : public PolygonalSurface< dimension >
    {
        OPENGEODE_DISABLE_COPY( PolygonalSurfaceView );
        PASSKEY(
            PolygonalSurfaceViewBuilder< dimension >, PolygonalSurfaceViewKey );

    public:
        PolygonalSurfaceView( PolygonalSurface< dimension >& surface );
        ~PolygonalSurfaceView();

        static MeshImpl impl_name_static()
        {
            return MeshImpl{ absl::StrCat(
                "PolygonalSurfaceView", dimension, "D" ) };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return PolygonalSurface< dimension >::type_name_static();
        }

        absl::string_view native_extension() const override
        {
            throw OpenGeodeException( "PolygonalSurfaceView cannot be saved" );
            return "";
        }

        index_t viewed_vertex( index_t vertex_id ) const;

        void add_viewed_vertex( index_t vertex_id, PolygonalSurfaceViewKey );

        index_t viewed_polygon( index_t polygon_id ) const;

        void add_viewed_polygon( index_t polygon_id, PolygonalSurfaceViewKey );

    private:
        const Point< dimension >& get_point( index_t vertex_id ) const override;

        index_t get_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const override;

        index_t get_nb_polygon_vertices( index_t polygon_id ) const override;

        absl::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( PolygonalSurfaceView );
} // namespace geode
