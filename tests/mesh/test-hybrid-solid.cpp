/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/geode_hybrid_solid_builder.h>
#include <geode/mesh/builder/solid_edges_builder.h>
#include <geode/mesh/builder/solid_facets_builder.h>
#include <geode/mesh/core/geode_hybrid_solid.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_facets.h>
#include <geode/mesh/io/hybrid_solid_input.h>
#include <geode/mesh/io/hybrid_solid_output.h>

#include <geode/tests/common.h>

void test_create_vertices( const geode::HybridSolid3D& hybrid_solid,
    geode::HybridSolidBuilder3D& builder )
{
    builder.create_point( { { 0, 0, 0 } } );
    builder.create_point( { { 1, 0, 0 } } );
    builder.create_point( { { 2, 1, 0 } } );
    builder.create_point( { { 1, 2, 0 } } );
    builder.create_point( { { 0, 2, 0 } } );
    builder.create_point( { { 0, 0, 1 } } );
    builder.create_point( { { 1, 0, 1 } } );
    builder.create_point( { { 2, 1, 1 } } );
    builder.create_point( { { 1, 2, 1 } } );
    builder.create_point( { { 0, 2, 1 } } );
    builder.create_point( { { 1, 1, 2 } } );
    OPENGEODE_EXCEPTION( hybrid_solid.is_vertex_isolated( 0 ),
        "[Test] Vertices should be isolated before polyhedra creation" );
    OPENGEODE_EXCEPTION( hybrid_solid.nb_vertices() == 11,
        "[Test] HybridSolid should have 11 vertices" );
}

void test_bounding_box( const geode::HybridSolid3D& hybrid_solid )
{
    geode::Point3D answer_min{ { 0, 0, 0 } };
    geode::Point3D answer_max{ { 2, 2, 2 } };
    const auto bbox = hybrid_solid.bounding_box();
    OPENGEODE_EXCEPTION( bbox.min() == answer_min,
        "[Test] Wrong computation of bounding box (min)" );
    OPENGEODE_EXCEPTION( bbox.max() == answer_max,
        "[Test] Wrong computation of bounding box (max)" );
}

void test_facets( const geode::HybridSolid3D& hybrid_solid )
{
    OPENGEODE_EXCEPTION(
        hybrid_solid.facets().facet_from_vertices( { 0, 1, 3, 4 } ) == 0,
        "[Test] Wrong facet from vertices" );
    OPENGEODE_EXCEPTION(
        hybrid_solid.facets().facet_from_vertices( { 8, 6, 7 } ) == 7,
        "[Test] Wrong facet from vertices" );

    geode::Point3D answer{ { 1.5, 0.5, 0.5 } };
    const auto vertices = hybrid_solid.facets().facet_vertices( 8 );
    OPENGEODE_EXCEPTION( hybrid_solid.facet_barycenter( vertices ) == answer,
        "[Test] Wrong facet barycenter" );
}

void test_edges( const geode::HybridSolid3D& hybrid_solid )
{
    geode::Point3D answer{ { 0.5, 0, 0 } };
    const auto vertices = hybrid_solid.edges().edge_vertices( 0 );
    OPENGEODE_EXCEPTION( hybrid_solid.edge_barycenter( vertices ) == answer,
        "[Test] Wrong edge barycenter" );
    OPENGEODE_EXCEPTION(
        hybrid_solid.edge_length( vertices ) == 1, "[Test] Wrong edge length" );
}

void test_create_polyhedra( const geode::HybridSolid3D& hybrid_solid,
    geode::HybridSolidBuilder3D& builder )
{
    builder.create_hexahedron( { 0, 1, 3, 4, 5, 6, 8, 9 } );
    OPENGEODE_EXCEPTION(
        hybrid_solid.polyhedron_type( hybrid_solid.nb_polyhedra() - 1 )
            == geode::HybridSolid3D::Type::HEXAHEDRON,
        "[Test] Wrong polyhedron type" );
    builder.create_prism( { 1, 2, 3, 6, 7, 8 } );
    OPENGEODE_EXCEPTION(
        hybrid_solid.polyhedron_type( hybrid_solid.nb_polyhedra() - 1 )
            == geode::HybridSolid3D::Type::PRISM,
        "[Test] Wrong polyhedron type" );
    builder.create_pyramid( { 5, 6, 8, 9, 10 } );
    OPENGEODE_EXCEPTION(
        hybrid_solid.polyhedron_type( hybrid_solid.nb_polyhedra() - 1 )
            == geode::HybridSolid3D::Type::PYRAMID,
        "[Test] Wrong polyhedron type" );
    builder.create_tetrahedron( { 6, 7, 8, 10 } );
    OPENGEODE_EXCEPTION(
        hybrid_solid.polyhedron_type( hybrid_solid.nb_polyhedra() - 1 )
            == geode::HybridSolid3D::Type::TETRAHEDRON,
        "[Test] Wrong polyhedron type" );
    OPENGEODE_EXCEPTION( hybrid_solid.nb_polyhedra() == 4,
        "[Test] HybridSolid should have 4 polyhedra" );
    OPENGEODE_EXCEPTION( hybrid_solid.facets().nb_facets() == 16,
        "[Test] HybridSolid should have 16 facets" );
    OPENGEODE_EXCEPTION( hybrid_solid.edges().nb_edges() == 22,
        "[Test] HybridSolid should have 22 edges" );
    OPENGEODE_EXCEPTION( !hybrid_solid.is_vertex_isolated( 0 ),
        "[Test] Vertices should not be isolated after polyhedra creation" );
}

