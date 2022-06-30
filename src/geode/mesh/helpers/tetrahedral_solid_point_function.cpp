/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/mesh/helpers/tetrahedral_solid_point_function.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolidPointFunction< dimension >::Impl
    {
    public:
        Impl( const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name,
            Point< dimension > value )
            : solid_( solid )
        {
            OPENGEODE_EXCEPTION(
                !solid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TetrahedralSolidPointFunction: attribute with "
                "name '",
                function_name, "' already exists." );
            function_attribute_ =
                solid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        Point< dimension > >(
                        function_name, std::move( value ), { false, true } );
        }

        Impl( const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name )
            : solid_( solid )
        {
            OPENGEODE_EXCEPTION(
                solid_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TetrahedralSolidPointFunction: attribute with "
                "name '",
                function_name, "' does not exist." );
            function_attribute_ =
                solid_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        Point< dimension > >(
                        function_name, Point< dimension >() );
        }

        void set_value( index_t vertex_id, Point< dimension > value )
        {
            function_attribute_->set_value( vertex_id, std::move( value ) );
        }

        const Point< dimension >& value( index_t vertex_id ) const
        {
            return function_attribute_->value( vertex_id );
        }

        Point< dimension > value(
            const Point< dimension >& point, index_t tetrahedron_id ) const
        {
            Point< dimension > point_value;
            const auto tetrahedron = solid_.tetrahedron( tetrahedron_id );
            const auto tetrahedron_vertices =
                solid_.polyhedron_vertices( tetrahedron_id );
            const auto barycentric_coords =
                tetrahedron_barycentric_coordinates( point, tetrahedron );
            for( const auto node_id : LIndices{ barycentric_coords } )
            {
                point_value +=
                    function_attribute_->value( tetrahedron_vertices[node_id] )
                    * barycentric_coords[node_id];
            }
            return point_value;
        }

    private:
        const TetrahedralSolid< dimension >& solid_;
        std::shared_ptr< VariableAttribute< Point< dimension > > >
            function_attribute_;
    };

    template < index_t dimension >
    TetrahedralSolidPointFunction< dimension >::TetrahedralSolidPointFunction(
        TetrahedralSolidPointFunction< dimension >&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    TetrahedralSolidPointFunction< dimension >::TetrahedralSolidPointFunction(
        const TetrahedralSolid< dimension >& solid,
        absl::string_view function_name,
        Point< dimension > value )
        : impl_{ solid, function_name, value }
    {
    }

    template < index_t dimension >
    TetrahedralSolidPointFunction< dimension >::TetrahedralSolidPointFunction(
        const TetrahedralSolid< dimension >& solid,
        absl::string_view function_name )
        : impl_{ solid, function_name }
    {
    }

    template < index_t dimension >
    TetrahedralSolidPointFunction< dimension >::~TetrahedralSolidPointFunction()
    {
    }

    template < index_t dimension >
    TetrahedralSolidPointFunction< dimension >
        TetrahedralSolidPointFunction< dimension >::create(
            const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name,
            Point< dimension > value )
    {
        return { solid, function_name, value };
    }

    template < index_t dimension >
    TetrahedralSolidPointFunction< dimension >
        TetrahedralSolidPointFunction< dimension >::find(
            const TetrahedralSolid< dimension >& solid,
            absl::string_view function_name )
    {
        return { solid, function_name };
    }

    template < index_t dimension >
    void TetrahedralSolidPointFunction< dimension >::set_value(
        index_t vertex_index, Point< dimension > value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    const Point< dimension >& TetrahedralSolidPointFunction< dimension >::value(
        index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    Point< dimension > TetrahedralSolidPointFunction< dimension >::value(
        const Point< dimension >& point, index_t tetrahedron_id ) const
    {
        return impl_->value( point, tetrahedron_id );
    }

    template class opengeode_mesh_api TetrahedralSolidPointFunction< 3 >;
} // namespace geode