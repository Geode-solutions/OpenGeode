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

#include <async++.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/internal/array_impl.hpp>

#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/internal/points_impl.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>

namespace geode
{
    namespace internal
    {
        template < index_t dimension >
        class GridImpl : public ArrayImpl< dimension >
        {
            friend class bitsery::Access;
            using VertexIndices = typename Grid< dimension >::VertexIndices;

        public:
            [[nodiscard]] index_t vertex_index( const Grid< dimension >& grid,
                const VertexIndices& index ) const
            {
                index_t vertex_id{ 0 };
                for( const auto d : LRange{ dimension } )
                {
                    OpenGeodeMeshException::check_assertion(
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

            [[nodiscard]] VertexIndices vertex_indices(
                const Grid< dimension >& grid, index_t index ) const
            {
                OpenGeodeMeshException::check_assertion(
                    index < grid.nb_grid_vertices(),
                    "[RegularGrid::vertex_index] Invalid index" );
                VertexIndices vertex_id;
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

        protected:
            void do_update_origin( RegularGrid< dimension >& grid,
                const Point< dimension >& origin )
            {
                const auto& coordinate_system = grid.grid_coordinate_system();
                std::array< Vector< dimension >, dimension > directions;
                for( const auto d : LRange{ dimension } )
                {
                    directions[d] = coordinate_system.direction( d );
                }
                do_update_origin_and_directions( grid, origin, directions );
            }

            void do_update_origin_and_directions(
                RegularGrid< dimension >& grid,
                const Point< dimension >& origin,
                const std::array< Vector< dimension >, dimension >& directions )
            {
                static constexpr index_t MIN_VERTICES_PER_TASK{ 16384 };
                const auto nu = grid.nb_vertices_in_direction( 0 );
                const auto nb_rows = grid.nb_grid_vertices() / nu;
                const auto rows_per_task =
                    std::max( index_t{ 1 }, MIN_VERTICES_PER_TASK / nu );
                const auto nb_tasks =
                    ( nb_rows + rows_per_task - 1 ) / rows_per_task;
                CoordinateReferenceSystemManagersBuilder< dimension > builder{
                    grid
                };
                absl::FixedArray< async::task< void > > tasks( nb_tasks );
                for( const auto task_id : Range{ nb_tasks } )
                {
                    const auto first_row = task_id * rows_per_task;
                    const auto last_row =
                        std::min( first_row + rows_per_task, nb_rows );
                    tasks[task_id] = async::spawn( [&builder, &grid, &origin,
                                                       &directions, first_row,
                                                       last_row, nu] {
                        for( const auto row : Range{ first_row, last_row } )
                        {
                            VertexIndices indices;
                            auto remaining_row = row;
                            for( const auto d : LRange{ 1, dimension } )
                            {
                                const auto nb_vertices =
                                    grid.nb_vertices_in_direction( d );
                                indices[d] = remaining_row % nb_vertices;
                                remaining_row /= nb_vertices;
                            }
                            for( const auto u : Range{ nu } )
                            {
                                auto point = origin + directions[0] * u;
                                for( const auto d : LRange{ 1, dimension } )
                                {
                                    point = point + directions[d] * indices[d];
                                }
                                builder.set_point( u + row * nu, point );
                            }
                        }
                    } );
                }
                for( auto& task : async::when_all( tasks ).get() )
                {
                    task.get();
                }
            }

        private:
            template < typename Archive >
            void serialize( Archive& serializer )
            {
                serializer.ext( *this,
                    Growable< Archive, GridImpl >{
                        { []( Archive& /*unused*/, GridImpl& /*unused*/ ) {},
                            []( Archive& archive, GridImpl& grid ) {
                                archive.ext(
                                    grid, bitsery::ext::BaseClass<
                                              ArrayImpl< dimension > >{} );
                            } } } );
            }
        };

    } // namespace internal
} // namespace geode
