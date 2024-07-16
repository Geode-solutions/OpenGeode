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

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/triangulated_surface_point_function.hpp>
#include <geode/mesh/helpers/triangulated_surface_scalar_function.hpp>

#include <geode/tests/common.hpp>

bool inexact_equal( double nb1, double nb2, double eps )
{
    return std::abs( nb1 - nb2 ) < eps;
}

void build_test_surface( geode::TriangulatedSurface2D& surface )
{
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( surface );
    builder->create_vertices( 7 );
    builder->set_point( 0, geode::Point2D{ { 0, 0 } } );
    builder->set_point( 1, geode::Point2D{ { 1, 0 } } );
    builder->set_point( 2, geode::Point2D{ { 0, 1 } } );
    builder->set_point( 3, geode::Point2D{ { 1, 1 } } );
    builder->set_point( 4, geode::Point2D{ { 2, 0.5 } } );
    builder->set_point( 5, geode::Point2D{ { 1, 1.75 } } );
    builder->set_point( 6, geode::Point2D{ { 1, 2 } } );
    builder->create_triangle( { 0, 1, 3 } );
    builder->create_triangle( { 0, 3, 2 } );
    builder->create_triangle( { 2, 3, 5 } );
    builder->create_triangle( { 1, 4, 3 } );
    builder->create_triangle( { 3, 4, 5 } );
    builder->create_triangle( { 2, 5, 6 } );
    builder->create_triangle( { 4, 6, 5 } );
    builder->compute_polygon_adjacencies();
    double surface_area{ 0 };
    for( const auto triangle_id : geode::LRange{ 7 } )
    {
        surface_area +=
            geode::triangle_signed_area( surface.triangle( triangle_id ) );
    }
    OPENGEODE_EXCEPTION( inexact_equal( surface_area, 2.5, 1e-7 ),
        "[Test] Total area of triangle should be 2.5." );
}

void test_scalar_function( geode::TriangulatedSurface2D& surface )
{
    const auto function_name = "scalar_function";
    auto scalar_function = geode::TriangulatedSurfaceScalarFunction2D::create(
        surface, function_name, 26 );
    scalar_function.set_value( 4, 22 );
    scalar_function.set_value( 6, 22 );
    for( const auto i : geode::LRange{ 7 } )
    {
        if( i == 4 || i == 6 )
        {
            OPENGEODE_EXCEPTION( scalar_function.value( i ) == 22,
                "[Test] Scalar function value is wrong." );
        }
        else
        {
            OPENGEODE_EXCEPTION( scalar_function.value( i ) == 26,
                "[Test] Scalar function value is wrong." );
        }
    }
    scalar_function.set_value( 5, 22 );
    geode::Point2D point{ { 1., 1. } };
    OPENGEODE_EXCEPTION(
        inexact_equal( scalar_function.value( point, 2 ), 26, 1e-7 ),
        "[Test] Scalar function value 1 is wrong." );
    point = geode::Point2D{ { 1., 1.8 } };
    OPENGEODE_EXCEPTION(
        inexact_equal( scalar_function.value( point, 5 ), 22, 1e-7 ),
        "[Test] Scalar function value 2 is wrong." );
    point = geode::Point2D{ { 1., 1.375 } };
    OPENGEODE_EXCEPTION(
        inexact_equal( scalar_function.value( point, 2 ), 24, 1e-7 ),
        "[Test] Scalar function value 3 is wrong." );
}

void test_point_function( geode::TriangulatedSurface2D& surface )
{
    const auto function_name = "point_function";
    auto point_function =
        geode::TriangulatedSurfacePointFunction< 2, 2 >::create(
            surface, function_name, geode::Point2D{ { 26, 3 } } );
    point_function.set_value( 4, geode::Point2D{ { 22, -3 } } );
    point_function.set_value( 6, geode::Point2D{ { 22, -3 } } );
    for( const auto i : geode::LRange{ 7 } )
    {
        if( i == 4 || i == 6 )
        {
            OPENGEODE_EXCEPTION(
                ( point_function.value( i ) == geode::Point2D{ { 22, -3 } } ),
                "[Test] Point function value is wrong." );
        }
        else
        {
            OPENGEODE_EXCEPTION(
                ( point_function.value( i ) == geode::Point2D{ { 26, 3 } } ),
                "[Test] Point function value is wrong." );
        }
    }
    point_function.set_value( 5, geode::Point2D{ { 22, -3 } } );
    geode::Point2D point{ { 1., 1. } };
    OPENGEODE_EXCEPTION( point_function.value( point, 2 )
                             .inexact_equal( geode::Point2D{ { 26, 3 } } ),
        "[Test] Point function value 1 is wrong." );
    point = geode::Point2D{ { 1., 1.8 } };
    OPENGEODE_EXCEPTION( point_function.value( point, 5 )
                             .inexact_equal( geode::Point2D{ { 22, -3 } } ),
        "[Test] Point function value 2 is wrong." );
    point = geode::Point2D{ { 1., 1.375 } };
    OPENGEODE_EXCEPTION( point_function.value( point, 2 )
                             .inexact_equal( geode::Point2D{ { 24, 0 } } ),
        "[Test] Point function value 3 is wrong." );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto surface = geode::TriangulatedSurface2D::create();
    build_test_surface( *surface );
    test_scalar_function( *surface );
    test_point_function( *surface );
}

OPENGEODE_TEST( "triangulated-surface-function" )