# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2022 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY:
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM:
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os, sys, platform
if sys.version_info >= (3,8,0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh

def test_create_vertices( polyhedral_solid, builder ):
    builder.create_point( geom.Point3D( [ 0.1, 0.2, 0.3 ] ) )
    builder.create_point( geom.Point3D( [ 2.1, 9.4, 6.7 ] ) )
    builder.create_point( geom.Point3D( [ 7.5, 5.2, 6.3 ] ) )
    builder.create_point( geom.Point3D( [ 8.1, 1.4, 4.7 ] ) )
    builder.create_point( geom.Point3D( [ 4.7, 2.1, 1.3 ] ) )
    builder.create_point( geom.Point3D( [ 9.3, 5.3, 6.7 ] ) )
    builder.create_point( geom.Point3D( [ 7.5, 4.2, 2.8 ] ) )
    builder.create_point( geom.Point3D( [ 2.2, 3.3, 4.4 ] ) )
    if polyhedral_solid.nb_vertices() != 8:
        raise ValueError( "[Test] PolyhedralSolid should have 8 vertices" )

def test_bounding_box( polyhedral_solid ):
    answer_min = geom.Point3D( [ 0.1, 0.2, 0.3 ] )
    answer_max = geom.Point3D( [ 9.3, 9.4, 6.7 ] )
    if polyhedral_solid.bounding_box().min() != answer_min:
        raise ValueError( "[Test] Wrong computation of bounding box (min)" )
    if polyhedral_solid.bounding_box().max() != answer_max:
        raise ValueError( "[Test] Wrong computation of bounding box (max)" )

def test_create_polyhedra( polyhedral_solid, builder ):
    builder.create_polyhedron( [ 0, 1, 2, 3, 4, 5 ], [ [ 0, 1, 2 ], [ 3, 5, 4 ], [ 0, 3, 4, 1 ], [ 0, 2, 5, 3 ], [ 1, 4, 5, 2 ] ] )
    builder.create_polyhedron( [ 3, 4, 5, 6 ], [ [ 1, 3, 2 ], [ 0, 2, 3 ], [ 3, 1, 0 ], [ 0, 1, 2 ] ] )
    builder.create_polyhedron( [ 4, 5, 6, 7 ], [ [ 1, 3, 2 ], [ 0, 2, 3 ], [ 3, 1, 0 ], [ 0, 1, 2 ] ] )
    if polyhedral_solid.nb_polyhedra() != 3:
        raise ValueError( "[Test] PolyhedralSolid should have 3 polyhedra" )
    if polyhedral_solid.facets().nb_facets() != 11:
        raise ValueError( "[Test] PolyhedralSolid should have 11 facets" )
    if polyhedral_solid.edges().nb_edges() != 15:
        raise ValueError( "[Test] PolyhedralSolid should have 15 edges" )
    answer = [3, 4, 5, 6]
    if polyhedral_solid.polyhedron_vertices( 1 ) != answer:
        raise ValueError("[Test] Wrong polyhedron vertices list" )

def test_create_facet_attribute( polyhedral_solid ):
    attribute = polyhedral_solid.facets().facet_attribute_manager().find_or_create_attribute_variable_uint( "test", basic.NO_ID )
    for f in range( polyhedral_solid.facets().nb_facets() ):
        attribute.set_value( f, f )

def test_create_edge_attribute( polyhedral_solid ):
    attribute = polyhedral_solid.edges().edge_attribute_manager().find_or_create_attribute_variable_uint( "test", basic.NO_ID )
    for e in range( polyhedral_solid.edges().nb_edges() ):
        vertices = polyhedral_solid.edges().edge_vertices( e )
        attribute.set_value( e, vertices[0] + vertices[1] )
    if attribute.value( 0 ) != 1:
        raise ValueError( "[Test] Wrong value for attribute on edge 0" )
    if attribute.value( 1 ) != 3:
        raise ValueError( "[Test] Wrong value for attribute on edge 1" )

def test_polyhedron_adjacencies( polyhedral_solid, builder ):
    builder.compute_polyhedron_adjacencies()
    if polyhedral_solid.polyhedron_adjacent( mesh.PolyhedronFacet( 0, 0 ) ):
        raise ValueError( "[Test] PolyhedralSolid adjacent index is not correct" )
    if polyhedral_solid.polyhedron_adjacent( mesh.PolyhedronFacet( 0, 1 ) ) != 1:
        raise ValueError( "[Test] PolyhedralSolid adjacent index is not correct" )
    if polyhedral_solid.polyhedron_adjacent( mesh.PolyhedronFacet( 0, 2 ) ):
        raise ValueError( "[Test] PolyhedralSolid adjacent index is not correct" )
    if polyhedral_solid.polyhedron_adjacent( mesh.PolyhedronFacet( 1, 0 ) ) != 2:
        raise ValueError( "[Test] PolyhedralSolid adjacent index is not correct" )
    if polyhedral_solid.polyhedron_adjacent( mesh.PolyhedronFacet( 1, 3 ) ) != 0:
        raise ValueError( "[Test] PolyhedralSolid adjacent index is not correct" )
    if polyhedral_solid.polyhedron_adjacent( mesh.PolyhedronFacet( 2, 3 ) ) != 1:
        raise ValueError( "[Test] PolyhedralSolid adjacent index is not correct" )
    if len( polyhedral_solid.polyhedra_around_vertex( 4 ) ) != 3:
        raise ValueError( "[Test] PolyhedralSolid should have 3 polyhedra around this vertex" )
    if len( polyhedral_solid.polyhedron_facets_on_border( 0 ) ) != 4:
        raise ValueError( "[Test] First polyhedron of PolyhedralSolid should have 4 facets on order" )

    edge_id = polyhedral_solid.edges().edge_from_vertices( [ 5, 4 ] )
    if edge_id != 8:
        raise ValueError( "[Test] Wrong edge index from vertices" )
    if len( polyhedral_solid.polyhedra_around_edge(  [ 5, 4 ]) ) != 3:
        raise ValueError( "[Test] PolyhedralSolid should have 3 polyhedra around this edge" )
    facet_id = polyhedral_solid.facets().facet_from_vertices(
        polyhedral_solid.polyhedron_facet_vertices( mesh.PolyhedronFacet( 1, 0 ) ) )
    polyhedra = polyhedral_solid.polyhedra_from_facet( polyhedral_solid.facets().facet_vertices( facet_id ) )
    if len( polyhedra ) != 2:
        raise ValueError( "[Test] Wrong number of polyhedra from facet" )
    if not 1 in polyhedra:
        raise ValueError( "[Test] Polyhedra from facet should contain 1" )
    if not 2 in polyhedra:
        raise ValueError( "[Test] Polyhedra from facet should contain 2" )


def test_delete_polyhedra( polyhedral_solid, builder ):
    to_delete = [False] * polyhedral_solid.nb_polyhedra()
    to_delete[0] = True
    builder.delete_polyhedra( to_delete )
    if polyhedral_solid.nb_polyhedra() != 2:
        raise ValueError( "[Test] PolyhedralSolid should have 2 polyhedra" )
    if polyhedral_solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 0 ) ) != 5:
        raise ValueError( "[Test] PolyhedralSolid vertex index is not correct" )
    if polyhedral_solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 1 ) ) != 3:
        raise ValueError( "[Test] PolyhedralSolid vertex index is not correct" )
    if polyhedral_solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 2 ) ) != 1:
        raise ValueError( "[Test] PolyhedralSolid vertex index is not correct" )
    if polyhedral_solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 3 ) ) != 7:
        raise ValueError( "[Test] PolyhedralSolid vertex index is not correct" )
    if polyhedral_solid.facets().nb_facets() != 11:
        raise ValueError( "[Test] PolyhedralSolid should have 11 facets" )
    if polyhedral_solid.edges().nb_edges() != 15:
        raise ValueError( "[Test] PolyhedralSolid should have 15 edges" )
    builder.edges_builder().delete_isolated_edges()
    builder.facets_builder().delete_isolated_facets()
    if polyhedral_solid.facets().nb_facets() != 8:
        raise ValueError( "[Test] PolyhedralSolid should have 8 facets" )
    if polyhedral_solid.edges().nb_edges() != 12:
        raise ValueError( "[Test] PolyhedralSolid should have 12 edges" )
    attribute = polyhedral_solid.edges().edge_attribute_manager().find_attribute_uint( "test" )
    if attribute.value( 0 ) != 1:
        raise ValueError( "[Test] Wrong value for attribute on edge 0 after polyhedron deletion" )
    if attribute.value( 1 ) != 3:
        raise ValueError( "[Test] Wrong value for attribute on edge 1 after polyhedron deletion" )

