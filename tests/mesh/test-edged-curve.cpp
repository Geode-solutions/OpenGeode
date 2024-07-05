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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/builder/geode/geode_edged_curve_builder.h>
#include <geode/mesh/core/geode/geode_edged_curve.h>
#include <geode/mesh/io/edged_curve_input.h>
#include <geode/mesh/io/edged_curve_output.h>

#include <geode/tests/common.h>

void test_create_vertices( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    builder.create_point( geode::Point3D{ { 0.1, 0.2, 0.3 } } );
    builder.create_point( geode::Point3D{ { 2.1, 9.4, 6.7 } } );
    OPENGEODE_EXCEPTION( edged_curve.nb_vertices() == 2,
        "[Test] EdgedCurve should have 2 vertices" );
    builder.create_vertices( 2 );
    builder.set_point( 2, geode::Point3D{ { 7.5, 5.2, 6.3 } } );
    builder.set_point( 3, geode::Point3D{ { 8.7, 1.4, 4.7 } } );
    OPENGEODE_EXCEPTION( edged_curve.nb_vertices() == 4,
        "[Test] EdgedCurve should have 4 vertices" );
}

void test_permutation( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 2, 0, 3, 1 };
    builder.permute_vertices( vertex_permutation );
    OPENGEODE_EXCEPTION(
        edged_curve.point( 2 ) == geode::Point3D( { 8.7, 1.4, 4.7 } ),
        "[Test] Point coordinates have not been correctly permuted" );
    OPENGEODE_EXCEPTION(
        edged_curve.point( 0 ) == geode::Point3D( { 7.5, 5.2, 6.3 } ),
        "[Test] Point coordinates have not been correctly permuted" );

    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 0 } ) == 1,
        "[Test] Wrong EdgeVertex after vertex permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 1 } ) == 3,
        "[Test] Wrong EdgeVertex after vertex permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 1, 0 } ) == 1,
        "[Test] Wrong EdgeVertex after vertex permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 1, 1 } ) == 0,
        "[Test] Wrong EdgeVertex after vertex permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 2, 0 } ) == 2,
        "[Test] Wrong EdgeVertex after vertex permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 2, 1 } ) == 0,
        "[Test] Wrong EdgeVertex after vertex permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 3, 0 } ) == 3,
        "[Test] Wrong EdgeVertex after vertex permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 3, 1 } ) == 0,
        "[Test] Wrong EdgeVertex after vertex permute" );

    std::vector< geode::index_t > edge_permutation{ 1, 3, 0, 2 };
    builder.permute_edges( edge_permutation );

    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 0 } ) == 1,
        "[Test] Wrong EdgeVertex after edge permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 1 } ) == 0,
        "[Test] Wrong EdgeVertex after edge permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 1, 0 } ) == 3,
        "[Test] Wrong EdgeVertex after edge permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 1, 1 } ) == 0,
        "[Test] Wrong EdgeVertex after edge permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 2, 0 } ) == 1,
        "[Test] Wrong EdgeVertex after edge permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 2, 1 } ) == 3,
        "[Test] Wrong EdgeVertex after edge permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 3, 0 } ) == 2,
        "[Test] Wrong EdgeVertex after edge permute" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 3, 1 } ) == 0,
        "[Test] Wrong EdgeVertex after edge permute" );

    const auto& edges_around_1 = edged_curve.edges_around_vertex( 1 );
    OPENGEODE_EXCEPTION( edges_around_1.size() == 2,
        "[Test] edges_around_1 should have 2 edges" );
    OPENGEODE_EXCEPTION( edges_around_1[0].edge_id == 2,
        "[Test] edges_around_1 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_1[0].vertex_id == 0,
        "[Test] edges_around_1 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_1[1].edge_id == 0,
        "[Test] edges_around_1 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_1[1].vertex_id == 0,
        "[Test] edges_around_1 has wrong value" );

    const auto& edges_around_0 = edged_curve.edges_around_vertex( 0 );
    OPENGEODE_EXCEPTION( edges_around_0.size() == 3,
        "[Test] edges_around_0 should have 3 edges" );
    OPENGEODE_EXCEPTION( edges_around_0[0].edge_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[0].vertex_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].edge_id == 3,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].vertex_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[2].edge_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[2].vertex_id == 1,
        "[Test] edges_around_0 has wrong value" );
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

