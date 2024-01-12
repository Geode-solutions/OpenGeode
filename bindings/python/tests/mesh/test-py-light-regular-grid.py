# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2023 Geode-solutions
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

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as geode
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh

def test_cell_number( grid ):
    if grid.nb_cells() != 750:
        raise ValueError( "[Test] Wrong total number of cells" )
    if grid.nb_cells_in_direction( 0 ) != 5:
        raise ValueError( "[Test] Wrong total number of cells along X" )
    if grid.nb_cells_in_direction( 1 ) != 10:
        raise ValueError( "[Test] Wrong total number of cells along Y" )
    if grid.nb_cells_in_direction( 2 ) != 15:
        raise ValueError( "[Test] Wrong total number of cells along Z" )

def test_cell_size( grid ):
    if grid.cell_length_in_direction( 0 ) != 1:
        raise ValueError( "[Test] Wrong length of cells along X" )
    if grid.cell_length_in_direction( 1 ) != 2:
        raise ValueError( "[Test] Wrong length of cells along Y" )
    if grid.cell_length_in_direction( 2 ) != 3:
        raise ValueError( "[Test] Wrong length of cells along Z" )
    if grid.cell_size() != 6:
        raise ValueError( "[Test] Wrong cell size" )
         

def test_cell_index( grid ):
    if grid.cell_index( [ 0, 0, 0 ] ) != 0:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( [ 1, 0, 0 ] ) != 1:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( [ 0, 1, 0 ] ) != 5:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( [ 0, 0, 1 ] ) != 50:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( [ 1, 1, 1 ] ) != 56:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( [ 2, 2, 2 ] ) != 112:
        raise ValueError( "[Test] Wrong cell index" )

    if grid.cell_index( grid.cell_indices( 0 ) ) != 0:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( grid.cell_indices( 1 ) ) != 1:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( grid.cell_indices( 5 ) ) != 5:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( grid.cell_indices( 50 ) ) != 50:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( grid.cell_indices( 56 ) ) != 56:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( grid.cell_indices( 112 ) ) != 112:
        raise ValueError( "[Test] Wrong cell index" )

    if grid.next_cell( [ 0, 0, 0 ], 0 ) != [ 1, 0, 0 ]:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.next_cell( [ 4, 0, 0 ], 0 ):
        raise ValueError( "[Test] Wrong cell index" )
    if grid.previous_cell( [ 0, 0, 0 ], 1 ):
        raise ValueError( "[Test] Wrong cell index" )
    if grid.previous_cell( [ 0, 0, 1 ], 2 ) != [ 0, 0, 0 ]:
        raise ValueError( "[Test] Wrong cell index" )

def test_cell_on_border( grid ):
    if not grid.is_cell_on_border( { { 0, 0, 0 } } ):
        raise ValueError( "[Test] Vertex is not on border where it should be." )
    if not grid.is_cell_on_border( { { 4, 8, 13 } } ):
        raise ValueError( "[Test] Vertex is not on border where it should be." )
    if grid.is_cell_on_border( { { 1, 2, 3 } } ):
        raise ValueError( "[Test] Vertex is on border where it should not be." )

def test_vertex_number( grid ):
    if grid.nb_grid_vertices() != 1056:
        raise ValueError( "[Test] Wrong total number of vertices" )
    if grid.nb_vertices_in_direction( 0 ) != 6:
        raise ValueError( "[Test] Wrong total number of vertices along X" )
    if grid.nb_vertices_in_direction( 1 ) != 11:
        raise ValueError( "[Test] Wrong total number of vertices along Y" )
    if grid.nb_vertices_in_direction( 2 ) != 16:
        raise ValueError( "[Test] Wrong total number of vertices along Z" )
    if grid.nb_cell_vertices() != 8:
        raise ValueError( "[Test] Wrong number of vertices in cells" )
    if grid.nb_vertices_on_borders() != 552:
        raise ValueError( "[Test] Wrong number of vertices on borders" )

