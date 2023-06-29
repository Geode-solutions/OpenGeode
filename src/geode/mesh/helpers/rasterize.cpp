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

#include <geode/mesh/helpers/rasterize.h>

#include <queue>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/attribute_manager.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/coordinate_system.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/mensuration.h>
#include <geode/geometry/perpendicular.h>
#include <geode/geometry/position.h>

#include <geode/mesh/core/grid.h>

namespace
{
    template < geode::index_t dimension >
    using CellIndices = typename geode::Grid< dimension >::CellIndices;

    template < geode::index_t dimension >
    std::vector< CellIndices< dimension > > paint_segment_axis(
        geode::index_t axis0,
        const std::array< double, dimension >& deltas,
        const std::array< int, dimension >& increments,
        CellIndices< dimension > index,
        const CellIndices< dimension >& end )
    {
        std::vector< CellIndices< dimension > > painted_cells;
        painted_cells.push_back( index );
        std::array< geode::index_t, dimension - 1 > axis;
        std::array< double, dimension - 1 > error;
        for( const auto i : geode::LRange{ dimension - 1 } )
        {
            axis[i] = ( axis0 + i + 1 ) % dimension;
            error[i] = 2 * deltas[axis[i]] - deltas[axis0];
        }
        while( index[axis0] != end[axis0] )
        {
            for( const auto i : geode::LRange{ dimension - 1 } )
            {
                if( error[i] >= 0 )
                {
                    index[axis[i]] += increments[axis[i]];
                    error[i] -= 2 * deltas[axis0];
                }
                error[i] += 2 * deltas[axis[i]];
            }
            index[axis0] += increments[axis0];
            painted_cells.push_back( index );
        }
        painted_cells.push_back( end );
        return painted_cells;
    }

    template < geode::index_t dimension >
    std::tuple< std::array< double, dimension >,
        const std::array< int, dimension > >
        compute_deltas( const CellIndices< dimension >& start,
            const CellIndices< dimension >& end )
    {
        std::array< double, dimension > deltas;
        std::array< int, dimension > increments;
        for( const auto i : geode::LRange{ dimension } )
        {
            if( end[i] > start[i] )
            {
                deltas[i] = end[i] - start[i];
                increments[i] = 1;
            }
            else
            {
                deltas[i] = start[i] - end[i];
                increments[i] = -1;
            }
        }
        return std::make_tuple( deltas, increments );
    }

    template < geode::index_t dimension >
    geode::index_t get_major_axis(
        const std::array< double, dimension >& deltas )
    {
        for( const auto i : geode::LRange{ dimension } )
        {
            bool ok{ true };
            for( const auto j : geode::LRange{ dimension } )
            {
                if( i != j && deltas[i] < deltas[j] )
                {
                    ok = false;
                    break;
                }
            }
            if( ok )
            {
                return i;
            }
        }
        OPENGEODE_ASSERT_NOT_REACHED(
            "[rasterize_segment] Main axis not found" );
        return geode::NO_ID;
    }

    template < geode::index_t dimension >
    std::tuple< geode::index_t, geode::index_t > get_major_plane(
        const std::array< typename geode::Grid< dimension >::Indices, 3 >&
            vertex_cells )
    {
        geode::index_t max_delta{ 0 };
        geode::index_t major_edge{ geode::NO_ID };
        geode::index_t major_axis{ geode::NO_ID };
        for( const auto e : geode::LRange{ 3 } )
        {
            for( const auto& e0 : vertex_cells[e] )
            {
                for( const auto& e1 : vertex_cells[( e + 1 ) % 3] )
                {
                    std::array< double, dimension > deltas;
                    std::array< int, dimension > increments;
                    std::tie( deltas, increments ) =
                        compute_deltas< dimension >( e0, e1 );
                    const auto i = get_major_axis< dimension >( deltas );
                    if( deltas[i] > max_delta )
                    {
                        major_edge = e;
                        major_axis = i;
                    }
                }
            }
        }
        return std::make_tuple( major_edge, major_axis );
    }

    template < geode::index_t dimension >
    std::vector< CellIndices< dimension > > paint_segment(
        const CellIndices< dimension >& start,
        const CellIndices< dimension >& end )
    {
        std::array< double, dimension > deltas;
        std::array< int, dimension > increments;
        std::tie( deltas, increments ) =
            compute_deltas< dimension >( start, end );
        const auto i = get_major_axis< dimension >( deltas );
        return paint_segment_axis< dimension >(
            i, deltas, increments, start, end );
    }