def test_io( polyhedral_solid, filename ):
    mesh.save_polyhedral_solid3D( polyhedral_solid, filename )
    new_polyhedral_solid = mesh.load_polyhedral_solid3D( filename )

    if new_polyhedral_solid.nb_vertices() != 8:
        raise ValueError( "[Test] Reloaded PolyhedralSolid should have 8 vertices" )
    if new_polyhedral_solid.facets().nb_facets() != 11:
        raise ValueError( "[Test] Reloaded PolyhedralSolid should have 11 facets" )
    if new_polyhedral_solid.edges().nb_edges() != 15:
        raise ValueError( "[Test] Reloaded PolyhedralSolid should have 15 edges" )
    if new_polyhedral_solid.nb_polyhedra() != 3:
        raise ValueError( "[Test] Reloaded PolyhedralSolid should have 3 polyhedra" )
    attribute = new_polyhedral_solid.facets().facet_attribute_manager().find_attribute_uint( "test" )
    for f in range( new_polyhedral_solid.facets().nb_facets() ):
        if attribute.value( f ) != f:
            raise ValueError( "[Test] Reloaded PolyhedralSolid has wrong attributes on its facets" )

def test_barycenters():
    polyhedral_solid = mesh.PolyhedralSolid3D.create()
    builder = mesh.PolyhedralSolidBuilder3D.create( polyhedral_solid )
    o = 0.0
    a = 0.6
    b = 2.4
    c = 1.8
    builder.create_point( geom.Point3D( [ o, o, o ] ) )
    builder.create_point( geom.Point3D( [ a, o, o ] ) )
    builder.create_point( geom.Point3D( [ o, o, c ] ) )
    builder.create_point( geom.Point3D( [ o, b, o ] ) )
    builder.create_point( geom.Point3D( [ a, b, o ] ) )
    builder.create_point( geom.Point3D( [ o, b, c ] ) )
    builder.create_polyhedron( [ 0, 1, 2, 3, 4, 5 ], [ [ 0, 1, 2 ], [ 0, 1, 4, 3 ], [ 1, 2, 5, 4 ], [ 0, 3, 5, 2 ], [ 3, 4, 5 ] ] )
    answer_facet_barycenter = geom.Point3D( [ a / 3., 0, c / 3. ] )
    if polyhedral_solid.facet_barycenter( polyhedral_solid.polyhedron_facet_vertices( mesh.PolyhedronFacet(0,0) ) ) != answer_facet_barycenter:
        raise ValueError( "[Test] PolyhedralSolid facet_barycenter is not correct" )
    answer_polyhedron_barycenter = geom.Point3D( [ a / 3., 0.5 * b, c / 3. ] )
    if polyhedral_solid.polyhedron_barycenter( 0 ) != answer_polyhedron_barycenter:
        raise ValueError( "[Test] PolyhedralSolid polyhedron barycenter is not correct" )

