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

#include <geode/basic/logger.hpp>

#include <geode/geometry/square_matrix.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

void test_matrix_2x2()
{
    geode::Logger::info( "Test Matrix 2x2" );
    const geode::SquareMatrix2D matrix{ { geode::Vector2D{ { -1, 1 } },
        geode::Vector2D{ { 2, 2 } } } };

    OPENGEODE_EXCEPTION( matrix.value( 0, 0 ) == -1,
        "[TEST] Wrong value for matrix coeff [0,0]" );
    OPENGEODE_EXCEPTION( matrix.value( 0, 1 ) == 1,
        "[TEST] Wrong value for matrix coeff [0,1]" );
    OPENGEODE_EXCEPTION( matrix.value( 1, 0 ) == 2,
        "[TEST] Wrong value for matrix coeff [1,0]" );
    OPENGEODE_EXCEPTION( matrix.value( 1, 1 ) == 2,
        "[TEST] Wrong value for matrix coeff [1,1]" );

    const auto mult_result = matrix * geode::Vector2D{ { 3, 5 } };
    const geode::Vector2D answer{ { 2, 16 } };
    OPENGEODE_EXCEPTION( mult_result == answer,
        "[TEST] Wrong result for matrix multiplication: ", mult_result.string(),
        " instead of [3,5]" );

    const auto det = matrix.determinant();
    OPENGEODE_EXCEPTION(
        det == -4, "[TEST] Wrong determinant value: ", det, " instead of -4" );

    const auto transpose_matrix = matrix.transpose();
    OPENGEODE_EXCEPTION( transpose_matrix.value( 0, 0 ) == -1,
        "[TEST] Wrong value for transpose matrix coeff [0,0]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 0, 1 ) == 2,
        "[TEST] Wrong value for transpose matrix coeff [0,1]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 1, 0 ) == 1,
        "[TEST] Wrong value for transpose matrix coeff [1,0]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 1, 1 ) == 2,
        "[TEST] Wrong value for transpose matrix coeff [1,1]" );

    const auto inverse_matrix = matrix.inverse();
    OPENGEODE_EXCEPTION( inverse_matrix.value( 0, 0 ) == -0.5,
        "[TEST] Wrong value for inverse matrix coeff [0,0]: ",
        inverse_matrix.value( 0, 0 ), " instead of ", -0.5 );
    OPENGEODE_EXCEPTION( inverse_matrix.value( 0, 1 ) == 0.25,
        "[TEST] Wrong value for inverse matrix coeff [0,1]: ",
        inverse_matrix.value( 0, 1 ), " instead of ", 0.25 );
    OPENGEODE_EXCEPTION( inverse_matrix.value( 1, 0 ) == 0.5,
        "[TEST] Wrong value for inverse matrix coeff [1,0]: ",
        inverse_matrix.value( 1, 0 ), " instead of ", 0.5 );
    OPENGEODE_EXCEPTION( inverse_matrix.value( 1, 1 ) == 0.25,
        "[TEST] Wrong value for inverse matrix coeff [1,1]: ",
        inverse_matrix.value( 1, 1 ), " instead of ", 0.25 );
}

void test_matrix_3x3()
{
    geode::Logger::info( "Test Matrix 3x3" );
    const geode::SquareMatrix3D matrix{ { geode::Vector3D{ { 1, 2, 3 } },
        geode::Vector3D{ { -2, 1, 0 } }, geode::Vector3D{ { 3, 0.5, 1 } } } };

    OPENGEODE_EXCEPTION( matrix.value( 0, 0 ) == 1,
        "[TEST] Wrong value for matrix coeff [0,0]" );
    OPENGEODE_EXCEPTION( matrix.value( 0, 1 ) == 2,
        "[TEST] Wrong value for matrix coeff [0,1]" );
    OPENGEODE_EXCEPTION( matrix.value( 0, 2 ) == 3,
        "[TEST] Wrong value for matrix coeff [0,2]" );
    OPENGEODE_EXCEPTION( matrix.value( 1, 0 ) == -2,
        "[TEST] Wrong value for matrix coeff [1,0]" );
    OPENGEODE_EXCEPTION( matrix.value( 1, 1 ) == 1,
        "[TEST] Wrong value for matrix coeff [1,1]" );
    OPENGEODE_EXCEPTION( matrix.value( 1, 2 ) == 0,
        "[TEST] Wrong value for matrix coeff [1,2]" );
    OPENGEODE_EXCEPTION( matrix.value( 2, 0 ) == 3,
        "[TEST] Wrong value for matrix coeff [2,0]" );
    OPENGEODE_EXCEPTION( matrix.value( 2, 1 ) == 0.5,
        "[TEST] Wrong value for matrix coeff [2,1]" );
    OPENGEODE_EXCEPTION( matrix.value( 2, 2 ) == 1,
        "[TEST] Wrong value for matrix coeff [2,2]" );

    const auto mult_result = matrix * geode::Vector3D{ { 3, 2, 1 } };
    const geode::Vector3D answer{ { 10, -4, 11 } };
    OPENGEODE_EXCEPTION( mult_result == answer,
        "[TEST] Wrong result for matrix multiplication: ", mult_result.string(),
        " instead of [10,-4,11]" );

    const auto det = matrix.determinant();
    OPENGEODE_EXCEPTION(
        det == -7, "[TEST] Wrong determinant value: ", det, " instead of -7" );

    const auto transpose_matrix = matrix.transpose();
    OPENGEODE_EXCEPTION( transpose_matrix.value( 0, 0 ) == 1,
        "[TEST] Wrong value for transpose matrix coeff [0,0]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 0, 1 ) == -2,
        "[TEST] Wrong value for transpose matrix coeff [0,1]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 0, 2 ) == 3,
        "[TEST] Wrong value for transpose matrix coeff [0,2]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 1, 0 ) == 2,
        "[TEST] Wrong value for transpose matrix coeff [1,0]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 1, 1 ) == 1,
        "[TEST] Wrong value for transpose matrix coeff [1,1]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 1, 2 ) == 0.5,
        "[TEST] Wrong value for transpose matrix coeff [1,2]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 2, 0 ) == 3,
        "[TEST] Wrong value for transpose matrix coeff [2,0]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 2, 1 ) == 0,
        "[TEST] Wrong value for transpose matrix coeff [2,1]" );
    OPENGEODE_EXCEPTION( transpose_matrix.value( 2, 2 ) == 1,
        "[TEST] Wrong value for transpose matrix coeff [2,2]" );

    const auto inverse_matrix = matrix.inverse();
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 0, 0 ) - -1. / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [0,0]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 0, 1 ) - 1. / 14 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [0,1]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 0, 2 ) - 3. / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [0,2]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 1, 0 ) - -2. / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [1,0]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 1, 1 ) - 8. / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [1,1]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 1, 2 ) - 6. / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [1,2]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 2, 0 ) - 4. / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [2,0]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 2, 1 ) - -5.5 / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [2,1]" );
    OPENGEODE_EXCEPTION( std::fabs( inverse_matrix.value( 2, 2 ) - -5. / 7 )
                             < geode::GLOBAL_EPSILON,
        "[TEST] Wrong value for inverse matrix coeff [2,2]" );
}

void test()
{
    test_matrix_2x2();
    test_matrix_3x3();
}

OPENGEODE_TEST( "vector" )