/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/mesh/helpers/triangulated_surface_scalar_function.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    template < index_t dimension >
    class TriangulatedSurfaceScalarFunction< dimension >::Impl
    {
    public:
        Impl( const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name,
            double value )
            : surface_( surface )
        {
            OPENGEODE_EXCEPTION(
                !surface_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TriangulatedSurfaceScalarFunction: attribute "
                "with name '",
                function_name, "' already exists." );
            function_attribute_ =
                surface_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        double >( function_name, value, { false, true } );
        }

        Impl( const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name )
            : surface_( surface )
        {
            OPENGEODE_EXCEPTION(
                surface_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TriangulatedSurfaceScalarFunction: attribute "
                "with name '",
                function_name, "' does not exist." );
            function_attribute_ =
                surface_.vertex_attribute_manager()
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
            const Point< dimension >& point, index_t triangle_id ) const
        {
            double point_value{ 0. };
            const auto triangle = surface_.triangle( triangle_id );
            const auto triangle_vertices =
                surface_.polygon_vertices( triangle_id );
            const auto barycentric_coords =
                triangle_barycentric_coordinates( point, triangle );
            for( const auto node_id : LIndices{ barycentric_coords } )
            {
                point_value +=
                    barycentric_coords[node_id]
                    * function_attribute_->value( triangle_vertices[node_id] );
            }
            return point_value;
        }

    private:
        const TriangulatedSurface< dimension >& surface_;
        std::shared_ptr< VariableAttribute< double > > function_attribute_;
    };

    template < index_t dimension >
    TriangulatedSurfaceScalarFunction< dimension >::
        TriangulatedSurfaceScalarFunction(
            TriangulatedSurfaceScalarFunction< dimension >&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceScalarFunction< dimension >::
        TriangulatedSurfaceScalarFunction(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name,
            double value )
        : impl_{ surface, function_name, value }
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceScalarFunction< dimension >::
        TriangulatedSurfaceScalarFunction(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name )
        : impl_{ surface, function_name }
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceScalarFunction<
        dimension >::~TriangulatedSurfaceScalarFunction()
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceScalarFunction< dimension >
        TriangulatedSurfaceScalarFunction< dimension >::create(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name,
            double value )
    {
        return { surface, function_name, value };
    }

    template < index_t dimension >
    TriangulatedSurfaceScalarFunction< dimension >
        TriangulatedSurfaceScalarFunction< dimension >::find(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name )
    {
        return { surface, function_name };
    }

    template < index_t dimension >
    void TriangulatedSurfaceScalarFunction< dimension >::set_value(
        index_t vertex_index, double value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    double TriangulatedSurfaceScalarFunction< dimension >::value(
        index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    double TriangulatedSurfaceScalarFunction< dimension >::value(
        const Point< dimension >& point, index_t triangle_id ) const
    {
        return impl_->value( point, triangle_id );
    }

    template class opengeode_mesh_api TriangulatedSurfaceScalarFunction< 2 >;
    template class opengeode_mesh_api TriangulatedSurfaceScalarFunction< 3 >;
} // namespace geode