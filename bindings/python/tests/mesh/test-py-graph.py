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

import opengeode_py_mesh as mesh

def test_create_vertices( graph, builder ):
    builder.create_vertex()
    if graph.nb_vertices() != 1:
        raise ValueError( "[Test] Graph should have 1 vertex" )
    builder.create_vertices( 3 )
    if graph.nb_vertices() != 4:
        raise ValueError( "[Test] Graph should have 4 vertices" )

def test_delete_vertex( graph, builder ):
    to_delete = [False] * graph.nb_vertices()
    to_delete[0] = True
    builder.delete_vertices( to_delete )
    if graph.nb_vertices() != 3:
        raise ValueError( "[Test] Graph should have 3 vertices" )
    if graph.nb_edges() != 2:
        raise ValueError( "[Test] Graph should have 2 edges" )

    edges_around_0 = graph.edges_around_vertex( 0 )
    if len( edges_around_0 ) != 1:
        raise ValueError( "[Test] edges_around_0 should have 1 edge" )
    if edges_around_0[0].edge_id != 1:
        raise ValueError( "[Test] edges_around_0 has wrong value" )
    if edges_around_0[0].vertex_id != 0:
        raise ValueError( "[Test] edges_around_0 has wrong value" )

    edges_around_2 = graph.edges_around_vertex( 2 )
    if len( edges_around_2 ) != 1:
        raise ValueError( "[Test] edges_around_2 should have 1 edge" )
    if edges_around_2[0].edge_id != 0:
        raise ValueError( "[Test] edges_around_2 has wrong value" )
    if edges_around_2[0].vertex_id != 0:
        raise ValueError( "[Test] edges_around_2 has wrong value" )

def test_create_edges( graph, builder ):
    builder.create_edge_with_vertices( 0, 1 )
    builder.create_edge_with_vertices( 0, 2 )
    builder.create_edge_with_vertices( 3, 2 )
    builder.create_edge_with_vertices( 1, 2 )
    if graph.nb_edges() != 4:
        raise ValueError( "[Test] Graph should have 4 edges" )

    edges_around_0 = graph.edges_around_vertex( 0 )
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

    edges_around_2 = graph.edges_around_vertex( 2 )
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

def test_delete_edge( graph, builder ):
    to_delete = [False] * graph.nb_edges()
    to_delete[0] = True
    builder.delete_edges( to_delete )
    if graph.nb_edges() != 1:
        raise ValueError( "[Test] Graph should have 1 edge" )
    if graph.edge_vertex( mesh.EdgeVertex( 0, 0 ) ) != 0:
        raise ValueError( "[Test] Graph edge vertex index is not correct" )
    if graph.edge_vertex( mesh.EdgeVertex( 0, 1 ) ) != 1:
        raise ValueError( "[Test] Graph edge vertex index is not correct" )

    builder.create_edges( 10 )
    builder.set_edge_vertex( mesh.EdgeVertex( 1, 0 ), 1 )
    builder.set_edge_vertex( mesh.EdgeVertex( 1, 1 ), 0 )
    if graph.nb_edges() != 11:
        raise ValueError( "[Test] Graph should have 11 edges" )

    to_delete.extend( [True] * 9 )
    builder.delete_edges( to_delete )
    if graph.nb_edges() != 1:
        raise ValueError( "[Test] Graph should have 1 edge" )
    if graph.edge_vertex( mesh.EdgeVertex( 0, 0 ) ) != 1:
        raise ValueError( "[Test] Graph edge vertex index is not correct (0, 0)" )
    if graph.edge_vertex( mesh.EdgeVertex( 0, 1 ) ) != 0:
        raise ValueError( "[Test] Graph edge vertex index is not correct (0, 1)" )

def test_io( graph, filename ):
    mesh.save_graph( graph, filename )
    new_graph = mesh.load_graph( filename )

def test_clone( graph ):
    graph2 = graph.clone()
    if graph2.nb_vertices() != 3:
        raise ValueError( "[Test] Graph2 should have 3 vertices" )
    if graph2.nb_edges() != 1:
        raise ValueError( "[Test] Graph2 should have 1 edge" )

def test_delete_isolated_vertices( graph, builder ):
    builder.delete_isolated_vertices()
    if graph.nb_vertices() != 2:
        raise ValueError( "[Test] Graph should have 2 vertices" )
    if graph.nb_edges() != 1:
        raise ValueError( "[Test] Graph2 should have 1 edge" )

if __name__ == '__main__':
    graph = mesh.Graph.create()
    builder = mesh.GraphBuilder.create( graph )
    
    test_create_vertices( graph, builder )
    test_create_edges( graph, builder )
    test_io( graph,  "test."  + graph.native_extension() )
    
    test_delete_vertex( graph, builder )
    test_delete_edge( graph, builder )
    test_clone( graph )
    test_delete_isolated_vertices( graph, builder )
