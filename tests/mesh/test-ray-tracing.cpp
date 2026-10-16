/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <absl/algorithm/container.h>

#include <geode/basic/range.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/information.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>
#include <geode/mesh/helpers/ray_tracing.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/tests/common.hpp>

namespace
{
    void test_ray_inside()
    {
        geode::Logger::info( "Test ray inside" );
        auto mesh = geode::SurfaceMesh3D::create();
        auto builder = geode::SurfaceMeshBuilder3D::create( *mesh );
        builder->create_point( geode::Point3D{ { -1, -1, 1 } } );
        builder->create_point( geode::Point3D{ { 1, -1, 1 } } );
        builder->create_point( geode::Point3D{ { 0, 1, 1 } } );
        builder->create_point( geode::Point3D{ { -1, -1, 2 } } );
        builder->create_point( geode::Point3D{ { 1, -1, 2 } } );
        builder->create_point( geode::Point3D{ { 0, 1, 2 } } );
        builder->create_polygon( { 0, 1, 2 } );
        builder->create_polygon( { 3, 4, 5 } );

        const auto aabb = geode::create_aabb_tree( *mesh );
        const geode::Vector3D direction{ { 0, 0, 1 } };
        const geode::Point3D origin{ { 0, 0, 0 } };
        const geode::Ray3D ray{ direction, origin };
        geode::RayTracing3D tracing{ *mesh, aabb.bounding_box(), ray };
        aabb.compute_ray_element_bbox_intersections( ray, tracing );
        const auto result = tracing.closest_polygon();
        geode::OpenGeodeMeshException::test(
            result.has_value(), "Ray inside no result" );
        geode::OpenGeodeMeshException::test(
            result->polygon == 0, "Ray inside wrong polygon" );
        geode::OpenGeodeMeshException::test(
            result->distance == 1, "Ray inside wrong distance" );
    }

    void test_ray_edge()
    {
        geode::Logger::info( "Test ray edge" );
        auto mesh = geode::SurfaceMesh3D::create();
        auto builder = geode::SurfaceMeshBuilder3D::create( *mesh );
        builder->create_point( geode::Point3D{ { 1, -1, 0 } } );
        builder->create_point( geode::Point3D{ { 1, 1, 0 } } );
        builder->create_point( geode::Point3D{ { 1, 0, -1 } } );
        builder->create_point( geode::Point3D{ { 1, 0, 3 } } );
        builder->create_polygon( { 0, 1, 2 } );
        builder->create_polygon( { 1, 0, 3 } );
        builder->compute_polygon_adjacencies();

        const auto aabb = geode::create_aabb_tree( *mesh );
        const geode::Vector3D direction{ { 1, 0, 0 } };
        const geode::Point3D origin{ { 0, 0, 1 } };
        const geode::Ray3D ray{ direction, origin };
        geode::RayTracing3D tracing{ *mesh, aabb.bounding_box(), ray };
        aabb.compute_ray_element_bbox_intersections( ray, tracing );
        const auto result = tracing.closest_polygon();
        geode::OpenGeodeMeshException::test(
            result.has_value(), "Ray edge no result" );
        geode::OpenGeodeMeshException::test(
            result->polygon == 1, "Ray edge wrong polygon" );
        geode::OpenGeodeMeshException::test(
            result->distance == 1, "Ray edge wrong distance" );
        geode::OpenGeodeMeshException::test(
            tracing.all_intersections().size() == 1,
            "Wrong size for ray tracing result for case edge" );
    }

    void test_ray_parallel()
    {
        geode::Logger::info( "Test ray parallel" );
        auto mesh = geode::SurfaceMesh3D::create();
        auto builder = geode::SurfaceMeshBuilder3D::create( *mesh );
        builder->create_point( geode::Point3D{ { -1, -1, 0 } } );
        builder->create_point( geode::Point3D{ { -1, 1, 0 } } );
        builder->create_point( geode::Point3D{ { -2, 0, 0 } } );
        builder->create_polygon( { 0, 1, 2 } );
        const auto aabb = geode::create_aabb_tree( *mesh );
        const geode::Vector3D direction{ { -1, 0, 0 } };
        const geode::Point3D origin{ { 0, 0, 0 } };
        const geode::Ray3D ray{ direction, origin };
        geode::RayTracing3D tracing{ *mesh, aabb.bounding_box(), ray };
        aabb.compute_ray_element_bbox_intersections( ray, tracing );
        const auto result = tracing.closest_polygon();
        geode::OpenGeodeMeshException::test(
            result.has_value(), "Ray edge no result" );
        geode::OpenGeodeMeshException::test(
            result->polygon == 0, "Ray edge wrong polygon" );
        geode::OpenGeodeMeshException::test(
            result->distance == 1, "Ray edge wrong distance" );
    }

    std::unique_ptr< geode::SurfaceMesh3D > create_quad_mesh()
    {
        auto mesh = geode::SurfaceMesh3D::create();
        auto builder = geode::SurfaceMeshBuilder3D::create( *mesh );
        builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
        builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
        builder->create_point( geode::Point3D{ { 1, 1, 0 } } );
        builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
        builder->create_point( geode::Point3D{ { 1, 2, 0 } } );
        builder->create_point( geode::Point3D{ { 0, 2, 0 } } );
        builder->create_polygon( { 0, 1, 2, 3 } );
        builder->create_polygon( { 3, 2, 4, 5 } );
        builder->compute_polygon_adjacencies();
        return mesh;
    }

