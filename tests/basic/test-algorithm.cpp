/*
 * Copyright (c) 2019 Geode-solutions
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

#include <functional>
#include <numeric>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

std::vector< bool > create_bool_vector()
{
    std::vector< bool > out( 4, false );
    out[1] = true;
    return out;
}

std::vector< double > create_double_vector()
{
    std::vector< double > out{ 0, 1.1, 2.2, 3.3 }; 
    return out;
}

void test_find()
{
    auto bool_vector = create_bool_vector();
    OPENGEODE_EXCEPTION( geode::find( bool_vector, true ) == 1,
        "[Test] Find result for bool true is not correct" );
    OPENGEODE_EXCEPTION( geode::find( bool_vector, false ) == 0,
        "[Test] Find result for bool false is not correct" );

    auto double_vector = create_double_vector();
    OPENGEODE_EXCEPTION( geode::find( double_vector, 2.2 ) == 2,
        "[Test] Find result for double 2.2 is not correct" );
    OPENGEODE_EXCEPTION( geode::find( double_vector, 1.3 ) == geode::NO_ID,
        "[Test] Find result for double 1.3 true is not correct" );
}

void test_contain()
{
    auto bool_vector = create_bool_vector();
    OPENGEODE_EXCEPTION( geode::contain( bool_vector, true ),
        "[Test] Contain result for bool true is not correct" );
    OPENGEODE_EXCEPTION( geode::contain( bool_vector, false ),
        "[Test] Contain result for bool false is not correct" );

    auto double_vector = create_double_vector();
    OPENGEODE_EXCEPTION( geode::contain( double_vector, 2.2 ),
        "[Test] Contain result for double 2.2 is not correct" );
    OPENGEODE_EXCEPTION( !geode::contain( double_vector, 1.3 ),
        "[Test] Contain result for double 1.3 is not correct" );
}

void test_delete_vector_elements()
{
    auto bool_vector = create_bool_vector();
    auto to_delete{ bool_vector };
    geode::delete_vector_elements( to_delete, bool_vector, false );
    OPENGEODE_EXCEPTION( bool_vector.size() == 4,
        "[Test] Delete elements result (size) for bool is not correct" );
    OPENGEODE_EXCEPTION( !bool_vector[0] & !bool_vector[1] & !bool_vector[2],
        "[Test] Delete elements result (values) for bool is not correct" );

    auto double_vector = create_double_vector();
    geode::delete_vector_elements( to_delete, double_vector, true );
    OPENGEODE_EXCEPTION( double_vector.size() == 3,
        "[Test] Delete elements result (size) for double is not correct" );
    OPENGEODE_EXCEPTION( ( double_vector[0] == 0. )
                             & ( double_vector[1] == 2 * 1.1 )
                             & ( double_vector[2] == 3 * 1.1 ),
        "[Test] Delete elements result (values) for double is not correct" );
}

void test_extract_vector_elements()
{
    auto bool_vector = create_bool_vector();
    auto to_keep{ bool_vector };
    auto bool_result = geode::extract_vector_elements( to_keep, bool_vector );
    OPENGEODE_EXCEPTION( bool_result.size() == 1,
        "[Test] Extract elements result (size) for bool is not correct" );
    OPENGEODE_EXCEPTION( bool_result[0],
        "[Test] Extract elements result (values) for bool is not correct" );

    auto double_vector = create_double_vector();
    auto double_result =
        geode::extract_vector_elements( to_keep, double_vector );
    OPENGEODE_EXCEPTION( double_result.size() == 1,
        "[Test] Extract elements result (size) for double is not correct" );
    OPENGEODE_EXCEPTION( double_result[0] == 1.1,
        "[Test] Extract elements result (values) for double is not correct" );
    std::vector< bool > keep_all( 4, true );
    auto double_copy_result =
        geode::extract_vector_elements( keep_all, double_vector );
    OPENGEODE_EXCEPTION( double_copy_result == double_vector,
        "[Test] Extract elements result (keep_all) for double is not correct" );
}

int main()
{
    using namespace geode;

    try
    {
        test_find();
        test_contain();
        test_delete_vector_elements();
        test_extract_vector_elements();

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
