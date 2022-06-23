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

def create_surface():
    surface = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create( surface )
    builder.create_point( geom.Point3D( [ 0.1, 0.2, 0.3 ] ) )
    builder.create_point( geom.Point3D( [ 2.1, 9.4, 6.7 ] ) )
    builder.create_point( geom.Point3D( [ 7.5, 5.2, 6.3 ] ) )
    builder.create_point( geom.Point3D( [ 8.1, 1.4, 4.7 ] ) )
    builder.create_point( geom.Point3D( [ 4.7, 2.1, 1.3 ] ) )
    builder.create_triangle( [ 0, 1, 2 ] )
    builder.create_triangle( [ 1, 3, 2 ] )
    builder.create_triangle( [ 3, 4, 2 ] )
    return surface

def test_rescale( surface ):
    builder = mesh.TriangulatedSurfaceBuilder3D.create( surface )
    mesh.rescale_triangulated_surface3D( surface, builder, [ 2, -2, 0.1 ] )
    if surface.point( 0 ) != [ 0.2, -0.4, 0.03 ]:
        raise ValueError( "[Test] Wrong rescale of vertex 0" )
    if surface.point( 1 ) != [ 4.2, -18.8, 0.67 ]:
        raise ValueError( "[Test] Wrong rescale of vertex 1" )
    if surface.point( 2 ) != [ 15.0, -10.4, 0.63 ]:
        raise ValueError( "[Test] Wrong rescale of vertex 2" )
    if surface.point( 3 ) != [ 16.2, -2.8, 0.47 ]:
        raise ValueError( "[Test] Wrong rescale of vertex 3" )
    if surface.point( 4 ) != [ 9.4, -4.2, 0.13 ]:
        raise ValueError( "[Test] Wrong rescale of vertex 4" )

def test_translate( surface ):
    builder = mesh.TriangulatedSurfaceBuilder3D.create( surface )
    mesh.translate_triangulated_surface3D( surface, builder, [ 2, -2, 1 ] )
    if surface.point( 0 ) != [ 2.1, -1.8, 1.3 ]:
        raise ValueError( "[Test] Wrong translation of vertex 0" )
    if surface.point( 1 ) != [ 4.1, 7.4, 7.7 ]:
        raise ValueError( "[Test] Wrong translation of vertex 1" )
    if surface.point( 2 ) != [ 9.5, 3.2, 7.3 ]:
        raise ValueError( "[Test] Wrong translation of vertex 2" )
    if surface.point( 3 ) != [ 10.1, -0.6, 5.7 ]:
        raise ValueError( "[Test] Wrong translation of vertex 3" )
    if surface.point( 4 ) != [ 6.7, 0.1, 2.3 ]:
        raise ValueError( "[Test] Wrong translation of vertex 4" )

if __name__ == '__main__':
    surface = create_surface()
    test_rescale( surface.clone() )
    test_translate( surface.clone() )