void test_polyhedron_adjacencies( const geode::HybridSolid3D& hybrid_solid,
    geode::HybridSolidBuilder3D& builder )
{
    builder.compute_polyhedron_adjacencies();
    OPENGEODE_EXCEPTION( !hybrid_solid.polyhedron_adjacent( { 0, 0 } ),
        "[Test] HybridSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_adjacent( { 0, 1 } ) == 2,
        "[Test] HybridSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !hybrid_solid.polyhedron_adjacent( { 0, 2 } ),
        "[Test] HybridSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_adjacent( { 1, 1 } ) == 3,
        "[Test] HybridSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_adjacent( { 1, 4 } ) == 0,
        "[Test] HybridSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_adjacent( { 2, 2 } ) == 3,
        "[Test] HybridSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedra_around_vertex( 6 ).size() == 4,
        "[Test] HybridSolid should have 4 polyhedra around this vertex" );
    OPENGEODE_EXCEPTION(
        hybrid_solid.polyhedron_facets_on_border( 0 ).size() == 4,
        "[Test] First polyhedron of HybridSolid should have 4 facets on "
        "border" );
}

void test_permutation(
    const geode::HybridSolid3D& solid, geode::HybridSolidBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 1, 7, 5, 4, 8, 6, 10, 2,
        9, 3, 0 };
    builder.permute_vertices( vertex_permutation );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 0 } ) == 10,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 1 } ) == 0,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 2 } ) == 9,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 3 } ) == 3,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 4 } ) == 2,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 5 } ) == 5,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 6 } ) == 4,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 7 } ) == 8,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 0 } ) == 0,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 1 } ) == 7,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 2 } ) == 9,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 3 } ) == 5,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 4 } ) == 1,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 5 } ) == 4,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 0 } ) == 2,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 1 } ) == 5,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 2 } ) == 4,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 3 } ) == 8,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 4 } ) == 6,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 0 } ) == 5,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 1 } ) == 1,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 2 } ) == 4,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 3 } ) == 6,
        "[Test] Wrong PolyhedronVertex after vertex permute" );

    std::vector< geode::index_t > polyhedron_permutation{ 3, 2, 0, 1 };
    builder.permute_polyhedra( polyhedron_permutation );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 0 } ) == 5,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 1 } ) == 1,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 2 } ) == 4,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 3 } ) == 6,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 0 } ) == 2,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 1 } ) == 5,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 2 } ) == 4,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 3 } ) == 8,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 4 } ) == 6,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 0 } ) == 10,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 1 } ) == 0,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 2 } ) == 9,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 3 } ) == 3,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 4 } ) == 2,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 5 } ) == 5,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 6 } ) == 4,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 7 } ) == 8,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 0 } ) == 0,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 1 } ) == 7,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 2 } ) == 9,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 3 } ) == 5,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 4 } ) == 1,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 3, 5 } ) == 4,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );

    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 1 } ) == 1,
        "[Test] Wrong Adjacency after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 2, 1 } ) == 1,
        "[Test] Wrong Adjacency after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 3 } ) == 3,
        "[Test] Wrong Adjacency after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 3, 4 } ) == 2,
        "[Test] Wrong Adjacency after polyhedron permute" );

    const auto polyhedra_5 = solid.polyhedra_around_vertex( 5 );
    OPENGEODE_EXCEPTION( polyhedra_5.size() == 4,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[0].polyhedron_id == 0,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[0].vertex_id == 0,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[1].polyhedron_id == 3,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[1].vertex_id == 3,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[2].polyhedron_id == 2,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[2].vertex_id == 5,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[3].polyhedron_id == 1,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[3].vertex_id == 1,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
}

void test_delete_polyhedra( const geode::HybridSolid3D& hybrid_solid,
    geode::HybridSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( hybrid_solid.nb_polyhedra(), false );
    to_delete.front() = true;
    builder.delete_polyhedra( to_delete );
    OPENGEODE_EXCEPTION( hybrid_solid.nb_polyhedra() == 3,
        "[Test] HybridSolid should have 3 polyhedra" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_vertex( { 0, 0 } ) == 2,
        "[Test] HybridSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_vertex( { 0, 1 } ) == 5,
        "[Test] HybridSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_vertex( { 0, 2 } ) == 4,
        "[Test] HybridSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_vertex( { 0, 3 } ) == 8,
        "[Test] HybridSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedron_vertex( { 0, 4 } ) == 6,
        "[Test] HybridSolid vertex index is not correct" );
    builder.edges_builder().delete_isolated_edges();
    builder.facets_builder().delete_isolated_facets();
    OPENGEODE_EXCEPTION( hybrid_solid.facets().nb_facets() == 14,
        "[Test] HybridSolid should have 14 facets" );
    OPENGEODE_EXCEPTION( hybrid_solid.edges().nb_edges() == 21,
        "[Test] HybridSolid should have 21 edges" );
}

