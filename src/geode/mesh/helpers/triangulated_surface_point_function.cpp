/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/mesh/helpers/triangulated_surface_point_function.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    template < index_t dimension >
    class TriangulatedSurfacePointFunction< dimension >::Impl
    {
    public:
        Impl( const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name,
            Point< dimension > value )
            : surface_( surface )
        {
            OPENGEODE_EXCEPTION(
                !surface_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TriangulatedSurfacePointFunction: attribute "
                "with name '",
                function_name, "' already exists." );
            function_attribute_ =
                surface_.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        Point< dimension > >(
                        function_name, std::move( value ), { false, true } );
        }

        Impl( const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name )
            : surface_( surface )
        {
            OPENGEODE_EXCEPTION(
                surface_.vertex_attribute_manager().attribute_exists(
                    function_name ),
                "Cannot create TriangulatedSurfacePointFunction: attribute "
                "with name '",
                function_name, "' does not exist." );
            function_attribute_ =
                surface_.vertex_attribute_manager()
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
            const Point< dimension >& point, index_t triangle_id ) const
        {
            Point< dimension > point_value;
            const auto triangle = surface_.triangle( triangle_id );
            const auto triangle_vertices =
                surface_.polygon_vertices( triangle_id );
            const auto barycentric_coords =
                triangle_barycentric_coordinates( point, triangle );
            for( const auto node_id : LIndices{ barycentric_coords } )
            {
                point_value +=
                    function_attribute_->value( triangle_vertices[node_id] )
                    * barycentric_coords[node_id];
            }
            return point_value;
        }

    private:
        const TriangulatedSurface< dimension >& surface_;
        std::shared_ptr< VariableAttribute< Point< dimension > > >
            function_attribute_;
    };

    template < index_t dimension >
    TriangulatedSurfacePointFunction< dimension >::
        TriangulatedSurfacePointFunction(
            TriangulatedSurfacePointFunction< dimension >&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    TriangulatedSurfacePointFunction< dimension >::
        TriangulatedSurfacePointFunction(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name,
            Point< dimension > value )
        : impl_{ surface, function_name, value }
    {
    }

    template < index_t dimension >
    TriangulatedSurfacePointFunction< dimension >::
        TriangulatedSurfacePointFunction(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name )
        : impl_{ surface, function_name }
    {
    }

    template < index_t dimension >
    TriangulatedSurfacePointFunction<
        dimension >::~TriangulatedSurfacePointFunction()
    {
    }

    template < index_t dimension >
    TriangulatedSurfacePointFunction< dimension >
        TriangulatedSurfacePointFunction< dimension >::create(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name,
            Point< dimension > value )
    {
        return { surface, function_name, value };
    }

    template < index_t dimension >
    TriangulatedSurfacePointFunction< dimension >
        TriangulatedSurfacePointFunction< dimension >::find(
            const TriangulatedSurface< dimension >& surface,
            absl::string_view function_name )
    {
        return { surface, function_name };
    }

    template < index_t dimension >
    void TriangulatedSurfacePointFunction< dimension >::set_value(
        index_t vertex_index, Point< dimension > value )
    {
        impl_->set_value( vertex_index, value );
    }

    template < index_t dimension >
    const Point< dimension >&
        TriangulatedSurfacePointFunction< dimension >::value(
            index_t vertex_index ) const
    {
        return impl_->value( vertex_index );
    }

    template < index_t dimension >
    Point< dimension > TriangulatedSurfacePointFunction< dimension >::value(
        const Point< dimension >& point, index_t triangle_id ) const
    {
        return impl_->value( point, triangle_id );
    }

    template class opengeode_mesh_api TriangulatedSurfacePointFunction< 2 >;
    template class opengeode_mesh_api TriangulatedSurfacePointFunction< 3 >;
} // namespace geode