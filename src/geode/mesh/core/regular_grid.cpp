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

#include <geode/mesh/core/regular_grid.h>

#include <absl/container/inlined_vector.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bounding_box.h>
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
    class RegularGrid< dimension >::Impl
    {
    public:
        Impl() = default;

        Impl( Point< dimension > origin,
            std::array< index_t, dimension > cells_number,
            std::array< double, dimension > cells_length )
            : origin_( std::move( origin ) ),
              cells_number_( std::move( cells_number ) ),
              cells_length_( std::move( cells_length ) )
        {
            double nb_cells_double{ 1 };
            for( const auto c : cells_number )
            {
                nb_cells_double *= static_cast< double >( c + 1 );
            }
            OPENGEODE_EXCEPTION(
                nb_cells_double < static_cast< double >( UINT_MAX ),
                "[RegularGrid] Creation of a grid for which the number of cell "
                "vertices exceeds the unsigned int limit. " );
            index_t nb_cells{ 1 };
            for( const auto c : cells_number )
            {
                nb_cells *= c;
            }
            cell_attribute_manager_.resize( nb_cells );
            resize_vertex_attribute_manager_to_right_size();
        }

        const Point< dimension >& origin() const
        {
            return origin_;
        }

        AttributeManager& cell_attribute_manager() const
        {
            return cell_attribute_manager_;
        }

        AttributeManager& vertex_attribute_manager() const
        {
            return vertex_attribute_manager_;
        }

        index_t nb_cells( index_t direction ) const
        {
            return cells_number_.at( direction );
        }

        double cell_length( index_t direction ) const
        {
            return cells_length_.at( direction );
        }

        double cell_size() const
        {
            double cell_size{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                cell_size *= cell_length( d );
            }
            return cell_size;
        }

        index_t cell_index( const GridCellIndices< dimension >& index ) const
        {
            index_t cell_id{ 0 };
            for( const auto d : LRange{ dimension } )
            {
                OPENGEODE_ASSERT( index[d] < cells_number_[d],
                    "[RegularGrid::cell_index] Invalid index" );

                index_t offset{ 1 };
                for( const auto d2 : LRange{ d } )
                {
                    offset *= cells_number_[d2];
                }
                cell_id += offset * index[d];
            }
            return cell_id;
        }

        GridCellIndices< dimension > cell_indices( index_t index ) const
        {
            OPENGEODE_ASSERT( index < cell_attribute_manager().nb_elements(),
                "[RegularGrid::cell_index] Invalid index" );
            GridCellIndices< dimension > cell_id;
            for( const auto d : LRange{ dimension } )
            {
                index_t offset{ 1 };
                for( const auto d2 : LRange{ dimension - d - 1 } )
                {
                    offset *= cells_number_[d2];
                }
                const auto value =
                    static_cast< index_t >( std::floor( index / offset ) );
                cell_id[dimension - d - 1] = value;
                index -= value * offset;
            }
            return cell_id;
        }

        absl::optional< GridCellIndices< dimension > > next_cell(
            const GridCellIndices< dimension >& index, index_t direction ) const
        {
            if( index[direction] + 1 < nb_cells( direction ) )
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

        index_t nb_vertices() const
        {
            return vertex_attribute_manager_.nb_elements();
        }

        index_t nb_vertices( index_t direction ) const
        {
            return cells_number_.at( direction ) + 1;
        }

        index_t nb_vertices_on_borders() const
        {
            index_t nb_inside_vertices{ 1 };
            for( const auto d : LRange{ dimension } )
            {
                if( nb_vertices( d ) < 2 )
                {
                    nb_inside_vertices *= nb_vertices( d );
                }
                else
                {
                    nb_inside_vertices *= nb_vertices( d ) - 2;
                }
            }
            return nb_vertices() - nb_inside_vertices;
        }

        index_t vertex_index(
            const GridVertexIndices< dimension >& index ) const
        {
            index_t vertex_id{ 0 };
            for( const auto d : LRange{ dimension } )
            {
                OPENGEODE_ASSERT( index[d] < nb_vertices( d ),
                    "[RegularGrid::vertex_index] Invalid index" );

                index_t offset{ 1 };
                for( const auto d2 : LRange{ d } )
                {
                    offset *= nb_vertices( d2 );
                }
                vertex_id += offset * index[d];
            }
            return vertex_id;
        }

        GridVertexIndices< dimension > vertex_indices( index_t index ) const
        {
            OPENGEODE_ASSERT( index < vertex_attribute_manager().nb_elements(),
                "[RegularGrid::vertex_index] Invalid index" );
            GridVertexIndices< dimension > vertex_id;
            for( const auto d : LRange{ dimension } )
            {
                index_t offset{ 1 };
                for( const auto d2 : LRange{ dimension - d - 1 } )
                {
                    offset *= nb_vertices( d2 );
                }
                const auto value =
                    static_cast< index_t >( std::floor( index / offset ) );
                vertex_id[dimension - d - 1] = value;
                index -= value * offset;
            }
            return vertex_id;
        }

        std::array< GridVertexIndices< dimension >,
            RegularGrid< dimension >::nb_cell_vertices_static() >
            cell_vertices( const GridCellIndices< dimension >& cell_id ) const
        {
            std::array< GridVertexIndices< dimension >,
                nb_cell_vertices_static() >
                cell_vertices;
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
            if( index[direction] + 1 < nb_vertices( direction ) )
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

        bool is_vertex_on_border(
            const GridVertexIndices< dimension >& vertex_index ) const
        {
            for( const auto d : LRange{ dimension } )
            {
                if( vertex_index[d] == 0
                    || vertex_index[d] == nb_vertices( d ) - 1 )
                {
                    return true;
                }
            }
            return false;
        }

        Point< dimension > point(
            const GridVertexIndices< dimension >& index ) const
        {
            Point< dimension > translation;
            for( const auto d : LRange{ dimension } )
            {
                OPENGEODE_ASSERT( index[d] < cells_number_[d] + 1,
                    "[RegularGrid::point] Invalid index" );
                translation.set_value( d, cells_length_[d] * index[d] );
            }
            return origin_ + translation;
        }

        Point< dimension > cell_barycenter(
            const GridCellIndices< dimension >& index ) const
        {
            Point< dimension > translation;
            for( const auto d : LRange{ dimension } )
            {
                OPENGEODE_ASSERT( index[d] < cells_number_[d],
                    "[RegularGrid::point] Invalid index" );
                translation.set_value(
                    d, cells_length_[d] * ( index[d] + 0.5 ) );
            }
            return origin_ + translation;
        }

        absl::optional< GridCellsAroundVertex< dimension > > cell(
            const Point< dimension >& query ) const
        {
            GridCellIndices< dimension > min;
            GridCellIndices< dimension > max;
            for( const auto d : LRange{ dimension } )
            {
                const auto value = ( query.value( d ) - origin_.value( d ) )
                                   / cells_length_[d];
                if( value < 0 || value >= cells_number_[d] )
                {
                    return absl::nullopt;
                }
                const auto floating_floor = std::floor( value );
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
                    max[d] = std::min( integer_floor + 1, cells_number_[d] );
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

    private:
        void resize_vertex_attribute_manager_to_right_size()
        {
            index_t nb_vertices{ 1 };
            for( const auto c : cells_number_ )
            {
                nb_vertices *= c + 1;
            }
            vertex_attribute_manager_.resize( nb_vertices );
        }

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& a, Impl& impl ) {
                         a.object( impl.cell_attribute_manager_ );
                         a.object( impl.origin_ );
                         a.container4b( impl.cells_number_ );
                         a.container8b( impl.cells_length_ );
                         impl.resize_vertex_attribute_manager_to_right_size();
                     },
                        []( Archive& a, Impl& impl ) {
                            a.object( impl.cell_attribute_manager_ );
                            a.object( impl.vertex_attribute_manager_ );
                            a.object( impl.origin_ );
                            a.container4b( impl.cells_number_ );
                            a.container8b( impl.cells_length_ );
                        } } } );
        }

    private:
        mutable AttributeManager cell_attribute_manager_;
        mutable AttributeManager vertex_attribute_manager_;
        Point< dimension > origin_;
        std::array< index_t, dimension > cells_number_;
        std::array< double, dimension > cells_length_;
    }; // namespace geode

    template < index_t dimension >
    RegularGrid< dimension >::RegularGrid( Point< dimension > origin,
        std::array< index_t, dimension > cells_number,
        std::array< double, dimension > cells_length )
        : impl_( std::move( origin ),
            std::move( cells_number ),
            std::move( cells_length ) )
    {
    }

    template < index_t dimension >
    RegularGrid< dimension >::RegularGrid( Point< dimension > origin,
        std::array< index_t, dimension > cells_number,
        double cells_length )
        : RegularGrid(
            std::move( origin ), std::move( cells_number ), [&cells_length]() {
                std::array< double, dimension > size;
                size.fill( cells_length );
                return size;
            }() )
    {
    }

    template < index_t dimension >
    RegularGrid< dimension >::RegularGrid()
    {
    }

    template < index_t dimension >
    RegularGrid< dimension >::RegularGrid( RegularGrid&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    RegularGrid< dimension >::~RegularGrid() // NOLINT
    {
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::nb_cells() const
    {
        return cell_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::nb_cells( index_t direction ) const
    {
        return impl_->nb_cells( direction );
    }

    template < index_t dimension >
    double RegularGrid< dimension >::cell_length( index_t direction ) const
    {
        return impl_->cell_length( direction );
    }

    template < index_t dimension >
    double RegularGrid< dimension >::cell_size() const
    {
        return impl_->cell_size();
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::cell_index(
        const GridCellIndices< dimension >& index ) const
    {
        return impl_->cell_index( index );
    }

    template < index_t dimension >
    GridCellIndices< dimension > RegularGrid< dimension >::cell_indices(
        index_t index ) const
    {
        return impl_->cell_indices( index );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
        RegularGrid< dimension >::next_cell(
            const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->next_cell( index, direction );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
        RegularGrid< dimension >::previous_cell(
            const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->previous_cell( index, direction );
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::nb_vertices() const
    {
        return impl_->nb_vertices();
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::nb_vertices( index_t direction ) const
    {
        return impl_->nb_vertices( direction );
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::nb_vertices_on_borders() const
    {
        return impl_->nb_vertices_on_borders();
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::vertex_index(
        const GridVertexIndices< dimension >& index ) const
    {
        return impl_->vertex_index( index );
    }

    template < index_t dimension >
    GridVertexIndices< dimension > RegularGrid< dimension >::vertex_indices(
        index_t index ) const
    {
        return impl_->vertex_indices( index );
    }

    template < index_t dimension >
    std::array< GridVertexIndices< dimension >,
        RegularGrid< dimension >::nb_cell_vertices_static() >
        RegularGrid< dimension >::cell_vertices(
            const GridCellIndices< dimension >& cell_id ) const
    {
        return impl_->cell_vertices( cell_id );
    }

    template < index_t dimension >
    GridVertexIndices< dimension >
        RegularGrid< dimension >::cell_vertex_indices(
            const GridCellIndices< dimension >& cell_id,
            local_index_t vertex_id ) const
    {
        return impl_->cell_vertex_indices( cell_id, vertex_id );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
        RegularGrid< dimension >::next_vertex(
            const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->next_vertex( index, direction );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
        RegularGrid< dimension >::previous_vertex(
            const GridCellIndices< dimension >& index, index_t direction ) const
    {
        return impl_->previous_vertex( index, direction );
    }

    template < index_t dimension >
    bool RegularGrid< dimension >::is_vertex_on_border(
        const GridVertexIndices< dimension >& vertex_index ) const
    {
        return impl_->is_vertex_on_border( vertex_index );
    }

    template < index_t dimension >
    absl::optional< GridCellsAroundVertex< dimension > >
        RegularGrid< dimension >::cell( const Point< dimension >& query ) const
    {
        return impl_->cell( query );
    }

    template < index_t dimension >
    const Point< dimension >& RegularGrid< dimension >::origin() const
    {
        return impl_->origin();
    }

    template < index_t dimension >
    Point< dimension > RegularGrid< dimension >::point(
        const GridVertexIndices< dimension >& index ) const
    {
        return impl_->point( index );
    }

    template < index_t dimension >
    Point< dimension > RegularGrid< dimension >::cell_barycenter(
        const GridCellIndices< dimension >& index ) const
    {
        return impl_->cell_barycenter( index );
    }

    template < index_t dimension >
    AttributeManager& RegularGrid< dimension >::cell_attribute_manager() const
    {
        return impl_->cell_attribute_manager();
    }

    template < index_t dimension >
    AttributeManager& RegularGrid< dimension >::vertex_attribute_manager() const
    {
        return impl_->vertex_attribute_manager();
    }

    template < index_t dimension >
    BoundingBox< dimension > RegularGrid< dimension >::bounding_box() const
    {
        BoundingBox< dimension > bbox;
        bbox.add_point( this->origin() );
        Point< dimension > extreme_point;
        for( const auto d : LRange{ dimension } )
        {
            extreme_point.set_value(
                d, origin().value( d ) + nb_cells( d ) * cell_length( d ) );
        }
        bbox.add_point( extreme_point );
        return bbox;
    }

    template < index_t dimension >
    RegularGrid< dimension > RegularGrid< dimension >::clone() const
    {
        RegularGrid< dimension > clone{ origin(), impl_->cells_numbers(),
            impl_->cells_lengths() };
        clone.cell_attribute_manager().copy( cell_attribute_manager() );
        clone.vertex_attribute_manager().copy( vertex_attribute_manager() );
        return clone;
    }

    template < index_t dimension >
    template < typename Archive >
    void RegularGrid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, RegularGrid >{},
            []( Archive& a, RegularGrid& regular_grid ) {
                a.object( regular_grid.impl_ );
            } );
    }

    template class opengeode_mesh_api RegularGrid< 2 >;
    template class opengeode_mesh_api RegularGrid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, RegularGrid< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, RegularGrid< 3 > );
} // namespace geode