    std::vector< CellIndices< 2 > > conservative_voxelization_triangle(
        const geode::Grid2D& grid,
        const geode::Triangle2D& triangle,
        const std::array< geode::Grid2D::CellsAroundVertex, 3 > vertex_cells )
    {
        geode_unused( vertex_cells );
        absl::flat_hash_map< geode::index_t,
            std::pair< geode::index_t, geode::index_t > >
            min_max;
        const auto& vertices = triangle.vertices();
        for( const auto e : geode::LRange{ 3 } )
        {
            const auto e_next = e == 2 ? 0 : e + 1;
            const auto segment_cells = geode::conservative_rasterize_segment(
                grid, { vertices[e].get(), vertices[e_next].get() } );
            for( const auto& cell : segment_cells )
            {
                auto it = min_max.try_emplace( cell[1], cell[0], cell[0] );
                if( !it.second )
                {
                    auto& value = it.first->second;
                    value.first = std::min( value.first, cell[0] );
                    value.second = std::max( value.second, cell[0] );
                }
            }
        }
        std::vector< CellIndices< 2 > > cells;
        for( const auto& it : min_max )
        {
            for( const auto i :
                geode::Range{ it.second.first, it.second.second + 1 } )
            {
                cells.emplace_back( CellIndices< 2 >{ i, it.first } );
            }
        }
        return cells;
    }

    std::array< std::pair< geode::Vector2D, double >, 3 > get_edge_projection(
        const geode::OwnerTriangle3D& triangle_in_grid,
        const std::array< geode::Segment3D, 3 >& edges_in_grid,
        const std::array< geode::index_t, 2 >& plane_axes,
        int normal_orientation )
    {
        std::array< std::pair< geode::Vector2D, double >, 3 > result;
        const auto& vertices = triangle_in_grid.vertices();
        for( const auto e : geode::LRange{ 3 } )
        {
            result[e].first = geode::Vector2D{
                { -1 * edges_in_grid[e].direction().value( plane_axes[1] ),
                    edges_in_grid[e].direction().value( plane_axes[0] ) }
            } * normal_orientation;
            const auto& vertex = vertices[e];
            result[e].second = -result[e].first.dot( geode::Vector2D{
                                   { vertex.value( plane_axes[0] ),
                                       vertex.value( plane_axes[1] ) } } )
                               + std::max( 0., result[e].first.value( 0 ) )
                               + std::max( 0., result[e].first.value( 1 ) );
        }
        return result;
    }

    template < geode::index_t dimension >
    geode::Point< dimension > compute_critical_point(
        const geode::Vector< dimension >& normal_in_grid )
    {
        geode::Point< dimension > critical;
        for( const auto d : geode::LRange{ dimension } )
        {
            if( normal_in_grid.value( d ) > 0. )
            {
                critical.set_value( d, 1 );
            }
        }
        return critical;
    }

    template < typename PointType, geode::index_t dimension >
    std::array< geode::Segment< dimension >, 3 > get_triangle_edges(
        const geode::GenericTriangle< PointType, dimension >& triangle )
    {
        const auto& vertices = triangle.vertices();
        return { geode::Segment< dimension >{ vertices[0], vertices[1] },
            geode::Segment< dimension >{ vertices[1], vertices[2] },
            geode::Segment< dimension >{ vertices[2], vertices[0] } };
    }

    void add_cells( std::vector< CellIndices< 3 > >& cells,
        std::vector< CellIndices< 3 > > new_cells )
    {
        for( auto&& new_cell : new_cells )
        {
            if( absl::c_find( cells, new_cell ) == cells.end() )
            {
                cells.emplace_back( new_cell );
            }
        }
    }

    geode::index_t max_number_cells(
        const CellIndices< 3 >& min, const CellIndices< 3 >& max )
    {
        geode::index_t nb_cells{ 1 };
        for( const auto d : geode::LRange{ 3 } )
        {
            nb_cells *= max[d] + 1 - min[d];
        }
        return nb_cells;
    }