def test_normals():
    polyhedral_solid = mesh.PolyhedralSolid3D.create()
    builder = mesh.PolyhedralSolidBuilder3D.create( polyhedral_solid )
    o = 0.0
    a = 0.6
    b = 2.4
    c = 1.8
    builder.create_point( geom.Point3D( [ o, b, -c ] ) )
    builder.create_point( geom.Point3D( [ o, o, o ] ) )
    builder.create_point( geom.Point3D( [ a, o, o ] ) )
    builder.create_point( geom.Point3D( [ a, b, o ] ) )
    builder.create_point( geom.Point3D( [ o, b, o ] ) )
    builder.create_point( geom.Point3D( [ o, b, c ] ) )
    builder.create_polyhedron( [ 0, 1, 2, 3, 4 ], [ [ 1, 2, 0 ], [ 1, 2, 3, 4 ], [ 2, 3, 0  ], [ 3, 4, 0 ], [4, 1, 0 ] ] )
    builder.create_polyhedron( [ 5, 1, 4, 3, 2 ], [ [ 1, 2, 0 ], [ 1, 2, 3, 4 ], [ 2, 3, 0  ], [ 3, 4, 0 ], [4, 1, 0 ] ] )

    answer_facet_normal = geom.Point3D( [ 0, 0, 1 ] )
    if polyhedral_solid.polyhedron_facet_normal( mesh.PolyhedronFacet( 0, 1 ) ) != answer_facet_normal:
        raise ValueError( "[Test] PolyhedralSolid polyhedron_facet_normal is not correct (0, 1)" )

    if polyhedral_solid.polyhedron_facet_normal( mesh.PolyhedronFacet( 1, 1 ) ) != answer_facet_normal * -1.:
        raise ValueError( "[Test] PolyhedralSolid polyhedron_facet_normal is not correct )1, 1)" )

