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

#include <fstream>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/builder/regular_grid_surface_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/gradient_computation.h>
#include <geode/mesh/io/regular_grid_output.h>
#include <geode/mesh/io/triangulated_surface_output.h>

#include <geode/tests/common.h>

void test_gradient_grid2D()
{
    auto grid = geode::RegularGrid< 2 >::create();
    auto builder = geode::RegularGridBuilder< 2 >::create( *grid );
    builder->initialize_grid( { { 0, 0 } }, { 3, 3 },
        { geode::Vector2D{ { 1, 0 } }, geode::Vector2D{ { 0, 1 } } } );
    const auto scalar_function_name = "scalar_function";
    auto attribute =
        grid->vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, double >(
                scalar_function_name, 0 );
    attribute->set_value( 1, 1 );
    attribute->set_value( 4, 1 );
    attribute->set_value( 6, 1 );
    attribute->set_value( 9, 1 );
    attribute->set_value( 2, 2 );
    attribute->set_value( 3, 3 );
    attribute->set_value( 7, 2 );
    attribute->set_value( 8, 2 );
    attribute->set_value( 10, 2 );
    attribute->set_value( 11, 3 );
    attribute->set_value( 12, 3 );
    attribute->set_value( 13, 2 );
    attribute->set_value( 14, 3 );
    attribute->set_value( 15, 8 );
    const auto gradient_name = geode::compute_surface_scalar_function_gradient(
        *grid, scalar_function_name );
    geode::Logger::info( "Gradient attribute name: ", gradient_name );
    geode::save_regular_grid< 2 >( *grid, "grid_with_gradient.og_rgd2d" );
}

void test_gradient_triangulated_surface2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 10 );
    builder->set_point( 0, { { 0, 0 } } );
    builder->set_point( 1, { { 1, 0 } } );
    builder->set_point( 2, { { 2, 0 } } );
    builder->set_point( 3, { { 0, 1 } } );
    builder->set_point( 4, { { 1, 1 } } );
    builder->set_point( 5, { { 0, 2 } } );
    builder->set_point( 6, { { 2, 2 } } );
    builder->set_point( 7, { { 3, 2 } } );
    builder->set_point( 8, { { 1, 3 } } );
    builder->set_point( 9, { { 2, 4 } } );
    builder->create_polygon( { 0, 1, 3 } );
    builder->create_polygon( { 1, 2, 4 } );
    builder->create_polygon( { 1, 4, 3 } );
    builder->create_polygon( { 3, 4, 5 } );
    builder->create_polygon( { 4, 6, 5 } );
    builder->create_polygon( { 2, 6, 4 } );
    builder->create_polygon( { 2, 7, 6 } );
    builder->create_polygon( { 6, 7, 9 } );
    builder->create_polygon( { 6, 9, 8 } );
    builder->create_polygon( { 5, 6, 8 } );
    builder->compute_polygon_adjacencies();
    const auto scalar_function_name = "scalar_function";
    auto attribute =
        surface->vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, double >(
                scalar_function_name, 0 );
    attribute->set_value( 1, 1 );
    attribute->set_value( 2, 1 );
    attribute->set_value( 3, 1 );
    attribute->set_value( 5, 1 );
    attribute->set_value( 6, 2 );
    attribute->set_value( 9, 3 );
    attribute->set_value( 7, 2 );
    attribute->set_value( 8, 2 );
    const auto gradient_name = geode::compute_surface_scalar_function_gradient(
        *surface, scalar_function_name );
    geode::Logger::info( "Gradient attribute name: ", gradient_name );
    geode::save_triangulated_surface< 2 >(
        *surface, "mesh_with_gradient.og_tsf2d" );
}

void test_gradient_grid3D()
{
    auto grid = geode::RegularGrid< 3 >::create();
    auto builder = geode::RegularGridBuilder< 3 >::create( *grid );
    builder->initialize_grid( { { 0, 0, 0 } }, { 2, 2, 2 },
        { geode::Vector3D{ { 1, 0, 0 } }, geode::Vector3D{ { 0, 1, 0 } },
            geode::Vector3D{ { 0, 0, 1 } } } );
    const auto scalar_function_name = "scalar_function";
    auto attribute =
        grid->vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, double >(
                scalar_function_name, 0 );
    attribute->set_value( 4, 1 );
    attribute->set_value( 10, 1 );
    attribute->set_value( 12, 1 );
    attribute->set_value( 14, 1 );
    attribute->set_value( 16, 1 );
    attribute->set_value( 22, 1 );
    attribute->set_value( 1, 2 );
    attribute->set_value( 3, 2 );
    attribute->set_value( 5, 2 );
    attribute->set_value( 7, 2 );
    attribute->set_value( 9, 2 );
    attribute->set_value( 11, 2 );
    attribute->set_value( 15, 2 );
    attribute->set_value( 17, 2 );
    attribute->set_value( 19, 2 );
    attribute->set_value( 21, 2 );
    attribute->set_value( 23, 2 );
    attribute->set_value( 25, 2 );
    attribute->set_value( 0, 3 );
    attribute->set_value( 2, 3 );
    attribute->set_value( 6, 3 );
    attribute->set_value( 8, 3 );
    attribute->set_value( 18, 3 );
    attribute->set_value( 20, 3 );
    attribute->set_value( 24, 3 );
    attribute->set_value( 26, 3 );
    const auto gradient_name = geode::compute_solid_scalar_function_gradient(
        *grid, scalar_function_name );
    geode::Logger::info( "Gradient attribute name: ", gradient_name );
    geode::save_regular_grid< 3 >( *grid, "grid_with_gradient.og_rgd3d" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();

    test_gradient_grid2D();
    test_gradient_triangulated_surface2D();
    test_gradient_grid3D();
}

OPENGEODE_TEST( "gradient-computation" )
