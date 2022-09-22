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

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh


def test_create_vertices(polygonal_surface, builder):
    builder.create_point(geom.Point3D([0.1, 0.2, 0.3]))
    builder.create_point(geom.Point3D([2.1, 9.4, 6.7]))
    builder.create_point(geom.Point3D([7.5, 5.2, 6.3]))
    builder.create_point(geom.Point3D([8.1, 1.4, 4.7]))
    builder.create_point(geom.Point3D([4.7, 2.1, 1.3]))
    builder.create_point(geom.Point3D([9.3, 5.3, 6.7]))
    builder.create_point(geom.Point3D([7.5, 4.2, 2.8]))
    if polygonal_surface.nb_vertices() != 7:
        raise ValueError("[Test] PolygonalSurface should have 7 vertices")


def test_bounding_box(polygonal_surface):
    answer_min = geom.Point3D([0.1, 0.2, 0.3])
    answer_max = geom.Point3D([9.3, 9.4, 6.7])
    if polygonal_surface.bounding_box().min() != answer_min:
        raise ValueError("[Test] Wrong computation of bounding box (min)")
    if polygonal_surface.bounding_box().max() != answer_max:
        raise ValueError("[Test] Wrong computation of bounding box (max)")


def test_create_polygons(polygonal_surface, builder):
    builder.create_polygon([0, 1, 2])
    builder.create_polygon([1, 3, 4, 2])
    builder.create_polygon([1, 5, 6, 3])
    if polygonal_surface.nb_polygons() != 3:
        raise ValueError("[Test] PolygonalSurface should have 3 polygons")
    if polygonal_surface.edges().nb_edges() != 9:
        raise ValueError("[Test] PolygonalSurface should have 9 edges")
    answer = [1, 3, 4, 2]
    if polygonal_surface.polygon_vertices(1) != answer:
        raise ValueError("[Test] Wrong polygon vertices list")


def test_create_edge_attribute(polygonal_surface):
    attribute = polygonal_surface.edges().edge_attribute_manager(
    ).find_or_create_attribute_variable_uint("test", basic.NO_ID)
    for e in range(polygonal_surface.edges().nb_edges()):
        attribute.set_value(e, e)


def test_polygon_adjacencies(polygonal_surface, builder):
    builder.compute_polygon_adjacencies()
    if polygonal_surface.polygon_adjacent(mesh.PolygonEdge(0, 0)):
        raise ValueError(
            "[Test] PolygonalSurface adjacent index is not correct")
    if polygonal_surface.polygon_adjacent(mesh.PolygonEdge(0, 1)) != 1:
        raise ValueError(
            "[Test] PolygonalSurface adjacent index is not correct")
    if polygonal_surface.polygon_adjacent(mesh.PolygonEdge(1, 3)) != 0:
        raise ValueError(
            "[Test] PolygonalSurface adjacent index is not correct")
    if polygonal_surface.polygon_adjacent_edge(mesh.PolygonEdge(0, 1)) != mesh.PolygonEdge(1, 3):
        raise ValueError(
            "[Test] PolygonalSurface adjacent index is not correct")

    if polygonal_surface.polygon_adjacent(mesh.PolygonEdge(2, 0)):
        raise ValueError(
            "[Test] PolygonalSurface adjacent index is not correct")
    if polygonal_surface.polygon_adjacent(mesh.PolygonEdge(2, 3)) != 1:
        raise ValueError(
            "[Test] PolygonalSurface adjacent index is not correct")

    if len(polygonal_surface.polygons_around_vertex(1)) != 3:
        raise ValueError(
            "[Test] PolygonalSurface should have 3 polygons around this vertex")


def test_polygon_edges_on_borders(polygonal_surface):
    result = polygonal_surface.polygon_edges_on_border(0)
    if len(result) != 2:
        raise ValueError(
            "[Test] Number of polygon edges on border index is not correct")


