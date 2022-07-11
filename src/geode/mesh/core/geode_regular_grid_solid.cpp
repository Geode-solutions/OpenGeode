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

#include <geode/mesh/core/geode_regular_grid_solid.h>

#include <fstream>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/core/private/grid_impl.h>
#include <geode/mesh/core/private/points_impl.h>
#include <geode/mesh/core/regular_grid_solid.h>

namespace
{
    static constexpr std::array< std::array< geode::local_index_t, 3 >, 8 >
        cell_vertices_translations{ { { 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 },
            { 1, 1, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 } } };

    // -X +X -Y +Y -Z +Z
    static constexpr std::array< std::array< geode::local_index_t, 4 >, 6 >
        cell_facet_vertices{ { { 0, 2, 6, 4 }, { 1, 5, 7, 3 }, { 0, 4, 5, 1 },
            { 2, 3, 7, 6 }, { 0, 1, 3, 2 }, { 4, 6, 7, 5 } } };
} // namespace

namespace geode
{
    class OpenGeodeRegularGrid< 3 >::Impl : public detail::PointsImpl< 3 >,
                                            public detail::GridImpl< 3 >
    {
        friend class bitsery::Access;

    public:
        Impl( OpenGeodeRegularGrid< 3 >& mesh )
            : detail::PointsImpl< 3 >( mesh )
        {
        }

        index_t get_polyhedron_vertex( const RegularGrid3D& grid,
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            auto cell_vertex =
                cell_indices( grid, polyhedron_vertex.polyhedron_id );
            for( const auto d : LRange{ 3 } )
            {
                cell_vertex[d] +=
                    cell_vertices_translations[polyhedron_vertex.vertex_id][d];
            }
            return vertex_index( grid, cell_vertex );
        }

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            const auto& facet = polyhedron_facet_vertex.polyhedron_facet;
            const auto vertex =
                cell_facet_vertices[facet.polyhedron_id]
                                   [polyhedron_facet_vertex.vertex_id];
            return { facet.polyhedron_id, vertex };
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::PointsImpl< 3 > >{} );
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::GridImpl< 3 > >{} );
                } );
        }
    };

    OpenGeodeRegularGrid< 3 >::OpenGeodeRegularGrid() : impl_( *this ) {}

    OpenGeodeRegularGrid< 3 >::OpenGeodeRegularGrid(
        OpenGeodeRegularGrid&& other )
        : RegularGrid< 3 >( std::move( other ) ),
          impl_( std::move( other.impl_ ) )
    {
    }

    OpenGeodeRegularGrid< 3 >::~OpenGeodeRegularGrid() // NOLINT
    {
    }

    const Point3D& OpenGeodeRegularGrid< 3 >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    const Point3D& OpenGeodeRegularGrid< 3 >::origin() const
    {
        return impl_->origin( *this );
    }

    index_t OpenGeodeRegularGrid< 3 >::cell_index(
        const GridCellIndices3D& index ) const
    {
        return impl_->cell_index( *this, index );
    }

    GridCellIndices3D OpenGeodeRegularGrid< 3 >::cell_indices(
        index_t index ) const
    {
        return impl_->cell_indices( *this, index );
    }

    index_t OpenGeodeRegularGrid< 3 >::vertex_index(
        const GridVertexIndices3D& index ) const
    {
        return impl_->vertex_index( *this, index );
    }

    GridVertexIndices3D OpenGeodeRegularGrid< 3 >::vertex_indices(
        index_t index ) const
    {
        return impl_->vertex_indices( *this, index );
    }

    index_t OpenGeodeRegularGrid< 3 >::get_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        return impl_->get_polyhedron_vertex( *this, polyhedron_vertex );
    }

    PolyhedronVertex OpenGeodeRegularGrid< 3 >::get_polyhedron_facet_vertex_id(
        const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return impl_->get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
    }

    absl::optional< index_t >
        OpenGeodeRegularGrid< 3 >::get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->cell_adjacent(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
    }

    void OpenGeodeRegularGrid< 3 >::update_origin(
        const Point3D& origin, OGRegularGridKey )
    {
        impl_->update_origin( *this, origin );
    }

    template < typename Archive >
    void OpenGeodeRegularGrid< 3 >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, OpenGeodeRegularGrid >{},
            []( Archive& a, OpenGeodeRegularGrid& point_set ) {
                a.ext(
                    point_set, bitsery::ext::BaseClass< RegularGrid< 3 > >{} );
                a.object( point_set.impl_ );
            } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeRegularGrid< 3 > );
} // namespace geode
