# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2020 Geode-solutions
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
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import OpenGeode_py_basic
import OpenGeode_py_geometry as geom
import OpenGeode_py_mesh as mesh

def test_create_vertices( edged_curve, builder ):
    builder.create_point( geom.Point3D( [ 0.1, 0.2, 0.3 ] ) )
    builder.create_point( geom.Point3D( [ 2.1, 9.4, 6.7 ] ) )
    if edged_curve.nb_vertices() != 2:
        raise ValueError( "[Test] EdgedCurve should have 2 vertices" )
    builder.create_vertices( 2 )
    builder.set_point( 2, geom.Point3D( [ 7.5, 5.2, 6.3 ] ) )
    builder.set_point( 3, geom.Point3D( [ 8.7, 1.4, 4.7 ] ) )
    if edged_curve.nb_vertices() != 4:
        raise ValueError( "[Test] EdgedCurve should have 4 vertices" )

def test_bounding_box( edged_curve ):
    answer_min = geom.Point3D( [ 0.1, 0.2, 0.3 ] )
    answer_max = geom.Point3D( [ 8.7, 9.4, 6.7 ] )
    if edged_curve.bounding_box().min() != answer_min:
        raise ValueError( "[Test] Wrong computation of bounding box (min)" )
    if edged_curve.bounding_box().max() != answer_max:
        raise ValueError( "[Test] Wrong computation of bounding box (max)" )

def test_delete_vertex( edged_curve, builder ):
    to_delete = [False] * edged_curve.nb_vertices()
    to_delete[0]= True
    builder.delete_vertices( to_delete )
    if edged_curve.nb_vertices() != 3:
        raise ValueError( "[Test] EdgedCurve should have 3 vertices" )
    answer = geom.Point3D( [ 2.1, 9.4, 6.7 ] )
    if edged_curve.point( 0 ) != answer:
        raise ValueError( "[Test] EdgedCurve vertex coordinates are not correct" )
    if edged_curve.nb_edges() != 2:
        raise ValueError( "[Test] EdgedCurve should have 2 edges" )

    edges_around_0 = edged_curve.edges_around_vertex( 0 )
    if len( edges_around_0 ) != 1:
        raise ValueError( "[Test] edges_around_0 should have 1 edge" )
    if edges_around_0[0].edge_id != 1:
        raise ValueError( "[Test] edges_around_0 has wrong value" )
    if edges_around_0[0].vertex_id != 0:
        raise ValueError( "[Test] edges_around_0 has wrong value" )

    edges_around_2 = edged_curve.edges_around_vertex( 2 )
    if len( edges_around_2 ) != 1:
        raise ValueError( "[Test] edges_around_2 should have 1 edge" )
    if edges_around_2[0].edge_id != 0:
        raise ValueError( "[Test] edges_around_2 has wrong value" )
    if edges_around_2[0].vertex_id != 0:
        raise ValueError( "[Test] edges_around_2 has wrong value" )