def test_vertex_index( grid ):
    if grid.vertex_index( [ 0, 0, 0 ] ) != 0:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( [ 1, 0, 0 ] ) != 1:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( [ 0, 1, 0 ] ) != 6:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( [ 0, 0, 1 ] ) != 66:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( [ 1, 1, 1 ] ) != 73:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( [ 2, 2, 2 ] ) != 146:
        raise ValueError( "[Test] Wrong vertex index" )

    if grid.vertex_index( grid.vertex_indices( 0 ) ) != 0:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( grid.vertex_indices( 1 ) ) != 1:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( grid.vertex_indices( 5 ) ) != 5:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( grid.vertex_indices( 50 ) ) != 50:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( grid.vertex_indices( 56 ) ) != 56:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.vertex_index( grid.vertex_indices( 112 ) ) != 112:
        raise ValueError( "[Test] Wrong vertex index" )

    if grid.next_vertex( [ 0, 0, 0 ], 0 ) != [ 1, 0, 0 ]:
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.next_vertex( [ 5, 0, 0 ], 0 ):
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.previous_vertex( [ 0, 0, 0 ], 1 ):
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.previous_vertex( [ 0, 0, 1 ], 2 ) != [ 0, 0, 0 ]:
        raise ValueError( "[Test] Wrong vertex index" )

    if grid.cell_vertices( [ 1, 1, 1 ] )[0] != [ 1, 1, 1 ]:
        raise ValueError( "[Test] Wrong cell node vertex index" )
    if grid.cell_vertex_indices( [ 1, 1, 1 ], 1 ) != [ 2, 1, 1 ]:
        raise ValueError( "[Test] Wrong cell node vertex index" )
    if grid.cell_vertex_indices( [ 1, 1, 1 ], 2 ) != [ 1, 2, 1 ]:
        raise ValueError( "[Test] Wrong cell node vertex index" )

def test_vertex_on_border( grid ):
    if not grid.is_grid_vertex_on_border( [ 0, 0, 0 ] ):
        raise ValueError("[Test] Vertex is not on border where it should be." )
    if not grid.is_grid_vertex_on_border( [ 0, 9, 0 ] ):
        raise ValueError("[Test] Vertex is not on border where it should be." )
    if grid.is_grid_vertex_on_border( [1, 2, 3 ] ):
        raise ValueError("[Test] Vertex is on border where it should not be." )