    std::vector< CellIndices< 3 > > conservative_voxelization_triangle(
        const geode::Grid3D& grid,
        const geode::Triangle3D& triangle,
        const std::array< geode::Grid3D::CellsAroundVertex, 3 > vertex_cells )
    {
        auto min = grid.cell_indices( grid.nb_cells() - 1 );
        auto max = grid.cell_indices( 0 );
        for( const auto v : geode::LRange{ 3 } )
        {
            for( const auto& index : vertex_cells[v] )
            {
                for( const auto d : geode::LRange{ 3 } )
                {
                    min[d] = std::min( min[d], index[d] );
                    max[d] = std::max( max[d], index[d] );
                }
            }
        }
        std::vector< CellIndices< 3 > > cells;
        cells.reserve( max_number_cells( min, max ) );
        const geode::OwnerTriangle3D triangle_in_grid{
            grid.grid_coordinate_system().coordinates( triangle.vertices()[0] ),
            grid.grid_coordinate_system().coordinates( triangle.vertices()[1] ),
            grid.grid_coordinate_system().coordinates( triangle.vertices()[2] )
        };
        const auto triangle_edges = get_triangle_edges( triangle );
        const auto normal_in_grid = triangle_in_grid.normal();
        if( !normal_in_grid
            || absl::c_count_if( triangle_edges,
                   []( const geode::Segment3D& segment ) {
                       return segment.length() <= geode::global_epsilon;
                   } )
                   > 0 )
        {
            for( const auto e : geode::LRange{ 3 } )
            {
                add_cells( cells,
                    geode::rasterize_segment( grid, triangle_edges[e] ) );
            }
            return cells;
        }
        const auto triangle_edges_in_grid =
            get_triangle_edges( triangle_in_grid );
        const auto critical_point =
            compute_critical_point( normal_in_grid.value() );
        const auto xy_params =
            get_edge_projection( triangle_in_grid, triangle_edges_in_grid,
                { 0, 1 }, ( normal_in_grid->value( 2 ) >= 0 ? 1 : -1 ) );
        const auto yz_params =
            get_edge_projection( triangle_in_grid, triangle_edges_in_grid,
                { 1, 2 }, ( normal_in_grid->value( 0 ) >= 0 ? 1 : -1 ) );
        const auto zx_params =
            get_edge_projection( triangle_in_grid, triangle_edges_in_grid,
                { 2, 0 }, ( normal_in_grid->value( 1 ) >= 0 ? 1 : -1 ) );
        const auto plane_in_grid = triangle_in_grid.plane().value();
        for( const auto k : geode::Range( min[2], max[2] + 1 ) )
        {
            for( const auto j : geode::Range( min[1], max[1] + 1 ) )
            {
                for( const auto i : geode::Range( min[0], max[0] + 1 ) )
                {
                    CellIndices< 3 > cur_cell{ { i, j, k } };
                    const geode::Point3D point{ { static_cast< double >( i ),
                        static_cast< double >( j ),
                        static_cast< double >( k ) } };

                    // Test triangle plane through box
                    const auto p_minus = point + critical_point;
                    double p_minus_dist;
                    std::tie( p_minus_dist, std::ignore ) =
                        geode::point_plane_signed_distance(
                            p_minus, plane_in_grid );
                    const geode::Point3D p_plus{
                        { 1. + i - critical_point.value( 0 ),
                            1. + j - critical_point.value( 1 ),
                            1. + k - critical_point.value( 2 ) }
                    };
                    double p_plus_dist;
                    std::tie( p_plus_dist, std::ignore ) =
                        geode::point_plane_signed_distance(
                            p_plus, plane_in_grid );
                    if( std::fabs( p_minus_dist ) > 2. * geode::global_epsilon
                        && std::fabs( p_plus_dist ) > 2. * geode::global_epsilon
                        && p_minus_dist * p_plus_dist > 0. )
                    {
                        continue;
                    }

                    // Tests projection plane
                    const geode::Vector2D p_xy{ { point.value( 0 ),
                        point.value( 1 ) } };
                    if( xy_params[0].first.dot( p_xy ) + xy_params[0].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }
                    if( xy_params[1].first.dot( p_xy ) + xy_params[1].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }
                    if( xy_params[2].first.dot( p_xy ) + xy_params[2].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }

                    const geode::Vector2D p_yz{ { point.value( 1 ),
                        point.value( 2 ) } };
                    if( yz_params[0].first.dot( p_yz ) + yz_params[0].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }
                    if( yz_params[1].first.dot( p_yz ) + yz_params[1].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }
                    if( yz_params[2].first.dot( p_yz ) + yz_params[2].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }

                    const geode::Vector2D p_zx{ { point.value( 2 ),
                        point.value( 0 ) } };
                    if( zx_params[0].first.dot( p_zx ) + zx_params[0].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }
                    if( zx_params[1].first.dot( p_zx ) + zx_params[1].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }
                    if( zx_params[2].first.dot( p_zx ) + zx_params[2].second
                        < -2. * geode::global_epsilon )
                    {
                        continue;
                    }
                    cells.emplace_back( std::move( cur_cell ) );
                }
            }
        }

        return cells;
    }