void test_create_edges( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    builder.create_edge( 0, 1 );
    builder.create_edge( 0, 2 );
    OPENGEODE_EXCEPTION(
        edged_curve.nb_edges() == 2, "[Test] EdgedCurve should have 2 edges" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 1 } ) == 1,
        "[Test] EdgedCurve edge vertex is not correct" );
    builder.create_edges( 2 );
    builder.set_edge_vertex( { 2, 0 }, 3 );
    builder.set_edge_vertex( { 2, 1 }, 2 );
    builder.set_edge_vertex( { 3, 0 }, 1 );
    builder.set_edge_vertex( { 3, 1 }, 2 );
    OPENGEODE_EXCEPTION(
        edged_curve.nb_edges() == 4, "[Test] EdgedCurve should have 4 edges" );

    const auto& edges_around_0 = edged_curve.edges_around_vertex( 0 );
    OPENGEODE_EXCEPTION( edges_around_0.size() == 2,
        "[Test] edges_around_0 should have 2 edges" );
    OPENGEODE_EXCEPTION( edges_around_0[0].edge_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[0].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].edge_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );

    const auto& edges_around_2 = edged_curve.edges_around_vertex( 2 );
    OPENGEODE_EXCEPTION( edges_around_2.size() == 3,
        "[Test] edges_around_2 should have 3 edges" );
    OPENGEODE_EXCEPTION( edges_around_2[0].edge_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[0].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].edge_id == 2,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].edge_id == 3,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );

    const auto vertices_around_1 = edged_curve.vertices_around_vertex( 1 );
    OPENGEODE_EXCEPTION( vertices_around_1.size() == 2,
        "[Test] vertices_around_1 should have 2 vertices, not ",
        vertices_around_1.size() );
    for( const auto vertex_id : vertices_around_1 )
    {
        OPENGEODE_EXCEPTION( vertex_id == 0 || vertex_id == 2,
            "vertices_around_1 has wrong values." );
    }
    const auto vertices_around_2 = edged_curve.vertices_around_vertex( 2 );
    OPENGEODE_EXCEPTION( vertices_around_2.size() == 3,
        "[Test] vertices_around_2 should have 3 vertex" );
    for( const auto vertex_id : vertices_around_2 )
    {
        OPENGEODE_EXCEPTION( vertex_id == 0 || vertex_id == 1 || vertex_id == 3,
            "vertices_around_2 has wrong values." );
    }
}

void test_delete_edge( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    std::vector< bool > to_delete( edged_curve.nb_edges(), false );
    to_delete.front() = true;
    builder.delete_edges( to_delete );
    OPENGEODE_EXCEPTION(
        edged_curve.nb_edges() == 3, "[Test] EdgedCurve should have 3 edges" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 0 } ) == 3,
        "[Test] EdgedCurve edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 0, 1 } ) == 0,
        "[Test] EdgedCurve edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 1, 0 } ) == 1,
        "[Test] EdgedCurve edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 1, 1 } ) == 3,
        "[Test] EdgedCurve edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 2, 0 } ) == 2,
        "[Test] EdgedCurve edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( edged_curve.edge_vertex( { 2, 1 } ) == 0,
        "[Test] EdgedCurve edge vertex index is not correct" );
}

