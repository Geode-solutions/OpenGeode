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

#include <geode/mesh/helpers/euclidean_distance_transform.hpp>

#include <async++.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/progress_logger.hpp>

#include <geode/mesh/core/grid.hpp>

namespace
{
    constexpr auto NO_DISTANCE = std::numeric_limits< double >::max();
} // namespace

namespace geode
{
    template < index_t dimension >
    class EuclideanDistanceTransform
    {
        using Index = typename Grid< dimension >::CellIndices;

    public:
        EuclideanDistanceTransform( const Grid< dimension >& grid,
            absl::Span< const Index > grid_cell_id,
            std::string_view distance_map_name )
            : grid_( grid ), squared_cell_length_{}
        {
            AttributeProperties attribute_properties;
            attribute_properties.assignable = false;
            attribute_properties.interpolable = false;
            attribute_properties.transferable = true;
            AttributeValues< double > distance_map_values;
            distance_map_values.default_value = NO_DISTANCE;
            distance_map_values.no_value = NO_DISTANCE;
            const auto distance_map_id =
                grid.cell_attribute_manager()
                    .template create_attribute< VariableAttribute, double >(
                        distance_map_name, distance_map_values,
                        attribute_properties );
            distance_map_ =
                grid.cell_attribute_manager()
                    .template find_attribute< VariableAttribute, double >(
                        distance_map_id );
            for( const auto d : LRange( dimension ) )
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
                        if( value <= 0. )
                        {
                            value = 0;
                            return;
                        }
                        if( value >= NO_DISTANCE )
                        {
                            // No seed: keep the attribute no_value
                            value = NO_DISTANCE;
                            return;
                        }
                        value = std::sqrt( value );
                    } );
                } );
            return;
        }

    private:
        /*!
         * Apply func( first_cell_of_line ) to every line of cells along
         * direction, in parallel (the range of lines is split in chunks).
         */
        template < typename Func >
        void parallel_lines( index_t direction, const Func& func ) const
        {
            index_t nb_lines{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                if( d != direction )
                {
                    nb_lines *= grid_.nb_cells_in_direction( d );
                }
            }
            async::parallel_for( async::irange( index_t{ 0 }, nb_lines ),
                [this, direction, &func]( index_t line ) {
                    Index index;
                    index[direction] = 0;
                    for( const auto d : LRange{ dimension } )
                    {
                        if( d == direction )
                        {
                            continue;
                        }
                        index[d] = line % grid_.nb_cells_in_direction( d );
                        line /= grid_.nb_cells_in_direction( d );
                    }
                    func( index );
                } );
        }

        /*!
         * Replace the squared distances f along every line in direction by
         * min_q( f(q) + (c - q)^2 * h^2 ), using the lower envelope of
         * parabolas (Felzenszwalb & Huttenlocher, 2012) in O(n) per line.
         * Cells with no distance (NO_DISTANCE) are ignored; a line without
         * any distance is left unchanged.
         */
        void combine_squared_distance_components( index_t direction )
        {
            parallel_lines( direction, [this, direction]( Index index ) {
                const auto nb_cells = grid_.nb_cells_in_direction( direction );
                const auto squared_length = squared_cell_length_[direction];
                thread_local std::vector< double > values;
                thread_local std::vector< index_t > parabolas;
                thread_local std::vector< double > boundaries;
                values.resize( nb_cells );
                parabolas.resize( nb_cells );
                boundaries.resize( nb_cells + 1 );
                for( const auto c : Range{ nb_cells } )
                {
                    index[direction] = c;
                    values[c] =
                        distance_map_->value( grid_.cell_index( index ) );
                }
                const auto intersection = [&squared_length]( index_t q,
                                              double f_q, index_t p,
                                              double f_p ) {
                    const auto q_value = static_cast< double >( q );
                    const auto p_value = static_cast< double >( p );
                    return ( ( f_q + squared_length * q_value * q_value )
                               - ( f_p + squared_length * p_value * p_value ) )
                           / ( 2. * squared_length * ( q_value - p_value ) );
                };
                index_t nb_parabolas{ 0 };
                for( const auto q : Range{ nb_cells } )
                {
                    if( values[q] >= NO_DISTANCE )
                    {
                        continue;
                    }
                    if( nb_parabolas == 0 )
                    {
                        parabolas[0] = q;
                        boundaries[0] = -std::numeric_limits< double >::max();
                        boundaries[1] = std::numeric_limits< double >::max();
                        nb_parabolas = 1;
                        continue;
                    }
                    auto last = nb_parabolas - 1;
                    auto boundary = intersection( q, values[q], parabolas[last],
                        values[parabolas[last]] );
                    while( boundary <= boundaries[last] )
                    {
                        // boundaries[0] is -max, so last never underflows
                        last--;
                        boundary = intersection( q, values[q], parabolas[last],
                            values[parabolas[last]] );
                    }
                    nb_parabolas = last + 2;
                    parabolas[last + 1] = q;
                    boundaries[last + 1] = boundary;
                    boundaries[last + 2] = std::numeric_limits< double >::max();
                }
                if( nb_parabolas == 0 )
                {
                    return;
                }
                index_t current{ 0 };
                for( const auto c : Range{ nb_cells } )
                {
                    while(
                        boundaries[current + 1] < static_cast< double >( c ) )
                    {
                        current++;
                    }
                    const auto parabola = parabolas[current];
                    index[direction] = c;
                    distance_map_->set_value( grid_.cell_index( index ),
                        values[parabola]
                            + directional_squared_distance(
                                c, parabola, direction ) );
                }
            } );
        }

        double directional_squared_distance( const index_t from,
            const index_t to,
            const index_t direction ) const
        {
            const auto directional_distance =
                static_cast< double >( from ) - static_cast< double >( to );
            return directional_distance * directional_distance
                   * squared_cell_length_[direction];
        }

    private:
        const Grid< dimension >& grid_;
        std::array< double, dimension > squared_cell_length_;
        std::shared_ptr< VariableAttribute< double > > distance_map_;
    };

    template < index_t dimension >
    void EuclideanDistanceTransform< dimension >::compute_squared_distance_map()
    {
        ProgressLogger logger{ Logger::LEVEL::info,
            absl::StrCat( "Compute ", dimension, "D euclidian distance" ),
            dimension };
        for( const auto d : LRange{ dimension } )
        {
            combine_squared_distance_components( d );
            logger.increment();
        }
    }

    template < index_t dimension >
    std::shared_ptr< VariableAttribute< double > > euclidean_distance_transform(
        const Grid< dimension >& grid,
        absl::Span< const typename Grid< dimension >::CellIndices >
            grid_cell_ids,
        std::string_view distance_map_name )
    {
        EuclideanDistanceTransform< dimension > edt{ grid, grid_cell_ids,
            distance_map_name };
        edt.compute_squared_distance_map();
        edt.squared_root_filter();
        return edt.distance_map();
    }

    template std::shared_ptr< VariableAttribute< double > >
        opengeode_mesh_api euclidean_distance_transform< 2 >( const Grid2D&,
            absl::Span< const Grid2D::CellIndices >,
            std::string_view );
    template std::shared_ptr< VariableAttribute< double > >
        opengeode_mesh_api euclidean_distance_transform< 3 >( const Grid3D&,
            absl::Span< const Grid3D::CellIndices >,
            std::string_view );
} // namespace geode
