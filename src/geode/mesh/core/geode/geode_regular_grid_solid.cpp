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

#include <geode/mesh/core/geode/geode_regular_grid_solid.h>

#include <fstream>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/core/private/grid_impl.h>
#include <geode/mesh/core/private/points_impl.h>
#include <geode/mesh/core/regular_grid_solid.h>

namespace
{
    static constexpr std::array< std::array< geode::local_index_t, 3 >, 8 >
        SOLID_CELL_VERTICES_TRANSLATIONS{ { { 0, 0, 0 }, { 1, 0, 0 },
            { 0, 1, 0 }, { 1, 1, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 0, 1, 1 },
            { 1, 1, 1 } } };

    // -X +X -Y +Y -Z +Z
    static constexpr std::array< std::array< geode::local_index_t, 4 >, 6 >
        SOLID_CELL_FACET_VERTICES{ { { 0, 2, 6, 4 }, { 1, 5, 7, 3 },
            { 0, 4, 5, 1 }, { 2, 3, 7, 6 }, { 0, 1, 3, 2 }, { 4, 6, 7, 5 } } };
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

        void update_origin( RegularGrid3D& grid, const Point3D& origin )
        {
            do_update_origin( grid, origin );
        }

        void update_origin_and_directions( RegularGrid3D& grid,
            const Point3D& origin,
            const std::array< Vector3D, 3 >& directions )
        {
            do_update_origin_and_directions( grid, origin, directions );
        }

        index_t get_polyhedron_vertex( const RegularGrid3D& grid,
            const PolyhedronVertex& polyhedron_vertex ) const
        {
            auto cell_vertex =
                cell_indices( grid, polyhedron_vertex.polyhedron_id );
            for( const auto d : LRange{ 3 } )
            {
                cell_vertex[d] +=
                    SOLID_CELL_VERTICES_TRANSLATIONS[polyhedron_vertex
                                                         .vertex_id][d];
            }
            return vertex_index( grid, cell_vertex );
        }

        PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            const auto& facet = polyhedron_facet_vertex.polyhedron_facet;
            const auto vertex =
                SOLID_CELL_FACET_VERTICES[facet.facet_id]
                                         [polyhedron_facet_vertex.vertex_id];
            return { facet.polyhedron_id, vertex };
        }

        absl::optional< index_t > cell_adjacent( const RegularGrid3D& grid,
            const PolyhedronFacet& polyhedron_facet ) const
        {
            const auto cell =
                cell_indices( grid, polyhedron_facet.polyhedron_id );
            const index_t direction =
                static_cast< index_t >( polyhedron_facet.facet_id / 2 );
            if( ( polyhedron_facet.facet_id & 1 ) == 0 /* modulo 2 */ )
            {
                if( const auto adj = grid.previous_cell( cell, direction ) )
                {
                    return grid.cell_index( adj.value() );
                }
            }
            else
            {
                if( const auto adj = grid.next_cell( cell, direction ) )
                {
                    return grid.cell_index( adj.value() );
                }
            }
            return absl::nullopt;
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::PointsImpl< 3 > >{} );
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::GridImpl< 3 > >{} );
                } } } );
        }
    };

    OpenGeodeRegularGrid< 3 >::OpenGeodeRegularGrid() : impl_( *this ) {}

    OpenGeodeRegularGrid< 3 >::OpenGeodeRegularGrid(
        OpenGeodeRegularGrid&& ) noexcept = default;

    OpenGeodeRegularGrid< 3 >& OpenGeodeRegularGrid< 3 >::operator=(
        OpenGeodeRegularGrid&& ) noexcept = default;

    OpenGeodeRegularGrid< 3 >::~OpenGeodeRegularGrid() = default;

    index_t OpenGeodeRegularGrid< 3 >::cell_index(
        const Grid3D::CellIndices& index ) const
    {
        return impl_->cell_index( *this, index );
    }

    Grid3D::CellIndices OpenGeodeRegularGrid< 3 >::cell_indices(
        index_t index ) const
    {
        return impl_->cell_indices( *this, index );
    }

    index_t OpenGeodeRegularGrid< 3 >::vertex_index(
        const Grid3D::VertexIndices& index ) const
    {
        return impl_->vertex_index( *this, index );
    }

    Grid3D::VertexIndices OpenGeodeRegularGrid< 3 >::vertex_indices(
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
        return impl_->cell_adjacent( *this, polyhedron_facet );
    }

    void OpenGeodeRegularGrid< 3 >::update_origin(
        const Point3D& origin, OGRegularGridKey )
    {
        impl_->update_origin( *this, origin );
    }

    void OpenGeodeRegularGrid< 3 >::update_origin_and_directions(
        const Point3D& origin,
        const std::array< Vector3D, 3 >& directions,
        OGRegularGridKey )
    {
        impl_->update_origin_and_directions( *this, origin, directions );
    }

    template < typename Archive >
    void OpenGeodeRegularGrid< 3 >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OpenGeodeRegularGrid >{
                { []( Archive& a, OpenGeodeRegularGrid& grid ) {
                     a.ext(
                         grid, bitsery::ext::BaseClass< RegularGrid< 3 > >{} );
                     a.object( grid.impl_ );
                     grid.impl_->initialize_crs( grid );
                 },
                    []( Archive& a, OpenGeodeRegularGrid& grid ) {
                        a.ext( grid,
                            bitsery::ext::BaseClass< RegularGrid< 3 > >{} );
                        a.object( grid.impl_ );
                    } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeRegularGrid< 3 > );
} // namespace geode
