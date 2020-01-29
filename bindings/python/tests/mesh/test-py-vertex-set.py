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

import OpenGeode_py_mesh as mesh

def test_create_vertices( vertex_set, builder ):
    builder.create_vertex()
    if vertex_set.nb_vertices() != 1:
        raise ValueError( "[Test] VertexSet should have 1 vertex" )
    builder.create_vertices( 5 )
    if vertex_set.nb_vertices() != 6:
        raise ValueError( "[Test] VertexSet should have 6 vertices" )

def test_delete_vertex( vertex_set, builder ):
    to_delete = [False] * vertex_set.nb_vertices()
    to_delete[0] = True
    builder.delete_vertices( to_delete )
    if vertex_set.nb_vertices() != 5:
        raise ValueError( "[Test] VertexSet should have 5 vertices" )

def test_clone( vertex_set ):
    vertex_set2 = vertex_set.clone()
    if vertex_set2.nb_vertices() != 5:
        raise ValueError( "[Test] VertexSet2 should have 5 vertices" )

if __name__ == '__main__':
    vertex_set = mesh.VertexSet.create()
    builder = mesh.VertexSetBuilder.create( vertex_set )
    test_create_vertices( vertex_set, builder )
    test_delete_vertex( vertex_set, builder )
    test_clone( vertex_set )