    absl::InlinedVector< CellIndices< 3 >, 6 > neighbors(
        const geode::Grid3D& grid, const CellIndices< 3 >& cell )
    {
        absl::InlinedVector< CellIndices< 3 >, 6 > neighbors;
        for( const auto d : geode::LRange{ 3 } )
        {
            if( const auto prev = grid.previous_cell( cell, d ) )
            {
                neighbors.push_back( prev.value() );
            }
            if( const auto next = grid.next_cell( cell, d ) )
            {
                neighbors.push_back( next.value() );
            }
        }
        return neighbors;
    }

    std::vector< CellIndices< 3 > > conservative_voxelization_segment(
        const geode::Grid3D& grid,
        const geode::Segment3D& segment,
        const std::array< geode::Grid3D::CellsAroundVertex, 2 > /*unused*/ )
    {
        auto cells = geode::rasterize_segment( grid, segment );
        std::vector< bool > tested_cells( grid.nb_cells(), false );
        std::queue< CellIndices< 3 > > to_test;
        for( const auto& cell : cells )
        {
            tested_cells[grid.cell_index( cell )] = true;
            for( auto&& neighbor : neighbors( grid, cell ) )
            {
                to_test.emplace( std::move( neighbor ) );
            }
        }
        const auto half_cell_size =
            std::sqrt( grid.cell_length_in_direction( 0 )
                           * grid.cell_length_in_direction( 0 )
                       + grid.cell_length_in_direction( 1 )
                             * grid.cell_length_in_direction( 1 )
                       + grid.cell_length_in_direction( 2 )
                             * grid.cell_length_in_direction( 2 ) )
            / 2.;
        while( !to_test.empty() )
        {
            const auto cell = to_test.front();
            to_test.pop();
            const auto cell_id = grid.cell_index( cell );
            if( tested_cells[cell_id] )
            {
                continue;
            }
            tested_cells[cell_id] = true;
            const auto center = grid.cell_barycenter( cell );
            if( geode::point_segment_distance( center, segment )
                <= half_cell_size )
            {
                for( auto&& neighbor : neighbors( grid, cell ) )
                {
                    to_test.emplace( std::move( neighbor ) );
                }
                cells.emplace_back( std::move( cell ) );
            }
        }
        return cells;
    }

