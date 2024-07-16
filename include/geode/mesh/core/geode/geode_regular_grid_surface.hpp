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

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeRegularGridBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeRegularGrid );
    ALIAS_2D( Point );
} // namespace geode

namespace geode
{
    template <>
    class opengeode_mesh_api OpenGeodeRegularGrid< 2 > : public RegularGrid< 2 >
    {
        OPENGEODE_DISABLE_COPY( OpenGeodeRegularGrid );
        PASSKEY( OpenGeodeRegularGridBuilder< 2 >, OGRegularGridKey );

    public:
        using Builder = OpenGeodeRegularGridBuilder< 2 >;
        static constexpr index_t dim{ 2 };

        OpenGeodeRegularGrid();
        OpenGeodeRegularGrid( OpenGeodeRegularGrid&& other ) noexcept;
        OpenGeodeRegularGrid& operator=(
            OpenGeodeRegularGrid&& other ) noexcept;
        ~OpenGeodeRegularGrid();

        static MeshImpl impl_name_static()
        {
            return MeshImpl{ "OpenGeodeRegularGrid2D" };
        }

        MeshImpl impl_name() const override
        {
            return impl_name_static();
        }

        MeshType type_name() const override
        {
            return RegularGrid< 2 >::type_name_static();
        }

        static std::string_view native_extension_static()
        {
            static const auto extension = "og_rgd2d";
            return extension;
        }

        std::string_view native_extension() const override
        {
            return native_extension_static();
        }

    public:
        void update_origin( const Point2D& origin, OGRegularGridKey );

        void update_origin_and_directions( const Point2D& origin,
            const std::array< Vector2D, 2 >& directions,
            OGRegularGridKey );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        index_t get_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const override;

        std::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const override;

        index_t cell_index( const Grid2D::CellIndices& index ) const override;

        Grid2D::CellIndices cell_indices( index_t index ) const override;

        index_t vertex_index(
            const Grid2D::VertexIndices& index ) const override;

        Grid2D::VertexIndices vertex_indices( index_t index ) const override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D( OpenGeodeRegularGrid );
} // namespace geode