def test_previous_next_on_border(polygonal_surface):
    if polygonal_surface.previous_on_border(mesh.PolygonEdge(0, 0)) != mesh.PolygonEdge(0, 2):
        raise ValueError("[Test] Previous edge on border index is not correct")
    if polygonal_surface.next_on_border(mesh.PolygonEdge(0, 0)) != mesh.PolygonEdge(2, 0):
        raise ValueError("[Test] Next edge on border index is not correct")


def test_delete_polygon(polygonal_surface, builder):
    to_delete = [False] * polygonal_surface.nb_polygons()
    to_delete[0] = True
    builder.delete_polygons(to_delete)
    if polygonal_surface.nb_polygons() != 2:
        raise ValueError("[Test] PolygonalSurface should have 2 polygons")
    if polygonal_surface.polygon_vertex(mesh.PolygonVertex(0, 0)) != 5:
        raise ValueError(
            "[Test] PolygonalSurface edge vertex index is not correct")
    if polygonal_surface.polygon_vertex(mesh.PolygonVertex(0, 1)) != 3:
        raise ValueError(
            "[Test] PolygonalSurface edge vertex index is not correct")
    if polygonal_surface.polygon_vertex(mesh.PolygonVertex(0, 2)) != 1:
        raise ValueError(
            "[Test] PolygonalSurface edge vertex index is not correct")
    builder.edges_builder().delete_isolated_edges()
    if polygonal_surface.edges().nb_edges() != 6:
        raise ValueError("[Test] PolygonalSurface should have 6 edges")

    attribute = polygonal_surface.edges(
    ).edge_attribute_manager().find_attribute_uint("test")
    for e in range(6):
        if attribute.value(e) != e:
            raise ValueError("[Test] Update of edge attributes after "
                             "polygon deletion is not correct")


def test_polygon_barycenter(polygonal_surface):
    answer = geom.Point3D([5.6, 4.525, 4.75])
    if polygonal_surface.polygon_barycenter(1) != answer:
        raise ValueError(
            "[Test] PolygonalSurface polygon barycenter is not correct")


def test_polygon_area():
    polygonal_surface = mesh.PolygonalSurface2D.create()
    builder = mesh.PolygonalSurfaceBuilder2D.create(polygonal_surface)
    a = 6.0
    b = 8.0
    c = 4.0

    builder.create_point(geom.Point2D([0.0, 0.0]))
    builder.create_point(geom.Point2D([a, 0.0]))
    builder.create_point(geom.Point2D([b, c]))
    builder.create_point(geom.Point2D([0.0, c]))

    builder.create_polygon([0, 1, 2, 3])

    if polygonal_surface.polygon_area(0) != 28:
        raise ValueError("[Test] PolygonalSurface polygon area is not correct")


def test_polygon_normal():
    polygonal_surface = mesh.PolygonalSurface3D.create()
    builder = mesh.PolygonalSurfaceBuilder3D.create(polygonal_surface)
    a = 6.0
    b = 8.0
    c = 4.0

    builder.create_point(geom.Point3D([0.0, 0.0, 0.0]))
    builder.create_point(geom.Point3D([a, 0.0, 0.0]))
    builder.create_point(geom.Point3D([b, c, 0.0]))
    builder.create_point(geom.Point3D([0.0, c, 0.0]))

    builder.create_polygon([0, 1, 2, 3])

    answer = geom.Vector3D([0., 0., 1.])
    if polygonal_surface.polygon_normal(0) != answer:
        raise ValueError(
            "[Test] PolygonalSurface polygon normal is not correct")


def test_polygon_vertex_normal():
    polygonal_surface = mesh.PolygonalSurface3D.create()
    builder = mesh.PolygonalSurfaceBuilder3D.create(polygonal_surface)

    builder.create_point(geom.Point3D([0.0, 0.0, 0.0]))
    builder.create_point(geom.Point3D([1.0, 0.0, 0.0]))
    builder.create_point(geom.Point3D([0.0, 1.0, 1.0]))
    builder.create_point(geom.Point3D([0.0, -1.0, 1.0]))

    builder.create_polygon([0, 1, 2])
    builder.create_polygon([0, 3, 1])
    builder.compute_polygon_adjacencies()

    answer = geom.Vector3D([0., 0., 1.])
    if polygonal_surface.polygon_vertex_normal(0) != answer:
        raise ValueError(
            "[Test] PolygonalSurface polygon vertex normal is not correct")


