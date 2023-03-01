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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/private/points_impl.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class ArrayImpl
        {
            friend class bitsery::Access;
            using CellIndices = typename CellArray< dimension >::CellIndices;

        public:
            index_t cell_index( const CellArray< dimension >& array,
                const CellIndices& index ) const
            {
                const auto nb_u = array.nb_cells_in_direction( 0 );
                auto cell_id = index[0] + index[1] * nb_u;
                if( dimension == 3 )
                {
                    cell_id +=
                        index[2] * nb_u * array.nb_cells_in_direction( 1 );
                }
                return cell_id;
            }

            CellIndices cell_indices(
                const CellArray< dimension >& array, index_t index ) const
            {
                OPENGEODE_ASSERT( index < array.nb_cells(),
                    "[CellArray::cell_index] Invalid index" );
                CellIndices cell_id;
                for( const auto d : LRange{ dimension } )
                {
                    index_t offset{ 1 };
                    for( const auto d2 : LRange{ dimension - d - 1 } )
                    {
                        offset *= array.nb_cells_in_direction( d2 );
                    }
                    const auto value =
                        static_cast< index_t >( std::floor( index / offset ) );
                    cell_id[dimension - d - 1] = value;
                    index -= value * offset;
                }
                return cell_id;
            }

        private:
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, Growable< Archive, ArrayImpl >{
                                        { []( Archive& /*unused*/,
                                              ArrayImpl& /*unused*/ ) {} } } );
            }
        };
    } // namespace detail
} // namespace geode
