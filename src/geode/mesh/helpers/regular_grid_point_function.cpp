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

#include <geode/mesh/helpers/regular_grid_point_function.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>
#include <geode/mesh/helpers/private/regular_grid_shape_function.h>

namespace geode
{
    template < index_t dimension, index_t point_dimension >
    class RegularGridPointFunction< dimension, point_dimension >::Impl
    {
    public:
        Impl( const RegularGrid< dimension >& grid,
            absl::string_view function_name,
            Point< point_dimension > value )
            : grid_( grid )
        {
            OPENGEODE_EXCEPTION(
                !grid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create RegularGridPointFunction: attribute with name",
                function_name, " already exists." );
            function_attribute_ =
                grid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        Point< point_dimension > >(
                        function_name, std::move( value ), { false, true } );
        }

        Impl( const RegularGrid< dimension >& grid,
            absl::string_view function_name )
            : grid_( grid )
        {
            OPENGEODE_EXCEPTION(
                grid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create RegularGridPointFunction: attribute with name",
                function_name, " does not exist." );
            function_attribute_ =
                grid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        Point< point_dimension > >( function_name,
                        Point< point_dimension >(), { false, true } );
        }

        void set_value(
            const typename Grid< dimension >::VertexIndices& vertex_id,
            Point< point_dimension > value )
        {
            function_attribute_->set_value(
                grid_.vertex_index( vertex_id ), std::move( value ) );
        }

        void set_value( index_t vertex_id, Point< point_dimension > value )
        {
            function_attribute_->set_value( vertex_id, std::move( value ) );
        }

        const Point< point_dimension >& value( index_t vertex_id ) const
        {
            return function_attribute_->value( vertex_id );
        }

        const Point< point_dimension >& value(
            const typename Grid< dimension >::VertexIndices& vertex_id ) const
        {
            return function_attribute_->value(
                grid_.vertex_index( vertex_id ) );
        }

        Point< point_dimension > value( const Point< dimension >& point,
            const typename Grid< dimension >::CellIndices& grid_cell_indices )
            const
        {
            Point< point_dimension > point_value;
            for( const auto node_id : LRange{ grid_.nb_cell_vertices() } )
            {
                point_value += function_attribute_->value( grid_.vertex_index(
                                   grid_.cell_vertex_indices(
                                       grid_cell_indices, node_id ) ) )
                               * detail::shape_function_value< dimension >(
                                   grid_, grid_cell_indices, node_id, point );
            }
            return point_value;
        }

    private:
        const RegularGrid< dimension >& grid_;
        std::shared_ptr< VariableAttribute< Point< point_dimension > > >
            function_attribute_;
    };

    template < index_t dimension, index_t point_dimension >
    RegularGridPointFunction< dimension, point_dimension >::
        RegularGridPointFunction(
            RegularGridPointFunction< dimension, point_dimension >&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension, index_t point_dimension >
    RegularGridPointFunction< dimension, point_dimension >::
        RegularGridPointFunction( const RegularGrid< dimension >& grid,
            absl::string_view function_name,
            Point< point_dimension > value )
        : impl_{ grid, function_name, value }
    {
    }

    template < index_t dimension, index_t point_dimension >
    RegularGridPointFunction< dimension, point_dimension >::
        RegularGridPointFunction( const RegularGrid< dimension >& grid,
            absl::string_view function_name )
        : impl_{ grid, function_name }
    {
    }

    template < index_t dimension, index_t point_dimension >
    RegularGridPointFunction< dimension,
        point_dimension >::~RegularGridPointFunction()
    {
    }

    template < index_t dimension, index_t point_dimension >
    RegularGridPointFunction< dimension, point_dimension >
        RegularGridPointFunction< dimension, point_dimension >::create(
            const RegularGrid< dimension >& grid,
            absl::string_view function_name,
            Point< point_dimension > value )
    {
        return { grid, function_name, value };
    }

    template < index_t dimension, index_t point_dimension >
    RegularGridPointFunction< dimension, point_dimension >
        RegularGridPointFunction< dimension, point_dimension >::find(
            const RegularGrid< dimension >& grid,
            absl::string_view function_name )
    {
        return { grid, function_name };
    }

    template < index_t dimension, index_t point_dimension >
    void RegularGridPointFunction< dimension, point_dimension >::set_value(
        const typename Grid< dimension >::VertexIndices& vertex_index,
        Point< point_dimension > value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension, index_t point_dimension >
    void RegularGridPointFunction< dimension, point_dimension >::set_value(
        index_t vertex_index, Point< point_dimension > value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension, index_t point_dimension >
    const Point< point_dimension >&
        RegularGridPointFunction< dimension, point_dimension >::value(
            const typename Grid< dimension >::VertexIndices& vertex_index )
            const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension, index_t point_dimension >
    const Point< point_dimension >&
        RegularGridPointFunction< dimension, point_dimension >::value(
            index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension, index_t point_dimension >
    Point< point_dimension > RegularGridPointFunction< dimension,
        point_dimension >::value( const Point< dimension >& point,
        const typename Grid< dimension >::CellIndices& grid_cell_indices ) const
    {
        return impl_->value( point, grid_cell_indices );
    }

    template class opengeode_mesh_api RegularGridPointFunction< 2, 2 >;
    template class opengeode_mesh_api RegularGridPointFunction< 2, 1 >;
    template class opengeode_mesh_api RegularGridPointFunction< 3, 2 >;
    template class opengeode_mesh_api RegularGridPointFunction< 3, 3 >;
} // namespace geode