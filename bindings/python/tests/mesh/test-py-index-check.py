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

import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def check_index_error(function, expected_message, label):
    try:
        function()
    except IndexError as error:
        check(
            expected_message in str(error),
            label
            + ": unexpected message '"
            + str(error)
            + "', expected '"
            + expected_message
            + "'",
        )
        return
    except Exception as error:
        raise ValueError(
            "[Test] " + label + ": expected IndexError, got " + repr(error)
        )
    raise ValueError("[Test] " + label + ": expected IndexError, nothing raised")


def create_triangle():
    surface = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_triangle([0, 1, 2])
    return surface, builder


def create_tetrahedron():
    solid = mesh.TetrahedralSolid3D.create()
    builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_point(geom.Point3D([0, 0, 1]))
    builder.create_tetrahedron([0, 1, 2, 3])
    return solid, builder


def test_point():
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    builder.create_point(geom.Point3D([1, 2, 3]))
    check(point_set.point(0).value(2) == 3, "Valid point should be unchanged")
    check_index_error(
        lambda: point_set.point(5), "[point] vertex 5 out of range [0, 1)", "point"
    )
    check_index_error(
        lambda: builder.set_point(5, geom.Point3D([0, 0, 0])),
        "[set_point] vertex 5 out of range [0, 1)",
        "set_point",
    )
    builder.set_point(0, geom.Point3D([4, 5, 6]))
    check(point_set.point(0).value(0) == 4, "Valid set_point should apply")
    crs = point_set.main_coordinate_reference_system_manager().active_coordinate_reference_system()
    check(crs.point(0).value(1) == 5, "Valid CRS point should be unchanged")
    check_index_error(
        lambda: crs.point(5), "[point] point 5 out of range [0, 1)", "crs point"
    )
    check_index_error(
        lambda: crs.set_point(5, geom.Point3D([0, 0, 0])),
        "[set_point] point 5 out of range [0, 1)",
        "crs set_point",
    )


def test_surface():
    surface, builder = create_triangle()
    check(surface.polygon_vertex(mesh.PolygonVertex(0, 2)) == 2, "Valid polygon_vertex")
    check_index_error(
        lambda: surface.polygon_vertex(mesh.PolygonVertex(4, 0)),
        "[polygon_vertex] polygon 4 out of range [0, 1)",
        "polygon_vertex polygon",
    )
    check_index_error(
        lambda: surface.polygon_vertex(mesh.PolygonVertex(0, 3)),
        "[polygon_vertex] local vertex 3 out of range [0, 3)",
        "polygon_vertex local vertex",
    )
    check_index_error(
        lambda: surface.polygon_adjacent(mesh.PolygonEdge(1, 0)),
        "[polygon_adjacent] polygon 1 out of range [0, 1)",
        "polygon_adjacent",
    )
    check_index_error(
        lambda: surface.polygon_area(7), "[polygon_area] polygon 7", "polygon_area"
    )
    check_index_error(
        lambda: surface.polygons_around_vertex(9),
        "[polygons_around_vertex] vertex 9 out of range [0, 3)",
        "polygons_around_vertex",
    )
    check_index_error(
        lambda: surface.edge_length([0, 8]),
        "[edge_length] vertex 8 out of range [0, 3)",
        "edge_length",
    )
    check_index_error(
        lambda: surface.polygon_normal(3), "[polygon_normal] polygon 3", "normal"
    )
    check_index_error(
        lambda: builder.set_polygon_vertex(mesh.PolygonVertex(0, 0), 10),
        "[set_polygon_vertex] vertex 10 out of range [0, 3)",
        "set_polygon_vertex vertex",
    )
    check_index_error(
        lambda: builder.set_polygon_vertex(mesh.PolygonVertex(2, 0), 1),
        "[set_polygon_vertex] polygon 2 out of range [0, 1)",
        "set_polygon_vertex polygon",
    )
    check_index_error(
        lambda: builder.set_polygon_adjacent(mesh.PolygonEdge(0, 0), 4),
        "[set_polygon_adjacent] polygon 4 out of range [0, 1)",
        "set_polygon_adjacent",
    )
    check(surface.polygon_vertex(mesh.PolygonVertex(0, 0)) == 0, "Surface unchanged")
    check(surface.polygon_area(0) == 0.5, "Valid polygon_area")


