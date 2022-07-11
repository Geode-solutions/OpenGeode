/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/core/geode_regular_grid_surface.h>

#include <fstream>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/regular_grid_surface_builder.h>
#include <geode/mesh/core/private/grid_impl.h>
#include <geode/mesh/core/private/points_impl.h>
#include <geode/mesh/core/regular_grid_surface.h>

namespace
{
    static constexpr std::array< std::array< geode::local_index_t, 2 >, 4 >
        cell_vertices_translations{ { { 0, 0 }, { 1, 0 }, { 0, 1 },
            { 1, 1 } } };

    // -X +X -Y +Y -Z +Z
    static constexpr std::array< std::array< geode::local_index_t, 4 >, 6 >
        cell_facet_vertices{ { { 0, 2, 6, 4 }, { 1, 5, 7, 2 }, { 0, 4, 5, 1 },
            { 2, 2, 7, 6 }, { 0, 1, 2, 2 }, { 4, 6, 7, 5 } } };
} // namespace

namespace geode
{
    class OpenGeodeRegularGrid< 2 >::Impl : public detail::PointsImpl< 2 >,
                                            public detail::GridImpl< 2 >
    {
        friend class bitsery::Access;

    public:
        Impl( OpenGeodeRegularGrid< 2 >& mesh )
            : detail::PointsImpl< 2 >( mesh )
        {
        }

        index_t get_polygon_vertex( const RegularGrid2D& grid,
            const PolygonVertex& polygon_vertex ) const
        {
            auto cell_vertex = cell_indices( grid, polygon_vertex.polygon_id );
            for( const auto d : LRange{ 2 } )
            {
                cell_vertex[d] +=
                    cell_vertices_translations[polygon_vertex.vertex_id][d];
            }
            return vertex_index( grid, cell_vertex );
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::PointsImpl< 2 > >{} );
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::GridImpl< 2 > >{} );
                } );
        }
    };

    OpenGeodeRegularGrid< 2 >::OpenGeodeRegularGrid() : impl_( *this ) {}

    OpenGeodeRegularGrid< 2 >::OpenGeodeRegularGrid(
        OpenGeodeRegularGrid&& other )
        : RegularGrid< 2 >( std::move( other ) ),
          impl_( std::move( other.impl_ ) )
    {
    }

    OpenGeodeRegularGrid< 2 >::~OpenGeodeRegularGrid() // NOLINT
    {
    }

    const Point2D& OpenGeodeRegularGrid< 2 >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    const Point2D& OpenGeodeRegularGrid< 2 >::origin() const
    {
        return impl_->origin( *this );
    }

    index_t OpenGeodeRegularGrid< 2 >::cell_index(
        const GridCellIndices2D& index ) const
    {
        return impl_->cell_index( *this, index );
    }

    GridCellIndices2D OpenGeodeRegularGrid< 2 >::cell_indices(
        index_t index ) const
    {
        return impl_->cell_indices( *this, index );
    }

    index_t OpenGeodeRegularGrid< 2 >::vertex_index(
        const GridVertexIndices2D& index ) const
    {
        return impl_->vertex_index( *this, index );
    }

    GridVertexIndices2D OpenGeodeRegularGrid< 2 >::vertex_indices(
        index_t index ) const
    {
        return impl_->vertex_indices( *this, index );
    }

    index_t OpenGeodeRegularGrid< 2 >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( *this, polygon_vertex );
    }

    absl::optional< index_t > OpenGeodeRegularGrid< 2 >::get_polygon_adjacent(
        const PolygonEdge& edge ) const
    {
        return impl_->cell_adjacent( *this, edge.polygon_id, edge.edge_id );
    }

    void OpenGeodeRegularGrid< 2 >::update_origin(
        const Point2D& origin, OGRegularGridKey )
    {
        impl_->update_origin( *this, origin );
    }

    template < typename Archive >
    void OpenGeodeRegularGrid< 2 >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, OpenGeodeRegularGrid >{},
            []( Archive& a, OpenGeodeRegularGrid& point_set ) {
                a.ext(
                    point_set, bitsery::ext::BaseClass< RegularGrid< 2 > >{} );
                a.object( point_set.impl_ );
            } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeRegularGrid< 2 > );
} // namespace geode
