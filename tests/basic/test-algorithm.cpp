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

#include <functional>
#include <numeric>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>
#include <geode/basic/uuid.h>

#include <geode/tests/common.h>

std::vector< bool > create_bool_vector()
{
    return { false, true, false, false };
}

std::vector< double > create_double_vector()
{
    return { 0.0, 1.1, 2.2, 3.3 };
}

std::vector< std::vector< std::pair< geode::uuid, geode::index_t > > >
    create_toto_vector()
{
    return { { { geode::uuid{}, 0 } }, { { geode::uuid{}, 1 } },
        { { geode::uuid{}, 2 } }, { { geode::uuid{}, 3 } } };
}

struct Toto
{
    geode::uuid tutu;
    geode::index_t titi;
};

std::vector< std::vector< Toto > > create_totostruct_vector()
{
    return { { { geode::uuid{}, 0 } }, { { geode::uuid{}, 1 } },
        { { geode::uuid{}, 2 } }, { { geode::uuid{}, 3 } } };
}

void test_delete_vector_elements()
{
    auto bool_vector = create_bool_vector();
    const auto to_delete = bool_vector;
    geode::delete_vector_elements( to_delete, bool_vector );
    OPENGEODE_EXCEPTION( bool_vector.size() == 3,
        "[Test] Delete elements result (size) for bool is not correct" );
    OPENGEODE_EXCEPTION( !bool_vector[0] & !bool_vector[1] & !bool_vector[2],
        "[Test] Delete elements result (values) for bool is not correct" );

    auto double_vector = create_double_vector();
    geode::delete_vector_elements( to_delete, double_vector );
    OPENGEODE_EXCEPTION( double_vector.size() == 3,
        "[Test] Delete elements result (size) for double is not correct" );
    OPENGEODE_EXCEPTION( ( double_vector[0] == 0.0 )
                             && ( double_vector[1] == 2.2 )
                             && ( double_vector[2] == 3.3 ),
        "[Test] Delete elements result (values) for double is not correct" );

    auto toto_vector = create_toto_vector();
    geode::delete_vector_elements( to_delete, toto_vector );
    OPENGEODE_EXCEPTION( toto_vector.size() == 3,
        "[Test] Delete elements result (size) for double is not correct" );
    toto_vector[0].front();
    toto_vector[1].front();
    toto_vector[2].front();
    OPENGEODE_EXCEPTION( ( toto_vector[0].front().second == 0 )
                             && ( toto_vector[1].front().second == 2 )
                             && ( toto_vector[2].front().second == 3 ),
        "[Test] Delete elements result (values) for double is not correct" );

    auto totos_vector = create_totostruct_vector();
    geode::delete_vector_elements( to_delete, totos_vector );
    OPENGEODE_EXCEPTION( totos_vector.size() == 3,
        "[Test] Delete elements result (size) for double is not correct" );
    OPENGEODE_EXCEPTION( ( totos_vector[0].front().titi == 0 )
                             && ( totos_vector[1].front().titi == 2 )
                             && ( totos_vector[2].front().titi == 3 ),
        "[Test] Delete elements result (values) for double is not correct" );
}

void test_extract_vector_elements()
{
    const auto bool_vector = create_bool_vector();
    const auto to_keep = bool_vector;
    const auto bool_result =
        geode::extract_vector_elements( to_keep, bool_vector );
    OPENGEODE_EXCEPTION( bool_result.size() == 1,
        "[Test] Extract elements result (size) for bool is not correct" );
    OPENGEODE_EXCEPTION( bool_result[0],
        "[Test] Extract elements result (values) for bool is not correct" );

    const auto double_vector = create_double_vector();
    const auto double_result =
        geode::extract_vector_elements( to_keep, double_vector );
    OPENGEODE_EXCEPTION( double_result.size() == 1,
        "[Test] Extract elements result (size) for double is not correct" );
    OPENGEODE_EXCEPTION( double_result[0] == 1.1,
        "[Test] Extract elements result (values) for double is not correct" );
    std::vector< bool > keep_all( 4, true );
    const auto double_copy_result =
        geode::extract_vector_elements( keep_all, double_vector );
    OPENGEODE_EXCEPTION( double_copy_result == double_vector,
        "[Test] Extract elements result (keep_all) for double is not correct" );
}

void test_sort_unique()
{
    std::vector< int > data{ 1, 2, 3, 1, 2, 3, 3, 4, 5, 4, 5, 6, 7 };
    geode::sort_unique( data );

    const std::vector< int > answer{ 1, 2, 3, 4, 5, 6, 7 };
    OPENGEODE_EXCEPTION( data == answer,
        "[Test] Vectors are not identical after sort_unique()" );
}

void test()
{
    test_delete_vector_elements();
    test_extract_vector_elements();
    test_sort_unique();
}

OPENGEODE_TEST( "algorithm" )
