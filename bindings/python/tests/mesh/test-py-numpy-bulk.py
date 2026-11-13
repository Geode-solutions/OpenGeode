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

import sys
import threading
import time

import numpy

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh

PERFORMANCE_BOUND_SECONDS = 1.0
NB_PERFORMANCE_POINTS = 100000
TIMEOUT = 60


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def check_raises(error_type, expected_message, function, label):
    try:
        function()
    except error_type as error:
        check(
            expected_message in str(error),
            f"{label}: unexpected message '{error}', expected '{expected_message}'",
        )
        check(
            type(error) is error_type,
            f"{label}: {type(error).__name__} is not exactly {error_type.__name__}",
        )
        return
    raise ValueError(f"[Test] {label}: {error_type.__name__} not raised")


def check_array(array, dtype, shape, label):
    check(isinstance(array, numpy.ndarray), f"{label}: not a numpy array")
    check(array.dtype == dtype, f"{label}: dtype {array.dtype}, expected {dtype}")
    check(array.shape == shape, f"{label}: shape {array.shape}, expected {shape}")


def random_points(count, dimension, seed=0):
    return numpy.random.default_rng(seed).random((count, dimension))


# Points


POINT_CASES = [
    (mesh.PointSet2D, mesh.PointSetBuilder2D, 2),
    (mesh.PointSet3D, mesh.PointSetBuilder3D, 3),
    (mesh.EdgedCurve2D, mesh.EdgedCurveBuilder2D, 2),
    (mesh.EdgedCurve3D, mesh.EdgedCurveBuilder3D, 3),
    (mesh.TriangulatedSurface2D, mesh.TriangulatedSurfaceBuilder2D, 2),
    (mesh.TriangulatedSurface3D, mesh.TriangulatedSurfaceBuilder3D, 3),
    (mesh.PolygonalSurface2D, mesh.PolygonalSurfaceBuilder2D, 2),
    (mesh.PolygonalSurface3D, mesh.PolygonalSurfaceBuilder3D, 3),
    (mesh.TetrahedralSolid3D, mesh.TetrahedralSolidBuilder3D, 3),
]


def test_points_round_trip():
    for mesh_class, builder_class, dimension in POINT_CASES:
        label = mesh_class.__name__
        element = mesh_class.create()
        builder = builder_class.create(element)
        empty = element.points_array()
        check_array(empty, numpy.float64, (0, dimension), label + " empty")
        points = random_points(7, dimension)
        check(builder.create_points(points) == 0, label + " first index")
        check(element.nb_vertices() == 7, label + " nb_vertices")
        result = element.points_array()
        check_array(result, numpy.float64, (7, dimension), label)
        check(numpy.array_equal(result, points), label + " round trip")
        for vertex in range(7):
            expected = [float(value) for value in points[vertex]]
            point = element.point(vertex)
            check(
                [point.value(i) for i in range(dimension)] == expected,
                label + " point() agrees with the array",
            )
        # Appending returns the index of the first new vertex
        more = random_points(3, dimension, seed=1)
        check(builder.create_points(more) == 7, label + " second first index")
        check(
            numpy.array_equal(
                element.points_array(), numpy.concatenate([points, more])
            ),
            label + " appended",
        )
        # Nothing is created with no point
        check(
            builder.create_points(numpy.empty((0, dimension))) == 10,
            label + " empty first index",
        )
        check(element.nb_vertices() == 10, label + " empty creation")


def test_solid_points():
    solid = mesh.TetrahedralSolid3D.create()
    check(solid.points_array().shape == (0, 3), "solid empty points")
    builder = mesh.SolidMeshBuilder3D.create(solid)
    points = random_points(4, 3)
    check(builder.create_points(points) == 0, "solid create_points")
    as_mesh = solid
    check(numpy.array_equal(as_mesh.points_array(), points), "solid points")
    for solid_class, builder_class in [
        (mesh.PolyhedralSolid3D, mesh.PolyhedralSolidBuilder3D),
        (mesh.HybridSolid3D, mesh.HybridSolidBuilder3D),
    ]:
        other = solid_class.create()
        other_builder = builder_class.create(other)
        other_builder.create_points(points)
        check(
            numpy.array_equal(other.points_array(), points),
            solid_class.__name__ + " points",
        )


def test_points_copy_semantics():
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    points = random_points(5, 3)
    original = points.copy()
    builder.create_points(points)
    points[:] = -1.0
    check(
        numpy.array_equal(point_set.points_array(), original),
        "the input array is copied",
    )
    result = point_set.points_array()
    result[:] = 42.0
    check(
        numpy.array_equal(point_set.points_array(), original),
        "the output array is a copy",
    )
    snapshot = point_set.points_array()
    builder.set_point(0, geom.Point3D([9.0, 9.0, 9.0]))
    builder.create_point(geom.Point3D([5.0, 5.0, 5.0]))
    check(numpy.array_equal(snapshot, original), "a snapshot is unchanged by edits")
    check(point_set.points_array().shape == (6, 3), "new snapshot")


