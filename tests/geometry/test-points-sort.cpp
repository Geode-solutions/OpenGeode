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

#include <geode/basic/logger.h>

#include <geode/geometry/point.h>
#include <geode/geometry/points_sort.h>

#include <geode/tests/common.h>

void test_lexicographic_mapping()
{
    const geode::Point3D pt0{ { 0., 0., 0. } };
    const geode::Point3D pt1{ { 1., 0., 0. } };
    const geode::Point3D pt2{ { 2., 0., 0. } };
    const geode::Point3D pt3{ { 2., 1., 0. } };
    const geode::Point3D pt4{ { 1., 1., 0. } };
    const geode::Point3D pt5{ { 0., 1., 0. } };
    const geode::Point3D pt6{ { 0., 2., 0. } };
    const geode::Point3D pt7{ { 1., 2., 0. } };
    const geode::Point3D pt8{ { 2., 2., 0. } };
    const geode::Point3D pt9{ { 0., 0., 1. } };
    const geode::Point3D pt10{ { 1., 0., 1. } };

    std::vector< geode::Point3D > pts{ pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7,
        pt8, pt9, pt10 };
    const auto mapping = geode::lexicographic_mapping< 3 >( pts );
    std::vector< geode::index_t > answer{ 0, 9, 5, 6, 1, 10, 4, 7, 2, 3, 8 };
    for( const auto m : geode::Indices{ mapping } )
    {
        OPENGEODE_EXCEPTION( mapping[m] == answer[m],
            "[Test] Wrong result in lexicographic sort" );
    }
}

void test()
{
    test_lexicographic_mapping();
}

OPENGEODE_TEST( "points_sort" )