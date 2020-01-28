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

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodePolygonalSurfaceBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class OpenGeodePolygonalSurface : public PolygonalSurface< dimension >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( OpenGeodePolygonalSurface );
        friend class OpenGeodePolygonalSurfaceBuilder< dimension >;

    public:
        OpenGeodePolygonalSurface();
        ~OpenGeodePolygonalSurface();

        static MeshType type_name_static()
        {
            return MeshType{ absl::StrCat(
                "OpenGeodePolygonalSurface", dimension, "D" ) };
        }

        MeshType type_name() const override
        {
            return type_name_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_psf", dimension, "d" );
            return extension;
        }

        absl::string_view native_extension() const override
        {
            return native_extension_static();
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        const Point< dimension >& get_point( index_t vertex_id ) const override;

        void set_vertex( index_t vertex_id, const Point< dimension >& point );

        index_t get_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const override;

        index_t get_nb_polygon_vertices( index_t polygon_id ) const override;

        index_t get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const override;

        void set_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id );

        void set_polygon_adjacent(
            const PolygonEdge& polygon_edge, index_t adjacent_id );

        void add_polygon( const std::vector< index_t >& vertices );

        void remove_polygons( const std::vector< bool >& to_delete );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( OpenGeodePolygonalSurface );
} // namespace geode
