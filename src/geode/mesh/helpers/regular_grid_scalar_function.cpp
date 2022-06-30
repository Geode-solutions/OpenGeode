/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/mesh/helpers/regular_grid_scalar_function.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/helpers/private/regular_grid_shape_function.h>

namespace geode
{
    template < index_t dimension >
    class RegularGridScalarFunction< dimension >::Impl
    {
    public:
        Impl( const RegularGrid< dimension >& grid,
            absl::string_view function_name,
            double value )
            : grid_( grid )
        {
            OPENGEODE_EXCEPTION(
                !grid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create RegularGridScalarFunction: attribute with name",
                function_name, " already exists." );
            function_attribute_ =
                grid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( function_name, value, { false, true } );
        }

        Impl( const RegularGrid< dimension >& grid,
            absl::string_view function_name )
            : grid_( grid )
        {
            OPENGEODE_EXCEPTION(
                grid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create RegularGridScalarFunction: attribute with name",
                function_name, " does not exist." );
            function_attribute_ =
                grid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( function_name, 0 );
        }

        void set_value(
            const GridVertexIndices< dimension >& vertex_id, double value )
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

        double value( const GridVertexIndices< dimension >& vertex_id ) const
        {
            return function_attribute_->value(
                grid_.vertex_index( vertex_id ) );
        }

        double value( const Point< dimension >& point,
            const GridCellIndices< dimension >& grid_cell_indices ) const
        {
            double point_value{ 0. };
            for( const auto node_id : LRange{ grid_.nb_cell_vertices() } )
            {
                point_value += detail::shape_function_value< dimension >(
                                   grid_, grid_cell_indices, node_id, point )
                               * function_attribute_->value( grid_.vertex_index(
                                   detail::cell_node_index< dimension >(
                                       grid_cell_indices, node_id ) ) );
            }
            return point_value;
        }

    private:
        const RegularGrid< dimension >& grid_;
        std::shared_ptr< VariableAttribute< double > > function_attribute_;
    };

    template < index_t dimension >
    RegularGridScalarFunction< dimension >::RegularGridScalarFunction(
        RegularGridScalarFunction< dimension >&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    RegularGridScalarFunction< dimension >::RegularGridScalarFunction(
        const RegularGrid< dimension >& grid,
        absl::string_view function_name,
        double value )
        : impl_{ grid, function_name, value }
    {
    }

    template < index_t dimension >
    RegularGridScalarFunction< dimension >::RegularGridScalarFunction(
        const RegularGrid< dimension >& grid, absl::string_view function_name )
        : impl_{ grid, function_name }
    {
    }

    template < index_t dimension >
    RegularGridScalarFunction< dimension >::~RegularGridScalarFunction()
    {
    }

    template < index_t dimension >
    RegularGridScalarFunction< dimension >
        RegularGridScalarFunction< dimension >::create(
            const RegularGrid< dimension >& grid,
            absl::string_view function_name,
            double value )
    {
        return { grid, function_name, value };
    }

    template < index_t dimension >
    RegularGridScalarFunction< dimension >
        RegularGridScalarFunction< dimension >::find(
            const RegularGrid< dimension >& grid,
            absl::string_view function_name )
    {
        return { grid, function_name };
    }

    template < index_t dimension >
    void RegularGridScalarFunction< dimension >::set_value(
        const GridVertexIndices< dimension >& vertex_index, double value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    void RegularGridScalarFunction< dimension >::set_value(
        index_t vertex_index, double value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    double RegularGridScalarFunction< dimension >::value(
        const GridVertexIndices< dimension >& vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    double RegularGridScalarFunction< dimension >::value(
        index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    double RegularGridScalarFunction< dimension >::value(
        const Point< dimension >& point,
        const GridCellIndices< dimension >& grid_cell_indices ) const
    {
        return impl_->value( point, grid_cell_indices );
    }

    template class opengeode_mesh_api RegularGridScalarFunction< 2 >;
    template class opengeode_mesh_api RegularGridScalarFunction< 3 >;
} // namespace geode