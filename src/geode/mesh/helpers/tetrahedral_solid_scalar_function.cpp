/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/mesh/helpers/tetrahedral_solid_scalar_function.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolidScalarFunction< dimension >::Impl
    {
    public:
        Impl( const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name,
            double value )
            : solid_( solid )
        {
            OPENGEODE_EXCEPTION(
                !solid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TetrahedralSolidScalarFunction: attribute with "
                "name '",
                function_name, "' already exists." );
            function_attribute_ =
                solid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( function_name, value, { false, true } );
        }

        Impl( const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name )
            : solid_( solid )
        {
            OPENGEODE_EXCEPTION(
                solid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TetrahedralSolidScalarFunction: attribute with "
                "name '",
                function_name, "' does not exist." );
            function_attribute_ =
                solid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( function_name, 0 );
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
            const Point< dimension >& point, index_t tetrahedron_id ) const
        {
            double point_value{ 0. };
            const auto tetrahedron = solid_.tetrahedron( tetrahedron_id );
            const auto tetrahedron_vertices =
                solid_.polyhedron_vertices( tetrahedron_id );
            const auto barycentric_coords =
                tetrahedron_barycentric_coordinates( point, tetrahedron );
            for( const auto node_id : LIndices{ barycentric_coords } )
            {
                point_value += barycentric_coords[node_id]
                               * function_attribute_->value(
                                   tetrahedron_vertices[node_id] );
            }
            return point_value;
        }

    private:
        const TetrahedralSolid< dimension >& solid_;
        std::shared_ptr< VariableAttribute< double > > function_attribute_;
    };

    template < index_t dimension >
    TetrahedralSolidScalarFunction< dimension >::TetrahedralSolidScalarFunction(
        TetrahedralSolidScalarFunction< dimension >&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    TetrahedralSolidScalarFunction< dimension >::TetrahedralSolidScalarFunction(
        const TetrahedralSolid< dimension >& solid,
        absl::string_view function_name,
        double value )
        : impl_{ solid, function_name, value }
    {
    }

    template < index_t dimension >
    TetrahedralSolidScalarFunction< dimension >::TetrahedralSolidScalarFunction(
        const TetrahedralSolid< dimension >& solid,
        absl::string_view function_name )
        : impl_{ solid, function_name }
    {
    }

    template < index_t dimension >
    TetrahedralSolidScalarFunction<
        dimension >::~TetrahedralSolidScalarFunction()
    {
    }

    template < index_t dimension >
    TetrahedralSolidScalarFunction< dimension >
        TetrahedralSolidScalarFunction< dimension >::create(
            const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name,
            double value )
    {
        return { solid, function_name, value };
    }

    template < index_t dimension >
    TetrahedralSolidScalarFunction< dimension >
        TetrahedralSolidScalarFunction< dimension >::find(
            const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name )
    {
        return { solid, function_name };
    }

    template < index_t dimension >
    void TetrahedralSolidScalarFunction< dimension >::set_value(
        index_t vertex_index, double value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    double TetrahedralSolidScalarFunction< dimension >::value(
        index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    double TetrahedralSolidScalarFunction< dimension >::value(
        const Point< dimension >& point, index_t tetrahedron_id ) const
    {
        return impl_->value( point, tetrahedron_id );
    }

    template class opengeode_mesh_api TetrahedralSolidScalarFunction< 3 >;
} // namespace geode