def test_points_input_dtypes():
    for dtype in [numpy.float32, numpy.float64, numpy.int8, numpy.int32,
                  numpy.int64, numpy.uint8, numpy.uint32, numpy.uint64]:
        point_set = mesh.PointSet3D.create()
        builder = mesh.PointSetBuilder3D.create(point_set)
        points = numpy.array([[1, 2, 3], [4, 5, 6]], dtype=dtype)
        builder.create_points(points)
        check(
            numpy.array_equal(point_set.points_array(), points.astype(numpy.float64)),
            f"points of dtype {dtype.__name__}",
        )
    # Lists and non contiguous arrays
    point_set = mesh.PointSet2D.create()
    builder = mesh.PointSetBuilder2D.create(point_set)
    builder.create_points([[0.5, 1.5], [2.5, 3.5]])
    base = numpy.arange(12, dtype=numpy.float64).reshape(4, 3)
    builder.create_points(base[:, :2])
    builder.create_points(numpy.asfortranarray(base[:2, :2]))
    expected = numpy.array(
        [[0.5, 1.5], [2.5, 3.5], [0, 1], [3, 4], [6, 7], [9, 10], [0, 1], [3, 4]],
        dtype=numpy.float64,
    )
    expected[6:] = [[0, 1], [3, 4]]
    check(numpy.array_equal(point_set.points_array(), expected), "layouts")
    # Booleans, complex numbers, strings and objects are not coordinates
    point_set3 = mesh.PointSet3D.create()
    builder3 = mesh.PointSetBuilder3D.create(point_set3)
    for bad in [
        numpy.array([[True, False, True]]),
        numpy.array([[1 + 1j, 2, 3]]),
        numpy.array([["1", "2", "3"]]),
        numpy.array([[None, None, None]], dtype=object),
    ]:
        check_raises(
            ValueError,
            "points must be an array of floats or integers, got dtype",
            lambda bad=bad: builder3.create_points(bad),
            f"dtype {bad.dtype}",
        )
    check(point_set3.nb_vertices() == 0, "bad dtypes do not modify the mesh")


def test_points_errors():
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    builder.create_points(random_points(2, 3))
    before = point_set.points_array()
    for bad, shape in [
        (numpy.zeros((4, 2)), "(4, 2)"),
        (numpy.zeros((4, 4)), "(4, 4)"),
        (numpy.zeros(3), "(3,)"),
        (numpy.zeros((2, 3, 1)), "(2, 3, 1)"),
        (numpy.zeros(()), "()"),
    ]:
        check_raises(
            ValueError,
            f"[create_points] points must have shape (n, 3), got {shape}",
            lambda bad=bad: builder.create_points(bad),
            f"shape {shape}",
        )
    check(numpy.array_equal(point_set.points_array(), before), "errors are atomic")
    point_set2 = mesh.PointSet2D.create()
    check_raises(
        ValueError,
        "must have shape (n, 2), got (1, 3)",
        lambda: mesh.PointSetBuilder2D.create(point_set2).create_points(
            numpy.zeros((1, 3))
        ),
        "2D builder given 3D points",
    )
    check_raises(
        ValueError,
        "points must have shape (n, 3), got ()",
        lambda: builder.create_points(object()),
        "not an array",
    )
    check_raises(
        ValueError,
        "points must have shape (n, 3), got ()",
        lambda: builder.create_points(None),
        "None",
    )
    check_raises(
        TypeError,
        "incompatible function arguments",
        lambda: builder.create_points([[0.0, 1.0, 2.0], [1.0]]),
        "ragged list",
    )
    check(point_set.nb_vertices() == 2, "errors do not create points")


def test_points_keyword_argument():
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    check(builder.create_points(points=random_points(2, 3)) == 0, "keyword points")


# Elements


def square_surface(builder_class=mesh.TriangulatedSurfaceBuilder3D,
                   surface_class=mesh.TriangulatedSurface3D, dimension=3):
    surface = surface_class.create()
    builder = builder_class.create(surface)
    points = numpy.zeros((5, dimension))
    points[1, 0] = 1
    points[2, :2] = 1
    points[3, 1] = 1
    points[4, 0] = 2
    builder.create_points(points)
    return surface, builder


