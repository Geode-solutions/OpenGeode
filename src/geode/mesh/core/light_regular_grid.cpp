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

#include <geode/mesh/core/light_regular_grid.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/grid_builder.hpp>
#include <geode/mesh/core/internal/grid_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class LightRegularGrid< dimension >::Impl
        : public internal::GridImpl< dimension >
    {
    public:
        Impl() = default;

        AttributeManager& cell_attribute_manager() const
        {
            return cell_attribute_manager_;
        }

        AttributeManager& vertex_attribute_manager() const
        {
            return vertex_attribute_manager_;
        }

        void initialize_attribute_managers(
            index_t nb_cells, index_t nb_vertices )
        {
            cell_attribute_manager_.resize( nb_cells );
            vertex_attribute_manager_.resize( nb_vertices );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.object( impl.cell_attribute_manager_ );
                    a.object( impl.vertex_attribute_manager_ );
                } } } );
        }

    private:
        mutable AttributeManager cell_attribute_manager_;
        mutable AttributeManager vertex_attribute_manager_;
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
        impl_->initialize_attribute_managers(
            this->nb_cells(), this->nb_grid_vertices() );
    }

    template < index_t dimension >
    LightRegularGrid< dimension >::LightRegularGrid( Point< dimension > origin,
        std::array< index_t, dimension > cells_number,
        std::array< Vector< dimension >, dimension > directions )
    {
        auto builder = GridBuilder< dimension >{ *this };
        builder.set_grid_origin( std::move( origin ) );
        std::array< double, dimension > cells_length;
        for( const auto d : LRange{ dimension } )
        {
            cells_length[d] = directions[d].length();
        }
        builder.set_grid_dimensions(
            std::move( cells_number ), std::move( cells_length ) );
        builder.set_grid_directions( std::move( directions ) );
        impl_->initialize_attribute_managers(
            this->nb_cells(), this->nb_grid_vertices() );
    }

    template < index_t dimension >
    LightRegularGrid< dimension >::LightRegularGrid(
        LightRegularGrid&& ) noexcept = default;

    template < index_t dimension >
    LightRegularGrid< dimension >::~LightRegularGrid() = default;

    template < index_t dimension >
    index_t LightRegularGrid< dimension >::vertex_index(
        const VertexIndices& index ) const
    {
        return impl_->vertex_index( *this, index );
    }

    template < index_t dimension >
    auto LightRegularGrid< dimension >::vertex_indices(
        index_t index ) const -> VertexIndices
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
    auto LightRegularGrid< dimension >::cell_indices(
        index_t index ) const -> CellIndices
    {
        return impl_->cell_indices( *this, index );
    }

    template < index_t dimension >
    Point< dimension > LightRegularGrid< dimension >::point(
        index_t vertex_id ) const
    {
        return this->grid_point( vertex_indices( vertex_id ) );
    }

    template < index_t dimension >
    auto LightRegularGrid< dimension >::vertices_around_vertex(
        index_t vertex_id ) const -> VerticesAroundVertex
    {
        VerticesAroundVertex result;
        const auto indices = this->vertex_indices( vertex_id );
        for( const auto d : LRange{ 2 } )
        {
            if( const auto next = this->next_vertex( indices, d ) )
            {
                result.push_back( this->vertex_index( next.value() ) );
            }
            if( const auto previous = this->previous_vertex( indices, d ) )
            {
                result.push_back( this->vertex_index( previous.value() ) );
            }
        }
        return result;
    }

    template < index_t dimension >
    AttributeManager&
        LightRegularGrid< dimension >::cell_attribute_manager() const
    {
        return impl_->cell_attribute_manager();
    }

    template < index_t dimension >
    AttributeManager&
        LightRegularGrid< dimension >::grid_vertex_attribute_manager() const
    {
        return impl_->vertex_attribute_manager();
    }

    template < index_t dimension >
    template < typename Archive >
    void LightRegularGrid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, LightRegularGrid >{
                { []( Archive& a, LightRegularGrid& grid ) {
                    a.ext(
                        grid, bitsery::ext::BaseClass< Grid< dimension > >{} );
                    a.ext( grid, bitsery::ext::BaseClass< Identifier >{} );
                    a.object( grid.impl_ );
                } } } );
    }

    template class opengeode_mesh_api LightRegularGrid< 2 >;
    template class opengeode_mesh_api LightRegularGrid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, LightRegularGrid< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, LightRegularGrid< 3 > );
} // namespace geode
