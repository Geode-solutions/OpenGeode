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

#include <geode/mesh/helpers/create_coordinate_system.hpp>

#include <async++.h>

#include <geode/geometry/coordinate_system.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
    template < typename Mesh >
    void create_mesh_coordinate_system( const Mesh& mesh,
        typename Mesh::Builder& builder,
        std::string_view new_coordinate_system_name,
        const geode::CoordinateSystem2D& input,
        const geode::CoordinateSystem2D& output )
    {
        auto new_crs = std::make_shared<
            geode::AttributeCoordinateReferenceSystem< Mesh::dim > >(
            mesh.vertex_attribute_manager(), new_coordinate_system_name );
        async::parallel_for(
            async::irange( geode::index_t{ 0 }, mesh.nb_vertices() ),
            [&mesh, &new_crs, &input, &output]( geode::index_t p ) {
                auto point = mesh.point( p );
                const geode::Point2D point2d{ { point.value( 0 ),
                    point.value( 1 ) } };
                const auto new_point2d =
                    output.global_coordinates( input.coordinates( point2d ) );
                point.set_value( 0, new_point2d.value( 0 ) );
                point.set_value( 1, new_point2d.value( 1 ) );
                new_crs->set_point( p, std::move( point ) );
            } );
        auto crs_builder =
            builder.main_coordinate_reference_system_manager_builder();
        crs_builder.register_coordinate_reference_system(
            new_coordinate_system_name, std::move( new_crs ) );
        crs_builder.set_active_coordinate_reference_system(
            new_coordinate_system_name );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    void create_edged_curve_coordinate_system(
        const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder,
        std::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output )
    {
        create_mesh_coordinate_system(
            mesh, builder, new_coordinate_system_name, input, output );
    }

    template < index_t dimension >
    void create_point_set_coordinate_system( const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder,
        std::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output )
    {
        create_mesh_coordinate_system(
            mesh, builder, new_coordinate_system_name, input, output );
    }

    template < index_t dimension >
    void create_surface_mesh_coordinate_system(
        const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder,
        std::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output )
    {
        create_mesh_coordinate_system(
            mesh, builder, new_coordinate_system_name, input, output );
    }

    template < index_t dimension >
    void create_solid_mesh_coordinate_system(
        const SolidMesh< dimension >& mesh,
        SolidMeshBuilder< dimension >& builder,
        std::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output )
    {
        create_mesh_coordinate_system(
            mesh, builder, new_coordinate_system_name, input, output );
    }

    template void opengeode_mesh_api create_edged_curve_coordinate_system(
        const EdgedCurve< 2 >&,
        EdgedCurveBuilder< 2 >&,
        std::string_view,
        const CoordinateSystem2D&,
        const CoordinateSystem2D& );
    template void opengeode_mesh_api create_point_set_coordinate_system(
        const PointSet< 2 >&,
        PointSetBuilder< 2 >&,
        std::string_view,
        const CoordinateSystem2D&,
        const CoordinateSystem2D& );
    template void opengeode_mesh_api create_surface_mesh_coordinate_system(
        const SurfaceMesh< 2 >&,
        SurfaceMeshBuilder< 2 >&,
        std::string_view,
        const CoordinateSystem2D&,
        const CoordinateSystem2D& );

    template void opengeode_mesh_api create_edged_curve_coordinate_system(
        const EdgedCurve< 3 >&,
        EdgedCurveBuilder< 3 >&,
        std::string_view,
        const CoordinateSystem2D&,
        const CoordinateSystem2D& );
    template void opengeode_mesh_api create_point_set_coordinate_system(
        const PointSet< 3 >&,
        PointSetBuilder< 3 >&,
        std::string_view,
        const CoordinateSystem2D&,
        const CoordinateSystem2D& );
    template void opengeode_mesh_api create_surface_mesh_coordinate_system(
        const SurfaceMesh< 3 >&,
        SurfaceMeshBuilder< 3 >&,
        std::string_view,
        const CoordinateSystem2D&,
        const CoordinateSystem2D& );
    template void opengeode_mesh_api create_solid_mesh_coordinate_system(
        const SolidMesh< 3 >&,
        SolidMeshBuilder< 3 >&,
        std::string_view,
        const CoordinateSystem2D&,
        const CoordinateSystem2D& );
} // namespace geode