def test_triangles_round_trip():
    for surface_class, builder_class, dimension in [
        (mesh.TriangulatedSurface2D, mesh.TriangulatedSurfaceBuilder2D, 2),
        (mesh.TriangulatedSurface3D, mesh.TriangulatedSurfaceBuilder3D, 3),
    ]:
        label = surface_class.__name__
        surface, builder = square_surface(builder_class, surface_class, dimension)
        check_array(surface.triangles_array(), numpy.uint32, (0, 3), label + " empty")
        triangles = numpy.array([[0, 1, 2], [0, 2, 3], [1, 4, 2]])
        check(builder.create_triangles(triangles) == 0, label + " first")
        check(surface.nb_polygons() == 3, label + " nb_polygons")
        result = surface.triangles_array()
        check_array(result, numpy.uint32, (3, 3), label)
        check(numpy.array_equal(result, triangles), label + " round trip")
        for triangle in range(3):
            for vertex in range(3):
                check(
                    surface.polygon_vertex(mesh.PolygonVertex(triangle, vertex))
                    == triangles[triangle, vertex],
                    label + " polygon_vertex agrees",
                )
        check(builder.create_triangles(triangles[:1]) == 3, label + " second first")
        check(
            numpy.array_equal(surface.triangles_array()[3:], triangles[:1]),
            label + " appended",
        )
        check(
            builder.create_triangles(numpy.empty((0, 3), dtype=numpy.int32)) == 4,
            label + " nothing created",
        )
        check(surface.nb_polygons() == 4, label + " nb_polygons after empty")
        # A triangle created one by one comes back too
        builder.create_triangle([4, 3, 0])
        check(
            list(surface.triangles_array()[4]) == [4, 3, 0],
            label + " create_triangle then array",
        )


def test_triangles_copy_and_dtypes():
    for dtype in [numpy.int8, numpy.int16, numpy.int32, numpy.int64,
                  numpy.uint8, numpy.uint16, numpy.uint32, numpy.uint64]:
        surface, builder = square_surface()
        triangles = numpy.array([[0, 1, 2], [2, 3, 0]], dtype=dtype)
        builder.create_triangles(triangles)
        result = surface.triangles_array()
        check(
            numpy.array_equal(result, triangles.astype(numpy.uint32)),
            f"triangles of dtype {dtype.__name__}",
        )
        result[:] = 0
        check(
            list(surface.triangles_array()[0]) == [0, 1, 2],
            "triangles array is a copy",
        )
    surface, builder = square_surface()
    builder.create_triangles([[0, 1, 2]])
    check(surface.nb_polygons() == 1, "list of lists")
    builder.create_triangles(numpy.array([[0, 1, 2], [1, 2, 3]])[::2])
    check(surface.nb_polygons() == 2, "non contiguous array")


def test_triangles_errors():
    surface, builder = square_surface()
    builder.create_triangles([[0, 1, 2]])
    before = surface.triangles_array()
    nb_vertices = surface.nb_vertices()
    check_raises(
        IndexError,
        "[create_triangles] vertex 5 out of range [0, 5) at row 2, column 2 of triangles",
        lambda: builder.create_triangles([[0, 1, 2], [3, 4, 0], [0, 1, 5]]),
        "vertex too large",
    )
    check_raises(
        IndexError,
        "vertex -1 out of range [0, 5) at row 0, column 2",
        lambda: builder.create_triangles(numpy.array([[0, 1, -1]])),
        "negative vertex",
    )
    check_raises(
        IndexError,
        "vertex 4294967295 out of range",
        lambda: builder.create_triangles(numpy.array([[0, 1, 4294967295]])),
        "NO_ID",
    )
    check_raises(
        IndexError,
        "vertex 18446744073709551615 out of range",
        lambda: builder.create_triangles(
            numpy.array([[0, 1, 2 ** 64 - 1]], dtype=numpy.uint64)
        ),
        "huge uint64",
    )
    check_raises(
        IndexError,
        "vertex -9223372036854775808 out of range",
        lambda: builder.create_triangles(
            numpy.array([[0, 1, -(2 ** 63)]], dtype=numpy.int64)
        ),
        "int64 minimum",
    )
    for bad, shape in [
        (numpy.zeros((2, 4), dtype=int), "(2, 4)"),
        (numpy.zeros((2, 2), dtype=int), "(2, 2)"),
        (numpy.zeros(3, dtype=int), "(3,)"),
        (numpy.zeros((1, 3, 1), dtype=int), "(1, 3, 1)"),
    ]:
        check_raises(
            ValueError,
            f"[create_triangles] triangles must have shape (n, 3), got {shape}",
            lambda bad=bad: builder.create_triangles(bad),
            f"shape {shape}",
        )
    for bad in [
        numpy.zeros((1, 3), dtype=numpy.float64),
        numpy.zeros((1, 3), dtype=numpy.float32),
        numpy.zeros((1, 3), dtype=bool),
        numpy.zeros((1, 3), dtype=object),
        numpy.zeros((1, 3), dtype=complex),
    ]:
        check_raises(
            ValueError,
            "triangles must be an array of integers, got dtype",
            lambda bad=bad: builder.create_triangles(bad),
            f"dtype {bad.dtype}",
        )
    check(surface.nb_vertices() == nb_vertices, "vertices unchanged")
    check(
        numpy.array_equal(surface.triangles_array(), before),
        "a failing create_triangles does not create any triangle",
    )
    check(surface.nb_polygons() == 1, "no triangle created by errors")


