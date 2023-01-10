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

#include <geode/basic/logger.h>

namespace
{
    double eval_length( double cell_lenth, geode::index_t increment )
    {
        geode_unused( increment );
        return cell_lenth;
    }
    double eval_squared_length( double cell_lenth, geode::index_t increment )
    {
        return cell_lenth + increment;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class EDTransform
    {
        using Index = GridCellIndices< dimension >;

    public:
        EDTransform( const RegularGrid< dimension >& grid,
            const std::vector< GridCellIndices< dimension > >& grid_cell_id,
            absl::string_view ed_attribute_name )
            : grid_{ grid }
        {
            init_distance_map( grid_cell_id, ed_attribute_name );
        }

        std::shared_ptr< geode::VariableAttribute< double > > get_ed_attribute()
        {
            return ed_map_attribute_;
        }

        void transform();

        void squared_root_filter()
        {
            async::parallel_for(
                async::irange( index_t{ 0 }, grid_.nb_cells() ),
                [this]( index_t cell ) {
                    ed_map_attribute_->modify_value( cell, []( double& value ) {
                        value = std::sqrt( value );
                    } );
                } );
            return;
        }

    private:
        void init_distance_map( const std::vector< Index >& grid_cell_id,
            absl::string_view ed_attribute_name )
        {
            ed_map_attribute_ =
                grid_.cell_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( ed_attribute_name,
                        std::numeric_limits< double >::max(), { true, false } );

            for( const auto& cell_id : grid_cell_id )
            {
                ed_map_attribute_->set_value( grid_.cell_index( cell_id ), 0. );
            }
        }

        void eval_distance( const Index& from_index,
            const Index& to_index,
            double cell_length,
            index_t count )
        {
            auto old_metric =
                ed_map_attribute_->value( grid_.cell_index( from_index ) );
            auto new_metric = old_metric + cell_length + count;

            paint_cell( to_index, new_metric );
        }

        index_t count_increment(
            const Index& to_index, const index_t increment )
        {
            return ed_map_attribute_->value( grid_.cell_index( to_index ) ) == 0
                       ? 0
                       : increment + 2;
        }
        void paint_cell( const Index& index, double distance )
        {
            ed_map_attribute_->modify_value(
                grid_.cell_index( index ), [distance]( double& value ) {
                    value = std::min( value, distance );
                } );
        }

    private:
        const geode::RegularGrid< dimension >& grid_;
        std::shared_ptr< geode::VariableAttribute< double > > ed_map_attribute_;
    };

    template <>
    void EDTransform< 2 >::transform()
    {
        geode::ProgressLogger logger{ "Compute euclidian distance", 2 };
        for( const auto d : geode::LRange{ 2 } )
        {
            const auto d2 = ( d + 1 ) % 2;
            std::vector< async::task< void > > tasks;
            tasks.reserve( grid_.nb_cells_in_direction( d2 ) );
            for( const auto c2 :
                geode::Range{ grid_.nb_cells_in_direction( d2 ) } )
            {
                tasks.emplace_back( async::spawn( [this, d, d2, c2] {
                    index_t count = 0;
                    for( const auto c :
                        geode::Range{ 1, grid_.nb_cells_in_direction( d ) } )
                    {
                        geode::GridCellIndices2D index;
                        index[d] = c;
                        index[d2] = c2;
                        geode::GridCellIndices2D prev_index = index;
                        prev_index[d] = c - 1;
                        eval_distance( prev_index, index,
                            grid_.cell_length_in_direction( d ), count );
                        count = count_increment( index, count );
                    }
                    count = 0;
                    for( const auto c : geode::ReverseRange{
                             grid_.nb_cells_in_direction( d ) - 1 } )
                    {
                        geode::GridCellIndices2D index;
                        index[d] = c;
                        index[d2] = c2;
                        geode::GridCellIndices2D prev_index = index;
                        prev_index[d] = c + 1;
                        eval_distance( prev_index, index,
                            grid_.cell_length_in_direction( d ), count );
                        count = count_increment( index, count );
                    }
                } ) );
            }
            for( auto& task :
                async::when_all( tasks.begin(), tasks.end() ).get() )
            {
                task.get();
            }
            logger.increment();
        }
    }

    template <>
    void EDTransform< 3 >::transform()
    {
        geode::ProgressLogger logger{ "Compute euclidian distance", 3 };
        for( const auto d : LRange{ 3 } )
        {
            const auto d2 = ( d + 1 ) % 3;
            const auto d3 = ( d + 2 ) % 3;
            std::vector< async::task< void > > tasks;
            tasks.reserve( grid_.nb_cells_in_direction( d2 )
                           * grid_.nb_cells_in_direction( d3 ) );
            for( const auto c3 : Range{ grid_.nb_cells_in_direction( d3 ) } )
            {
                for( const auto c2 :
                    Range{ grid_.nb_cells_in_direction( d2 ) } )
                {
                    tasks.emplace_back( async::spawn( [this, d, d2, d3, c2,
                                                          c3] {
                        index_t count = 0;
                        for( const auto c :
                            Range{ 1, grid_.nb_cells_in_direction( d ) } )
                        {
                            Index index;
                            index[d] = c;
                            index[d2] = c2;
                            index[d3] = c3;
                            Index prev_index = index;
                            prev_index[d] = c - 1;
                            eval_distance( prev_index, index,
                                grid_.cell_length_in_direction( d ), count );
                            count = count_increment( index, count );
                        }
                        count = 0;
                        for( const auto c : ReverseRange{
                                 grid_.nb_cells_in_direction( d ) - 1 } )
                        {
                            Index index;
                            index[d] = c;
                            index[d2] = c2;
                            index[d3] = c3;
                            Index prev_index = index;
                            prev_index[d] = c + 1;
                            eval_distance( prev_index, index,
                                grid_.cell_length_in_direction( d ), count );
                            count = count_increment( index, count );
                        }
                    } ) );
                }
            }
            for( auto& task :
                async::when_all( tasks.begin(), tasks.end() ).get() )
            {
                task.get();
            }
            logger.increment();
        }
    }

    template < index_t dimension >
    std::shared_ptr< VariableAttribute< double > > euclidean_distance_transform(
        const RegularGrid< dimension >& grid,
        const std::vector< GridCellIndices< dimension > >& grid_cell_ids,
        absl::string_view ed_attribute_name )
    {
        EDTransform< dimension > edt( grid, grid_cell_ids, ed_attribute_name );
        edt.transform();
        edt.squared_root_filter();
        return edt.get_ed_attribute();
    }

    template std::shared_ptr< VariableAttribute< double > > opengeode_mesh_api
        euclidean_distance_transform< 2 >( const RegularGrid2D&,
            const std::vector< GridCellIndices2D >&,
            absl::string_view );
    template std::shared_ptr< VariableAttribute< double > > opengeode_mesh_api
        euclidean_distance_transform< 3 >( const RegularGrid3D&,
            const std::vector< GridCellIndices3D >&,
            absl::string_view );
} // namespace geode