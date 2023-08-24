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

#include <geode/basic/cell_array.h>
#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( GridBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    FORWARD_DECLARATION_DIMENSION_CLASS( CoordinateSystem );
    class AttributeManager;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class Grid : public CellArray< dimension >
    {
        OPENGEODE_DISABLE_COPY( Grid );
        PASSKEY( GridBuilder< dimension >, GridKey );
        friend class bitsery::Access;

    public:
        using Builder = GridBuilder< dimension >;
        static constexpr auto dim = dimension;
        using CellIndices = typename CellArray< dimension >::CellIndices;
        using VertexIndices = CellIndices;
        using CellVertices = std::array< VertexIndices, 1 << dimension >;
        using CellsAroundVertex =
            absl::InlinedVector< CellIndices, 1 << dimension >;

        ~Grid();

        OPENGEODE_MESH_DEPRECATED const Point< dimension >&
            origin() const; /* Access the origin through the grid
                                                        coordinate system*/

        const CoordinateSystem< dimension >& grid_coordinate_system() const;

        local_index_t nb_cell_vertices() const
        {
            return nb_cell_vertices_static();
        }

        static constexpr local_index_t nb_cell_vertices_static()
        {
            /* returns 1*pow(2,dimension) using binary operators (faster). */
            return 1 << dimension;
        }

        double cell_length_in_direction( index_t direction ) const;

        double cell_size() const;

        Point< dimension > grid_point( const VertexIndices& index ) const;

        index_t nb_grid_vertices() const;

        index_t nb_vertices_in_direction( index_t direction ) const;

        index_t nb_vertices_on_borders() const;

        virtual index_t vertex_index( const VertexIndices& index ) const = 0;

        virtual VertexIndices vertex_indices( index_t index ) const = 0;

        CellVertices cell_vertices( const CellIndices& cell_id ) const;

        VertexIndices cell_vertex_indices(
            const CellIndices& cell_id, local_index_t vertex_id ) const;

        local_index_t cell_local_vertex(
            const CellIndices& cell_id, const VertexIndices& vertex_id ) const;

        absl::optional< VertexIndices > next_vertex(
            const VertexIndices& index, index_t direction ) const;

        absl::optional< VertexIndices > previous_vertex(
            const VertexIndices& index, index_t direction ) const;

        bool is_grid_vertex_on_border( const VertexIndices& index ) const;

        Point< dimension > cell_barycenter( const CellIndices& cell_id ) const;

        /*!
         * Return true if the query point is inside the grid, up to a
         * global_epsilon away from the grid bounding box.
         */
        bool contains( const Point< dimension >& query ) const;

        /*!
         * Returns the closest grid vertex to the query point.
         */
        VertexIndices closest_vertex( const Point< dimension >& query ) const;

        /*!
         * Return the cell(s) containing the query point
         * @param[in] query Position of point
         * @return None, one or several cell index/indices that contain the
         * query point.
         * @detail When query point is geometrically near to cell limit, several
         * cell indices are returned: they correspond the potential cells that
         * may contain the point.
         */
        CellsAroundVertex cells( const Point< dimension >& query ) const;

        virtual AttributeManager& cell_attribute_manager() const = 0;

        virtual AttributeManager& grid_vertex_attribute_manager() const = 0;

        BoundingBox< dimension > grid_bounding_box() const;

    public:
        void set_grid_origin( Point< dimension > origin, GridKey );

        void set_grid_dimensions( std::array< index_t, dimension > cells_number,
            std::array< double, dimension > cells_length,
            GridKey );

        void set_grid_directions(
            std::array< Vector< dimension >, dimension > directions, GridKey );

        void copy( const Grid< dimension >& grid, GridKey );

    protected:
        Grid();
        Grid( Grid&& other );
        Grid& operator=( Grid&& other );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

        using CellArray< dimension >::set_array_dimensions;
        using CellArray< dimension >::copy;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Grid );
} // namespace geode
