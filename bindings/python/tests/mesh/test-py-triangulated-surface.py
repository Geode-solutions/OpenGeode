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

def test_create_vertices( surface, builder ):
    builder.create_point( geom.Point3D( [ 0.1, 0.2, 0.3 ] ) )
    builder.create_point( geom.Point3D( [ 2.1, 9.4, 6.7 ] ) )
    builder.create_point( geom.Point3D( [ 7.5, 5.2, 6.3 ] ) )
    builder.create_point( geom.Point3D( [ 8.1, 1.4, 4.7 ] ) )
    builder.create_point( geom.Point3D( [ 4.7, 2.1, 1.3 ] ) )
    if surface.nb_vertices() != 5:
        raise ValueError( "[Test] TriangulatedSurface should have 5 vertices" )

def test_create_polygons(  surface, builder ):
    builder.create_triangle( [ 0, 1, 2 ] )
    builder.create_triangle( [ 1, 3, 2 ] )
    builder.create_polygon( [ 3, 4, 2 ] )
    if surface.nb_polygons() != 3:
        raise ValueError( "[Test] TriangulatedSurface should have 3 triangles" )
    if surface.edges().nb_edges() != 7:
        raise ValueError( "[Test] TriangulatedSurface should have 7 edges" )

def test_polygon_adjacencies( surface, builder ):
    builder.compute_polygon_adjacencies()
    if surface.polygon_adjacent( mesh.PolygonEdge( 0, 0 ) ):
        raise ValueError( "[Test] TriangulatedSurface adjacent index is not correct" )
    if surface.polygon_adjacent( mesh.PolygonEdge( 0, 1 ) ) != 1:
        raise ValueError( "[Test] TriangulatedSurface adjacent index is not correct" )
    if surface.polygon_adjacent( mesh.PolygonEdge( 1, 2 ) ) != 0:
        raise ValueError( "[Test] TriangulatedSurface adjacent index is not correct" )
    if surface.polygon_adjacent_edge( mesh.PolygonEdge( 0, 1 ) ) != mesh.PolygonEdge( 1, 2 ):
        raise ValueError( "[Test] TriangulatedSurface adjacent index is not correct" )

    if surface.polygon_adjacent( mesh.PolygonEdge( 2, 2 ) ) != 1:
        raise ValueError( "[Test] TriangulatedSurface adjacent index is not correct" )
    if surface.polygon_adjacent( mesh.PolygonEdge( 2, 0 ) ):
        raise ValueError( "[Test] TriangulatedSurface adjacent index is not correct" )

def test_delete_polygon( surface, builder ):
    to_delete = [False] * surface.nb_polygons()
    to_delete[0] = True
    builder.delete_polygons( to_delete )
    if surface.nb_polygons() != 2:
        raise ValueError( "[Test] TriangulatedSurface should have 2 polygons" )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 0 ) ) != 3:
        raise ValueError( "[Test] TriangulatedSurface edge vertex index is not correct" )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 1 ) ) != 2:
        raise ValueError( "[Test] TriangulatedSurface edge vertex index is not correct" )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 2 ) ) != 1:
        raise ValueError( "[Test] TriangulatedSurface edge vertex index is not correct" )
    builder.edges_builder().delete_isolated_edges()
    if surface.edges().nb_edges() != 5:
        raise ValueError( "[Test] TriangulatedSurface should have 5 edges" )

def test_io( surface, filename ):
    mesh.save_triangulated_surface3D( surface, filename )
    new_surface = mesh.load_triangulated_surface3D( filename )

def test_clone( surface ):
    surface2 = surface.clone()
    if surface2.nb_vertices() != 5:
        raise ValueError( "[Test] TriangulatedSurface2 should have 5 vertices" )
    if surface2.edges().nb_edges() != 5:
        raise ValueError( "[Test] TriangulatedSurface2 should have 5 edges" )
    if surface2.nb_polygons() != 2:
        raise ValueError( "[Test] TriangulatedSurface2 should have 2 polygons" )

