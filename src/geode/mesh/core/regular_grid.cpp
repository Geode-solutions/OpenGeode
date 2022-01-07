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

namespace geode
{
    template < index_t dimension >
    class RegularGrid< dimension >::Impl
    {
    public:
        Impl() = default;

        Impl( Point< dimension > origin,
            std::array< index_t, dimension > cells_number,
            std::array< double, dimension > cells_size )
            : origin_( std::move( origin ) ),
              cells_number_( std::move( cells_number ) ),
              cells_size_( std::move( cells_size ) )
        {
            index_t nb_cells{ 1 };
            for( const auto c : cells_number )
            {
                nb_cells *= c;
            }
            cell_attribute_manager_.resize( nb_cells );
            resize_vertex_attribute_manager_to_right_size();
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

        double cell_size( index_t direction ) const
        {
            return cells_size_.at( direction );
        }

        index_t cell_index( const GridCellIndex< dimension >& index ) const
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

        GridCellIndex< dimension > cell_index( index_t index ) const
        {
            OPENGEODE_ASSERT( index < cell_attribute_manager().nb_elements(),
                "[RegularGrid::cell_index] Invalid index" );
            GridCellIndex< dimension > cell_id;
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

        absl::optional< GridCellIndex< dimension > > next_cell(
            const GridCellIndex< dimension >& index, index_t direction ) const
        {
            if( index[direction] + 1 < nb_cells( direction ) )
            {
                auto next = index;
                next[direction]++;
                return next;
            }
            return absl::nullopt;
        }

        absl::optional< GridCellIndex< dimension > > previous_cell(
            const GridCellIndex< dimension >& index, index_t direction ) const
        {
            if( index[direction] > 0 )
            {
                auto prev = index;
                prev[direction]--;
                return prev;
            }
            return absl::nullopt;
        }

        index_t nb_vertices( index_t direction ) const
        {
            return cells_number_.at( direction ) + 1;
        }

        index_t vertex_index( const GridVertexIndex< dimension >& index ) const
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

        GridVertexIndex< dimension > vertex_index( index_t index ) const
        {
            OPENGEODE_ASSERT( index < vertex_attribute_manager().nb_elements(),
                "[RegularGrid::vertex_index] Invalid index" );
            GridVertexIndex< dimension > vertex_id;
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

        absl::optional< GridVertexIndex< dimension > > next_vertex(
            const GridVertexIndex< dimension >& index, index_t direction ) const
        {
            if( index[direction] + 1 < nb_vertices( direction ) )
            {
                auto next = index;
                next[direction]++;
                return next;
            }
            return absl::nullopt;
        }

        absl::optional< GridVertexIndex< dimension > > previous_vertex(
            const GridVertexIndex< dimension >& index, index_t direction ) const
        {
            if( index[direction] > 0 )
            {
                auto prev = index;
                prev[direction]--;
                return prev;
            }
            return absl::nullopt;
        }

        Point< dimension > point(
            const GridVertexIndex< dimension >& index ) const
        {
            Point< dimension > translation;
            for( const auto d : LRange{ dimension } )
            {
                OPENGEODE_ASSERT( index[d] < cells_number_[d] + 1,
                    "[RegularGrid::point] Invalid index" );
                translation.set_value( d, cells_size_[d] * index[d] );
            }
            return origin_ + translation;
        }

        const Point< dimension >& origin() const
        {
            return origin_;
        }

        absl::optional< GridCellIndices< dimension > > cell(
            const Point< dimension >& query ) const
        {
            GridCellIndices< dimension > indices;
            GridCellIndex< dimension > min;
            GridCellIndex< dimension > max;
            for( const auto d : LRange{ dimension } )
            {
                const auto value =
                    ( query.value( d ) - origin_.value( d ) ) / cells_size_[d];
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
            indices.push_back( min );
            for( const auto d : LRange{ dimension } )
            {
                if( max[d] != min[d] )
                {
                    for( const auto& index : indices )
                    {
                        indices.push_back( index );
                        indices.back()[d] = max[d];
                    }
                }
            }
            return indices;
        }

        const std::array< index_t, dimension >& cells_numbers() const
        {
            return cells_number_;
        }

        const std::array< double, dimension >& cells_sizes() const
        {
            return cells_size_;
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
                         a.container8b( impl.cells_size_ );
                         impl.resize_vertex_attribute_manager_to_right_size();
                     },
                        []( Archive& a, Impl& impl ) {
                            a.object( impl.cell_attribute_manager_ );
                            a.object( impl.vertex_attribute_manager_ );
                            a.object( impl.origin_ );
                            a.container4b( impl.cells_number_ );
                            a.container8b( impl.cells_size_ );
                        } } } );
        }

