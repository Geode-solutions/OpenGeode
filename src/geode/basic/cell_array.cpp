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

#include <geode/basic/cell_array.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    template < index_t dimension >
    class CellArray< dimension >::Impl
    {
    public:
        Impl() = default;
        Impl( std::array< index_t, dimension > cells_number )
            : cells_number_{ std::move( cells_number ) }
        {
        }

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
            return cells_number_[direction];
        }

        absl::optional< CellIndices > next_cell(
            const CellIndices& index, index_t direction ) const
        {
            if( index[direction] + 1 < nb_cells_in_direction( direction ) )
            {
                auto next = index;
                next[direction]++;
                return next;
            }
            return absl::nullopt;
        }

        absl::optional< CellIndices > previous_cell(
            const CellIndices& index, index_t direction ) const
        {
            if( index[direction] > 0 )
            {
                auto prev = index;
                prev[direction]--;
                return prev;
            }
            return absl::nullopt;
        }

        bool is_cell_on_border( const CellIndices& cell_indices ) const
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

        void set_array_dimensions(
            std::array< index_t, dimension > cells_number )
        {
            cells_number_ = std::move( cells_number );
            OPENGEODE_EXCEPTION( nb_cells() != 0,
                "[CellArray] Creation of a array with no cells "
                "in one direction." );
        }

        void copy( const CellArray< dimension >::Impl& impl )
        {
            cells_number_ = impl.cells_number_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.container4b( impl.cells_number_ );
                } );
        }

    private:
        std::array< index_t, dimension > cells_number_;
    };

    template < index_t dimension >
    CellArray< dimension >::CellArray()
    {
    }

    template < index_t dimension >
    CellArray< dimension >::CellArray(
        std::array< index_t, dimension > cells_number )
        : impl_{ std::move( cells_number ) }
    {
    }

    template < index_t dimension >
    CellArray< dimension >::CellArray( CellArray&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    auto CellArray< dimension >::operator=( CellArray&& other ) -> CellArray&
    {
        *impl_ = std::move( *other.impl_ );
        return *this;
    }

    template < index_t dimension >
    CellArray< dimension >::~CellArray() // NOLINT
    {
    }

    template < index_t dimension >
    index_t CellArray< dimension >::nb_cells() const
    {
        return impl_->nb_cells();
    }

    template < index_t dimension >
    index_t CellArray< dimension >::nb_cells_in_direction(
        index_t direction ) const
    {
        return impl_->nb_cells_in_direction( direction );
    }

    template < index_t dimension >
    auto CellArray< dimension >::next_cell( const CellIndices& index,
        index_t direction ) const -> absl::optional< CellIndices >
    {
        return impl_->next_cell( index, direction );
    }

    template < index_t dimension >
    auto CellArray< dimension >::previous_cell( const CellIndices& index,
        index_t direction ) const -> absl::optional< CellIndices >
    {
        return impl_->previous_cell( index, direction );
    }

    template < index_t dimension >
    bool CellArray< dimension >::is_cell_on_border(
        const CellIndices& cell_indices ) const
    {
        return impl_->is_cell_on_border( cell_indices );
    }

    template < index_t dimension >
    void CellArray< dimension >::set_array_dimensions(
        std::array< index_t, dimension > cells_number )
    {
        impl_->set_array_dimensions( std::move( cells_number ) );
    }

    template < index_t dimension >
    void CellArray< dimension >::copy( const CellArray< dimension >& array )
    {
        impl_->copy( *array.impl_ );
    }

    template < index_t dimension >
    template < typename Archive >
    void CellArray< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, CellArray >{},
            []( Archive& a, CellArray& array ) {
                a.object( array.impl_ );
            } );
    }

    template class opengeode_basic_api CellArray< 1 >;
    template class opengeode_basic_api CellArray< 2 >;
    template class opengeode_basic_api CellArray< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, CellArray< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, CellArray< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_basic_api, CellArray< 3 > );
} // namespace geode
