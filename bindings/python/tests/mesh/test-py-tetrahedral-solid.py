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

import os, sys, platform
if sys.version_info >= (3,8,0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh

def test_create_vertices( solid, builder ):
    builder.create_point( geom.Point3D( [ 0.1, 0.2, 0.3 ] ) )
    builder.create_point( geom.Point3D( [ 2.1, 9.4, 6.7 ] ) )
    builder.create_point( geom.Point3D( [ 7.5, 5.2, 6.3 ] ) )
    builder.create_point( geom.Point3D( [ 8.1, 1.4, 4.7 ] ) )
    builder.create_point( geom.Point3D( [ 4.7, 2.1, 1.3 ] ) )
    builder.create_point( geom.Point3D( [ 1.6, 8.7, 6.1 ] ) )
    if solid.nb_vertices() != 6:
        raise ValueError( "[Test] TetrahedralSolid should have 6 vertices" )

def test_create_tetrahedra( solid, builder ):
    builder.create_tetrahedron( [ 0, 1, 2, 3 ] )
    builder.create_tetrahedron( [ 1, 2, 3, 4 ] )
    builder.create_polyhedron( [ 1, 4, 3, 5 ], [ [ 0, 0, 0 ], [ 0, 0, 0 ], [ 0, 0, 0 ], [ 0, 0, 0 ] ] )
    if solid.nb_polyhedra() != 3:
        raise ValueError( "[Test] TetrahedralSolid should have 3 tetrahedra" )
    if solid.nb_facets() != 10:
        raise ValueError( "[Test] PolyhedralSolid should have 10 facets" )
    if solid.edges().nb_edges() != 12:
        raise ValueError( "[Test] PolyhedralSolid should have 12 edges" )

def test_polyhedron_adjacencies( solid, builder ):
    builder.compute_polyhedron_adjacencies()
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 0, 0 ) ) != 1:
        raise ValueError( "[Test] TetrahedralSolid adjacent index is not correct" )
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 0, 1 ) ):
        raise ValueError( "[Test] TetrahedralSolid adjacent index is not correct" )
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 1, 3 ) ) != 0:
        raise ValueError( "[Test] TetrahedralSolid adjacent index is not correct" )
    if solid.polyhedron_adjacent_facet( mesh.PolyhedronFacet( 0, 0 ) ) != mesh.PolyhedronFacet( 1, 3 ):
        raise ValueError( "[Test] TetrahedralSolid adjacent index is not correct" )

    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 2, 3 ) ) != 1:
        raise ValueError( "[Test] TetrahedralSolid adjacent index is not correct" )
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 2, 0 ) ):
        raise ValueError( "[Test] TetrahedralSolid adjacent index is not correct" )

def test_delete_vertex( solid, builder ):
    to_delete = [False] * solid.nb_vertices()
    to_delete[0] = True
    builder.delete_vertices( to_delete )
    if solid.nb_vertices() != 5:
        raise ValueError( "[Test] TetrahedralSolid should have 5 vertices" )
    answer = geom.Point3D( [ 2.1, 9.4, 6.7 ] )
    if solid.point( 0 ) != answer:
        raise ValueError( "[Test] TetrahedralSolid vertex coordinates are not correct" )
    if solid.nb_polyhedra() != 2:
        raise ValueError( "[Test] TetrahedralSolid should have 2 tetrahedra" )
    if solid.polyhedron_adjacent( mesh.PolyhedronFacet( 1, 3 ) ) != 0:
        raise ValueError( "[Test] TetrahedralSolid adjacent index is not correct" )
    if solid.nb_facets() != 7:
        raise ValueError( "[Test] PolyhedralSolid should have 7 facets" )
    if solid.edges().nb_edges() != 9:
        raise ValueError( "[Test] PolyhedralSolid should have 9 edges" )

def test_delete_polyhedron( solid, builder ):
    to_delete = [False] * solid.nb_polyhedra()
    to_delete[0] = True
    builder.delete_polyhedra( to_delete )
    if solid.nb_polyhedra() != 1:
        raise ValueError( "[Test] TetrahedralSolid should have 1 polyhedron" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 0 ) ) != 0:
        raise ValueError( "[Test] TetrahedralSolid facet vertex index is not correct" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 1 ) ) != 3:
        raise ValueError( "[Test] TetrahedralSolid facet vertex index is not correct" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 2 ) ) != 2:
        raise ValueError( "[Test] TetrahedralSolid facet vertex index is not correct" )
    if solid.polyhedron_vertex( mesh.PolyhedronVertex( 0, 3 ) ) != 4:
        raise ValueError( "[Test] TetrahedralSolid facet vertex index is not correct" )

def test_io( solid, filename ):
    mesh.save_tetrahedral_solid3D( solid, filename )
    new_solid = mesh.load_tetrahedral_solid3D( filename )
    if new_solid.nb_vertices() != 6:
        raise ValueError( "[Test] Reloaded TetrahedralSolid should have 6 vertices" )
    if new_solid.nb_facets() != 10:
        raise ValueError( "[Test] Reloaded TetrahedralSolid should have 10 facets" )
    if new_solid.edges().nb_edges() != 12:
        raise ValueError( "[Test] Reloaded TetrahedralSolid should have 12 edges" )
    if new_solid.nb_polyhedra() != 3:
        raise ValueError( "[Test] Reloaded TetrahedralSolid should have 3 polyhedra" )

def test_clone( solid ):
    solid2 = solid.clone()
    if solid2.nb_vertices() != 5:
        raise ValueError( "[Test] TetrahedralSolid2 should have 5 vertices" )
    if solid2.nb_facets() != 4:
        raise ValueError( "[Test] TetrahedralSolid2 should have 4 facets" )
    if solid2.nb_polyhedra() != 1:
        raise ValueError( "[Test] TetrahedralSolid2 should have 1 polyhedron" )

def test_delete_all( solid, builder ):
    builder.delete_isolated_vertices()
    if solid.nb_vertices() != 5:
        raise ValueError( "[Test]TetrahedralSolid should have 5 vertices" )
    if solid.nb_facets() != 4:
        raise ValueError( "[Test]TetrahedralSolid should have 4 facets" )
    if solid.nb_polyhedra() != 1:
        raise ValueError( "[Test]TetrahedralSolid should have 1 polyhedron" )

    to_delete = [True] * solid.nb_polyhedra()
    builder.delete_polyhedra( to_delete )
    if solid.nb_vertices() != 5:
        raise ValueError( "[Test]TetrahedralSolid should have 5 vertices" )
    if solid.nb_facets() != 0:
        raise ValueError( "[Test]TetrahedralSolid should have 0 facet" )
    if solid.nb_polyhedra() != 0:
        raise ValueError( "[Test]TetrahedralSolid should have 0 polyhedron" )
    if len( solid.polyhedra_around_vertex( 0 ) ) != 0:
        raise ValueError( "[Test] No more polyhedron around vertices" )

    builder.delete_isolated_vertices()
    if solid.nb_vertices() != 0:
        raise ValueError( "[Test]TetrahedralSolid should have 0 vertex" )

if __name__ == '__main__':
    solid = mesh.TetrahedralSolid3D.create()
    solid.enable_edges()
    builder = mesh.TetrahedralSolidBuilder3D.create( solid )
    
    test_create_vertices( solid, builder )
    test_create_tetrahedra( solid, builder )
    test_polyhedron_adjacencies( solid, builder )
    test_io( solid, "test." + solid.native_extension() )
    
    test_delete_vertex( solid, builder )
    test_delete_polyhedron( solid, builder )
    test_clone( solid )
