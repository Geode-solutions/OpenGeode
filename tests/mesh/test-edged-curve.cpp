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

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/builder/geode_edged_curve_builder.h>
#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/io/edged_curve_input.h>
#include <geode/mesh/io/edged_curve_output.h>

#include <geode/tests/common.h>

void test_create_vertices( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    OPENGEODE_EXCEPTION( edged_curve.nb_vertices() == 2,
        "[Test] EdgedCurve should have 2 vertices" );
    builder.create_vertices( 2 );
    builder.set_point( 2, { { 7.5, 5.2, 6.3 } } );
    builder.set_point( 3, { { 8.7, 1.4, 4.7 } } );
    OPENGEODE_EXCEPTION( edged_curve.nb_vertices() == 4,
        "[Test] EdgedCurve should have 4 vertices" );
}

void test_bounding_box( const geode::EdgedCurve3D& edged_curve )
{
    geode::Point3D answer_min{ { 0.1, 0.2, 0.3 } };
    geode::Point3D answer_max{ { 8.7, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( edged_curve.bounding_box().min() == answer_min,
        "[Test] Wrong computation of bounding box (min)" );
    OPENGEODE_EXCEPTION( edged_curve.bounding_box().max() == answer_max,
        "[Test] Wrong computation of bounding box (max)" );
}

void test_delete_vertex( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    std::vector< bool > to_delete( edged_curve.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( edged_curve.nb_vertices() == 3,
        "[Test] EdgedCurve should have 3 vertices" );
    const geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( edged_curve.point( 0 ) == answer,
        "[Test] EdgedCurve vertex coordinates are not correct" );
    OPENGEODE_EXCEPTION(
        edged_curve.nb_edges() == 2, "[Test] EdgedCurve should have 2 edges" );

    const auto& edges_around_0 = edged_curve.edges_around_vertex( 0 );
    OPENGEODE_EXCEPTION( edges_around_0.size() == 1,
        "[Test] edges_around_0 should have 1 edge" );
    OPENGEODE_EXCEPTION( edges_around_0[0].edge_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[0].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );

    const auto& edges_around_2 = edged_curve.edges_around_vertex( 2 );
    OPENGEODE_EXCEPTION( edges_around_2.size() == 1,
        "[Test] edges_around_2 should have 1 edge" );
    OPENGEODE_EXCEPTION( edges_around_2[0].edge_id == 0,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[0].vertex_id == 0,
        "[Test] edges_around_2 has wrong value" );
}

void test_create_edges( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    builder.create_edge( 0, 1 );
    builder.create_edge( 0, 2 );
    OPENGEODE_EXCEPTION(
        edged_curve.nb_edges() == 2, "[Test] EdgedCurve should have 2 edges" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 1 } ) == 1,
        "[Test] EdgedCurve edge vertex is not correct" );
    builder.create_edges( 2 );
    builder.set_edge_vertex( { 2, 0 }, 3 );
    builder.set_edge_vertex( { 2, 1 }, 2 );
    builder.set_edge_vertex( { 3, 0 }, 1 );
    builder.set_edge_vertex( { 3, 1 }, 2 );
    OPENGEODE_EXCEPTION(
        edged_curve.nb_edges() == 4, "[Test] EdgedCurve should have 4 edges" );

    const auto& edges_around_0 = edged_curve.edges_around_vertex( 0 );
    OPENGEODE_EXCEPTION( edges_around_0.size() == 2,
        "[Test] edges_around_0 should have 2 edges" );
    OPENGEODE_EXCEPTION( edges_around_0[0].edge_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[0].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].edge_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );

    const auto& edges_around_2 = edged_curve.edges_around_vertex( 2 );
    OPENGEODE_EXCEPTION( edges_around_2.size() == 3,
        "[Test] edges_around_2 should have 3 edges" );
    OPENGEODE_EXCEPTION( edges_around_2[0].edge_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[0].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].edge_id == 2,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].edge_id == 3,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
}

void test_delete_edge( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    std::vector< bool > to_delete( edged_curve.nb_edges(), false );
    to_delete.front() = true;
    builder.delete_edges( to_delete );
    OPENGEODE_EXCEPTION(
        edged_curve.nb_edges() == 1, "[Test] EdgedCurve should have 1 edges" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 0 } ) == 0,
        "[Test] EdgedCurve edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 1 } ) == 1,
        "[Test] EdgedCurve edge vertex index is not correct" );
}

void test_io(
    const geode::EdgedCurve3D& edged_curve, const std::string& filename )
{
    geode::save_edged_curve( edged_curve, filename );
    const auto new_edged_curve = geode::load_edged_curve< 3 >(
        geode::OpenGeodeEdgedCurve3D::type_name_static(), filename );
}

void test_edge_requests( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    OPENGEODE_EXCEPTION( edged_curve.edge_barycenter( 0 ).inexact_equal(
                             geode::Point3D( { 4.8, 7.3, 6.5 } ), 1e-15 ),
        "[Test] Edge barycenter is not correct" );
    const auto p0 = builder.create_point( { { 1, 1, 1 } } );
    const auto p1 = builder.create_point( { { 1, 4, -3 } } );
    OPENGEODE_EXCEPTION(
        edged_curve.edge_length( builder.create_edge( p0, p1 ) ) == 5,
        "[Test] Edge length is not correct" );
}

void test_clone( const geode::EdgedCurve3D& edged_curve )
{
    auto attribute =
        edged_curve.edge_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, int >(
                "test", 0 );
    attribute->set_value( 0, 42 );

    const auto edged_curve2 = edged_curve.clone();
    OPENGEODE_EXCEPTION( edged_curve2->nb_vertices() == 3,
        "[Test] EdgedCurve2 should have 3 vertices" );
    OPENGEODE_EXCEPTION( edged_curve2->nb_edges() == 1,
        "[Test] EdgedCurve2 should have 1 edge" );

    const auto attribute2 =
        edged_curve2->edge_attribute_manager().find_attribute< int >( "test" );
    OPENGEODE_EXCEPTION( attribute2->value( 0 ) == 42,
        "[Test] EdgedCurve2 attribute should be 42" );
}

void test()
{
    auto edged_curve = geode::EdgedCurve3D::create(
        geode::OpenGeodeEdgedCurve3D::type_name_static() );
    auto builder = geode::EdgedCurveBuilder3D::create( *edged_curve );

    test_create_vertices( *edged_curve, *builder );
    test_create_edges( *edged_curve, *builder );
    test_bounding_box( *edged_curve );
    test_io( *edged_curve,
        absl::StrCat( "test.", edged_curve->native_extension() ) );

    test_delete_vertex( *edged_curve, *builder );
    test_delete_edge( *edged_curve, *builder );
    test_clone( *edged_curve );

    test_edge_requests( *edged_curve, *builder );
}

OPENGEODE_TEST( "edged-curve" )