    private:
        mutable AttributeManager cell_attribute_manager_;
        mutable AttributeManager vertex_attribute_manager_;
        Point< dimension > origin_;
        std::array< index_t, dimension > cells_number_;
        std::array< double, dimension > cells_size_;
    }; // namespace geode

    template < index_t dimension >
    RegularGrid< dimension >::RegularGrid( Point< dimension > origin,
        std::array< index_t, dimension > cells_number,
        std::array< double, dimension > cells_size )
        : impl_( std::move( origin ),
            std::move( cells_number ),
            std::move( cells_size ) )
    {
    }

    template < index_t dimension >
    RegularGrid< dimension >::RegularGrid( Point< dimension > origin,
        std::array< index_t, dimension > cells_number,
        double cells_size )
        : RegularGrid(
            std::move( origin ), std::move( cells_number ), [&cells_size]() {
                std::array< double, dimension > size;
                size.fill( cells_size );
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
    double RegularGrid< dimension >::cell_size( index_t direction ) const
    {
        return impl_->cell_size( direction );
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::cell_index(
        const GridCellIndex< dimension >& index ) const
    {
        return impl_->cell_index( index );
    }

    template < index_t dimension >
    GridCellIndex< dimension > RegularGrid< dimension >::cell_index(
        index_t index ) const
    {
        return impl_->cell_index( index );
    }

    template < index_t dimension >
    absl::optional< GridCellIndex< dimension > >
        RegularGrid< dimension >::next_cell(
            const GridCellIndex< dimension >& index, index_t direction ) const
    {
        return impl_->next_cell( index, direction );
    }

    template < index_t dimension >
    absl::optional< GridCellIndex< dimension > >
        RegularGrid< dimension >::previous_cell(
            const GridCellIndex< dimension >& index, index_t direction ) const
    {
        return impl_->previous_cell( index, direction );
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::nb_vertices() const
    {
        return vertex_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::nb_vertices( index_t direction ) const
    {
        return impl_->nb_vertices( direction );
    }

    template < index_t dimension >
    index_t RegularGrid< dimension >::vertex_index(
        const GridCellIndex< dimension >& index ) const
    {
        return impl_->vertex_index( index );
    }

    template < index_t dimension >
    GridCellIndex< dimension > RegularGrid< dimension >::vertex_index(
        index_t index ) const
    {
        return impl_->vertex_index( index );
    }

    template < index_t dimension >
    absl::optional< GridCellIndex< dimension > >
        RegularGrid< dimension >::next_vertex(
            const GridCellIndex< dimension >& index, index_t direction ) const
    {
        return impl_->next_vertex( index, direction );
    }

    template < index_t dimension >
    absl::optional< GridCellIndex< dimension > >
        RegularGrid< dimension >::previous_vertex(
            const GridCellIndex< dimension >& index, index_t direction ) const
    {
        return impl_->previous_vertex( index, direction );
    }

    template < index_t dimension >
    absl::optional< GridCellIndices< dimension > >
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
        const GridCellIndex< dimension >& index ) const
    {
        return impl_->point( index );
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
                d, origin().value( d ) + nb_cells( d ) * cell_size( d ) );
        }
        bbox.add_point( extreme_point );
        return bbox;
    }

    template < index_t dimension >
    RegularGrid< dimension > RegularGrid< dimension >::clone() const
    {
        RegularGrid< dimension > clone{ origin(), impl_->cells_numbers(),
            impl_->cells_sizes() };
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
