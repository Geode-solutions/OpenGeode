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

#include <async++.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/internal/array_impl.hpp>

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
            index_t vertex_index( const Grid< dimension >& grid,
                const VertexIndices& index ) const
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

            VertexIndices vertex_indices(
                const Grid< dimension >& grid, index_t index ) const
            {
                OPENGEODE_ASSERT( index < grid.nb_grid_vertices(),
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
                const Point< dimension >& origin );

            void do_update_origin_and_directions(
                RegularGrid< dimension >& grid,
                const Point< dimension >& origin,
                const std::array< Vector< dimension >, dimension >&
                    directions );

        private:
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    Growable< Archive, GridImpl >{
                        { []( Archive& /*unused*/, GridImpl& /*unused*/ ) {},
                            []( Archive& a, GridImpl& grid ) {
                                a.ext( grid, bitsery::ext::BaseClass<
                                                 ArrayImpl< dimension > >{} );
                            } } } );
            }
        };

        template <>
        inline void GridImpl< 2 >::do_update_origin(
            RegularGrid< 2 >& grid, const Point< 2 >& origin )
        {
            const auto du = grid.cell_length_in_direction( 0 );
            const auto dv = grid.cell_length_in_direction( 1 );
            const auto nu = grid.nb_vertices_in_direction( 0 );
            const auto nv = grid.nb_vertices_in_direction( 1 );
            index_t task_id{ 0 };
            CoordinateReferenceSystemManagersBuilder2D builder{ grid };
            absl::FixedArray< async::task< void > > tasks( nv );
            for( const auto v : Range{ nv } )
            {
                tasks[task_id++] =
                    async::spawn( [&builder, &origin, v, du, dv, nu, nv] {
                        for( const auto u : Range{ nu } )
                        {
                            const auto vertex = u + v * nu;
                            const Point2D translation{ { u * du, v * dv } };
                            builder.set_point( vertex, origin + translation );
                        }
                    } );
            }
            for( auto& task : async::when_all( tasks ).get() )
            {
                task.get();
            }
        }

        template <>
        inline void GridImpl< 2 >::do_update_origin_and_directions(
            RegularGrid< 2 >& grid,
            const Point< 2 >& origin,
            const std::array< Vector2D, 2 >& directions )
        {
            const auto nu = grid.nb_vertices_in_direction( 0 );
            const auto nv = grid.nb_vertices_in_direction( 1 );
            const auto& vector_u = directions[0];
            const auto& vector_v = directions[1];
            index_t task_id{ 0 };
            CoordinateReferenceSystemManagersBuilder2D builder{ grid };
            absl::FixedArray< async::task< void > > tasks( nv );
            for( const auto v : Range{ nv } )
            {
                tasks[task_id++] = async::spawn(
                    [&builder, &origin, v, &vector_u, &vector_v, nu, nv] {
                        for( const auto u : Range{ nu } )
                        {
                            const auto vertex = u + v * nu;
                            builder.set_point(
                                vertex, origin + vector_u * u + vector_v * v );
                        }
                    } );
            }
            for( auto& task : async::when_all( tasks ).get() )
            {
                task.get();
            }
        }

        template <>
        inline void GridImpl< 3 >::do_update_origin(
            RegularGrid< 3 >& grid, const Point< 3 >& origin )
        {
            const auto du = grid.cell_length_in_direction( 0 );
            const auto dv = grid.cell_length_in_direction( 1 );
            const auto dw = grid.cell_length_in_direction( 2 );
            const auto nu = grid.nb_vertices_in_direction( 0 );
            const auto nv = grid.nb_vertices_in_direction( 1 );
            const auto nw = grid.nb_vertices_in_direction( 2 );
            index_t task_id{ 0 };
            CoordinateReferenceSystemManagersBuilder3D builder{ grid };
            absl::FixedArray< async::task< void > > tasks( nw * nv );
            for( const auto w : Range{ nw } )
            {
                for( const auto v : Range{ nv } )
                {
                    tasks[task_id++] = async::spawn(
                        [&builder, &origin, v, w, du, dv, dw, nu, nv, nw] {
                            for( const auto u : Range{ nu } )
                            {
                                const auto vertex = u + v * nu + w * nu * nv;
                                const Point3D translation{ { u * du, v * dv,
                                    w * dw } };
                                builder.set_point(
                                    vertex, origin + translation );
                            }
                        } );
                }
            }
            for( auto& task : async::when_all( tasks ).get() )
            {
                task.get();
            }
        }

        template <>
        inline void GridImpl< 3 >::do_update_origin_and_directions(
            RegularGrid< 3 >& grid,
            const Point< 3 >& origin,
            const std::array< Vector3D, 3 >& directions )
        {
            const auto nu = grid.nb_vertices_in_direction( 0 );
            const auto nv = grid.nb_vertices_in_direction( 1 );
            const auto nw = grid.nb_vertices_in_direction( 2 );
            const auto& vector_u = directions[0];
            const auto& vector_v = directions[1];
            const auto& vector_w = directions[2];
            index_t task_id{ 0 };
            CoordinateReferenceSystemManagersBuilder3D builder{ grid };
            absl::FixedArray< async::task< void > > tasks( nw * nv );
            for( const auto w : Range{ nw } )
            {
                for( const auto v : Range{ nv } )
                {
                    tasks[task_id++] =
                        async::spawn( [&builder, &origin, v, w, &vector_u,
                                          &vector_v, &vector_w, nu, nv, nw] {
                            for( const auto u : Range{ nu } )
                            {
                                const auto vertex = u + v * nu + w * nu * nv;
                                builder.set_point( vertex, origin + vector_u * u
                                                               + vector_v * v
                                                               + vector_w * w );
                            }
                        } );
                }
            }
            for( auto& task : async::when_all( tasks ).get() )
            {
                task.get();
            }
        }
    } // namespace internal
} // namespace geode
