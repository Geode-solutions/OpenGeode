/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/helpers/grid_scalar_function.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/grid.hpp>
#include <geode/mesh/helpers/internal/grid_shape_function.hpp>

namespace geode
{
    template < index_t dimension >
    class GridScalarFunction< dimension >::Impl
    {
    public:
        Impl( const Grid< dimension >& grid,
            std::string_view function_name,
            double value )
            : grid_( grid )
        {
            OPENGEODE_EXCEPTION(
                !grid_.grid_vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create GridScalarFunction: attribute with name ",
                function_name, " already exists." );
            function_attribute_ =
                grid_.grid_vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( function_name, value, { false, true } );
        }

        Impl( const Grid< dimension >& grid, std::string_view function_name )
            : grid_( grid )
        {
            OPENGEODE_EXCEPTION(
                grid_.grid_vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create GridScalarFunction: attribute with name",
                function_name, " does not exist." );
            function_attribute_ =
                grid_.grid_vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( function_name, 0, { false, true } );
        }

        void set_value(
            const typename Grid< dimension >::VertexIndices& vertex_id,
            double value )
        {
            function_attribute_->set_value(
                grid_.vertex_index( vertex_id ), value );
        }

        void set_value( index_t vertex_id, double value )
        {
            function_attribute_->set_value( vertex_id, value );
        }

        double value( index_t vertex_id ) const
        {
            return function_attribute_->value( vertex_id );
        }

        double value(
            const typename Grid< dimension >::VertexIndices& vertex_id ) const
        {
            return function_attribute_->value(
                grid_.vertex_index( vertex_id ) );
        }

        double value( const Point< dimension >& point,
            const typename Grid< dimension >::CellIndices& grid_cell_indices )
            const
        {
            double point_value{ 0. };
            const auto point_in_grid =
                grid_.grid_coordinate_system().coordinates( point );
            for( const auto node_id : LRange{ grid_.nb_cell_vertices() } )
            {
                point_value += internal::shape_function_value< dimension >(
                                   grid_cell_indices, node_id, point_in_grid )
                               * function_attribute_->value( grid_.vertex_index(
                                   grid_.cell_vertex_indices(
                                       grid_cell_indices, node_id ) ) );
            }
            return point_value;
        }

    private:
        const Grid< dimension >& grid_;
        std::shared_ptr< VariableAttribute< double > > function_attribute_;
    };

    template < index_t dimension >
    GridScalarFunction< dimension >::GridScalarFunction(
        GridScalarFunction< dimension >&& ) noexcept = default;

    template < index_t dimension >
    GridScalarFunction< dimension >::GridScalarFunction(
        const Grid< dimension >& grid,
        std::string_view function_name,
        double value )
        : impl_{ grid, function_name, value }
    {
    }

    template < index_t dimension >
    GridScalarFunction< dimension >::GridScalarFunction(
        const Grid< dimension >& grid, std::string_view function_name )
        : impl_{ grid, function_name }
    {
    }

    template < index_t dimension >
    GridScalarFunction< dimension >::~GridScalarFunction() = default;

    template < index_t dimension >
    GridScalarFunction< dimension > GridScalarFunction< dimension >::create(
        const Grid< dimension >& grid,
        std::string_view function_name,
        double value )
    {
        return { grid, function_name, value };
    }

    template < index_t dimension >
    GridScalarFunction< dimension > GridScalarFunction< dimension >::find(
        const Grid< dimension >& grid, std::string_view function_name )
    {
        return { grid, function_name };
    }

    template < index_t dimension >
    void GridScalarFunction< dimension >::set_value(
        const typename Grid< dimension >::VertexIndices& vertex_index,
        double value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    void GridScalarFunction< dimension >::set_value(
        index_t vertex_index, double value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    double GridScalarFunction< dimension >::value(
        const typename Grid< dimension >::VertexIndices& vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    double GridScalarFunction< dimension >::value( index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    double GridScalarFunction< dimension >::value(
        const Point< dimension >& point,
        const typename Grid< dimension >::CellIndices& grid_cell_indices ) const
    {
        return impl_->value( point, grid_cell_indices );
    }

    template class opengeode_mesh_api GridScalarFunction< 2 >;
    template class opengeode_mesh_api GridScalarFunction< 3 >;
} // namespace geode