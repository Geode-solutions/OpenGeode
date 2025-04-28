/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <optional>

#include <geode/basic/common.hpp>
#include <geode/basic/pimpl.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    template < index_t dimension >
    class CellArray
    {
        OPENGEODE_DISABLE_COPY( CellArray );
        friend class bitsery::Access;

    public:
        static constexpr auto dim = dimension;
        using CellIndices = std::array< index_t, dimension >;

        virtual ~CellArray();

        [[nodiscard]] local_index_t nb_cell_neighbors() const
        {
            return nb_cell_neighbors_static();
        }

        [[nodiscard]] static constexpr local_index_t nb_cell_neighbors_static()
        {
            return 2 * dimension;
        }

        [[nodiscard]] index_t nb_cells() const;

        [[nodiscard]] index_t nb_cells_in_direction( index_t direction ) const;

        [[nodiscard]] virtual index_t cell_index(
            const CellIndices& index ) const = 0;

        [[nodiscard]] virtual CellIndices cell_indices(
            index_t index ) const = 0;

        [[nodiscard]] std::optional< CellIndices > next_cell(
            const CellIndices& index, index_t direction ) const;

        [[nodiscard]] std::optional< CellIndices > previous_cell(
            const CellIndices& index, index_t direction ) const;

        [[nodiscard]] bool is_cell_on_border(
            const CellIndices& cell_indices ) const;

    protected:
        CellArray();
        CellArray( std::array< index_t, dimension > cells_number );
        CellArray( CellArray&& other ) noexcept;
        CellArray& operator=( CellArray&& other ) noexcept;

        void set_array_dimensions(
            std::array< index_t, dimension > cells_number );

        void copy( const CellArray< dimension >& array );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( CellArray );
} // namespace geode
