/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/growable.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/internal/points_impl.hpp>

namespace geode::internal
{
    template < index_t dimension >
    class ArrayImpl
    {
        friend class bitsery::Access;
        using CellIndices = typename CellArray< dimension >::CellIndices;

    public:
        [[nodiscard]] index_t cell_index( const CellArray< dimension >& array,
            const CellIndices& index ) const
        {
            index_t cell_id{ index[0] };
            index_t offset{ 1 };
            for( const auto d : LRange{ 1, dimension } )
            {
                offset *= array.nb_cells_in_direction( d - 1 );
                cell_id += index[d] * offset;
            }
            return cell_id;
        }

        [[nodiscard]] CellIndices cell_indices(
            const CellArray< dimension >& array, index_t index ) const
        {
            OpenGeodeBasicException::check_assertion( index < array.nb_cells(),
                "[CellArray::cell_index] Invalid index" );
            CellIndices cell_id;
            for( const auto d : LRange{ dimension } )
            {
                const auto nb_cells = array.nb_cells_in_direction( d );
                cell_id[d] = index % nb_cells;
                index /= nb_cells;
            }
            return cell_id;
        }

    private:
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, ArrayImpl >{
                    { []( Archive& /*unused*/, ArrayImpl& /*unused*/ ) {} } } );
        }
    };
} // namespace geode::internal