def test_create_edges( edged_curve, builder ):
    builder.create_edge_with_vertices( 0, 1 )
    builder.create_edge_with_vertices( 0, 2 )
    if edged_curve.nb_edges() != 2:
        raise ValueError( "[Test] EdgedCurve should have 2 edges" )
    if edged_curve.edge_vertex( mesh.EdgeVertex( 0, 1 ) ) != 1:
        raise ValueError( "[Test] EdgedCurve edge vertex is not correct" )
    builder.create_edges( 2 )
    builder.set_edge_vertex( mesh.EdgeVertex( 2, 0 ), 3 )
    builder.set_edge_vertex( mesh.EdgeVertex( 2, 1 ), 2 )
    builder.set_edge_vertex( mesh.EdgeVertex( 3, 0 ), 1 )
    builder.set_edge_vertex( mesh.EdgeVertex( 3, 1 ), 2 )
    if edged_curve.nb_edges() != 4:
        raise ValueError( "[Test] EdgedCurve should have 4 edges" )

    edges_around_0 = edged_curve.edges_around_vertex( 0 )
    if len( edges_around_0 ) != 2:
        raise ValueError( "[Test] edges_around_0 should have 2 edges" )
    if edges_around_0[0].edge_id != 0:
        raise ValueError( "[Test] edges_around_0 has wrong value" )
    if edges_around_0[0].vertex_id != 0:
        raise ValueError( "[Test] edges_around_0 has wrong value" )
    if edges_around_0[1].edge_id != 1:
        raise ValueError( "[Test] edges_around_0 has wrong value" )
    if edges_around_0[1].vertex_id != 0:
        raise ValueError( "[Test] edges_around_0 has wrong value" )

    edges_around_2 = edged_curve.edges_around_vertex( 2 )
    if len( edges_around_2 ) != 3:
        raise ValueError( "[Test] edges_around_2 should have 3 edges" )
    if edges_around_2[0].edge_id != 1:
        raise ValueError( "[Test] edges_around_2 has wrong value" )
    if edges_around_2[0].vertex_id != 1:
        raise ValueError( "[Test] edges_around_2 has wrong value" )
    if edges_around_2[1].edge_id != 2:
        raise ValueError( "[Test] edges_around_2 has wrong value" )
    if edges_around_2[1].vertex_id != 1:
        raise ValueError( "[Test] edges_around_2 has wrong value" )
    if edges_around_2[2].edge_id != 3:
        raise ValueError( "[Test] edges_around_2 has wrong value" )
    if edges_around_2[2].vertex_id != 1:
        raise ValueError( "[Test] edges_around_2 has wrong value" )

def test_delete_edge( edged_curve, builder ):
    to_delete = [False] * edged_curve.nb_edges()
    to_delete[0]= True
    builder.delete_edges( to_delete )
    if edged_curve.nb_edges() != 1:
        raise ValueError( "[Test] EdgedCurve should have 1 edges" )
    if edged_curve.edge_vertex( mesh.EdgeVertex( 0, 0 ) ) != 0:
        raise ValueError( "[Test] EdgedCurve edge vertex index is not correct" )
    if edged_curve.edge_vertex( mesh.EdgeVertex( 0, 1 ) ) != 1:
        raise ValueError( "[Test] EdgedCurve edge vertex index is not correct" )

def test_io( edged_curve, filename ):
    mesh.save_edged_curve3D( edged_curve, filename )
    new_edged_curve = mesh.EdgedCurve3D.create()
    mesh.load_edged_curve3D( new_edged_curve, filename )

def test_edge_requests( edged_curve, builder ):
    if not edged_curve.edge_barycenter( 0 ).inexact_equal( geom.Point3D( [ 4.8, 7.3, 6.5 ] ), 1e-15 ):
        raise ValueError( "[Test] Edge barycenter is not correct" )
    p0 = builder.create_point( geom.Point3D( [ 1, 1, 1 ] ) )
    p1 = builder.create_point( geom.Point3D( [ 1, 4, -3 ] ) )
    if edged_curve.edge_length( builder.create_edge_with_vertices( p0, p1 ) ) != 5:
        raise ValueError( "[Test] Edge length is not correct" )

def test_clone( edged_curve ):
    attribute = edged_curve.edge_attribute_manager().find_or_create_attribute_variable_int( "test", 0 )
    attribute.set_value( 0, 42 )

    edged_curve2 = edged_curve.clone()
    if edged_curve2.nb_vertices() != 3:
        raise ValueError( "[Test] EdgedCurve2 should have 3 vertices" )
    if edged_curve2.nb_edges() != 1:
        raise ValueError( "[Test] EdgedCurve2 should have 1 edge" )

    attribute2 = edged_curve2.edge_attribute_manager().find_attribute_int( "test" )
    if attribute2.value( 0 ) != 42:
        raise ValueError( "[Test] EdgedCurve2 attribute should be 42" )

edged_curve = mesh.EdgedCurve3D.create()
builder = mesh.EdgedCurveBuilder3D.create( edged_curve )

test_create_vertices( edged_curve, builder )
test_create_edges( edged_curve, builder )
test_bounding_box( edged_curve )
test_io( edged_curve, "test." + edged_curve.native_extension() )

test_delete_vertex( edged_curve, builder )
test_delete_edge( edged_curve, builder )
test_clone( edged_curve )

test_edge_requests( edged_curve, builder )
