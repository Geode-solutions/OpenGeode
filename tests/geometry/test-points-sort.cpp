/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <algorithm>
#include <random>

#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/points_sort.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

#include <absl/algorithm/container.h>

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
        geode::OpenGeodeGeometryException::test(
            mapping[m] == answer[m], "Wrong result in lexicographic sort" );
    }
}

template < geode::index_t dimension >
double average_step( const std::vector< geode::Point< dimension > >& points,
    const std::vector< geode::index_t >& mapping )
{
    double total{ 0 };
    for( const auto i : geode::Range{ 1, mapping.size() } )
    {
        total += geode::Vector< dimension >{ points[mapping[i - 1]],
            points[mapping[i]] }
                     .length();
    }
    return total / ( mapping.size() - 1 );
}

template < geode::index_t dimension >
void check_permutation( std::vector< geode::index_t > mapping )
{
    absl::c_sort( mapping );
    for( const auto i : geode::Indices{ mapping } )
    {
        geode::OpenGeodeGeometryException::test(
            mapping[i] == i, "Morton mapping should be a permutation" );
    }
}

void test_flat_morton_mapping()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -10, 10 };
    std::vector< geode::Point3D > flat_points;
    std::vector< geode::Point2D > points2d;
    std::vector< geode::Point3D > points3d;
    for( const auto unused : geode::Range{ 20000 } )
    {
        geode_unused( unused );
        const auto x = distribution( generator );
        const auto y = distribution( generator );
        flat_points.emplace_back( geode::Point3D{ { x, y, 0 } } );
        points2d.emplace_back( geode::Point2D{ { x, y } } );
        points3d.emplace_back(
            geode::Point3D{ { x, y, distribution( generator ) } } );
    }
    // Duplicated points
    for( const auto p : geode::Range{ 100 } )
    {
        flat_points.push_back( flat_points[p] );
        points2d.push_back( points2d[p] );
        points3d.push_back( points3d[p] );
    }
    const auto flat_mapping = geode::morton_mapping< 3 >( flat_points );
    const auto mapping2d = geode::morton_mapping< 2 >( points2d );
    const auto mapping3d = geode::morton_mapping< 3 >( points3d );
    check_permutation< 3 >( flat_mapping );
    check_permutation< 2 >( mapping2d );
    check_permutation< 3 >( mapping3d );
    const auto flat_step = average_step< 3 >( flat_points, flat_mapping );
    const auto step2d = average_step< 2 >( points2d, mapping2d );
    geode::OpenGeodeGeometryException::test( flat_step < 1.2 * step2d,
        "Flat 3D Morton mapping should be as local as the 2D one: ", flat_step,
        " vs ", step2d );
    const auto same_points =
        std::vector< geode::Point3D >( 1000, geode::Point3D{ { 1, 2, 3 } } );
    check_permutation< 3 >( geode::morton_mapping< 3 >( same_points ) );
}

void test()
{
    test_lexicographic_mapping();
    test_flat_morton_mapping();
}

OPENGEODE_TEST( "points_sort" )