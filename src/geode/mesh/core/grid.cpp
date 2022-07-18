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

#include <geode/mesh/core/grid.h>

#include <absl/container/inlined_vector.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

namespace
{
    template < geode::index_t dimension >
    bool vertex_is_on_axis_origin(
        geode::local_index_t vertex_id, geode::local_index_t axis_id )
    {
        /* returns ((vertex_id / pow(2,axis_id)) modulo 2) using binary
         * operators (faster). */
        return ( ( vertex_id / ( 1 << axis_id ) ) & 1 ) == 0;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class Grid< dimension >::Impl
    {
    public:
        Impl() = default;

        index_t nb_cells() const
        {
            index_t result{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                result *= nb_cells_in_direction( d );
            }
            return result;
        }

        index_t nb_cells_in_direction( index_t direction ) const
        {
            return cells_number_.at( direction );
        }

        double cell_length_in_direction( index_t direction ) const
        {
            return cells_length_.at( direction );
        }

        double cell_size() const
        {
            double cell_size{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                cell_size *= cell_length_in_direction( d );
            }
            return cell_size;
        }

        absl::optional< GridCellIndices< dimension > > next_cell(
            const GridCellIndices< dimension >& index, index_t direction ) const
        {
            if( index[direction] + 1 < nb_cells_in_direction( direction ) )
            {
                auto next = index;
                next[direction]++;
                return next;
            }
            return absl::nullopt;
        }

        absl::optional< GridCellIndices< dimension > > previous_cell(
            const GridCellIndices< dimension >& index, index_t direction ) const
        {
            if( index[direction] > 0 )
            {
                auto prev = index;
                prev[direction]--;
                return prev;
            }
            return absl::nullopt;
        }

        bool is_cell_on_border(
            const GridCellIndices< dimension >& cell_indices ) const
        {
            for( const auto d : LRange{ dimension } )
            {
                if( cell_indices[d] == 0
                    || cell_indices[d] == nb_cells_in_direction( d ) - 1 )
                {
                    return true;
                }
            }
            return false;
        }

        index_t nb_vertices() const
        {
            index_t result{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                result *= nb_vertices_in_direction( d );
            }
            return result;
        }

        index_t nb_vertices_in_direction( index_t direction ) const
        {
            return cells_number_.at( direction ) + 1;
        }

        index_t nb_vertices_on_borders() const
        {
            index_t nb_inside_vertices{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                const auto nb_vertices_d = nb_vertices_in_direction( d );
                if( nb_vertices_d < 2 )
                {
                    nb_inside_vertices *= nb_vertices_d;
                }
                else
                {
                    nb_inside_vertices *= nb_vertices_d - 2;
                }
            }
            return nb_vertices() - nb_inside_vertices;
        }

        GridCellVertices< dimension > cell_vertices(
            const GridCellIndices< dimension >& cell_id ) const
        {
            GridCellVertices< dimension > cell_vertices;
            for( const auto vertex_local_id :
                LRange{ nb_cell_vertices_static() } )
            {
                cell_vertices[vertex_local_id] =
                    cell_vertex_indices( cell_id, vertex_local_id );
            }
            return cell_vertices;
        }

        GridVertexIndices< dimension > cell_vertex_indices(
            const GridCellIndices< dimension >& cell_id,
            local_index_t vertex_id ) const
        {
            auto node_index = cell_id;
            for( const auto d : LRange{ dimension } )
            {
                if( !vertex_is_on_axis_origin< dimension >( vertex_id, d ) )
                {
                    node_index[d] += 1;
                }
            }
            return node_index;
        }

        absl::optional< GridVertexIndices< dimension > > next_vertex(
            const GridVertexIndices< dimension >& index,
            index_t direction ) const
        {
            if( index[direction] + 1 < nb_vertices_in_direction( direction ) )
            {
                auto next = index;
                next[direction]++;
                return next;
            }
            return absl::nullopt;
        }

        absl::optional< GridVertexIndices< dimension > > previous_vertex(
            const GridVertexIndices< dimension >& index,
            index_t direction ) const
        {
            if( index[direction] > 0 )
            {
                auto prev = index;
                prev[direction]--;
                return prev;
            }
            return absl::nullopt;
        }

        bool contains( const geode::Point< dimension >& origin,
            const Point< dimension >& query ) const
        {
            for( const auto d : LRange{ dimension } )
            {
                const auto value =
                    point_local_grid_coordinate( origin, query, d );
                if( value < -global_epsilon
                    || value > cells_number_[d] + global_epsilon )
                {
                    return false;
                }
            }
            return true;
        }

        GridVertexIndices< dimension > closest_vertex(
            const geode::Point< dimension >& origin,
            const Point< dimension >& query ) const
        {
            GridVertexIndices< dimension > closest;
            for( const auto d : LRange{ dimension } )
            {
                const auto value =
                    point_local_grid_coordinate( origin, query, d );
                if( value < 0. )
                {
                    closest[d] = 0;
                }
                else if( value > cells_number_[d] )
                {
                    closest[d] = cells_number_[d];
                }
                else
                {
                    closest[d] = std::round( value );
                }
            }
            return closest;
        }

        GridCellsAroundVertex< dimension > cells(
            const geode::Point< dimension >& origin,
            const Point< dimension >& query ) const
        {
            GridCellIndices< dimension > min;
            GridCellIndices< dimension > max;
            for( const auto d : LRange{ dimension } )
            {
                const auto value =
                    point_local_grid_coordinate( origin, query, d );
                if( value < -global_epsilon
                    || value > cells_number_[d] + global_epsilon )
                {
                    return {};
                }
                auto floating_floor = std::floor( value );
                if( floating_floor < 0 )
                {
                    floating_floor = 0;
                }
                else if( floating_floor > cells_number_[d] - 1 )
                {
                    floating_floor = cells_number_[d] - 1;
                }
                const auto integer_floor =
                    static_cast< index_t >( floating_floor );
                min[d] = integer_floor;
                max[d] = integer_floor;
                const auto remainder = value - floating_floor;
                if( remainder < global_epsilon )
                {
                    min[d] = integer_floor > 0 ? integer_floor - 1 : 0;
                }
                else if( remainder > 1 - global_epsilon )
                {
                    max[d] =
                        std::min( integer_floor + 1, cells_number_[d] - 1 );
                }
            }
            GridCellsAroundVertex< dimension > cells_around_point;
            cells_around_point.push_back( min );
            for( const auto d : LRange{ dimension } )
            {
                if( max[d] != min[d] )
                {
                    for( const auto& cell_indices : cells_around_point )
                    {
                        cells_around_point.push_back( cell_indices );
                        cells_around_point.back()[d] = max[d];
                    }
                }
            }
            return cells_around_point;
        }

        const std::array< index_t, dimension >& cells_numbers() const
        {
            return cells_number_;
        }

        const std::array< double, dimension >& cells_lengths() const
        {
            return cells_length_;
        }

        void set_grid_dimensions( std::array< index_t, dimension > cells_number,
            std::array< double, dimension > cells_length )
        {
            cells_number_ = std::move( cells_number );
            cells_length_ = std::move( cells_length );
            for( const auto direction : LRange{ dimension } )
            {
                OPENGEODE_EXCEPTION( cells_length_[direction] > global_epsilon,
                    "[Grid] Creation of a grid with a cell length "
                    "smaller than epsilon in direction ",
                    direction, "." );
            }
            double nb_vertices_double{ 1 };
            for( const auto c : cells_number_ )
            {
                nb_vertices_double *= static_cast< double >( c + 1 );
            }
            OPENGEODE_EXCEPTION(
                nb_vertices_double < static_cast< double >( UINT_MAX ),
                "[Grid] Creation of a grid for which the number of cell "
                "vertices exceeds the unsigned int limit." );
            OPENGEODE_EXCEPTION( nb_cells() != 0,
                "[Grid] Creation of a grid with no cells "
                "in one direction." );
        }

        void copy( const Grid< dimension >::Impl& impl )
        {
            cells_number_ = impl.cells_number_;
            cells_length_ = impl.cells_length_;
        }

    private:
        double point_local_grid_coordinate(
            const geode::Point< dimension >& origin,
            const geode::Point< dimension >& query,
            local_index_t axis ) const
        {
            return ( query.value( axis ) - origin.value( axis ) )
                   / cells_length_[axis];
        }

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.container4b( impl.cells_number_ );
                    a.container8b( impl.cells_length_ );
                } );
        }

    private:
        std::array< index_t, dimension > cells_number_;
        std::array< double, dimension > cells_length_;
    };

    template < index_t dimension >
    Grid< dimension >::Grid()
    {
    }

    template < index_t dimension >
    Grid< dimension >::Grid( Grid&& other ) : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Grid< dimension >::~Grid() // NOLINT
    {
    }

    template < index_t dimension >
    index_t Grid< dimension >::nb_cells() const
    {
        return impl_->nb_cells();
    }

    template < index_t dimension >
    index_t Grid< dimension >::nb_cells_in_direction( index_t direction ) const
    {
        return impl_->nb_cells_in_direction( direction );
    }

    template < index_t dimension >
    double Grid< dimension >::cell_length_in_direction(
        index_t direction ) const
    {
        return impl_->cell_length_in_direction( direction );
    }

    template < index_t dimension >
    double Grid< dimension >::cell_size() const
    {
        return impl_->cell_size();
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > > Grid< dimension >::next_cell(
        const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->next_cell( index, direction );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
        Grid< dimension >::previous_cell(
            const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->previous_cell( index, direction );
    }

    template < index_t dimension >
    bool Grid< dimension >::is_cell_on_border(
        const GridVertexIndices< dimension >& cell_indices ) const
    {
        return impl_->is_cell_on_border( cell_indices );
    }

    template < index_t dimension >
    index_t Grid< dimension >::nb_vertices_in_direction(
        index_t direction ) const
    {
        return impl_->nb_vertices_in_direction( direction );
    }

    template < index_t dimension >
    index_t Grid< dimension >::nb_vertices_on_borders() const
    {
        return impl_->nb_vertices_on_borders();
    }

    template < index_t dimension >
    GridCellVertices< dimension > Grid< dimension >::cell_vertices(
        const GridCellIndices< dimension >& cell_id ) const
    {
        return impl_->cell_vertices( cell_id );
    }

    template < index_t dimension >
    GridVertexIndices< dimension > Grid< dimension >::cell_vertex_indices(
        const GridCellIndices< dimension >& cell_id,
        local_index_t vertex_id ) const
    {
        return impl_->cell_vertex_indices( cell_id, vertex_id );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
        Grid< dimension >::next_vertex(
            const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->next_vertex( index, direction );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
        Grid< dimension >::previous_vertex(
            const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->previous_vertex( index, direction );
    }

    template < index_t dimension >
    bool Grid< dimension >::contains( const Point< dimension >& query ) const
    {
        return impl_->contains( origin(), query );
    }

    template < index_t dimension >
    GridVertexIndices< dimension > Grid< dimension >::closest_vertex(
        const Point< dimension >& query ) const
    {
        return impl_->closest_vertex( origin(), query );
    }

    template < index_t dimension >
    GridCellsAroundVertex< dimension > Grid< dimension >::cells(
        const Point< dimension >& query ) const
    {
        return impl_->cells( origin(), query );
    }

    template < index_t dimension >
    void Grid< dimension >::set_grid_dimensions(
        std::array< index_t, dimension > cells_number,
        std::array< double, dimension > cells_length,
        GridKey )
    {
        return impl_->set_grid_dimensions(
            std::move( cells_number ), std::move( cells_length ) );
    }

    template < index_t dimension >
    void Grid< dimension >::copy( const Grid< dimension >& grid, GridKey )
    {
        impl_->copy( *grid.impl_ );
    }

    template < index_t dimension >
    template < typename Archive >
    void Grid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, Grid >{},
            []( Archive& a, Grid& grid ) {
                a.object( grid.impl_ );
            } );
    }

    template class opengeode_mesh_api Grid< 2 >;
    template class opengeode_mesh_api Grid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Grid< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Grid< 3 > );
} // namespace geode
