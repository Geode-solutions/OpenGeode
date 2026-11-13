# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2026 Geode-solutions
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

import geode_test_env

import opengeode_py_mesh as mesh
import opengeode


def test_create_vertices(graph, builder):
    builder.create_vertex()
    if graph.nb_vertices() != 1:
        raise ValueError("[Test] Graph should have 1 vertex")
    builder.create_vertices(3)
    if graph.nb_vertices() != 4:
        raise ValueError("[Test] Graph should have 4 vertices")


def test_create_edges(graph, builder):
    builder.create_edge_with_vertices(0, 1)
    builder.create_edge_with_vertices(0, 2)
    builder.create_edge_with_vertices(3, 2)
    builder.create_edge_with_vertices(1, 2)
    if graph.nb_edges() != 4:
        raise ValueError("[Test] Graph should have 4 edges")
    answer = [3, 2]
    if graph.edge_vertices(2) != answer:
        raise ValueError("[Test] Wrong edge vertices")

    edges_around_0 = graph.edges_around_vertex(0)
    if len(edges_around_0) != 2:
        raise ValueError("[Test] edges_around_0 should have 2 edges")
    if edges_around_0[0].edge_id != 0:
        raise ValueError("[Test] edges_around_0 has wrong value")
    if edges_around_0[0].vertex_id != 0:
        raise ValueError("[Test] edges_around_0 has wrong value")
    if edges_around_0[1].edge_id != 1:
        raise ValueError("[Test] edges_around_0 has wrong value")
    if edges_around_0[1].vertex_id != 0:
        raise ValueError("[Test] edges_around_0 has wrong value")

    edges_around_2 = graph.edges_around_vertex(2)
    if len(edges_around_2) != 3:
        raise ValueError("[Test] edges_around_2 should have 3 edges")
    if edges_around_2[0].edge_id != 1:
        raise ValueError("[Test] edges_around_2 has wrong value")
    if edges_around_2[0].vertex_id != 1:
        raise ValueError("[Test] edges_around_2 has wrong value")
    if edges_around_2[1].edge_id != 2:
        raise ValueError("[Test] edges_around_2 has wrong value")
    if edges_around_2[1].vertex_id != 1:
        raise ValueError("[Test] edges_around_2 has wrong value")
    if edges_around_2[2].edge_id != 3:
        raise ValueError("[Test] edges_around_2 has wrong value")
    if edges_around_2[2].vertex_id != 1:
        raise ValueError("[Test] edges_around_2 has wrong value")


def test_delete_edge(graph, builder):
    to_delete = [False] * graph.nb_edges()
    to_delete[0] = True
    builder.delete_edges(to_delete)
    if graph.nb_edges() != 3:
        raise ValueError("[Test] Graph should have 3 edges")
    if graph.edge_vertex(mesh.EdgeVertex(0, 0)) != 0:
        raise ValueError("[Test] Graph edge vertex index is not correct")
    if graph.edge_vertex(mesh.EdgeVertex(0, 1)) != 2:
        raise ValueError("[Test] Graph edge vertex index is not correct")

    builder.create_edges(10)
    builder.set_edge_vertex(mesh.EdgeVertex(1, 0), 1)
    builder.set_edge_vertex(mesh.EdgeVertex(1, 1), 0)
    if graph.nb_edges() != 13:
        raise ValueError("[Test] Graph should have 13 edges")

    to_delete[-1] = True
    to_delete.extend([True] * 9)
    builder.delete_edges(to_delete)
    if graph.nb_edges() != 2:
        raise ValueError("[Test] Graph should have 2 edges")
    if graph.edge_vertex(mesh.EdgeVertex(0, 0)) != 1:
        raise ValueError(
            "[Test] Graph edge vertex index is not correct (0, 0)")
    if graph.edge_vertex(mesh.EdgeVertex(0, 1)) != 0:
        raise ValueError(
            "[Test] Graph edge vertex index is not correct (0, 1)")


def test_io(graph, filename):
    mesh.save_graph(graph, filename)
    new_graph = opengeode.load_one(mesh.load_graph(filename), filename)


def test_clone(graph):
    graph2 = graph.clone()
    if graph2.nb_vertices() != 4:
        raise ValueError("[Test] Graph2 should have 4 vertices")
    if graph2.nb_edges() != 2:
        raise ValueError("[Test] Graph2 should have 2 edges")


def test_delete_isolated_vertices(graph, builder):
    builder.delete_isolated_vertices()
    if graph.nb_vertices() != 3:
        raise ValueError("[Test] Graph should have 3 vertices")
    if graph.nb_edges() != 2:
        raise ValueError("[Test] Graph2 should have 2 edges")


def expect_data_error(function, message):
    try:
        function()
    except opengeode.OpenGeodeDataError:
        return
    raise ValueError(f"[Test] {message} should raise an OpenGeodeDataError")


def test_invalid_edge_operations():
    graph = mesh.Graph.create()
    builder = mesh.GraphBuilder.create(graph)
    builder.create_vertices(3)
    for vertices in [(0, 1), (1, 2), (0, 2)]:
        builder.create_edge_with_vertices(*vertices)

    def check_intact(when):
        if graph.nb_edges() != 3:
            raise ValueError(f"[Test] Wrong number of edges after {when}")
        expected = [[0, 1], [1, 2], [0, 2]]
        if [graph.edge_vertices(e) for e in range(3)] != expected:
            raise ValueError(f"[Test] Edge vertices changed after {when}")
        for vertex in range(3):
            around = graph.edges_around_vertex(vertex)
            if len(around) != 2:
                raise ValueError(f"[Test] Edges around vertex {vertex} changed after {when}")
            for edge_vertex in around:
                if graph.edge_vertex(edge_vertex) != vertex:
                    raise ValueError(f"[Test] Wrong edge around vertex {vertex} after {when}")

    for permutation in [[0, 1], [0, 1, 2, 3], [0, 0, 1], [0, 1, 3], []]:
        expect_data_error(
            lambda: builder.permute_edges(permutation), f"permute_edges{permutation}"
        )
        check_intact(f"permute_edges{permutation}")
    for flags in [[True], [True, False, False, False], [False], []]:
        expect_data_error(
            lambda: builder.delete_edges(flags), f"delete_edges{flags}"
        )
        check_intact(f"delete_edges{flags}")
    # The graph is still usable
    if builder.permute_edges([2, 1, 0]) != [2, 1, 0]:
        raise ValueError("[Test] Wrong mapping of a valid permutation")
    if builder.delete_edges([False, True, False]) != [0, opengeode.NO_ID, 1]:
        raise ValueError("[Test] Wrong mapping of a valid deletion")


if __name__ == '__main__':
    mesh.OpenGeodeMeshLibrary.initialize()
    graph = mesh.Graph.create()
    builder = mesh.GraphBuilder.create(graph)

    test_create_vertices(graph, builder)
    test_create_edges(graph, builder)
    test_io(graph,  "test." + graph.native_extension())

    test_delete_edge(graph, builder)
    test_clone(graph)
    test_delete_isolated_vertices(graph, builder)
    test_invalid_edge_operations()