def test_io(polygonal_surface, filename):
    mesh.save_polygonal_surface3D(polygonal_surface, filename)
    new_polygonal_surface = mesh.load_polygonal_surface3D(filename)

    if new_polygonal_surface.nb_vertices() != 7:
        raise ValueError(
            "[Test] Reloaded PolygonalSurface should have 7 vertices")
    if new_polygonal_surface.edges().nb_edges() != 9:
        raise ValueError(
            "[Test] Reloaded PolygonalSurface should have 9 edges")
    if new_polygonal_surface.nb_polygons() != 3:
        raise ValueError(
            "[Test] Reloaded PolygonalSurface should have 3 polygons")
    attribute = new_polygonal_surface.edges(
    ).edge_attribute_manager().find_attribute_uint("test")
    for e in range(new_polygonal_surface.edges().nb_edges()):
        if attribute.value(e) != e:
            raise ValueError(
                "[Test] Reloaded PolygonalSurface has wrong attributes on its edges")


def test_clone(polygonal_surface):
    polygonal_surface2 = polygonal_surface.clone()
    if polygonal_surface2.nb_vertices() != 7:
        raise ValueError("[Test] PolygonalSurface2 should have 7 vertices")
    if polygonal_surface2.edges().nb_edges() != 6:
        raise ValueError("[Test] PolygonalSurface2 should have 6 edges")
    if polygonal_surface2.nb_polygons() != 2:
        raise ValueError("[Test] PolygonalSurface2 should have 2 polygon")


def test_set_polygon_vertex(polygonal_surface, builder):
    builder.set_polygon_vertex(mesh.PolygonVertex(0, 2), 1)
    builder.edges_builder().delete_isolated_edges()
    if polygonal_surface.polygon_vertex(mesh.PolygonVertex(0, 2)) != 1:
        raise ValueError(
            "[Test] PolygonVertex after set_polygon_vertex is wrong")

    vertices = polygonal_surface.edges().edge_vertices(1)
    if vertices[0] != 1 and vertices[1] != 4:
        raise ValueError(
            "[Test] Edge vertices after set_polygon_vertex is wrong")


def test_delete_all(polygonal_surface, builder):
    to_delete = [True] * polygonal_surface.nb_polygons()
    builder.delete_polygons(to_delete)
    if polygonal_surface.nb_vertices() != 7:
        raise ValueError("[Test] PolygonalSurface should have 7 vertices")
    if polygonal_surface.nb_polygons() != 0:
        raise ValueError("[Test] PolygonalSurface should have 0 polygon")
    builder.edges_builder().delete_isolated_edges()
    if polygonal_surface.edges().nb_edges() != 0:
        raise ValueError("[Test] PolygonalSurface should have 0 edge")
    if len(polygonal_surface.polygons_around_vertex(0)) != 0:
        raise ValueError("[Test] No more polygon around vertices")
    builder.delete_isolated_vertices()
    if polygonal_surface.nb_vertices() != 0:
        raise ValueError("[Test] PolygonalSurface should have 0 vertex")