    std::vector< CellIndices< 2 > > conservative_voxelization_segment(
        const geode::Grid2D& grid,
        const geode::Segment2D& segment,
        const std::array< geode::Grid2D::CellsAroundVertex, 2 > vertex_cells )
    {
        OPENGEODE_ASSERT( segment.length() > geode::global_epsilon,
            "[conservative_voxelization_segment] Segment should be longer than "
            "epsilon" );
        std::vector< CellIndices< 2 > > cells;
        auto min = grid.cell_indices( grid.nb_cells() - 1 );
        auto max = grid.cell_indices( 0 );
        for( const auto v : geode::LRange{ 2 } )
        {
            for( const auto& index : vertex_cells[v] )
            {
                for( const auto d : geode::LRange{ 2 } )
                {
                    min[d] = std::min( min[d], index[d] );
                    max[d] = std::max( max[d], index[d] );
                }
            }
        }
        const auto& seg_vertices = segment.vertices();
        const auto pt0_in_grid =
            grid.grid_coordinate_system().coordinates( seg_vertices[0] );
        const auto pt1_in_grid =
            grid.grid_coordinate_system().coordinates( seg_vertices[1] );
        const geode::Segment2D segment_in_grid{ pt0_in_grid, pt1_in_grid };
        const auto normal_in_grid =
            geode::perpendicular( segment_in_grid.direction() );
        const auto critical_point = compute_critical_point( normal_in_grid );

        for( const auto j : geode::Range( min[1], max[1] + 1 ) )
        {
            for( const auto i : geode::Range( min[0], max[0] + 1 ) )
            {
                CellIndices< 2 > cur_cell{ { i, j } };
                const geode::Point2D point{ { static_cast< double >( i ),
                    static_cast< double >( j ) } };

                // Test segment line through box
                const auto p_minus = point + critical_point;
                const auto p_minus_dist = geode::point_line_signed_distance(
                    p_minus, { segment_in_grid } );
                const geode::Point2D p_plus{ { 1. + i
                                                   - critical_point.value( 0 ),
                    1. + j - critical_point.value( 1 ) } };
                const auto p_plus_dist = geode::point_line_signed_distance(
                    p_plus, { segment_in_grid } );
                if( std::fabs( p_minus_dist ) > 2. * geode::global_epsilon
                    && std::fabs( p_plus_dist ) > 2. * geode::global_epsilon
                    && p_minus_dist * p_plus_dist > 0. )
                {
                    continue;
                }

                cells.emplace_back( std::move( cur_cell ) );
            }
        }
        return cells;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::vector< CellIndices< dimension > > rasterize_segment(
        const Grid< dimension >& grid, const Segment< dimension >& segment )
    {
        const auto start = grid.cells( segment.vertices().front() );
        const auto end = grid.cells( segment.vertices().back() );
        OPENGEODE_EXCEPTION( !start.empty() && !end.empty(),
            "[rasterize_segment] Segment is not included in the given Grid" );
        if( start == end )
        {
            return { start.begin(), start.end() };
        }

        std::vector< CellIndices< dimension > > cells;
        for( const auto& start_id : start )
        {
            for( const auto& end_id : end )
            {
                for( const auto& cell :
                    paint_segment< dimension >( start_id, end_id ) )
                {
                    cells.push_back( cell );
                }
            }
        }
        sort_unique( cells );
        return cells;
    }

    template < index_t dimension >
    std::vector< CellIndices< dimension > > conservative_rasterize_segment(
        const Grid< dimension >& grid, const Segment< dimension >& segment )
    {
        std::array< typename Grid< dimension >::CellsAroundVertex, 2 >
            vertex_cells;
        const auto& vertices = segment.vertices();
        for( const auto v : LRange{ 2 } )
        {
            vertex_cells[v] = grid.cells( vertices[v] );
            OPENGEODE_EXCEPTION( !vertex_cells[v].empty(),
                "[conservative_rasterize_segment] Segment is "
                "not included in the given Grid" );
        }
        if( vertex_cells[0] == vertex_cells[1] )
        {
            return { vertex_cells[0].begin(), vertex_cells[0].end() };
        }
        return conservative_voxelization_segment( grid, segment, vertex_cells );
    }

    template < index_t dimension >
    std::vector< CellIndices< dimension > > rasterize_triangle(
        const Grid< dimension >& grid, const Triangle< dimension >& triangle )
    {
        std::array< typename Grid< dimension >::CellsAroundVertex, 3 >
            vertex_cells;
        const auto& vertices = triangle.vertices();
        for( const auto v : LRange{ 3 } )
        {
            vertex_cells[v] = grid.cells( vertices[v].get() );
            OPENGEODE_EXCEPTION( !vertex_cells[v].empty(),
                "[rasterize_triangle] Triangle is not included in "
                "the given Grid" );
        }
        if( vertex_cells[0] == vertex_cells[1]
            && vertex_cells[1] == vertex_cells[2] )
        {
            return { vertex_cells[0].begin(), vertex_cells[0].end() };
        }
        return conservative_voxelization_triangle(
            grid, triangle, vertex_cells );
    }

    template std::vector< CellIndices< 2 > > opengeode_mesh_api
        rasterize_segment< 2 >( const Grid2D&, const Segment2D& );

    template std::vector< CellIndices< 3 > > opengeode_mesh_api
        rasterize_segment< 3 >( const Grid3D&, const Segment3D& );

    template std::vector< CellIndices< 2 > > opengeode_mesh_api
        conservative_rasterize_segment< 2 >( const Grid2D&, const Segment2D& );

    template std::vector< CellIndices< 3 > > opengeode_mesh_api
        conservative_rasterize_segment< 3 >( const Grid3D&, const Segment3D& );

    template std::vector< CellIndices< 2 > > opengeode_mesh_api
        rasterize_triangle< 2 >( const Grid2D&, const Triangle2D& );

    template std::vector< CellIndices< 3 > > opengeode_mesh_api
        rasterize_triangle< 3 >( const Grid3D&, const Triangle3D& );
} // namespace geode
