/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/logger.h>

#include <geode/geometry/nn_search.h>

#include <geode/tests/common.h>

void test()
{
    const geode::NNSearch2D search{ { { { 0.1, 4.2 } }, { { 5.9, 7.3 } },
        { { 1.8, -5 } }, { { -7.3, -1.6 } } } };

    OPENGEODE_EXCEPTION( search.closest_neighbor( { { 0, 0 } } ) == 0,
        "[Test] Error in closest neighbor" );
    OPENGEODE_EXCEPTION( search.closest_neighbor( { { 1, -4 } } ) == 2,
        "[Test] Error in closest neighbor" );

    const std::vector< geode::index_t > answer_radius{ 0, 2 };
    OPENGEODE_EXCEPTION(
        search.radius_neighbors( { { 0, 0 } }, 5.4 ) == answer_radius,
        "[Test] Error in radius neighbors" );

    const std::vector< geode::index_t > answer_neighbors{ 2, 0 };
    OPENGEODE_EXCEPTION(
        search.neighbors( { { -1, -1 } }, 2 ) == answer_neighbors,
        "[Test] Error in neighbors" );

    const geode::Point3D p0{ { 0.1, 2.9, 5.4 } };
    const geode::Point3D p1{ { 2.4, 8.1, 7.6 } };
    const geode::Point3D p2{ { 8.1, 4.2, 3.8 } };
    const geode::Point3D p3{ { 3.1, 9.4, 9.7 } };
    const geode::NNSearch3D colocator( { p0, p0, p1, p0, p2, p1, p3 } );

    const auto colocated_info = colocator.colocated_index_mapping( 1e-8 );
    OPENGEODE_EXCEPTION( colocated_info.nb_colocated_points() == 3,
        "[Test] Should be 3 colocated points" );
    const absl::FixedArray< geode::index_t > mapping_answer{ 0, 0, 1, 0, 2, 1,
        3 };
    OPENGEODE_EXCEPTION( colocated_info.colocated_mapping == mapping_answer,
        "[Test] Error in colocated mapping" );
    const absl::FixedArray< geode::Point3D > points_answer{ p0, p1, p2, p3 };
    OPENGEODE_EXCEPTION( colocated_info.unique_points == points_answer,
        "[Test] Error in unique points" );
}

OPENGEODE_TEST( "nnsearch" )