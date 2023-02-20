/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <absl/container/inlined_vector.h>
#include <absl/types/optional.h>

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( GridBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    using GridCellIndices = std::array< index_t, dimension >;
    ALIAS_2D_AND_3D( GridCellIndices );

    template < index_t dimension >
    using GridVertexIndices = std::array< index_t, dimension >;
    ALIAS_2D_AND_3D( GridVertexIndices );

    template < index_t dimension >
    using GridCellVertices =
        std::array< GridVertexIndices< dimension >, 1 << dimension >;
    ALIAS_2D_AND_3D( GridCellVertices );

    template < index_t dimension >
    using GridCellsAroundVertex =
        absl::InlinedVector< GridCellIndices< dimension >, 1 << dimension >;
    ALIAS_2D_AND_3D( GridCellsAroundVertex );

    template < index_t dimension >
    class Grid
    {
        OPENGEODE_DISABLE_COPY( Grid );
        PASSKEY( GridBuilder< dimension >, GridKey );
        friend class bitsery::Access;

    public:
        using Builder = GridBuilder< dimension >;
        static constexpr auto dim = dimension;

        Grid();

        Grid( Grid&& );

        virtual ~Grid();

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto ext = absl::StrCat( "og_rgd", dimension, "d" );
            return ext;
        }

        virtual const Point< dimension >& origin() const = 0;

        local_index_t nb_cell_vertices() const
        {
            return nb_cell_vertices_static();
        }

        static constexpr local_index_t nb_cell_vertices_static()
        {
            /* returns 1*pow(2,dimension) using binary operators (faster). */
            return 1 << dimension;
        }

        local_index_t nb_cell_facets() const
        {
            return nb_cell_facets_static();
        }

        static constexpr local_index_t nb_cell_facets_static()
        {
            return 2 * dimension;
        }

        index_t nb_cells() const;

        index_t nb_cells_in_direction( index_t direction ) const;

        double cell_length_in_direction( index_t direction ) const;

        double cell_size() const;

        virtual index_t cell_index(
            const GridCellIndices< dimension >& index ) const = 0;

        virtual GridCellIndices< dimension > cell_indices(
            index_t index ) const = 0;

        absl::optional< GridCellIndices< dimension > > next_cell(
            const GridCellIndices< dimension >& index,
            index_t direction ) const;

        absl::optional< GridCellIndices< dimension > > previous_cell(
            const GridCellIndices< dimension >& index,
            index_t direction ) const;

        bool is_cell_on_border(
            const GridCellIndices< dimension >& cell_indices ) const;

        index_t nb_vertices_in_direction( index_t direction ) const;

        index_t nb_vertices_on_borders() const;

        virtual index_t vertex_index(
            const GridVertexIndices< dimension >& index ) const = 0;

        virtual GridVertexIndices< dimension > vertex_indices(
            index_t index ) const = 0;

        GridCellVertices< dimension > cell_vertices(
            const GridCellIndices< dimension >& cell_id ) const;

        GridVertexIndices< dimension > cell_vertex_indices(
            const GridCellIndices< dimension >& cell_id,
            local_index_t vertex_id ) const;

        absl::optional< GridVertexIndices< dimension > > next_vertex(
            const GridVertexIndices< dimension >& index,
            index_t direction ) const;

        absl::optional< GridVertexIndices< dimension > > previous_vertex(
            const GridVertexIndices< dimension >& index,
            index_t direction ) const;

        /*!
         * Return true if the query point is inside the grid, up to a
         * global_epsilon away from the grid bounding box.
         */
        bool contains( const Point< dimension >& query ) const;

        /*!
         * Returns the closest grid vertex to the query point.
         */
        GridVertexIndices< dimension > closest_vertex(
            const Point< dimension >& query ) const;

        /*!
         * Return the cell(s) containing the query point
         * @param[in] query Position of point
         * @return No, one index or several cell indices that contain the query
         * point.
         * @detail When query point is geometrically near to cell limit, several
         * cell indices are returned: they correspond the potential cells that
         * may contain the point.
         */
        GridCellsAroundVertex< dimension > cells(
            const Point< dimension >& query ) const;

    public:
        void set_grid_dimensions( std::array< index_t, dimension > cells_number,
            std::array< double, dimension > cells_length,
            GridKey );

        void copy( const Grid< dimension >& grid, GridKey );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Grid );
} // namespace geode
