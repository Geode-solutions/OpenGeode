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

#include <geode/mesh/core/light_regular_grid.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/grid_builder.h>
#include <geode/mesh/core/private/grid_impl.h>

namespace geode
{
    template < index_t dimension >
    class LightRegularGrid< dimension >::Impl
        : public detail::GridImpl< dimension >
    {
    public:
        Impl() = default;

        AttributeManager& cell_attribute_manager() const
        {
            return cell_attribute_manager_;
        }

        void initialize_attribute_manager( index_t nb_cells )
        {
            cell_attribute_manager_.resize( nb_cells );
        }

    private:
        mutable AttributeManager cell_attribute_manager_;
    };

    template < index_t dimension >
    LightRegularGrid< dimension >::LightRegularGrid( Point< dimension > origin,
        std::array< index_t, dimension > cells_number,
        std::array< double, dimension > cells_length )
    {
        auto builder = GridBuilder< dimension >{ *this };
        builder.set_grid_origin( std::move( origin ) );
        builder.set_grid_dimensions(
            std::move( cells_number ), std::move( cells_length ) );
        impl_->initialize_attribute_manager( this->nb_cells() );
    }

    template < index_t dimension >
    LightRegularGrid< dimension >::LightRegularGrid( LightRegularGrid&& other )
        : Grid< dimension >( std::move( other ) ),
          impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    LightRegularGrid< dimension >::~LightRegularGrid() // NOLINT
    {
    }

    template < index_t dimension >
    index_t LightRegularGrid< dimension >::vertex_index(
        const VertexIndices& index ) const
    {
        return impl_->vertex_index( *this, index );
    }

    template < index_t dimension >
    auto LightRegularGrid< dimension >::vertex_indices( index_t index ) const
        -> VertexIndices
    {
        return impl_->vertex_indices( *this, index );
    }

    template < index_t dimension >
    index_t LightRegularGrid< dimension >::cell_index(
        const CellIndices& index ) const
    {
        return impl_->cell_index( *this, index );
    }

    template < index_t dimension >
    auto LightRegularGrid< dimension >::cell_indices( index_t index ) const
        -> CellIndices
    {
        return impl_->cell_indices( *this, index );
    }

    template < index_t dimension >
    AttributeManager&
        LightRegularGrid< dimension >::cell_attribute_manager() const
    {
        return impl_->cell_attribute_manager();
    }

    template class opengeode_mesh_api LightRegularGrid< 2 >;
    template class opengeode_mesh_api LightRegularGrid< 3 >;
} // namespace geode