def test_triangles_atomic_with_late_invalid_row():
    surface, builder = square_surface()
    rows = numpy.zeros((1000, 3), dtype=numpy.int64)
    rows[:, 1] = 1
    rows[:, 2] = 2
    rows[-1, 2] = 99
    check_raises(
        IndexError, "at row 999, column 2 of triangles", lambda: builder.create_triangles(rows),
        "late invalid row",
    )
    check(surface.nb_polygons() == 0, "no row applied")


def test_polygons():
    for surface_class, builder_class, dimension in [
        (mesh.PolygonalSurface2D, mesh.PolygonalSurfaceBuilder2D, 2),
        (mesh.PolygonalSurface3D, mesh.PolygonalSurfaceBuilder3D, 3),
    ]:
        label = surface_class.__name__
        surface, builder = square_surface(builder_class, surface_class, dimension)
        vertices, offsets = surface.polygons_array()
        check_array(vertices, numpy.uint32, (0,), label + " empty vertices")
        check_array(offsets, numpy.uint64, (1,), label + " empty offsets")
        check(offsets[0] == 0, label + " empty offsets value")
        flat = numpy.array([0, 1, 2, 3, 1, 4, 2])
        limits = numpy.array([0, 4, 7])
        check(builder.create_polygons(flat, limits) == 0, label + " first")
        check(surface.nb_polygons() == 2, label + " nb_polygons")
        vertices, offsets = surface.polygons_array()
        check_array(vertices, numpy.uint32, (7,), label + " vertices")
        check_array(offsets, numpy.uint64, (3,), label + " offsets")
        check(list(vertices) == list(flat), label + " vertices values")
        check(list(offsets) == [0, 4, 7], label + " offsets values")
        check(surface.nb_polygon_vertices(0) == 4, label + " quad")
        check(surface.nb_polygon_vertices(1) == 3, label + " triangle")
        # Appending a triangle and a pentagon
        check(
            builder.create_polygons(
                numpy.array([0, 1, 2, 0, 1, 2, 3, 4], dtype=numpy.uint8),
                numpy.array([0, 3, 8], dtype=numpy.int16),
            )
            == 2,
            label + " second first",
        )
        vertices, offsets = surface.polygons_array()
        check(list(offsets) == [0, 4, 7, 10, 15], label + " appended offsets")
        check(
            list(vertices[offsets[3]:offsets[4]]) == [0, 1, 2, 3, 4],
            label + " appended pentagon",
        )
        # Nothing is created without polygons
        check(
            builder.create_polygons(numpy.empty(0, dtype=int), [0]) == 4,
            label + " empty first",
        )
        check(surface.nb_polygons() == 4, label + " empty creation")
        # Round trip into another surface
        other, other_builder = square_surface(builder_class, surface_class, dimension)
        other_builder.create_polygons(vertices, offsets)
        other_vertices, other_offsets = other.polygons_array()
        check(
            numpy.array_equal(vertices, other_vertices)
            and numpy.array_equal(offsets, other_offsets),
            label + " round trip",
        )
        # Copies
        vertices[:] = 0
        check(
            list(surface.polygons_array()[0][:4]) == [0, 1, 2, 3],
            label + " copy semantics",
        )


def test_empty_inputs():
    # An empty array of any dtype and shape creates nothing and returns the
    # index the first element would have
    for empty in [[], numpy.empty(0), numpy.empty((0, 3)), numpy.empty((0, 7), dtype=bool),
                  numpy.empty(0, dtype=object), numpy.empty((0, 3), dtype=numpy.float32)]:
        point_set = mesh.PointSet3D.create()
        builder = mesh.PointSetBuilder3D.create(point_set)
        builder.create_points(random_points(2, 3))
        check(builder.create_points(empty) == 2, "empty points")
        check(point_set.nb_vertices() == 2, "empty points create nothing")
        surface, surface_builder = square_surface()
        surface_builder.create_triangles([[0, 1, 2]])
        check(surface_builder.create_triangles(empty) == 1, "empty triangles")
        check(surface.nb_polygons() == 1, "empty triangles create nothing")
        solid = mesh.TetrahedralSolid3D.create()
        solid_builder = mesh.TetrahedralSolidBuilder3D.create(solid)
        check(solid_builder.create_tetrahedra(empty) == 0, "empty tetrahedra")
        curve = mesh.EdgedCurve3D.create()
        curve_builder = mesh.EdgedCurveBuilder3D.create(curve)
        check(curve_builder.create_edges_with_vertices(empty) == 0, "empty edges")
        polygonal, polygonal_builder = square_surface(
            mesh.PolygonalSurfaceBuilder3D, mesh.PolygonalSurface3D, 3
        )
        polygonal_builder.create_polygons([0, 1, 2], [0, 3])
        check(polygonal_builder.create_polygons(empty, [0]) == 1, "empty polygons")
        check(polygonal.nb_polygons() == 1, "empty polygons create nothing")
    # Offsets are still checked
    check_raises(
        ValueError,
        "offsets must start at 0",
        lambda: polygonal_builder.create_polygons([], [1]),
        "empty vertices with bad offsets",
    )