def test_delete_all( triangulated_surface, builder ):
    builder.delete_isolated_vertices()
    if triangulated_surface.nb_vertices() != 3:
        raise ValueError( "[Test]TriangulatedSurface should have 3 vertices" )
    if triangulated_surface.edges().nb_edges() != 3:
        raise ValueError( "[Test]TriangulatedSurface should have 3 edges" )
    if triangulated_surface.nb_polygons() != 1:
        raise ValueError( "[Test]TriangulatedSurface should have 1 polygon" )

    to_delete = [True] * surface.nb_polygons()
    builder.delete_polygons( to_delete )
    if triangulated_surface.nb_vertices() != 3:
        raise ValueError( "[Test]TriangulatedSurface should have 3 vertices" )
    builder.edges_builder().delete_isolated_edges()
    if triangulated_surface.edges().nb_edges() != 0:
        raise ValueError( "[Test]TriangulatedSurface should have 0 edge" )
    if triangulated_surface.nb_polygons() != 0:
        raise ValueError( "[Test]TriangulatedSurface should have 0 polygon" )
    if len( triangulated_surface.polygons_around_vertex( 0 ) ) != 0:
        raise ValueError( "[Test] No more polygon around vertices" )

    builder.delete_isolated_vertices()
    if triangulated_surface.nb_vertices() != 0:
        raise ValueError( "[Test]TriangulatedSurface should have 0 vertex" )

def test_permutation( surface, builder ):
    builder.permute_vertices( [4, 2, 1, 0, 3] )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 0 ) ) != 3:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 1 ) ) != 2:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 1, 0 ) ) != 2:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 1, 1 ) ) != 4:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 1, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 2, 0 ) ) != 4:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 2, 1 ) ) != 0:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 2, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolygonVertex after vertex permute" )

    builder.permute_polygons( [2, 0, 1] )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 0 ) ) != 4:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 1 ) ) != 0:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 0, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 1, 0 ) ) != 3:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 1, 1 ) ) != 2:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 1, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 2, 0 ) ) != 2:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 2, 1 ) ) != 4:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )
    if surface.polygon_vertex( mesh.PolygonVertex( 2, 2 ) ) != 1:
        raise ValueError( "[Test] Wrong PolygonVertex after polygon permute" )

    if surface.polygon_adjacent( mesh.PolygonEdge( 0, 2 ) ) != 2:
        raise ValueError( "[Test] Wrong Adjacency after polygon permute" )
    if surface.polygon_adjacent( mesh.PolygonEdge( 2, 1 ) ) != 0:
        raise ValueError( "[Test] Wrong Adjacency after polygon permute" )
    if surface.polygon_adjacent( mesh.PolygonEdge( 1, 1 ) ) != 2:
        raise ValueError( "[Test] Wrong Adjacency after polygon permute" )
    if surface.polygon_adjacent( mesh.PolygonEdge( 1, 0 ) ):
        raise ValueError( "[Test] Wrong Adjacency after polygon permute" )

    polygons_2 = surface.polygons_around_vertex( 2 )
    if len(polygons_2) != 2:
        raise ValueError( "[Test] Wrong polygons_2 after polygon permute" )
    if polygons_2[0].polygon_id != 2:
        raise ValueError( "[Test] Wrong polygons_2 after polygon permute" )
    if polygons_2[0].vertex_id != 0:
        raise ValueError( "[Test] Wrong polygons_2 after polygon permute" )
    if polygons_2[1].polygon_id != 1:
        raise ValueError( "[Test] Wrong polygons_2 after polygon permute" )
    if polygons_2[1].vertex_id != 1:
        raise ValueError( "[Test] Wrong polygons_2 after polygon permute" )

    polygons_4 = surface.polygons_around_vertex( 4 )
    if len(polygons_4) != 2:
        raise ValueError( "[Test] Wrong polygons_4 after polygon permute" )
    if polygons_4[0].polygon_id != 0:
        raise ValueError( "[Test] Wrong polygons_4 after polygon permute" )
    if polygons_4[0].vertex_id != 0:
        raise ValueError( "[Test] Wrong polygons_4 after polygon permute" )
    if polygons_4[1].polygon_id != 2:
        raise ValueError( "[Test] Wrong polygons_4 after polygon permute" )
    if polygons_4[1].vertex_id != 1:
        raise ValueError( "[Test] Wrong polygons_4 after polygon permute" )

if __name__ == '__main__':
    surface = mesh.TriangulatedSurface3D.create()
    surface.enable_edges()
    builder = mesh.TriangulatedSurfaceBuilder3D.create( surface )
    
    test_create_vertices( surface, builder )
    test_create_polygons( surface, builder )
    test_polygon_adjacencies( surface, builder )
    test_io( surface, "test." + surface.native_extension() )
    
    test_permutation( surface, builder )
    test_delete_polygon( surface, builder )
    test_clone( surface )