void test_io(
    const geode::EdgedCurve3D& edged_curve, const std::string& filename )
{
    geode::save_edged_curve( edged_curve, filename );
    geode::load_edged_curve< 3 >( filename );
    const auto reload = geode::load_edged_curve< 3 >(
        geode::OpenGeodeEdgedCurve3D::impl_name_static(), filename );
    OPENGEODE_EXCEPTION( reload->nb_edges() == edged_curve.nb_edges(),
        "[Test] Reload EdgedCurve has wrong number of edges" );
    OPENGEODE_EXCEPTION( reload->nb_vertices() == edged_curve.nb_vertices(),
        "[Test] Reload EdgedCurve has wrong number of vertices" );
    auto manager = reload->texture_manager();
    auto texture_names = manager.texture_names();
    OPENGEODE_EXCEPTION( texture_names.size() == 1,
        "[Test] Reloaded EdgedCurve has wrong number of textures" );
    OPENGEODE_EXCEPTION( texture_names[0] == "texture",
        "[Test] Reloaded EdgedCurve has wrong texture name" );
    for( const auto vertex_id : geode::Range{ edged_curve.nb_vertices() } )
    {
        OPENGEODE_EXCEPTION( edged_curve.point( vertex_id )
                                 .inexact_equal( reload->point( vertex_id ) ),
            "[Test] Reloaded EdgedCurve has wrong point coordinates" );
    }
}

void test_backward_io( const std::string& filename )
{
    const auto curve = geode::load_edged_curve< 3 >( filename );
    OPENGEODE_EXCEPTION( curve->nb_edges() == 4,
        "[Test] Backward EdgedCurve has wrong number of edges" );
    OPENGEODE_EXCEPTION( curve->nb_vertices() == 4,
        "[Test] Backward EdgedCurve has wrong number of vertices" );
}

void test_edge_requests( const geode::EdgedCurve3D& edged_curve,
    geode::EdgedCurveBuilder3D& builder )
{
    OPENGEODE_EXCEPTION( edged_curve.edge_barycenter( 0 ).inexact_equal(
                             geode::Point3D( { 4.8, 7.3, 6.5 } ) ),
        "[Test] Edge barycenter is not correct" );
    const auto p0 = builder.create_point( geode::Point3D{ { 1, 1, 1 } } );
    const auto p1 = builder.create_point( geode::Point3D{ { 1, 4, -3 } } );
    OPENGEODE_EXCEPTION(
        edged_curve.edge_length( builder.create_edge( p0, p1 ) ) == 5,
        "[Test] Edge length is not correct" );
}

void test_clone( const geode::EdgedCurve3D& edged_curve )
{
    auto attribute =
        edged_curve.edge_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, int >(
                "test", 0 );
    attribute->set_value( 0, 42 );

    const auto edged_curve_clone = edged_curve.clone();
    geode::OpenGeodeEdgedCurve3D edged_curve2{ std::move(
        *dynamic_cast< geode::OpenGeodeEdgedCurve3D* >(
            edged_curve_clone.get() ) ) };
    OPENGEODE_EXCEPTION( edged_curve2.nb_vertices() == 4,
        "[Test] EdgedCurve2 should have 4 vertices" );
    OPENGEODE_EXCEPTION(
        edged_curve2.nb_edges() == 3, "[Test] EdgedCurve2 should have 3 edge" );

    const auto attribute2 =
        edged_curve2.edge_attribute_manager().find_attribute< int >( "test" );
    OPENGEODE_EXCEPTION( attribute2->value( 0 ) == 42,
        "[Test] EdgedCurve2 attribute should be 42" );
}

void test_texture( const geode::EdgedCurve3D& edged_curve )
{
    auto manager = edged_curve.texture_manager();
    manager.find_or_create_texture( "texture" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto edged_curve = geode::EdgedCurve3D::create(
        geode::OpenGeodeEdgedCurve3D::impl_name_static() );
    auto builder = geode::EdgedCurveBuilder3D::create( *edged_curve );

    test_create_vertices( *edged_curve, *builder );
    test_create_edges( *edged_curve, *builder );
    test_bounding_box( *edged_curve );
    test_texture( *edged_curve );

    test_io( *edged_curve,
        absl::StrCat( "test.", edged_curve->native_extension() ) );
    test_backward_io( absl::StrCat(
        geode::data_path, "test_v12.", edged_curve->native_extension() ) );

    test_permutation( *edged_curve, *builder );
    test_delete_edge( *edged_curve, *builder );
    test_clone( *edged_curve );

    test_edge_requests( *edged_curve, *builder );
}

OPENGEODE_TEST( "edged-curve" )