def test_empty_columns_are_not_empty_inputs():
    # No row is nothing to create, but no column is a wrong shape
    for shape in [(5, 0), (2, 0)]:
        empty = numpy.empty(shape)
        point_set = mesh.PointSet3D.create()
        builder = mesh.PointSetBuilder3D.create(point_set)
        check_raises(
            ValueError,
            f"points must have shape (n, 3), got {shape}",
            lambda: builder.create_points(empty),
            f"create_points of an array of shape {shape}",
        )
        check(point_set.nb_vertices() == 0, "no point created from a wrong shape")
        surface, surface_builder = square_surface()
        check_raises(
            ValueError,
            f"must have shape (n, 3), got {shape}",
            lambda: surface_builder.create_triangles(empty.astype(numpy.int32)),
            f"create_triangles of an array of shape {shape}",
        )
        check(surface.nb_polygons() == 0, "no triangle created from a wrong shape")
        curve = mesh.EdgedCurve3D.create()
        curve_builder = mesh.EdgedCurveBuilder3D.create(curve)
        check_raises(
            ValueError,
            f"must have shape (n, 2), got {shape}",
            lambda: curve_builder.create_edges_with_vertices(empty.astype(numpy.int32)),
            f"create_edges_with_vertices of an array of shape {shape}",
        )
    # No row, whatever the number of columns, stays accepted
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    check(builder.create_points(numpy.empty((0, 0))) == 0, "(0, 0) points")
    check(builder.create_points(numpy.empty((0, 5))) == 0, "(0, 5) points")


def test_builder_without_mesh():
    # A builder that does not know its mesh cannot check the vertices
    surface, builder = square_surface()
    del builder._mesh
    check_raises(
        RuntimeError,
        "[create_triangles] the builder does not know its mesh",
        lambda: builder.create_triangles([[0, 1, 2]]),
        "triangles",
    )
    check(surface.nb_polygons() == 0, "nothing created without mesh")
    polygonal, polygonal_builder = square_surface(
        mesh.PolygonalSurfaceBuilder3D, mesh.PolygonalSurface3D, 3
    )
    del polygonal_builder._mesh
    check_raises(
        RuntimeError,
        "[create_polygons] the builder does not know its mesh",
        lambda: polygonal_builder.create_polygons([0, 1, 2], [0, 3]),
        "polygons",
    )
    solid = mesh.TetrahedralSolid3D.create()
    solid_builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    del solid_builder._mesh
    check_raises(
        RuntimeError,
        "[create_tetrahedra] the builder does not know its mesh",
        lambda: solid_builder.create_tetrahedra([[0, 1, 2, 3]]),
        "tetrahedra",
    )
    curve = mesh.EdgedCurve3D.create()
    curve_builder = mesh.EdgedCurveBuilder3D.create(curve)
    del curve_builder._mesh
    check_raises(
        RuntimeError,
        "[create_edges_with_vertices] the builder does not know its mesh",
        lambda: curve_builder.create_edges_with_vertices([[0, 1]]),
        "edges",
    )
    # Points do not need the mesh
    check(curve_builder.create_points(random_points(2, 3)) == 0, "points without mesh")


def test_keyword_arguments():
    surface, builder = square_surface(
        mesh.PolygonalSurfaceBuilder3D, mesh.PolygonalSurface3D, 3
    )
    check(builder.create_polygons(vertices=[0, 1, 2], offsets=[0, 3]) == 0, "polygons")
    surface, builder = square_surface()
    check(builder.create_triangles(triangles=[[0, 1, 2]]) == 0, "triangles")
    solid = mesh.TetrahedralSolid3D.create()
    solid_builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    solid_builder.create_points(points=random_points(4, 3))
    check(solid_builder.create_tetrahedra(tetrahedra=[[0, 1, 2, 3]]) == 0, "tetrahedra")
    curve = mesh.EdgedCurve3D.create()
    curve_builder = mesh.EdgedCurveBuilder3D.create(curve)
    curve_builder.create_points(random_points(2, 3))
    check(curve_builder.create_edges_with_vertices(edges=[[0, 1]]) == 0, "edges")


def test_polygons_of_a_triangulated_surface():
    surface, builder = square_surface()
    builder.create_triangles([[0, 1, 2], [2, 3, 0]])
    vertices, offsets = surface.polygons_array()
    check(list(vertices) == [0, 1, 2, 2, 3, 0], "triangulated vertices")
    check(list(offsets) == [0, 3, 6], "triangulated offsets")
    check(
        surface.triangles_array().reshape(-1).tolist() == vertices.tolist(),
        "triangles and polygons agree",
    )


