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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>

#include <geode/geometry/point.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class GridImpl
        {
            friend class bitsery::Access;

        public:
            const Point< dimension >& origin(
                const RegularGrid< dimension >& grid ) const
            {
                return grid.point( 0 );
            }

            index_t cell_index( const RegularGrid< dimension >& grid,
                const GridCellIndices< dimension >& index ) const
            {
                index_t cell_id{ 0 };
                for( const auto d : LRange{ dimension } )
                {
                    OPENGEODE_ASSERT(
                        index[d] < grid.nb_cells_in_direction( d ),
                        "[RegularGrid::cell_index] Invalid index" );

                    index_t offset{ 1 };
                    for( const auto d2 : LRange{ d } )
                    {
                        offset *= grid.nb_cells_in_direction( d2 );
                    }
                    cell_id += offset * index[d];
                }
                return cell_id;
            }

            GridCellIndices< dimension > cell_indices(
                const RegularGrid< dimension >& grid, index_t index ) const
            {
                OPENGEODE_ASSERT( index < grid.nb_cells(),
                    "[RegularGrid::cell_index] Invalid index" );
                GridCellIndices< dimension > cell_id;
                for( const auto d : LRange{ dimension } )
                {
                    index_t offset{ 1 };
                    for( const auto d2 : LRange{ dimension - d - 1 } )
                    {
                        offset *= grid.nb_cells_in_direction( d2 );
                    }
                    const auto value =
                        static_cast< index_t >( std::floor( index / offset ) );
                    cell_id[dimension - d - 1] = value;
                    index -= value * offset;
                }
                return cell_id;
            }

            index_t vertex_index( const RegularGrid< dimension >& grid,
                const GridVertexIndices< dimension >& index ) const
            {
                index_t vertex_id{ 0 };
                for( const auto d : LRange{ dimension } )
                {
                    OPENGEODE_ASSERT(
                        index[d] < grid.nb_vertices_in_direction( d ),
                        "[RegularGrid::vertex_index] Invalid index" );

                    index_t offset{ 1 };
                    for( const auto d2 : LRange{ d } )
                    {
                        offset *= grid.nb_vertices_in_direction( d2 );
                    }
                    vertex_id += offset * index[d];
                }
                return vertex_id;
            }

            GridVertexIndices< dimension > vertex_indices(
                const RegularGrid< dimension >& grid, index_t index ) const
            {
                OPENGEODE_ASSERT( index < grid.nb_vertices(),
                    "[RegularGrid::vertex_index] Invalid index" );
                GridVertexIndices< dimension > vertex_id;
                for( const auto d : LRange{ dimension } )
                {
                    index_t offset{ 1 };
                    for( const auto d2 : LRange{ dimension - d - 1 } )
                    {
                        offset *= grid.nb_vertices_in_direction( d2 );
                    }
                    const auto value =
                        static_cast< index_t >( std::floor( index / offset ) );
                    vertex_id[dimension - d - 1] = value;
                    index -= value * offset;
                }
                return vertex_id;
            }

        private:
            template < typename Archive >
            void serialize( Archive& archive )
            {
                // archive.ext( *this, DefaultGrowable< Archive, GridImpl >{},
                //     []( Archive& a, GridImpl& impl ) {
                //         a.ext( impl.points_, bitsery::ext::StdSmartPtr{} );
                //     } );
            }
        };
    } // namespace detail
} // namespace geode
