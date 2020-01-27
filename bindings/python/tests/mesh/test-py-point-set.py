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

def test_create_vertices( point_set, builder ):
    builder.create_point( geom.Point3D( [ 0.1, 0.2, 0.3 ] ) )
    builder.create_point( geom.Point3D( [ 2.1, 9.4, 6.7 ] ) )
    if point_set.nb_vertices() != 2:
        raise ValueError( "[Test] PointSet should have 2 vertices" )
    builder.create_vertices( 2 )
    if point_set.nb_vertices() != 4:
        raise ValueError( "[Test] PointSet should have 4 vertices" )
    if point_set.point( 2 ) != geom.Point3D():
        raise ValueError( "[Test] Default coordinates are not correct" )
    builder.set_point( 2, geom.Point3D( [ 2.3, 5.0, -1.2 ] ) )
    if point_set.point( 2 ) != geom.Point3D( [ 2.3, 5.0, -1.2 ] ):
        raise ValueError( "[Test] Point coordinates have not been correctly set" )

def test_bounding_box( point_set ):
    answer_min = geom.Point3D( [ 0.0, 0.0, -1.2 ] )
    if point_set.bounding_box().min() != answer_min:
        raise ValueError( "[Test] Wrong computation of bounding box (min)" )
    answer_max = geom.Point3D( [ 2.3, 9.4, 6.7 ] )
    if point_set.bounding_box().max() != answer_max:
        raise ValueError( "[Test] Wrong computation of bounding box (max)" )

def test_create_vertex_attribute( point_set ):
    manager = point_set.vertex_attribute_manager()
    attribute = point_set.vertex_attribute_manager().find_or_create_attribute_constant_bool( "test", True )
    if attribute.constant_value() != True:
        raise ValueError( "[Test] PointSet attribute value should be true" )

def test_delete_vertex( point_set, builder ):
    to_delete = [False] * point_set.nb_vertices()
    to_delete[0]= True
    builder.delete_vertices( to_delete )
    if point_set.nb_vertices() != 3:
        raise ValueError( "[Test] PointSet should have 3 vertices" )
    answer = geom.Point3D( [ 2.1, 9.4, 6.7 ] )
    if point_set.point( 0 ) != answer:
        raise ValueError( "[Test] PointSet vertex coordinates are not correct" )

def test_io( point_set, filename ):
    mesh.save_point_set3D( point_set, filename )
    new_point_set = mesh.PointSet3D.create()
    mesh.load_point_set3D( new_point_set, filename )

def test_clone( point_set ):
    point_set2 = point_set.clone()
    if point_set2.nb_vertices() != 3:
        raise ValueError( "[Test] PointSet2 should have 3 vertices" )

    attribute = point_set2.vertex_attribute_manager().find_attribute_bool( "test" )
    if attribute.value( 0 ) != True:
        raise ValueError( "[Test] PointSet2 attribute value should be true" )

    answer = geom.Point3D( [ 2.1, 9.4, 6.7 ] )
    if point_set2.point( 0 ) != answer:
        raise ValueError( "[Test] PointSet2 vertex coordinates are not correct" )

point_set = mesh.PointSet3D.create()
builder = mesh.PointSetBuilder3D.create( point_set )
test_create_vertices( point_set, builder )
test_bounding_box( point_set )
test_create_vertex_attribute( point_set )
test_io( point_set, "test." + point_set.native_extension() )
test_delete_vertex( point_set, builder )
test_clone( point_set )