def test_polygons_errors():
    surface, builder = square_surface(
        mesh.PolygonalSurfaceBuilder3D, mesh.PolygonalSurface3D, 3
    )
    builder.create_polygons([0, 1, 2], [0, 3])
    before = surface.polygons_array()
    cases = [
        ([0, 1, 2], [0, 2], "end at the number of vertices (3), got 0 and 2"),
        ([0, 1, 2], [1, 3], "start at 0"),
        ([0, 1, 2], [0, 3, 3], "polygon 1 must have between 3 and"),
        ([0, 1, 2], [0, 4, 3], "polygon 1 must have between 3 and"),
        ([0, 1, 2, 3], [0, 2, 4], "polygon 0 must have between 3 and"),
        ([0, 1, 2, 3, 0, 1, 2], [0, 3, 6, 7], "polygon 2 must have"),
        ([0, 1, 2], [0], "end at the number of vertices (3), got 0 and 0"),
    ]
    for vertices, offsets, message in cases:
        check_raises(
            ValueError,
            message,
            lambda v=vertices, o=offsets: builder.create_polygons(v, o),
            f"offsets {offsets}",
        )
    check_raises(
        ValueError,
        "offsets must have at least one value",
        lambda: builder.create_polygons([], numpy.empty(0, dtype=int)),
        "empty offsets",
    )
    check_raises(
        ValueError,
        "vertices must have shape (n,), got (1, 3)",
        lambda: builder.create_polygons(numpy.zeros((1, 3), dtype=int), [0, 3]),
        "2D vertices",
    )
    check_raises(
        ValueError,
        "offsets must have shape (n,), got (1, 2)",
        lambda: builder.create_polygons([0, 1, 2], numpy.array([[0, 3]])),
        "2D offsets",
    )
    check_raises(
        ValueError,
        "offsets must be an array of integers, got dtype float64",
        lambda: builder.create_polygons([0, 1, 2], numpy.array([0.0, 3.0])),
        "float offsets",
    )
    check_raises(
        ValueError,
        "vertices must be an array of integers, got dtype float64",
        lambda: builder.create_polygons(numpy.array([0.0, 1.0, 2.0]), [0, 3]),
        "float vertices",
    )
    check_raises(
        ValueError,
        "value -1 at position 0 of offsets does not fit in the 64-bit unsigned",
        lambda: builder.create_polygons([0, 1, 2], numpy.array([-1, 3])),
        "negative offset",
    )
    check_raises(
        IndexError,
        "vertex 5 out of range [0, 5) at position 3 of vertices",
        lambda: builder.create_polygons([0, 1, 2, 5, 1, 2], [0, 3, 6]),
        "vertex out of range",
    )
    check_raises(
        IndexError,
        "vertex -2 out of range",
        lambda: builder.create_polygons([0, 1, -2], [0, 3]),
        "negative vertex",
    )
    check(surface.nb_polygons() == 1, "errors do not create polygons")
    after = surface.polygons_array()
    check(
        numpy.array_equal(before[0], after[0]) and numpy.array_equal(before[1], after[1]),
        "errors do not modify polygons",
    )
    # The triangulated surface builder has no create_polygons: triangles only
    triangulated_builder = mesh.TriangulatedSurfaceBuilder3D.create(
        mesh.TriangulatedSurface3D.create()
    )
    check(
        not hasattr(triangulated_builder, "create_polygons"),
        "create_polygons is only for polygonal surfaces",
    )


