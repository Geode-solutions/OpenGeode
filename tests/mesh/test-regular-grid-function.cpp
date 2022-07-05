/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/point.h>

#include <geode/mesh/core/regular_grid.h>
#include <geode/mesh/helpers/regular_grid_point_function.h>
#include <geode/mesh/helpers/regular_grid_scalar_function.h>

#include <geode/tests/common.h>

bool inexact_equal( double nb1, double nb2, double eps )
{
    return std::abs( nb1 - nb2 ) < eps;
}

void test_scalar_function()
{
    geode::RegularGrid3D grid{ { { 1.5, 0, 1 } }, { 5, 10, 15 }, { 1, 2, 3 } };
    const auto function_name = "scalar_function";
    auto scalar_function =
        geode::RegularGridScalarFunction3D::create( grid, function_name, 26 );
    scalar_function.set_value( { 1, 2, 3 }, 22 );
    for( const auto i : geode::LRange{ 6 } )
    {
        for( const auto j : geode::LRange{ 11 } )
        {
            for( const auto k : geode::LRange{ 16 } )
            {
                if( i == 1 && j == 2 && k == 3 )
                {
                    OPENGEODE_EXCEPTION(
                        scalar_function.value(
                            geode::GridVertexIndices3D{ { i, j, k } } )
                            == 22,
                        "[Test] Object function value is wrong." );
                }
                else
                {
                    OPENGEODE_EXCEPTION(
                        scalar_function.value(
                            geode::GridVertexIndices3D{ { i, j, k } } )
                            == 26,
                        "[Test] Object function value is wrong." );
                }
            }
        }
    }
    scalar_function.set_value( 217, 22 );
    OPENGEODE_EXCEPTION( scalar_function.value( 217 ) == 22,
        "[Test] Object function value is wrong." );
    scalar_function.set_value( { 1, 2, 4 }, 22 );
    scalar_function.set_value( { 1, 3, 4 }, 22 );
    scalar_function.set_value( { 2, 2, 3 }, 22 );
    scalar_function.set_value( { 2, 3, 3 }, 22 );
    scalar_function.set_value( { 2, 2, 4 }, 22 );
    scalar_function.set_value( { 2, 3, 4 }, 22 );
    geode::Point3D point{ { 2.6, 4.1, 11.2 } };
    auto cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION(
        inexact_equal(
            scalar_function.value( point, cell_indices[0] ), 22, 1e-7 ),
        "[Test] Object function value 1 is wrong." );
    point = { { 1.5, 0, 1 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION(
        inexact_equal(
            scalar_function.value( point, cell_indices[0] ), 26, 1e-7 ),
        "[Test] Object function value 2 is wrong." );
    point = { { 3, 4, 10 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION(
        inexact_equal(
            scalar_function.value( point, cell_indices[0] ), 22, 1e-7 ),
        "[Test] Object function value 3 is wrong." );
    point = { { 3, 5, 8.5 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION(
        inexact_equal(
            scalar_function.value( point, cell_indices[0] ), 24, 1e-7 ),
        "[Test] Object function value 4 is wrong." );
    point = { { 3.9, 7.4, 11.05 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION(
        inexact_equal(
            scalar_function.value( point, cell_indices[0] ), 25.28, 1e-7 ),
        "[Test] Object function value 5 is wrong." );
}

void test_point_function()
{
    geode::RegularGrid3D grid{ { { 1.5, 0, 1 } }, { 5, 10, 15 }, { 1, 2, 3 } };
    const auto function_name = "point_function";
    auto point_function = geode::RegularGridPointFunction3D::create(
        grid, function_name, { { 26, 2, -10 } } );
    point_function.set_value( { 1, 2, 3 }, { { 22, -3, -20 } } );
    for( const auto i : geode::LRange{ 6 } )
    {
        for( const auto j : geode::LRange{ 11 } )
        {
            for( const auto k : geode::LRange{ 16 } )
            {
                if( i == 1 && j == 2 && k == 3 )
                {
                    OPENGEODE_EXCEPTION(
                        ( point_function.value(
                              geode::GridVertexIndices3D{ { i, j, k } } )
                            == geode::Point3D{ { 22, -3, -20 } } ),
                        "[Test] Point function value is wrong." );
                }
                else
                {
                    OPENGEODE_EXCEPTION(
                        ( point_function.value(
                              geode::GridVertexIndices3D{ { i, j, k } } )
                            == geode::Point3D{ { 26, 2, -10 } } ),
                        "[Test] Point function value is wrong." );
                }
            }
        }
    }
    point_function.set_value( 217, { { 22, -3, -20 } } );
    OPENGEODE_EXCEPTION(
        ( point_function.value( 217 ) == geode::Point3D{ { 22, -3, -20 } } ),
        "[Test] Point function value is wrong." );
    point_function.set_value( { 1, 2, 4 }, { { 22, -3, -20 } } );
    point_function.set_value( { 1, 3, 4 }, { { 22, -3, -20 } } );
    point_function.set_value( { 2, 2, 3 }, { { 22, -3, -20 } } );
    point_function.set_value( { 2, 3, 3 }, { { 22, -3, -20 } } );
    point_function.set_value( { 2, 2, 4 }, { { 22, -3, -20 } } );
    point_function.set_value( { 2, 3, 4 }, { { 22, -3, -20 } } );
    geode::Point3D point{ { 2.6, 4.1, 11.2 } };
    auto cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION( point_function.value( point, cell_indices[0] )
                             .inexact_equal( { { 22, -3, -20 } }, 1e-7 ),
        "[Test] Point function value 1 is wrong." );
    point = { { 1.5, 0, 1 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION( point_function.value( point, cell_indices[0] )
                             .inexact_equal( { { 26, 2, -10 } }, 1e-7 ),
        "[Test] Point function value 2 is wrong." );
    point = { { 3, 4, 10 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION( point_function.value( point, cell_indices[0] )
                             .inexact_equal( { { 22, -3, -20 } }, 1e-7 ),
        "[Test] Point function value 3 is wrong." );
    point = { { 3, 5, 8.5 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION( point_function.value( point, cell_indices[0] )
                             .inexact_equal( { { 24, -0.5, -15 } }, 1e-7 ),
        "[Test] Point function value 4 is wrong." );
    point = { { 3.9, 7.4, 11.05 } };
    cell_indices = grid.cells( point );
    OPENGEODE_EXCEPTION( point_function.value( point, cell_indices[0] )
                             .inexact_equal( { { 25.28, 1.1, -11.8 } }, 1e-7 ),
        "[Test] Point function value 5 is wrong." );
}

void test()
{
    test_scalar_function();
    test_point_function();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "regular-grid-function" )