void test_io(
    const geode::HybridSolid3D& hybrid_solid, const std::string& filename )
{
    geode::save_hybrid_solid( hybrid_solid, filename );
    geode::load_hybrid_solid< 3 >( filename );
    const auto new_hybrid_solid = geode::load_hybrid_solid< 3 >(
        geode::OpenGeodeHybridSolid3D::impl_name_static(), filename );

    OPENGEODE_EXCEPTION( new_hybrid_solid->nb_vertices() == 11,
        "[Test] Reloaded HybridSolid should have 11 vertices" );
    OPENGEODE_EXCEPTION( new_hybrid_solid->nb_polyhedra() == 4,
        "[Test] HybridSolid should have 4 polyhedra" );
    OPENGEODE_EXCEPTION( new_hybrid_solid->facets().nb_facets() == 16,
        "[Test] HybridSolid should have 16 facets" );
    OPENGEODE_EXCEPTION( new_hybrid_solid->edges().nb_edges() == 22,
        "[Test] HybridSolid should have 22 edges" );
    OPENGEODE_EXCEPTION(
        new_hybrid_solid->facets().facet_from_vertices(
            new_hybrid_solid->polyhedron_facet_vertices( { 1, 0 } ) )
            == hybrid_solid.facets().facet_from_vertices(
                hybrid_solid.polyhedron_facet_vertices( { 1, 0 } ) ),
        "[Test] Reloaded HybridSolid has wrong polyhedron facet index" );
}

void test_clone( const geode::HybridSolid3D& hybrid_solid )
{
    const auto hybrid_solid2 = hybrid_solid.clone();
    OPENGEODE_EXCEPTION( hybrid_solid2->nb_vertices() == 11,
        "[Test] Reloaded HybridSolid should have 11 vertices" );
    OPENGEODE_EXCEPTION( hybrid_solid2->nb_polyhedra() == 3,
        "[Test] HybridSolid should have 3 polyhedra" );
    OPENGEODE_EXCEPTION( hybrid_solid2->facets().nb_facets() == 14,
        "[Test] HybridSolid should have 14 facets" );
    OPENGEODE_EXCEPTION( hybrid_solid2->edges().nb_edges() == 21,
        "[Test] HybridSolid should have 21 edges" );
}

void test_delete_all( const geode::HybridSolid3D& hybrid_solid,
    geode::HybridSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( hybrid_solid.nb_polyhedra(), true );
    builder.delete_polyhedra( to_delete );

    OPENGEODE_EXCEPTION( hybrid_solid.nb_vertices() == 11,
        "[Test] HybridSolid should have 11 vertices" );
    OPENGEODE_EXCEPTION( hybrid_solid.is_vertex_isolated( 0 ),
        "[Test] Vertices should be isolated after polyhedra deletion" );
    OPENGEODE_EXCEPTION( hybrid_solid.nb_polyhedra() == 0,
        "[Test] HybridSolid should have 0 polyhedron" );
    builder.edges_builder().delete_isolated_edges();
    builder.facets_builder().delete_isolated_facets();
    OPENGEODE_EXCEPTION( hybrid_solid.facets().nb_facets() == 0,
        "[Test] HybridSolid should have 0 facet" );
    OPENGEODE_EXCEPTION( hybrid_solid.edges().nb_edges() == 0,
        "[Test] HybridSolid should have 0 edge" );
    OPENGEODE_EXCEPTION( hybrid_solid.polyhedra_around_vertex( 0 ).empty(),
        "[Test] No more polyhedra around vertices" );

    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION( hybrid_solid.nb_vertices() == 0,
        "[Test]HybridSolid should have 0 vertex" );
}

void test()
{
    auto hybrid_solid = geode::HybridSolid3D::create(
        geode::OpenGeodeHybridSolid3D::impl_name_static() );
    hybrid_solid->enable_edges();
    hybrid_solid->enable_facets();
    auto builder = geode::HybridSolidBuilder3D::create( *hybrid_solid );

    test_create_vertices( *hybrid_solid, *builder );
    test_create_polyhedra( *hybrid_solid, *builder );
    test_edges( *hybrid_solid );
    test_facets( *hybrid_solid );
    test_polyhedron_adjacencies( *hybrid_solid, *builder );
    test_io( *hybrid_solid,
        absl::StrCat( "test.", hybrid_solid->native_extension() ) );

    test_permutation( *hybrid_solid, *builder );
    test_delete_polyhedra( *hybrid_solid, *builder );
    test_clone( *hybrid_solid );
    test_delete_all( *hybrid_solid, *builder );
}

OPENGEODE_TEST( "hybrid-solid" )
