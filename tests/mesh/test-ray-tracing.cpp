/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/aabb.h>

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>
#include <geode/mesh/helpers/ray_tracing.h>

#include <geode/tests/common.h>

void test_ray_inside()
{
    auto mesh = geode::SurfaceMesh3D::create();
    auto builder = geode::SurfaceMeshBuilder3D::create( *mesh );
    builder->create_point( { { -1, -1, 1 } } );
    builder->create_point( { { 1, -1, 1 } } );
    builder->create_point( { { 0, 1, 1 } } );
    builder->create_point( { { -1, -1, 2 } } );
    builder->create_point( { { 1, -1, 2 } } );
    builder->create_point( { { 0, 1, 2 } } );
    builder->create_polygon( { 0, 1, 2 } );
    builder->create_polygon( { 3, 4, 5 } );

    const auto aabb = geode::create_aabb_tree( *mesh );
    const geode::Vector3D direction{ { 0, 0, 1 } };
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Ray3D ray{ direction, origin };
    geode::RayTracing3D tracing{ *mesh, ray };
    aabb.compute_ray_element_bbox_intersections( ray, tracing );
    const auto result = tracing.closest_polygon();
    OPENGEODE_EXCEPTION( result, "[Test] Ray inside no result" );
    OPENGEODE_EXCEPTION(
        result->polygon == 0, "[Test] Ray inside wrong polygon" );
    OPENGEODE_EXCEPTION(
        result->distance == 1, "[Test] Ray inside wrong distance" );
}

void test_ray_edge()
{
    auto mesh = geode::SurfaceMesh3D::create();
    auto builder = geode::SurfaceMeshBuilder3D::create( *mesh );
    builder->create_point( { { 1, -1, 0 } } );
    builder->create_point( { { 1, 1, 0 } } );
    builder->create_point( { { 1, 0, 1 } } );
    builder->create_polygon( { 0, 1, 2 } );

    const auto aabb = geode::create_aabb_tree( *mesh );
    const geode::Vector3D direction{ { 1, 0, 0 } };
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Ray3D ray{ direction, origin };
    geode::RayTracing3D tracing{ *mesh, ray };
    aabb.compute_ray_element_bbox_intersections( ray, tracing );
    const auto result = tracing.closest_polygon();
    OPENGEODE_EXCEPTION( result, "[Test] Ray edge no result" );
    OPENGEODE_EXCEPTION(
        result->polygon == 0, "[Test] Ray edge wrong polygon" );
    OPENGEODE_EXCEPTION(
        result->distance == 1, "[Test] Ray edge wrong distance" );
}

void test_ray_parallel()
{
    auto mesh = geode::SurfaceMesh3D::create();
    auto builder = geode::SurfaceMeshBuilder3D::create( *mesh );
    builder->create_point( { { -1, -1, 0 } } );
    builder->create_point( { { -1, 1, 0 } } );
    builder->create_point( { { -2, 0, 0 } } );
    builder->create_polygon( { 0, 1, 2 } );

    const auto aabb = geode::create_aabb_tree( *mesh );
    const geode::Vector3D direction{ { -1, 0, 0 } };
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Ray3D ray{ direction, origin };
    geode::RayTracing3D tracing{ *mesh, ray };
    aabb.compute_ray_element_bbox_intersections( ray, tracing );
    const auto result = tracing.closest_polygon();
    OPENGEODE_EXCEPTION( result, "[Test] Ray edge no result" );
    OPENGEODE_EXCEPTION(
        result->polygon == 0, "[Test] Ray edge wrong polygon" );
    OPENGEODE_EXCEPTION(
        result->distance == 1, "[Test] Ray edge wrong distance" );
}

void test()
{
    test_ray_inside();
    test_ray_edge();
    test_ray_parallel();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "ray-tracing" )