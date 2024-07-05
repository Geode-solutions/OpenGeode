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

#include <geode/basic/assert.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/mensuration.h>
#include <geode/geometry/point.h>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/helpers/tetrahedral_solid_point_function.h>
#include <geode/mesh/helpers/tetrahedral_solid_scalar_function.h>

#include <geode/tests/common.h>

bool inexact_equal( double nb1, double nb2, double eps )
{
    return std::abs( nb1 - nb2 ) < eps;
}

void build_test_solid( geode::TetrahedralSolid3D& solid )
{
    auto builder = geode::TetrahedralSolidBuilder3D::create( solid );
    builder->create_vertices( 8 );
    builder->set_point( 0, geode::Point3D{ { 0, 0, 0 } } );
    builder->set_point( 1, geode::Point3D{ { 1, 0, 0 } } );
    builder->set_point( 2, geode::Point3D{ { 1, 1, 0 } } );
    builder->set_point( 3, geode::Point3D{ { 0, 1, 0 } } );
    builder->set_point( 4, geode::Point3D{ { 0, 0, 1 } } );
    builder->set_point( 5, geode::Point3D{ { 1, 0, 1 } } );
    builder->set_point( 6, geode::Point3D{ { 1, 1, 1 } } );
    builder->set_point( 7, geode::Point3D{ { 0, 1, 1 } } );
    builder->create_tetrahedron( { 0, 4, 1, 3 } );
    builder->create_tetrahedron( { 1, 2, 3, 6 } );
    builder->create_tetrahedron( { 1, 4, 5, 6 } );
    builder->create_tetrahedron( { 3, 7, 4, 6 } );
    builder->create_tetrahedron( { 1, 4, 6, 3 } );
    builder->compute_polyhedron_adjacencies();
    double solid_volume{ 0 };
    for( const auto tet_id : geode::LRange{ 5 } )
    {
        solid_volume +=
            geode::tetrahedron_signed_volume( solid.tetrahedron( tet_id ) );
    }
    OPENGEODE_EXCEPTION( inexact_equal( solid_volume, 1, 1e-7 ),
        "[Test] Total volume of tetrahedra should be 1." );
}

void test_scalar_function( geode::TetrahedralSolid3D& solid )
{
    const auto function_name = "scalar_function";
    auto scalar_function = geode::TetrahedralSolidScalarFunction3D::create(
        solid, function_name, 26 );
    scalar_function.set_value( 2, 22 );
    for( const auto i : geode::LRange{ 8 } )
    {
        if( i == 2 )
        {
            OPENGEODE_EXCEPTION( scalar_function.value( i ) == 22,
                "[Test] Object function value is wrong." );
        }
        else
        {
            OPENGEODE_EXCEPTION( scalar_function.value( i ) == 26,
                "[Test] Object function value is wrong." );
        }
    }
    scalar_function.set_value( 1, 22 );
    scalar_function.set_value( 3, 22 );
    scalar_function.set_value( 6, 22 );
    geode::Point3D point{ { 0.91, 0.87, 0.19 } };
    OPENGEODE_EXCEPTION(
        inexact_equal( scalar_function.value( point, 1 ), 22, 1e-7 ),
        "[Test] Object function value 1 is wrong." );
    point = geode::Point3D{ { 0., 0., 1. } };
    OPENGEODE_EXCEPTION(
        inexact_equal( scalar_function.value( point, 4 ), 26, 1e-7 ),
        "[Test] Object function value 2 is wrong." );
    point = geode::Point3D{ { 0.5, 0.5, 0.5 } };
    OPENGEODE_EXCEPTION(
        inexact_equal( scalar_function.value( point, 4 ), 23, 1e-7 ),
        "[Test] Object function value 3 is wrong." );
}

void test_point_function( geode::TetrahedralSolid3D& solid )
{
    const auto function_name = "point_function";
    auto point_function = geode::TetrahedralSolidPointFunction3D::create(
        solid, function_name, geode::Point3D{ { 26, 3, -10 } } );
    point_function.set_value( 2, geode::Point3D{ { 22, -3, -20 } } );
    for( const auto i : geode::LRange{ 8 } )
    {
        if( i == 2 )
        {
            OPENGEODE_EXCEPTION( ( point_function.value( i )
                                     == geode::Point3D{ { 22, -3, -20 } } ),
                "[Test] Object function value is wrong." );
        }
        else
        {
            OPENGEODE_EXCEPTION( ( point_function.value( i )
                                     == geode::Point3D{ { 26, 3, -10 } } ),
                "[Test] Object function value is wrong." );
        }
    }
    point_function.set_value( 3, geode::Point3D{ { 22, -3, -20 } } );
    point_function.set_value( 1, geode::Point3D{ { 22, -3, -20 } } );
    point_function.set_value( 6, geode::Point3D{ { 22, -3, -20 } } );
    geode::Point3D point{ { 0.91, 0.87, 0.19 } };
    OPENGEODE_EXCEPTION(
        point_function.value( point, 1 )
            .inexact_equal( geode::Point3D{ { 22, -3, -20 } } ),
        "[Test] Object function value 1 is wrong." );
    point = geode::Point3D{ { 0., 0., 1. } };
    OPENGEODE_EXCEPTION( point_function.value( point, 4 )
                             .inexact_equal( geode::Point3D{ { 26, 3, -10 } } ),
        "[Test] Object function value 2 is wrong." );
    point = geode::Point3D{ { 0.5, 0.5, 0.5 } };
    OPENGEODE_EXCEPTION(
        point_function.value( point, 4 )
            .inexact_equal( geode::Point3D{ { 23, -1.5, -17.5 } } ),
        "[Test] Object function value 3 is wrong." );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto solid = geode::TetrahedralSolid3D::create();
    build_test_solid( *solid );
    test_scalar_function( *solid );
    test_point_function( *solid );
}

OPENGEODE_TEST( "tetrahedral-solid-function" )