def test_solid():
    solid, builder = create_tetrahedron()
    check(
        solid.polyhedron_vertex(mesh.PolyhedronVertex(0, 3)) == 3,
        "Valid polyhedron_vertex",
    )
    check_index_error(
        lambda: solid.polyhedron_vertex(mesh.PolyhedronVertex(2, 0)),
        "[polyhedron_vertex] polyhedron 2 out of range [0, 1)",
        "polyhedron_vertex",
    )
    check_index_error(
        lambda: solid.polyhedron_vertex(mesh.PolyhedronVertex(0, 4)),
        "[polyhedron_vertex] local vertex 4 out of range [0, 4)",
        "polyhedron_vertex local",
    )
    check_index_error(
        lambda: solid.polyhedron_adjacent(mesh.PolyhedronFacet(0, 4)),
        "[polyhedron_adjacent] local facet 4 out of range [0, 4)",
        "polyhedron_adjacent",
    )
    check_index_error(
        lambda: solid.polyhedron_volume(1),
        "[polyhedron_volume] polyhedron 1",
        "polyhedron_volume",
    )
    check_index_error(
        lambda: builder.set_polyhedron_vertex(mesh.PolyhedronVertex(0, 0), 8),
        "[set_polyhedron_vertex] vertex 8 out of range [0, 4)",
        "set_polyhedron_vertex",
    )
    check_index_error(
        lambda: builder.set_point(6, geom.Point3D([0, 0, 0])),
        "[set_point] vertex 6 out of range [0, 4)",
        "solid set_point",
    )


def test_graph():
    curve = mesh.EdgedCurve3D.create()
    builder = mesh.EdgedCurveBuilder3D.create(curve)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_edge_with_vertices(0, 1)
    check(curve.edge_vertex(mesh.EdgeVertex(0, 1)) == 1, "Valid edge_vertex")
    check(curve.edge_length(0) == 1, "Valid edge_length")
    check_index_error(
        lambda: curve.edge_vertex(mesh.EdgeVertex(3, 0)),
        "[edge_vertex] edge 3 out of range [0, 1)",
        "edge_vertex",
    )
    check_index_error(
        lambda: curve.edges_around_vertex(2),
        "[edges_around_vertex] vertex 2 out of range [0, 2)",
        "edges_around_vertex",
    )
    check_index_error(
        lambda: curve.segment(1), "[segment] edge 1 out of range [0, 1)", "segment"
    )
    check_index_error(
        lambda: builder.set_edge_vertex(mesh.EdgeVertex(0, 0), 5),
        "[set_edge_vertex] vertex 5 out of range [0, 2)",
        "set_edge_vertex",
    )
    check_index_error(
        lambda: builder.set_edge_vertex(mesh.EdgeVertex(1, 0), 1),
        "[set_edge_vertex] edge 1 out of range [0, 1)",
        "set_edge_vertex edge",
    )


def test_attribute():
    manager = basic.AttributeManager()
    manager.resize(3)
    attribute_id = manager.create_attribute_variable_double("values", 0.5)
    attribute = manager.find_attribute_variable_double(attribute_id)
    read_only = manager.find_read_only_attribute_double(attribute_id)
    attribute.set_value(1, 2.0)
    check(read_only.value(1) == 2.0, "Valid attribute value should be unchanged")
    check_index_error(
        lambda: read_only.value(3),
        "[value] element 3 out of range [0, 3)",
        "attribute value",
    )
    check_index_error(
        lambda: attribute.set_value(7, 1.0),
        "[set_value] element 7 out of range [0, 3)",
        "attribute set_value",
    )
    constant_id = manager.create_attribute_constant_double("constant", 1.5)
    constant = manager.find_read_only_attribute_double(constant_id)
    check(constant.value(1000) == 1.5, "Constant attribute accepts any element")


if __name__ == "__main__":
    mesh.OpenGeodeMeshLibrary.initialize()
    test_point()
    test_surface()
    test_solid()
    test_graph()
    test_attribute()
