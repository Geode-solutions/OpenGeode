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

#pragma once

#include <array>

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeTriangulatedSurfaceBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class OpenGeodeTriangulatedSurface : public TriangulatedSurface< dimension >
    {
        OPENGEODE_DISABLE_COPY( OpenGeodeTriangulatedSurface );
        PASSKEY( OpenGeodeTriangulatedSurfaceBuilder< dimension >,
            OGTriangulatedSurfaceKey );

    public:
        using Builder = OpenGeodeTriangulatedSurfaceBuilder< dimension >;
        static constexpr auto dim = dimension;

        OpenGeodeTriangulatedSurface();
        OpenGeodeTriangulatedSurface(
            OpenGeodeTriangulatedSurface&& other ) noexcept;
        OpenGeodeTriangulatedSurface& operator=(
            OpenGeodeTriangulatedSurface&& other ) noexcept;
        ~OpenGeodeTriangulatedSurface();

        static MeshImpl impl_name_static()
        {
            return MeshImpl{ absl::StrCat(
                "OpenGeodeTriangulatedSurface", dimension, "D" ) };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return TriangulatedSurface< dimension >::type_name_static();
        }

        static std::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_tsf", dimension, "d" );
            return extension;
        }

        std::string_view native_extension() const override
        {
            return native_extension_static();
        }

    public:
        void set_vertex( index_t vertex_id,
            Point< dimension > point,
            OGTriangulatedSurfaceKey );

        void set_polygon_vertex( const PolygonVertex& polygon_vertex,
            index_t vertex_id,
            OGTriangulatedSurfaceKey );

        void set_polygon_adjacent( const PolygonEdge& polygon_edge,
            index_t adjacent_id,
            OGTriangulatedSurfaceKey );

        void add_triangle( const std::array< index_t, 3 >& vertices,
            OGTriangulatedSurfaceKey );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        index_t get_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const override;

        std::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( OpenGeodeTriangulatedSurface );
} // namespace geode