def test_tetrahedra():
    solid = mesh.TetrahedralSolid3D.create()
    builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    check_array(solid.tetrahedra_array(), numpy.uint32, (0, 4), "empty tetrahedra")
    points = numpy.array(
        [[0, 0, 0], [1, 0, 0], [0, 1, 0], [0, 0, 1], [1, 1, 1]], dtype=float
    )
    builder.create_points(points)
    tetrahedra = numpy.array([[0, 1, 2, 3], [1, 2, 3, 4]])
    check(builder.create_tetrahedra(tetrahedra) == 0, "tetrahedra first")
    check(solid.nb_polyhedra() == 2, "nb_polyhedra")
    result = solid.tetrahedra_array()
    check_array(result, numpy.uint32, (2, 4), "tetrahedra")
    check(numpy.array_equal(result, tetrahedra), "tetrahedra round trip")
    for t in range(2):
        for v in range(4):
            check(
                solid.polyhedron_vertex(mesh.PolyhedronVertex(t, v))
                == tetrahedra[t, v],
                "polyhedron_vertex agrees",
            )
    check(
        builder.create_tetrahedra(numpy.array([[4, 3, 2, 1]], dtype=numpy.uint16)) == 2,
        "tetrahedra second first",
    )
    check(list(solid.tetrahedra_array()[2]) == [4, 3, 2, 1], "appended")
    check(builder.create_tetrahedra(numpy.empty((0, 4), dtype=int)) == 3, "empty")
    result[:] = 0
    check(list(solid.tetrahedra_array()[0]) == [0, 1, 2, 3], "copy semantics")
    before = solid.tetrahedra_array()
    check_raises(
        IndexError,
        "[create_tetrahedra] vertex 5 out of range [0, 5) at row 1, column 3 of tetrahedra",
        lambda: builder.create_tetrahedra([[0, 1, 2, 3], [1, 2, 3, 5]]),
        "tetrahedra vertex",
    )
    check_raises(
        IndexError,
        "vertex -1 out of range",
        lambda: builder.create_tetrahedra([[0, 1, 2, -1]]),
        "tetrahedra negative",
    )
    check_raises(
        ValueError,
        "tetrahedra must have shape (n, 4), got (1, 3)",
        lambda: builder.create_tetrahedra([[0, 1, 2]]),
        "tetrahedra shape",
    )
    check_raises(
        ValueError,
        "tetrahedra must be an array of integers, got dtype float64",
        lambda: builder.create_tetrahedra(numpy.zeros((1, 4))),
        "tetrahedra dtype",
    )
    check(numpy.array_equal(solid.tetrahedra_array(), before), "tetrahedra atomic")


def test_edges():
    for curve_class, builder_class, dimension in [
        (mesh.EdgedCurve2D, mesh.EdgedCurveBuilder2D, 2),
        (mesh.EdgedCurve3D, mesh.EdgedCurveBuilder3D, 3),
        (mesh.Graph, mesh.GraphBuilder, None),
    ]:
        label = curve_class.__name__
        curve = curve_class.create()
        builder = builder_class.create(curve)
        if dimension is None:
            builder.create_vertices(4)
        else:
            builder.create_points(random_points(4, dimension))
        check_array(curve.edges_array(), numpy.uint32, (0, 2), label + " empty")
        edges = numpy.array([[0, 1], [1, 2], [3, 0]])
        check(builder.create_edges_with_vertices(edges) == 0, label + " first")
        check(curve.nb_edges() == 3, label + " nb_edges")
        result = curve.edges_array()
        check_array(result, numpy.uint32, (3, 2), label)
        check(numpy.array_equal(result, edges), label + " round trip")
        for e in range(3):
            for v in range(2):
                check(
                    curve.edge_vertex(mesh.EdgeVertex(e, v)) == edges[e, v],
                    label + " edge_vertex agrees",
                )
        check(
            builder.create_edges_with_vertices(numpy.array([[2, 3]], dtype=numpy.uint8))
            == 3,
            label + " second first",
        )
        check(
            builder.create_edges_with_vertices(numpy.empty((0, 2), dtype=int)) == 4,
            label + " empty",
        )
        check(curve.nb_edges() == 4, label + " edges after empty")
        result[:] = 0
        check(list(curve.edges_array()[0]) == [0, 1], label + " copy semantics")
        before = curve.edges_array()
        check_raises(
            IndexError,
            "[create_edges_with_vertices] vertex 4 out of range [0, 4) at row 1, column 1 of edges",
            lambda: builder.create_edges_with_vertices([[0, 1], [1, 4]]),
            label + " vertex",
        )
        check_raises(
            IndexError,
            "vertex -3 out of range",
            lambda: builder.create_edges_with_vertices([[-3, 1]]),
            label + " negative",
        )
        check_raises(
            ValueError,
            "edges must have shape (n, 2), got (2, 3)",
            lambda: builder.create_edges_with_vertices(numpy.zeros((2, 3), dtype=int)),
            label + " shape",
        )
        check_raises(
            ValueError,
            "edges must be an array of integers, got dtype float32",
            lambda: builder.create_edges_with_vertices(numpy.zeros((1, 2), dtype=numpy.float32)),
            label + " dtype",
        )
        check(numpy.array_equal(curve.edges_array(), before), label + " atomic")
    # Edges created without vertices have no endpoint
    curve = mesh.EdgedCurve3D.create()
    builder = mesh.EdgedCurveBuilder3D.create(curve)
    builder.create_edges(2)
    check(
        curve.edges_array().tolist() == [[basic.NO_ID] * 2] * 2,
        "edges without vertex",
    )


def test_surface_points_and_meshes_of_other_types():
    surface, builder = square_surface()
    check(
        numpy.array_equal(surface.points_array()[:, 0], [0, 1, 1, 0, 2]),
        "surface points",
    )
    as_surface = mesh.SurfaceMesh3D
    check(
        isinstance(surface, as_surface)
        and numpy.array_equal(as_surface.points_array(surface), surface.points_array()),
        "points_array is defined on SurfaceMesh",
    )
    # The generic builder of a surface mesh creates points too
    base_builder = mesh.SurfaceMeshBuilder3D.create(surface)
    check(base_builder.create_points(random_points(2, 3)) == 5, "surface mesh builder")
    check(surface.points_array().shape == (7, 3), "surface points after creation")


