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

#include <geode/tests/common.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/helpers/convert_edged_curve.h>

std::vector< std::unique_ptr< geode::EdgedCurve2D > > create_curves()
{
    std::vector< geode::Point2D > points{ { { 0, 0 } }, { { 2, 1 } },
        { { 4, -1 } }, { { 4, 2 } }, { { 0, 4 } }, { { 3, 3 } }, { { 6, 3 } },
        { { 9, 1 } } };

    auto mesh0 = geode::EdgedCurve2D::create();
    auto builder0 = geode::EdgedCurveBuilder2D::create( *mesh0 );
    builder0->create_point( points[0] );
    builder0->create_point( points[1] );
    builder0->create_point( points[2] );
    builder0->create_point( points[3] );
    builder0->create_edge( 0, 1 );
    builder0->create_edge( 1, 2 );
    builder0->create_edge( 2, 3 );

    auto mesh1 = geode::EdgedCurve2D::create();
    auto builder1 = geode::EdgedCurveBuilder2D::create( *mesh1 );
    builder1->create_point( points[1] );
    builder1->create_point( points[2] );
    builder1->create_point( points[3] );
    builder1->create_edge( 0, 2 );
    builder1->create_edge( 1, 2 );

    auto mesh2 = geode::EdgedCurve2D::create();
    auto builder2 = geode::EdgedCurveBuilder2D::create( *mesh2 );
    builder2->create_point( points[4] );
    builder2->create_point( points[5] );
    builder2->create_point( points[6] );
    builder2->create_point( points[6] );
    builder2->create_point( points[7] );
    builder2->create_edge( 0, 1 );
    builder2->create_edge( 1, 2 );
    builder2->create_edge( 3, 4 );

    std::vector< std::unique_ptr< geode::EdgedCurve2D > > curves;
    curves.push_back( std::move( mesh0 ) );
    curves.push_back( std::move( mesh1 ) );
    curves.push_back( std::move( mesh2 ) );
    return curves;
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();

    const auto curves = create_curves();
    const auto merged = geode::merge_edged_curves< 2 >(
        { *curves[0], *curves[1], *curves[2] } );
    OPENGEODE_EXCEPTION(
        merged->nb_vertices() == 8, "[Test] Wrong number of vertices" );
    OPENGEODE_EXCEPTION(
        merged->nb_edges() == 7, "[Test] Wrong number of edges" );
}

OPENGEODE_TEST( "merge-curve" )