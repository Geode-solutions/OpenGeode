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
    if grid.cell_length( 0 ) != 1:
        raise ValueError( "[Test] Wrong length of cells along X" )
    if grid.cell_length( 1 ) != 2:
        raise ValueError( "[Test] Wrong length of cells along Y" )
    if grid.cell_length( 2 ) != 3:
        raise ValueError( "[Test] Wrong length of cells along Z" )
    if grid.cell_size() != 6:
        raise ValueError( "[Test] Wrong cell size" )

def test_cell_index( grid ):
    if grid.cell_index(  [ 0, 0, 0 ] ) != 0:
        raise ValueError( "[Test] Wrong cell index" )
    if grid.cell_index( [ 1, 0, 0 ] )  != 1:
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

def test_vertex_number( grid ):
    if grid.nb_vertices() != 1056:
        raise ValueError( "[Test] Wrong total number of vertices" )
    if grid.nb_vertices_in_direction( 0 ) != 6:
        raise ValueError( "[Test] Wrong total number of vertices along X" )
    if grid.nb_vertices_in_direction( 1 ) != 11:
        raise ValueError( "[Test] Wrong total number of vertices along Y" )
    if grid.nb_vertices_in_direction( 2 ) != 16:
        raise ValueError( "[Test] Wrong total number of vertices along Z" )
    if grid.nb_cell_vertices() != 8:
        raise ValueError( "[Test] Wrong number of vertices in cells" )

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
    if grid.previous_vertex( [ 0, 0, 0 ], 1 ) :
        raise ValueError( "[Test] Wrong vertex index" )
    if grid.previous_vertex( [ 0, 0, 1 ], 2 ) != [ 0, 0, 0 ]:
        raise ValueError( "[Test] Wrong vertex index" )

def test_vertex_on_border( grid ):
    if not grid.is_vertex_on_border( [ 0, 0, 0 ] ):
        raise ValueError( "[Test] Vertex is not on border where it should be." )
    if not grid.is_vertex_on_border( [ 0, 9, 0 ] ):
        raise ValueError( "[Test] Vertex is not on border where it should be." )
    if grid.is_vertex_on_border( [ 1, 2, 3 ] ):
        raise ValueError( "[Test] Vertex is on border where it should not be." )

def test_cell_geometry( grid ):
    if grid.point( [ 0, 0, 0 ] ) != geom.Point3D( [ 1.5, 0, 1 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )
    if grid.point( [ 0, 0, 1 ] ) != geom.Point3D( [ 1.5, 0, 4 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )
    if grid.point( [ 1, 1, 1 ] ) != geom.Point3D( [ 2.5, 2, 4 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )
    if grid.point( [ 2, 1, 4 ] ) != geom.Point3D( [ 3.5, 2, 13 ] ):
        raise ValueError( "[Test] Wrong point coordinates" )

def test_cell_query( grid ):
    if grid.cells( geom.Point3D( [ 0, 0, 0 ] ) ):
        raise ValueError( "[Test] Wrong query result" )
    result = grid.cells( geom.Point3D( [ 2, 2, 2 ] ) )
    if len( result ) != 2 or result[0] != [ 0, 0, 0 ] or result[-1] != [ 0, 1, 0 ]:
        raise ValueError( "[Test] Wrong query result" )
    result = grid.cells( geom.Point3D( [ 5, 7, 9 ] ) )
    if len( result ) != 1 or result[0] != [ 3, 3, 2 ]:
        raise ValueError( "[Test] Wrong query result" )
    result = grid.cells( geom.Point3D( [ 4.5, 6, 7 - 1e-10 ] ) )
    if len( result ) != 8 or result[0] != [ 2, 2, 1 ] or result[1] != [ 3, 2, 1 ] or result[2] != [ 2, 3, 1 ] or result[3] != [ 3, 3, 1 ] or result[4] != [ 2, 2, 2 ] or result[5] != [ 3, 2, 2 ] or result[6] != [ 2, 3, 2 ] or result[7] != [ 3, 3, 2 ]:
        raise ValueError( "[Test] Wrong query result" )
    result = grid.cells( geom.Point3D( [ 1.5 - geode.global_epsilon / 2, -geode.global_epsilon / 2, 1 - geode.global_epsilon / 2 ] ) )
    if len( result ) != 1 or result[0] != [ 0, 0, 0 ]:
        raise ValueError( "[Test] Wrong query result for point near origin." )
    result = grid.cells( geom.Point3D( [ 6.5 + geode.global_epsilon / 2, 20 + geode.global_epsilon / 2, 46 + geode.global_epsilon / 2 ] ) );
    if len( result ) != 1 or result[0] != [ 4, 9, 14 ]:
        "[Test] Wrong query result for point near origin furthest corner." )

def test_boundary_box( grid ):
    bbox = grid.bounding_box()
    min = geom.Point3D( [ 1.5, 0, 1 ] )
    if bbox.min() != min:
        raise ValueError( "[Test] Wrong bounding box min" )
    max = geom.Point3D( [ 6.5, 20, 46 ] )
    if bbox.max() != max:
        raise ValueError( "[Test] Wrong bounding box max" )

def test_clone( grid ):
    attribute_name = "int_attribute"
    attribute_name_d = "double_attribute"
    attribute = grid.cell_attribute_manager().find_or_create_attribute_variable_int( attribute_name, 0 )
    attribute_d = grid.vertex_attribute_manager().find_or_create_attribute_variable_double( attribute_name_d, 0 )
    for c in range( grid.nb_cells() ):
        attribute.set_value( c, 2 * c )
    for c in range( grid.nb_vertices() ):
        attribute_d.set_value( c, 2 * c )
    clone = grid.clone()
    if clone.origin() != grid.origin():
        raise ValueError( "[Test] Wrong clone origin" )
    if not clone.cell_attribute_manager().attribute_exists( attribute_name ):
        raise ValueError( "[Test] Clone missing attribute" )
    if not clone.vertex_attribute_manager().attribute_exists( attribute_name_d ):
        raise ValueError( "[Test] Clone missing attribute" )
    clone_attribute = clone.cell_attribute_manager().find_attribute_int( attribute_name )
    for c in range( clone.nb_cells() ):
        if clone_attribute.value( c ) != 2 * c:
            raise ValueError( "[Test] Wrong clone attribute" )
    clone_attribute_d = clone.vertex_attribute_manager().find_attribute_double( attribute_name_d )
    for c in range( clone.nb_vertices() ):
        if clone_attribute_d.value( c ) != 2 * c:
            raise ValueError( "[Test] Wrong clone attribute" )

def test_io( grid, filename ):
    mesh.save_regular_grid3D( grid, filename )
    mesh.load_regular_grid3D( filename )

if __name__ == '__main__':
    print(dir(mesh))
    grid = mesh.RegularGrid3D( geom.Point3D( [ 1.5, 0, 1 ] ), [ 5, 10, 15 ], [ 1., 2., 3. ] )
    test_cell_number( grid )
    test_cell_size( grid )
    test_cell_index( grid )
    test_vertex_number( grid )
    test_vertex_index( grid )
    test_vertex_on_border( grid )
    test_cell_geometry( grid )
    test_cell_query( grid )
    test_boundary_box( grid )
    test_clone( grid )
    test_io( grid, "test." + grid.native_extension() )