# Performance


def test_performance():
    points = random_points(NB_PERFORMANCE_POINTS, 3)
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    start = time.perf_counter()
    builder.create_points(points)
    creation = time.perf_counter() - start
    start = time.perf_counter()
    result = point_set.points_array()
    reading = time.perf_counter() - start
    check(numpy.array_equal(result, points), "performance round trip")

    surface = mesh.TriangulatedSurface3D.create()
    surface_builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    surface_builder.create_points(points)
    triangles = numpy.random.default_rng(1).integers(
        0, NB_PERFORMANCE_POINTS, (NB_PERFORMANCE_POINTS, 3)
    )
    start = time.perf_counter()
    surface_builder.create_triangles(triangles)
    triangles_creation = time.perf_counter() - start
    start = time.perf_counter()
    read_triangles = surface.triangles_array()
    triangles_reading = time.perf_counter() - start
    check(numpy.array_equal(read_triangles, triangles), "triangles performance round trip")

    # The per element loop that the arrays replace
    loop_point_set = mesh.PointSet3D.create()
    loop_builder = mesh.PointSetBuilder3D.create(loop_point_set)
    start = time.perf_counter()
    for row in points[:20000]:
        loop_builder.create_point(geom.Point3D(row.tolist()))
    loop = (time.perf_counter() - start) * (NB_PERFORMANCE_POINTS / 20000)

    print(
        f"[Performance] {NB_PERFORMANCE_POINTS} points: create_points "
        f"{creation * 1000:.1f} ms, points_array {reading * 1000:.1f} ms, "
        f"create_triangles {triangles_creation * 1000:.1f} ms, "
        f"triangles_array {triangles_reading * 1000:.1f} ms; "
        f"create_point loop (extrapolated) {loop * 1000:.1f} ms"
    )
    for name, duration in [
        ("create_points", creation),
        ("points_array", reading),
        ("create_triangles", triangles_creation),
        ("triangles_array", triangles_reading),
    ]:
        check(
            duration < PERFORMANCE_BOUND_SECONDS,
            f"{name} of {NB_PERFORMANCE_POINTS} elements took {duration:.2f} s",
        )


# GIL


class HugeSwitchInterval:
    def __enter__(self):
        self.previous = sys.getswitchinterval()
        sys.setswitchinterval(1000)

    def __exit__(self, *args):
        sys.setswitchinterval(self.previous)


def main_runs_during(work):
    """Run work in a thread with a huge switch interval: the main thread runs
    during the call only if the call released the GIL."""
    ready = threading.Event()
    finished = threading.Event()
    box = {}

    def target():
        ready.set()
        try:
            work()
        except BaseException as error:
            box["error"] = error
        finished.set()

    thread = threading.Thread(target=target)
    thread.start()
    check(ready.wait(TIMEOUT), "worker did not start")
    main_ran = not finished.is_set()
    thread.join(TIMEOUT)
    if "error" in box:
        raise box["error"]
    return main_ran


def assert_gil_released(name, work):
    with HugeSwitchInterval():
        for _ in range(3):
            if main_runs_during(work):
                return
    raise ValueError(f"[Test] {name} kept the GIL during the whole call")


def test_gil_released():
    nb_points = 3000000
    points = random_points(nb_points, 3)
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    assert_gil_released("create_points", lambda: builder.create_points(points))
    assert_gil_released("points_array", lambda: point_set.points_array())
    surface = mesh.TriangulatedSurface3D.create()
    surface_builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    surface_builder.create_points(points[:1000])
    triangles = numpy.random.default_rng(2).integers(0, 1000, (2000000, 3))
    assert_gil_released(
        "create_triangles", lambda: surface_builder.create_triangles(triangles)
    )
    assert_gil_released("triangles_array", lambda: surface.triangles_array())
    assert_gil_released("polygons_array", lambda: surface.polygons_array())


if __name__ == "__main__":
    mesh.OpenGeodeMeshLibrary.initialize()
    test_points_round_trip()
    test_solid_points()
    test_points_copy_semantics()
    test_points_input_dtypes()
    test_points_errors()
    test_points_keyword_argument()
    test_triangles_round_trip()
    test_triangles_copy_and_dtypes()
    test_triangles_errors()
    test_triangles_atomic_with_late_invalid_row()
    test_polygons()
    test_empty_inputs()
    test_empty_columns_are_not_empty_inputs()
    test_builder_without_mesh()
    test_keyword_arguments()
    test_polygons_of_a_triangulated_surface()
    test_polygons_errors()
    test_tetrahedra()
    test_edges()
    test_surface_points_and_meshes_of_other_types()
    test_performance()
    test_gil_released()
