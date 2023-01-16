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

#include <geode/mesh/helpers/euclidean_distance_transform.h>

#include <async++.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/progress_logger.h>

#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>

namespace geode
{
    template < index_t dimension >
    class EuclideanDistanceTransform
    {
        using Index = GridCellIndices< dimension >;

    public:
        EuclideanDistanceTransform( const RegularGrid< dimension >& grid,
            absl::Span< const GridCellIndices< dimension > > grid_cell_id,
            absl::string_view distance_map_name )
            : grid_{ grid }, squared_cell_length_{}, distance_map_{ grid_.cell_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( distance_map_name,
                        std::numeric_limits< double >::max() }
        {
            for( const auto d : Range( dimension ) )
            {
                squared_cell_length_[d] = grid_.cell_length_in_direction( d )
                                          * grid_.cell_length_in_direction( d );
            }
            for( const auto& cell_id : grid_cell_id )
            {
                distance_map_->set_value( grid_.cell_index( cell_id ), 0. );
            }
        }

        std::shared_ptr< VariableAttribute< double > > distance_map() const
        {
            return distance_map_;
        }

        void compute_squared_distance_map();

        void squared_root_filter()
        {
            async::parallel_for(
                async::irange( index_t{ 0 }, grid_.nb_cells() ),
                [this]( index_t cell ) {
                    distance_map_->modify_value( cell, []( double& value ) {
                        value = std::sqrt( value );
                    } );
                } );
            return;
        }

    private:
        void propagate_directional_squared_distance( const index_t direction );

        double propagate_directional_step_squared_distance(
            const Index& from_index,
            const Index& to_index,
            const index_t direction,
            const double last_step_squared_distance )
        {
            const auto old_distance =
                distance_map_->value( grid_.cell_index( from_index ) );
            const auto step_squared_distance =
                old_distance == 0 ? squared_cell_length_[direction]
                                  : last_step_squared_distance
                                        + 2 * squared_cell_length_[direction];
            const auto new_distance = old_distance + step_squared_distance;
            distance_map_->modify_value(
                grid_.cell_index( to_index ), [new_distance]( double& value ) {
                    value = std::min( value, new_distance );
                } );
            return step_squared_distance;
        }

        void combine_squared_distance_components( const index_t direction );

        double directional_squared_distance( const index_t from,
            const index_t to,
            const index_t direction ) const
        {
            return ( static_cast< double >( from )
                       - static_cast< double >( to ) )
                   * ( static_cast< double >( from )
                       - static_cast< double >( to ) )
                   * squared_cell_length_[direction];
        }

    private:
        const RegularGrid< dimension >& grid_;
        std::array< double, dimension > squared_cell_length_;
        std::shared_ptr< VariableAttribute< double > > distance_map_;
    };