def test_permutation(surface, builder):
    builder.permute_vertices([4, 2, 6, 1, 5, 0, 3])
    if surface.polygon_vertex(mesh.PolygonVertex(0, 0)) != 5:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(0, 1)) != 3:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(0, 2)) != 1:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(1, 0)) != 3:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(1, 1)) != 6:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(1, 2)) != 0:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(1, 3)) != 1:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 0)) != 3:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 1)) != 4:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 2)) != 2:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 3)) != 6:
        raise ValueError("[Test] Wrong PolygonVertex after vertex permute")

    builder.permute_polygons([2, 0, 1])
    if surface.polygon_vertex(mesh.PolygonVertex(0, 0)) != 3:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(0, 1)) != 4:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(0, 2)) != 2:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(0, 3)) != 6:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(1, 0)) != 5:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(1, 1)) != 3:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(1, 2)) != 1:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 0)) != 3:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 1)) != 6:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 2)) != 0:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")
    if surface.polygon_vertex(mesh.PolygonVertex(2, 3)) != 1:
        raise ValueError("[Test] Wrong PolygonVertex after polygon permute")

    if surface.polygon_adjacent(mesh.PolygonEdge(0, 3)) != 2:
        raise ValueError("[Test] Wrong Adjacency after polygon permute")
    if surface.polygon_adjacent(mesh.PolygonEdge(2, 0)) != 0:
        raise ValueError("[Test] Wrong Adjacency after polygon permute")
    if surface.polygon_adjacent(mesh.PolygonEdge(1, 1)) != 2:
        raise ValueError("[Test] Wrong Adjacency after polygon permute")
    if surface.polygon_adjacent(mesh.PolygonEdge(1, 0)):
        raise ValueError("[Test] Wrong Adjacency after polygon permute")

    polygons_3 = surface.polygons_around_vertex(3)
    if len(polygons_3) != 3:
        raise ValueError("[Test] Wrong polygons_3 after polygon permute")
    if polygons_3[0].polygon_id != 0:
        raise ValueError("[Test] Wrong polygons_3 after polygon permute")
    if polygons_3[0].vertex_id != 0:
        raise ValueError("[Test] Wrong polygons_3 after polygon permute")
    if polygons_3[1].polygon_id != 2:
        raise ValueError("[Test] Wrong polygons_3 after polygon permute")
    if polygons_3[1].vertex_id != 0:
        raise ValueError("[Test] Wrong polygons_3 after polygon permute")
    if polygons_3[2].polygon_id != 1:
        raise ValueError("[Test] Wrong polygons_3 after polygon permute")
    if polygons_3[2].vertex_id != 1:
        raise ValueError("[Test] Wrong polygons_3 after polygon permute")

    polygons_6 = surface.polygons_around_vertex(6)
    if len(polygons_6) != 2:
        raise ValueError("[Test] Wrong polygons_6 after polygon permute")
    if polygons_6[0].polygon_id != 0:
        raise ValueError("[Test] Wrong polygons_6 after polygon permute")
    if polygons_6[0].vertex_id != 3:
        raise ValueError("[Test] Wrong polygons_6 after polygon permute")
    if polygons_6[1].polygon_id != 2:
        raise ValueError("[Test] Wrong polygons_6 after polygon permute")
    if polygons_6[1].vertex_id != 1:
        raise ValueError("[Test] Wrong polygons_6 after polygon permute")


if __name__ == '__main__':
    mesh.OpenGeodeMesh.initialize()
    polygonal_surface = mesh.PolygonalSurface3D.create()
    if polygonal_surface.is_triangulated_type():
        raise ValueError("[Test] Wrong surface type")
    if not polygonal_surface.is_polygonal_type():
        raise ValueError("[Test] Wrong surface type")
    polygonal_surface.enable_edges()
    builder = mesh.PolygonalSurfaceBuilder3D.create(polygonal_surface)

    test_create_vertices(polygonal_surface, builder)
    test_bounding_box(polygonal_surface)
    test_create_polygons(polygonal_surface, builder)
    test_create_edge_attribute(polygonal_surface)
    test_polygon_adjacencies(polygonal_surface, builder)
    test_polygon_edges_on_borders(polygonal_surface)
    test_previous_next_on_border(polygonal_surface)
    test_polygon_barycenter(polygonal_surface)
    test_polygon_area()
    test_polygon_normal()
    test_polygon_vertex_normal()

    test_io(polygonal_surface, "test." + polygonal_surface.native_extension())

    test_permutation(polygonal_surface, builder)
    test_delete_polygon(polygonal_surface, builder)
    test_clone(polygonal_surface)
    test_set_polygon_vertex(polygonal_surface, builder)
    test_delete_all(polygonal_surface, builder)