    std::vector< geode::RayTracing3D::PolygonDistance > trace_vertical_ray(
        const geode::SurfaceMesh3D& mesh, const geode::Point3D& origin )
    {
        const auto aabb = geode::create_aabb_tree( mesh );
        const geode::Vector3D direction{ { 0, 0, 1 } };
        const geode::Ray3D ray{ direction, origin };
        geode::RayTracing3D tracing{ mesh, aabb.bounding_box(), ray };
        aabb.compute_ray_element_bbox_intersections( ray, tracing );
        return tracing.all_intersections();
    }

    void test_ray_quad_vertex()
    {
        geode::Logger::info( "Test ray quad vertex" );
        const auto mesh = create_quad_mesh();
        const auto results =
            trace_vertical_ray( *mesh, geode::Point3D{ { 0, 1, -1 } } );
        geode::OpenGeodeMeshException::test( results.size() == 1,
            "Ray through quad vertex 3 should give one intersection, not ",
            results.size() );
        const auto& result = results.front();
        const auto vertex_id = result.polygon == 0 ? 3 : 0;
        const auto expected_position = result.polygon == 0
                                           ? geode::POSITION::vertex3
                                           : geode::POSITION::vertex0;
        geode::OpenGeodeMeshException::test(
            result.position == expected_position,
            "Ray through quad vertex: wrong position" );
        geode::OpenGeodeMeshException::test(
            mesh->polygon_vertex( { result.polygon,
                static_cast< geode::local_index_t >( vertex_id ) } )
                == 3,
            "Ray through quad vertex: wrong vertex" );
        geode::OpenGeodeMeshException::test(
            result.distance == 1, "Ray through quad vertex: wrong distance" );
    }

    void test_ray_quad_edge()
    {
        geode::Logger::info( "Test ray quad edge" );
        const auto mesh = create_quad_mesh();
        const auto results =
            trace_vertical_ray( *mesh, geode::Point3D{ { 0.5, 1, -1 } } );
        geode::OpenGeodeMeshException::test( results.size() == 1,
            "Ray through shared quad edge should give one intersection, not ",
            results.size() );
        const auto& result = results.front();
        const auto expected_position = result.polygon == 0
                                           ? geode::POSITION::edge2
                                           : geode::POSITION::edge0;
        geode::OpenGeodeMeshException::test(
            result.position == expected_position,
            "Ray through shared quad edge: wrong position" );
    }

    void test_ray_quad_diagonal()
    {
        geode::Logger::info( "Test ray quad diagonal" );
        const auto mesh = create_quad_mesh();
        const auto results =
            trace_vertical_ray( *mesh, geode::Point3D{ { 0.5, 0.5, -1 } } );
        geode::OpenGeodeMeshException::test( results.size() == 1,
            "Ray through quad diagonal should give one intersection, not ",
            results.size() );
        geode::OpenGeodeMeshException::test( results.front().polygon == 0,
            "Ray through quad diagonal: wrong polygon" );
        geode::OpenGeodeMeshException::test(
            results.front().position == geode::POSITION::inside,
            "Ray through quad diagonal should be inside the polygon" );
    }

    geode::RayTracing3D create_tracing_from_local_ray(
        const geode::SurfaceMesh3D& mesh, const geode::BoundingBox3D& bbox )
    {
        const geode::OwnerRay3D ray{ geode::Vector3D{ { 0, 0, 1 } },
            geode::Point3D{ { 0.25, 0.75, -2 } } };
        return geode::RayTracing3D{ mesh, bbox, ray };
    }

    double scribble_stack( double seed )
    {
        std::array< double, 512 > values;
        for( const auto i : geode::Indices{ values } )
        {
            values[i] = seed * ( i + 1 );
        }
        return absl::c_accumulate( values, 0. );
    }

    void test_ray_origin_lifetime()
    {
        geode::Logger::info( "Test ray origin lifetime" );
        const auto mesh = create_quad_mesh();
        const auto aabb = geode::create_aabb_tree( *mesh );
        auto tracing =
            create_tracing_from_local_ray( *mesh, aabb.bounding_box() );
        geode::Logger::debug( "Scribble ", scribble_stack( 1e30 ) );
        for( const auto polygon : geode::Range{ mesh->nb_polygons() } )
        {
            geode::OpenGeodeMeshException::test(
                !tracing( polygon ), "Tracing should not stop" );
        }
        const auto result = tracing.closest_polygon();
        geode::OpenGeodeMeshException::test(
            result.has_value(), "Ray origin lifetime: no result" );
        geode::OpenGeodeMeshException::test(
            std::fabs( result->distance - 2 ) < geode::GLOBAL_EPSILON,
            "Ray origin lifetime: wrong distance ", result->distance );
    }

    void test()
    {
        geode::OpenGeodeMeshLibrary::initialize();
        test_ray_inside();
        test_ray_edge();
        test_ray_parallel();
        test_ray_quad_vertex();
        test_ray_quad_edge();
        test_ray_quad_diagonal();
        test_ray_origin_lifetime();
    }
} // namespace

OPENGEODE_TEST( "ray-tracing" )