    template <>
    void
        EuclideanDistanceTransform< 2 >::propagate_directional_squared_distance(
            const index_t d )
    {
        const auto d2 = d == 1 ? 0 : 1;
        absl::FixedArray< async::task< void > > tasks(
            grid_.nb_cells_in_direction( d2 ) );
        index_t task_id{ 0 };
        for( const auto c2 : Range{ grid_.nb_cells_in_direction( d2 ) } )
        {
            tasks[task_id++] = async::spawn( [this, d, d2, c2] {
                double step_squared_distance{ 0. };
                for( const auto c :
                    Range{ 1, grid_.nb_cells_in_direction( d ) } )
                {
                    GridCellIndices2D index;
                    index[d] = c;
                    index[d2] = c2;
                    auto prev_index = index;
                    prev_index[d] = c - 1;
                    step_squared_distance =
                        propagate_directional_step_squared_distance(
                            prev_index, index, d, step_squared_distance );
                }
                step_squared_distance = 0.;
                for( const auto c :
                    ReverseRange{ grid_.nb_cells_in_direction( d ) - 1 } )
                {
                    GridCellIndices2D index;
                    index[d] = c;
                    index[d2] = c2;
                    auto prev_index = index;
                    prev_index[d] = c + 1;
                    step_squared_distance =
                        propagate_directional_step_squared_distance(
                            prev_index, index, d, step_squared_distance );
                }
            } );
        }
        for( auto& task : async::when_all( tasks.begin(), tasks.end() ).get() )
        {
            task.get();
        }
    }
    template <>
    void
        EuclideanDistanceTransform< 3 >::propagate_directional_squared_distance(
            const index_t d )
    {
        const auto d2 = d == 2 ? 0 : d + 1;
        const auto d3 = d2 == 2 ? 0 : d2 + 1;
        absl::FixedArray< async::task< void > > tasks(
            grid_.nb_cells_in_direction( d2 )
            * grid_.nb_cells_in_direction( d3 ) );
        index_t task_id{ 0 };
        for( const auto c3 : Range{ grid_.nb_cells_in_direction( d3 ) } )
        {
            for( const auto c2 : Range{ grid_.nb_cells_in_direction( d2 ) } )
            {
                tasks[task_id++] = async::spawn( [this, d, d2, d3, c2, c3] {
                    double step_squared_distance{ 0 };
                    for( const auto c :
                        Range{ 1, grid_.nb_cells_in_direction( d ) } )
                    {
                        Index index;
                        index[d] = c;
                        index[d2] = c2;
                        index[d3] = c3;
                        Index prev_index = index;
                        prev_index[d] = c - 1;
                        step_squared_distance =
                            propagate_directional_step_squared_distance(
                                prev_index, index, d, step_squared_distance );
                    }
                    step_squared_distance = 0;
                    for( const auto c :
                        ReverseRange{ grid_.nb_cells_in_direction( d ) - 1 } )
                    {
                        Index index;
                        index[d] = c;
                        index[d2] = c2;
                        index[d3] = c3;
                        Index prev_index = index;
                        prev_index[d] = c + 1;
                        step_squared_distance =
                            propagate_directional_step_squared_distance(
                                prev_index, index, d, step_squared_distance );
                    }
                } );
            }
        }
        for( auto& task : async::when_all( tasks.begin(), tasks.end() ).get() )
        {
            task.get();
        }
    }

