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

#include <geode/mesh/core/geode/geode_regular_grid_surface.hpp>

#include <fstream>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/regular_grid_surface_builder.hpp>
#include <geode/mesh/core/internal/grid_impl.hpp>
#include <geode/mesh/core/internal/points_impl.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>

namespace
{
    static constexpr std::array< std::array< geode::local_index_t, 2 >, 4 >
        SURFACE_CELL_VERTICES_TRANSLATIONS{ { { 0, 0 }, { 1, 0 }, { 1, 1 },
            { 0, 1 } } };
    static constexpr std::array< std::pair< bool, geode::index_t >, 4 >
        SURFACE_CELL_ADJACENT_DIRETIONS{ { { false, 1 }, { true, 0 },
            { true, 1 }, { false, 0 } } };
} // namespace

namespace geode
{
    class OpenGeodeRegularGrid< 2 >::Impl : public internal::PointsImpl< 2 >,
                                            public internal::GridImpl< 2 >
    {
        friend class bitsery::Access;

    public:
        Impl( OpenGeodeRegularGrid< 2 >& mesh )
            : internal::PointsImpl< 2 >( mesh )
        {
        }

        void update_origin( RegularGrid2D& grid, const Point2D& origin )
        {
            do_update_origin( grid, origin );
        }

        void update_origin_and_directions( RegularGrid2D& grid,
            const Point2D& origin,
            const std::array< Vector2D, 2 >& directions )
        {
            do_update_origin_and_directions( grid, origin, directions );
        }

        index_t get_polygon_vertex( const RegularGrid2D& grid,
            const PolygonVertex& polygon_vertex ) const
        {
            auto cell_vertex = cell_indices( grid, polygon_vertex.polygon_id );
            for( const auto d : LRange{ 2 } )
            {
                cell_vertex[d] +=
                    SURFACE_CELL_VERTICES_TRANSLATIONS[polygon_vertex.vertex_id]
                                                      [d];
            }
            return vertex_index( grid, cell_vertex );
        }

        std::optional< index_t > cell_adjacent(
            const RegularGrid2D& grid, const PolygonEdge& edge ) const
        {
            const auto cell = cell_indices( grid, edge.polygon_id );
            const auto& direction =
                SURFACE_CELL_ADJACENT_DIRETIONS[edge.edge_id];
            if( direction.first )
            {
                if( const auto adj = grid.next_cell( cell, direction.second ) )
                {
                    return grid.cell_index( adj.value() );
                }
            }
            else
            {
                if( const auto adj =
                        grid.previous_cell( cell, direction.second ) )
                {
                    return grid.cell_index( adj.value() );
                }
            }
            return std::nullopt;
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{ { []( Archive& a,
                                                                 Impl& impl ) {
                a.ext( impl,
                    bitsery::ext::BaseClass< internal::PointsImpl< 2 > >{} );
                a.ext( impl,
                    bitsery::ext::BaseClass< internal::GridImpl< 2 > >{} );
            } } } );
        }
    };

    OpenGeodeRegularGrid< 2 >::OpenGeodeRegularGrid() : impl_( *this ) {}

    OpenGeodeRegularGrid< 2 >::OpenGeodeRegularGrid(
        OpenGeodeRegularGrid&& ) noexcept = default;

    OpenGeodeRegularGrid< 2 >& OpenGeodeRegularGrid< 2 >::operator=(
        OpenGeodeRegularGrid&& ) noexcept = default;

    OpenGeodeRegularGrid< 2 >::~OpenGeodeRegularGrid() = default;

    index_t OpenGeodeRegularGrid< 2 >::cell_index(
        const Grid2D::CellIndices& index ) const
    {
        return impl_->cell_index( *this, index );
    }

    Grid2D::CellIndices OpenGeodeRegularGrid< 2 >::cell_indices(
        index_t index ) const
    {
        return impl_->cell_indices( *this, index );
    }

    index_t OpenGeodeRegularGrid< 2 >::vertex_index(
        const Grid2D::VertexIndices& index ) const
    {
        return impl_->vertex_index( *this, index );
    }

    Grid2D::VertexIndices OpenGeodeRegularGrid< 2 >::vertex_indices(
        index_t index ) const
    {
        return impl_->vertex_indices( *this, index );
    }

    index_t OpenGeodeRegularGrid< 2 >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( *this, polygon_vertex );
    }

    std::optional< index_t > OpenGeodeRegularGrid< 2 >::get_polygon_adjacent(
        const PolygonEdge& edge ) const
    {
        return impl_->cell_adjacent( *this, edge );
    }

    void OpenGeodeRegularGrid< 2 >::update_origin(
        const Point2D& origin, OGRegularGridKey )
    {
        impl_->update_origin( *this, origin );
    }

    void OpenGeodeRegularGrid< 2 >::update_origin_and_directions(
        const Point2D& origin,
        const std::array< Vector2D, 2 >& directions,
        OGRegularGridKey )
    {
        impl_->update_origin_and_directions( *this, origin, directions );
    }

    template < typename Archive >
    void OpenGeodeRegularGrid< 2 >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OpenGeodeRegularGrid >{
                { []( Archive& a, OpenGeodeRegularGrid& grid ) {
                     a.ext(
                         grid, bitsery::ext::BaseClass< RegularGrid< 2 > >{} );
                     a.object( grid.impl_ );
                     grid.impl_->initialize_crs( grid );
                 },
                    []( Archive& a, OpenGeodeRegularGrid& grid ) {
                        a.ext( grid,
                            bitsery::ext::BaseClass< RegularGrid< 2 > >{} );
                        a.object( grid.impl_ );
                    } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeRegularGrid< 2 > );
} // namespace geode