def test_cell_geometry( grid ):
    if grid.grid_point( [ 0, 0, 0 ] ) != geom.Point3D( [ 1.5, 0, 1 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )
    if grid.grid_point( [ 0, 0, 1 ] ) != geom.Point3D( [ 1.5, 0, 4 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )
    if grid.grid_point( [ 1, 1, 1 ] ) != geom.Point3D( [ 2.5, 2, 4 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )
    if grid.grid_point( [ 2, 1, 4 ] ) != geom.Point3D( [ 3.5, 2, 13 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )

def test_cell_query( grid ):
    if grid.contains( geom.Point3D( [ 0, 0, 0 ] ) ):
        raise ValueError( "[Test] Wrong result on contain: point is shown inside of grid where it is not." )
    if grid.contains( geom.Point3D( [ 1.5, 0, 0 ] ) ):
        raise ValueError( "[Test] Wrong result on contain: point is shown inside of grid where it is not." )
    if len( grid.cells( geom.Point3D( [ 0, 0, 0 ] ) ) ) != 0:
        raise ValueError( "[Test] Wrong query result: point is shown inside of grid where it is not." )
    result = grid.cells( geom.Point3D( [ 2, 2, 2 ] ) )
    if len( result ) != 2 or result[0] != [ 0, 0, 0 ] or result[1] != [ 0, 1, 0 ]:
        raise ValueError( "[Test] Wrong query result" )
    result = grid.cells( geom.Point3D( [ 5, 7, 9 ] ) )
    if len( result ) != 1 or result[0] != [ 3, 3, 2 ]:
        raise ValueError( "[Test] Wrong query result" )
    result = grid.cells( geom.Point3D([ 4.5, 6, 7 - 1e-10 ] ) )
    if len( result ) != 8 or result[0] != [ 2, 2, 1 ] or result[1] != [ 3, 2, 1 ] or result[2] != [ 2, 3, 1 ] or result[3] != [ 3, 3, 1 ] or result[4] != [ 2, 2, 2 ] or result[5] != [ 3, 2, 2 ] or result[6] != [ 2, 3, 2 ] or result[7] != [ 3, 3, 2 ]:
        raise ValueError( "[Test] Wrong query result" )
    near_origin_point = geom.Point3D([ 1.5 - geode.global_epsilon / 2, -geode.global_epsilon / 2, 1 - geode.global_epsilon / 2 ])
    if not grid.contains( near_origin_point ):
        raise ValueError( "[Test] Wrong result on contain: point is shown outside of grid when it should be inside." )
    result = grid.cells( near_origin_point )
    if len( result ) != 1 or result[0] != [ 0, 0, 0 ]:
        raise ValueError( "[Test] Wrong query result for point near origin." )
    grid_furthest_point = geom.Point3D([ 6.5 + geode.global_epsilon / 2, 20 + geode.global_epsilon / 2, 46 + geode.global_epsilon / 2 ])
    if not grid.contains( grid_furthest_point ):
        raise ValueError( "[Test] Wrong result on contain: point is shown outside of grid when it should be inside." )
    result = grid.cells( grid_furthest_point )
    if len( result ) != 1 or result[0] != [ 4, 9, 14 ]:
        raise ValueError( "[Test] Wrong query result for point near origin furthest corner." )

def test_boundary_box( grid ):
    bbox = grid.grid_bounding_box()
    min = geom.Point3D([ 1.5, 0, 1 ])
    max = geom.Point3D([ 6.5, 20, 46 ])
    if bbox.min() != min:
        raise ValueError( "[Test] Wrong bounding box min" )
    if bbox.max() != max:
        raise ValueError( "[Test] Wrong bounding box max" )

def test_closest_vertex( grid ):
    p0 = geom.Point3D([ 1.5, 0, 1 ]) # (0,0,0)
    p1 = geom.Point3D([ 6.5, 20, 46 ]) # (5,10,15)
    p2 = geom.Point3D([ 0, -1, -1 ]) # (0,0,0)
    p3 = geom.Point3D([ 10, 30, 50 ]) # (5,10,15)
    p4 = geom.Point3D([ 3.55, 3.9, 7.5 ]) # (2,2,2)

    result = grid.closest_vertex( p0 )
    answer = [ 0, 0, 0 ]
    if result != answer:
        raise ValueError( "[Test] Wrong result for closest vertex for query p0" )

    result = grid.closest_vertex( p1 )
    answer = [ 5, 10, 15 ]
    if result != answer:
        raise ValueError( "[Test] Wrong result for closest vertex for query p1" )

    result = grid.closest_vertex( p2 )
    answer = [ 0, 0, 0 ]
    if result != answer:
        raise ValueError( "[Test] Wrong result for closest vertex for query p2" )

    result = grid.closest_vertex( p3 )
    answer = [ 5, 10, 15 ]
    if result != answer:
        raise ValueError( "[Test] Wrong result for closest vertex for query p3" )

    result = grid.closest_vertex( p4 )
    answer = [ 2, 2, 2 ]
    if result != answer:
        raise ValueError( "[Test] Wrong result for closest vertex for query p4" )

def test_attribute( grid ):
    attribute = grid.cell_attribute_manager().find_or_create_attribute_variable_double( "toto", -1 )
    attribute.set_value( 10, 10 )
    if attribute.value( 0 ) != -1:
        raise ValueError( "[Test] Wrong attribute value" )
    if attribute.value( 10 ) != 10:
        raise ValueError( "[Test] Wrong attribute value" )
    if attribute.value( grid.nb_cells() - 1 ) != -1:
        raise ValueError( "[Test] Wrong attribute value" )
    
def test_io(grid, filename):
    mesh.save_light_regular_grid3D(grid, filename)
    mesh.load_light_regular_grid3D(filename)

if __name__ == '__main__':
    mesh.OpenGeodeMeshLibrary.initialize()
    grid = mesh.LightRegularGrid3D(geom.Point3D([1.5, 0, 1]), [5, 10, 15], [1., 2., 3.])
    test_cell_number( grid )
    test_cell_index( grid )
    test_vertex_number( grid )
    test_vertex_index( grid )
    test_vertex_on_border( grid )
    test_cell_geometry( grid )
    test_cell_query( grid )
    test_boundary_box( grid )
    test_closest_vertex( grid )
    test_attribute( grid )
    test_io(grid, "test." + grid.native_extension())