    template <>
    void EuclideanDistanceTransform< 2 >::combine_squared_distance_components(
        const index_t d )
    {
        const auto d2 = d == 1 ? 0 : 1;
        absl::FixedArray< async::task< void > > tasks(
            grid_.nb_cells_in_direction( d2 ) );
        index_t task_id{ 0 };
        for( const auto c2 : Range{ grid_.nb_cells_in_direction( d2 ) } )
        {
            tasks[task_id++] = async::spawn( [this, d, d2, c2] {
                absl::FixedArray< double > temps_dist(
                    grid_.nb_cells_in_direction( d ) );
                for( const auto c :
                    Range{ 0, grid_.nb_cells_in_direction( d ) } )
                {
                    auto min_dist = std::numeric_limits< double >::max();
                    for( const auto cf :
                        Range{ c, grid_.nb_cells_in_direction( d ) } )
                    {
                        const auto step_squared_distance =
                            directional_squared_distance( c, cf, d );
                        if( min_dist < step_squared_distance )
                        {
                            break;
                        }
                        GridCellIndices2D index;
                        index[d] = cf;
                        index[d2] = c2;
                        min_dist = std::min( min_dist,
                            distance_map_->value( grid_.cell_index( index ) )
                                + step_squared_distance );
                    }
                    for( const auto cb : ReverseRange{ c, 0 } )
                    {
                        const auto step_squared_distance =
                            directional_squared_distance( c, cb, d );
                        if( min_dist < step_squared_distance )
                        {
                            break;
                        }
                        GridCellIndices2D index;
                        index[d] = cb;
                        index[d2] = c2;
                        min_dist = std::min( min_dist,
                            distance_map_->value( grid_.cell_index( index ) )
                                + step_squared_distance );
                    }
                    temps_dist[c] = min_dist;
                }
                for( const auto c :
                    Range{ 0, grid_.nb_cells_in_direction( d ) } )
                {
                    GridCellIndices2D index;
                    index[d] = c;
                    index[d2] = c2;
                    distance_map_->set_value(
                        grid_.cell_index( index ), temps_dist[c] );
                }
            } );
        }
        for( auto& task : async::when_all( tasks.begin(), tasks.end() ).get() )
        {
            task.get();
        }
    }
    template <>
    void EuclideanDistanceTransform< 3 >::combine_squared_distance_components(
        const index_t d )
    {
        const auto d2 = d == 2 ? 0 : d + 1;
        const auto d3 = d2 == 2 ? 0 : d2 + 1;
        absl::FixedArray< async::task< void > > tasks(
            grid_.nb_cells_in_direction( d2 )
            * grid_.nb_cells_in_direction( d3 ) );
        index_t task_id{ 0 };
        for( const auto c3 : Range{ grid_.nb_cells_in_direction( d3 ) } )
        {
            for( const auto c2 : Range{ grid_.nb_cells_in_direction( d2 ) } )
            {
                tasks[task_id++] = async::spawn( [this, d, d2, d3, c2, c3] {
                    absl::FixedArray< double > temps_dist(
                        grid_.nb_cells_in_direction( d ) );
                    for( const auto c :
                        Range{ 0, grid_.nb_cells_in_direction( d ) } )
                    {
                        auto min_dist = std::numeric_limits< double >::max();
                        for( const auto cf :
                            Range{ c, grid_.nb_cells_in_direction( d ) } )
                        {
                            const auto step_squared_distance =
                                directional_squared_distance( c, cf, d );
                            if( min_dist < step_squared_distance )
                            {
                                break;
                            }
                            GridCellIndices3D index;
                            index[d] = cf;
                            index[d2] = c2;
                            index[d3] = c3;
                            min_dist = std::min(
                                min_dist, distance_map_->value(
                                              grid_.cell_index( index ) )
                                              + step_squared_distance );
                        }
                        for( const auto cb : ReverseRange{ c, 0 } )
                        {
                            const auto step_squared_distance =
                                directional_squared_distance( c, cb, d );
                            if( min_dist < step_squared_distance )
                            {
                                break;
                            }
                            GridCellIndices3D index;
                            index[d] = cb;
                            index[d2] = c2;
                            index[d3] = c3;
                            min_dist = std::min(
                                min_dist, distance_map_->value(
                                              grid_.cell_index( index ) )
                                              + step_squared_distance );
                        }
                        temps_dist[c] = min_dist;
                    }
                    for( const auto c :
                        Range{ 0, grid_.nb_cells_in_direction( d ) } )
                    {
                        GridCellIndices3D index;
                        index[d] = c;
                        index[d2] = c2;
                        index[d3] = c3;
                        distance_map_->set_value(
                            grid_.cell_index( index ), temps_dist[c] );
                    }
                } );
            }
        }
        for( auto& task : async::when_all( tasks.begin(), tasks.end() ).get() )
        {
            task.get();
        }
    }
    template <>
    void EuclideanDistanceTransform< 2 >::compute_squared_distance_map()
    {
        ProgressLogger logger{ "Compute 2D euclidian distance", 2 };
        propagate_directional_squared_distance( 0 );
        logger.increment();
        combine_squared_distance_components( 1 );
        logger.increment();
    }
    template <>
    void EuclideanDistanceTransform< 3 >::compute_squared_distance_map()
    {
        ProgressLogger logger{ "Compute 3D euclidian distance", 3 };
        propagate_directional_squared_distance( 0 );
        logger.increment();
        combine_squared_distance_components( 1 );
        logger.increment();
        combine_squared_distance_components( 2 );
        logger.increment();
    }

    template < index_t dimension >
    std::shared_ptr< VariableAttribute< double > > euclidean_distance_transform(
        const RegularGrid< dimension >& grid,
        absl::Span< const GridCellIndices< dimension > > grid_cell_ids,
        absl::string_view distance_map_name )
    {
        EuclideanDistanceTransform< dimension > edt{ grid, grid_cell_ids,
            distance_map_name };
        edt.compute_squared_distance_map();
        edt.squared_root_filter();
        return edt.distance_map();
    }

    template std::shared_ptr< VariableAttribute< double > > opengeode_mesh_api
        euclidean_distance_transform< 2 >( const RegularGrid2D&,
            absl::Span< const GridCellIndices2D >,
            absl::string_view );
    template std::shared_ptr< VariableAttribute< double > > opengeode_mesh_api
        euclidean_distance_transform< 3 >( const RegularGrid3D&,
            absl::Span< const GridCellIndices3D >,
            absl::string_view );
} // namespace geode