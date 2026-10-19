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
            return flat_index( array.cell_strides(), index );
        }

        [[nodiscard]] CellIndices cell_indices(
            const CellArray< dimension >& array, index_t index ) const
        {
            OpenGeodeBasicException::check_assertion( index < array.nb_cells(),
                "[CellArray::cell_index] Invalid index" );
            return multi_index( array.cell_strides(), index );
        }

    protected:
        [[nodiscard]] static const std::array< index_t, dimension >&
            vertex_strides( const CellArray< dimension >& array )
        {
            return array.vertex_strides();
        }

        [[nodiscard]] static index_t flat_index(
            const std::array< index_t, dimension >& strides,
            const CellIndices& index )
        {
            index_t result{ 0 };
            for( const auto d : LRange{ dimension } )
            {
                result += index[d] * strides[d];
            }
            return result;
        }

        [[nodiscard]] static CellIndices multi_index(
            const std::array< index_t, dimension >& strides, index_t index )
        {
            CellIndices result;
            for( local_index_t d = dimension; d-- > 0; )
            {
                result[d] = index / strides[d];
                index -= result[d] * strides[d];
            }
            return result;
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
