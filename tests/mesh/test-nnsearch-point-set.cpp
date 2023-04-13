/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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
#include <geode/geometry/point.h>

#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/helpers/nnsearch_mesh.h>

#include <geode/tests/common.h>

void add_points( geode::PointSetBuilder3D& builder )
{
    for( const auto i : geode::LRange{ 3 } )
    {
        for( const auto j : geode::LRange{ 3 } )
        {
            for( const auto k : geode::LRange{ 3 } )
            {
                builder.create_point(
                    { { static_cast< double >( i ), static_cast< double >( j ),
                        static_cast< double >( k ) } } );
                builder.create_point(
                    { { static_cast< double >( i ), static_cast< double >( j ),
                        static_cast< double >( k ) } } );
            }
        }
    }
}

void check_nnsearch( const geode::NNSearch3D& nnsearch )
{
    const auto mappings =
        nnsearch.colocated_index_mapping( geode::global_epsilon );
    OPENGEODE_EXCEPTION( nnsearch.nb_points() == 54,
        "[Test] Wrong computation of NNSearch points" );
    OPENGEODE_EXCEPTION( mappings.nb_unique_points() == 27,
        "[Test] Wrong computation of NNSearch unique points" );
    geode::index_t counter{ 0 };
    for( const auto i : geode::LRange{ 3 } )
    {
        for( const auto j : geode::LRange{ 3 } )
        {
            for( const auto k : geode::LRange{ 3 } )
            {
                const geode::Point3D query{ { static_cast< double >( i ),
                    static_cast< double >( j ), static_cast< double >( k ) } };
                OPENGEODE_EXCEPTION(
                    mappings.colocated_mapping[nnsearch.closest_neighbor(
                        query )]
                        == counter,
                    "[Test] Wrong computation of NNSearch" );
                counter++;
            };
        }
    }
}

void test_nn_search3d()
{
    auto pointset = geode::PointSet3D::create();
    auto pointset_builder = geode::PointSetBuilder3D::create( *pointset );
    add_points( *pointset_builder );
    OPENGEODE_EXCEPTION( pointset->nb_vertices() == 54,
        "[Test] Wrong computation of PointSet3D points" );

    check_nnsearch( geode::create_nn_search( *pointset ) );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_nn_search3d();
}

OPENGEODE_TEST( "nnsearch-point-set" )