def test_clone( polyhedral_solid ):
    polyhedral_solid2 = polyhedral_solid.clone()
    if polyhedral_solid2.nb_vertices() != 8:
        raise ValueError( "[Test] PolyhedralSolid2 should have 8 vertices" )
    if polyhedral_solid2.facets().nb_facets() != 8:
        raise ValueError( "[Test]PolyhedralSolid2 should have 8 facets" )
    if polyhedral_solid2.nb_polyhedra() != 2:
        raise ValueError( "[Test] PolyhedralSolid2 should have 2 polyhedra" )

def test_set_polyhedron_vertex( polyhedral_solid, builder ):
    builder.set_polyhedron_vertex( mesh.PolyhedronVertex( 0, 2 ), 2 )
    builder.facets_builder().delete_isolated_facets()

    if polyhedral_solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 2 ) ) != 2:
        raise ValueError( "[Test] PolyhedronVertex after set_polyhedron_vertex is wrong" )
    if polyhedral_solid.polyhedron_facet_vertex( mesh.PolyhedronFacetVertex( mesh.PolyhedronFacet( 0, 1 ), 1 ) ) != 4:
        raise ValueError( "[Test] PolyhedronFacetVertex after set_polyhedron_vertex is wrong" )

def test_delete_all( polyhedral_solid, builder ):
    to_delete = [True] * polyhedral_solid.nb_polyhedra()
    builder.delete_polyhedra( to_delete )

    if polyhedral_solid.nb_vertices() != 8:
        raise ValueError( "[Test] PolyhedralSolid should have 8 vertices" )
    if polyhedral_solid.nb_polyhedra() != 0:
        raise ValueError( "[Test] PolyhedralSolid should have 0 polyhedron" )
    builder.edges_builder().delete_isolated_edges()
    builder.facets_builder().delete_isolated_facets()
    if polyhedral_solid.facets().nb_facets() != 0:
        raise ValueError( "[Test] PolyhedralSolid should have 0 facet" )
    if polyhedral_solid.edges().nb_edges() != 0:
        raise ValueError( "[Test] PolyhedralSolid should have 0 edge" )
    if len( polyhedral_solid.polyhedra_around_vertex( 0 ) ) != 0:
        raise ValueError( "[Test] No more polyhedra around vertices" )

    builder.delete_isolated_vertices()
    if polyhedral_solid.nb_vertices() != 0:
        raise ValueError( "[Test]PolyhedralSolid should have 0 vertex" )

