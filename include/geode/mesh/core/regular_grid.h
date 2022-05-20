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

#pragma once

#include <absl/container/inlined_vector.h>
#include <absl/types/optional.h>

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    class AttributeManager;
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

    /*!
     * Interface class to represent regular grids.
     */
    template < index_t dimension >
    class RegularGrid
    {
        OPENGEODE_DISABLE_COPY( RegularGrid );
        friend class bitsery::Access;

    public:
        RegularGrid( Point< dimension > origin,
            std::array< index_t, dimension > cells_number,
            std::array< double, dimension > cells_length );

        RegularGrid( Point< dimension > origin,
            std::array< index_t, dimension > cells_number,
            double cells_length );

        RegularGrid( RegularGrid&& );

        ~RegularGrid();

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto ext = absl::StrCat( "og_rgd", dimension, "d" );
            return ext;
        }

        local_index_t nb_cell_vertices() const
        {
            return nb_cell_vertices_static();
        }

        static constexpr local_index_t nb_cell_vertices_static()
        {
            /* returns 1*pow(2,dimension) using binary operators (faster). */
            return 1 << dimension;
        }

        const Point< dimension >& origin() const;

        AttributeManager& cell_attribute_manager() const;

        AttributeManager& vertex_attribute_manager() const;

        index_t nb_cells() const;

        index_t nb_cells( index_t direction ) const;

        double cell_length( index_t direction ) const;

        double cell_size() const;

        index_t cell_index( const GridCellIndices< dimension >& index ) const;

        GridCellIndices< dimension > cell_indices( index_t index ) const;

        absl::optional< GridCellIndices< dimension > > next_cell(
            const GridCellIndices< dimension >& index,
            index_t direction ) const;

        absl::optional< GridCellIndices< dimension > > previous_cell(
            const GridCellIndices< dimension >& index,
            index_t direction ) const;

        bool is_cell_on_border(
            const GridCellIndices< dimension >& cell_indices ) const;

        index_t nb_vertices() const;

        index_t nb_vertices( index_t direction ) const;

        index_t nb_vertices_on_borders() const;

        index_t vertex_index(
            const GridVertexIndices< dimension >& index ) const;

        GridVertexIndices< dimension > vertex_indices( index_t index ) const;

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

        bool is_vertex_on_border(
            const GridVertexIndices< dimension >& vertex_index ) const;

        Point< dimension > point(
            const GridVertexIndices< dimension >& index ) const;

        Point< dimension > cell_barycenter(
            const GridCellIndices< dimension >& index ) const;

        /*!
         * Returns if the query point is inside the grid, up to a global_epsilon
         * away from the grid bounding box.
         */
        bool contains( const Point< dimension >& query ) const;

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

        /*!
         * Compute the bounding box of the grid
         */
        BoundingBox< dimension > bounding_box() const;

        RegularGrid< dimension > clone() const;

    private:
        RegularGrid();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( RegularGrid );
} // namespace geode
