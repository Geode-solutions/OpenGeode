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

#include <absl/container/inlined_vector.h>

#include <geode/basic/cell_array.hpp>
#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/common.hpp>

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

        std::optional< VertexIndices > next_vertex(
            const VertexIndices& index, index_t direction ) const;

        std::optional< VertexIndices > previous_vertex(
            const VertexIndices& index, index_t direction ) const;

        bool is_grid_vertex_on_border( const VertexIndices& index ) const;

        Point< dimension > cell_barycenter( const CellIndices& cell_id ) const;

        /*!
         * Return true if the query point is inside the grid, up to a
         * GLOBAL_EPSILON away from the grid bounding box.
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

        CellsAroundVertex cells_around( VertexIndices vertex_indices ) const;

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
        Grid( Grid&& other ) noexcept;
        Grid& operator=( Grid&& other ) noexcept;

    private:
        template < typename Archive >
        void serialize( Archive& archive );

        using CellArray< dimension >::set_array_dimensions;
        using CellArray< dimension >::copy;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Grid );

    namespace detail
    {
        static constexpr std::array< std::array< geode::local_index_t, 2 >, 12 >
            CELL_EDGE_VERTICES_3D{ { { 0, 1 }, { 0, 2 }, { 2, 3 }, { 1, 3 },
                { 4, 5 }, { 4, 6 }, { 6, 7 }, { 5, 7 }, { 0, 4 }, { 1, 5 },
                { 2, 6 }, { 3, 7 } } };

        static constexpr std::array< std::array< geode::local_index_t, 4 >, 6 >
            CELL_FACET_VERTICES_3D{ { { 0, 2, 6, 4 }, { 1, 5, 7, 3 },
                { 0, 4, 5, 1 }, { 2, 3, 7, 6 }, { 0, 1, 3, 2 },
                { 4, 6, 7, 5 } } };

        static constexpr std::array< std::array< geode::local_index_t, 3 >, 8 >
            CELL_EDGES_AROUND_VERTEX_3D{ { { 0, 1, 8 }, { 0, 3, 9 },
                { 1, 2, 10 }, { 2, 3, 11 }, { 4, 5, 8 }, { 4, 7, 9 },
                { 5, 6, 10 }, { 6, 7, 11 } } };

        static constexpr std::array< std::array< geode::local_index_t, 3 >, 8 >
            CELL_FACETS_AROUND_VERTEX_3D{ { { 0, 2, 4 }, { 1, 2, 4 },
                { 0, 3, 4 }, { 1, 3, 4 }, { 0, 2, 5 }, { 1, 2, 5 }, { 0, 3, 5 },
                { 1, 3, 5 } } };
    } // namespace detail
} // namespace geode