def test_permutation( solid, builder ):
    builder.permute_vertices( [4, 2, 6, 1, 5, 0, 7, 3] )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 0 ) ) != 5:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 1 ) ) != 3:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 3 ) ) != 7:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 4 ) ) != 0:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 5 ) ) != 4:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 0 ) ) != 7:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 1 ) ) != 0:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 2 ) ) != 4:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 3 ) ) != 2:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 0 ) ) != 0:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 1 ) ) != 4:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 2 ) ) != 2:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 3 ) ) != 6:
        raise ValueError( "[Test] Wrong PolyhedronVertex after vertex permute" )

    builder.permute_polyhedra( [2, 0, 1] )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 0 ) ) != 0:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 1 ) ) != 4:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 2 ) ) != 2:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 3 ) ) != 6:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 0 ) ) != 5:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 1 ) ) != 3:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 3 ) ) != 7:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 4 ) ) != 0:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 1, 5 ) ) != 4:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 0 ) ) != 7:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 1 ) ) != 0:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 2 ) ) != 4:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 2, 3 ) ) != 2:
        raise ValueError( "[Test] Wrong PolyhedronVertex after polyhedron permute" )

    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 0, 2 ) ):
        raise ValueError( "[Test] Wrong Adjacency after polyhedron permute" )
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 2, 3 ) ) != 1:
        raise ValueError( "[Test] Wrong Adjacency after polyhedron permute" )
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 0, 3 ) ) != 2:
        raise ValueError( "[Test] Wrong Adjacency after polyhedron permute" )
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 1, 1 ) ) != 2:
        raise ValueError( "[Test] Wrong Adjacency after polyhedron permute" )

    polyhedra_0 = solid.polyhedra_around_vertex( 0 )
    if len(polyhedra_0) != 3:
        raise ValueError( "[Test] Wrong polyhedra_0 after polyhedron permute" )
    if polyhedra_0[0].polyhedron_id != 0:
        raise ValueError( "[Test] Wrong polyhedra_0 after polyhedron permute" )
    if polyhedra_0[0].vertex_id != 0:
        raise ValueError( "[Test] Wrong polyhedra_0 after polyhedron permute" )
    if polyhedra_0[1].polyhedron_id != 2:
        raise ValueError( "[Test] Wrong polyhedra_0 after polyhedron permute" )
    if polyhedra_0[1].vertex_id != 1:
        raise ValueError( "[Test] Wrong polyhedra_0 after polyhedron permute" )
    if polyhedra_0[2].polyhedron_id != 1:
        raise ValueError( "[Test] Wrong polyhedra_0 after polyhedron permute" )
    if polyhedra_0[2].vertex_id != 4:
        raise ValueError( "[Test] Wrong polyhedra_0 after polyhedron permute" )

if __name__ == '__main__':
    polyhedral_solid = mesh.PolyhedralSolid3D.create()
    polyhedral_solid.enable_edges()
    polyhedral_solid.enable_facets()
    builder = mesh.PolyhedralSolidBuilder3D.create( polyhedral_solid )
    
    test_create_vertices( polyhedral_solid, builder )
    test_create_polyhedra( polyhedral_solid, builder )
    test_create_facet_attribute( polyhedral_solid )
    test_create_edge_attribute( polyhedral_solid )
    test_polyhedron_adjacencies( polyhedral_solid, builder )
    test_io( polyhedral_solid, "test." + polyhedral_solid.native_extension() )
    
    test_permutation( polyhedral_solid, builder )
    test_delete_polyhedra( polyhedral_solid, builder )
    test_clone( polyhedral_solid )
    test_set_polyhedron_vertex( polyhedral_solid, builder )
    test_delete_all( polyhedral_solid, builder )
    
    test_barycenters()
    test_normals()
