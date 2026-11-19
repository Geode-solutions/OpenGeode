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


import gc
import weakref

import geode_test_env

import opengeode_py_basic
import opengeode_py_geometry as geom
import opengeode_py_image as image
import opengeode_py_mesh as mesh


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def allocate_junk():
    gc.collect()
    return [geom.Point3D([1e6 + i, -1e6 - i, 42.0]) for i in range(10000)]


def test_builder_keeps_mesh_alive():
    surface = mesh.TriangulatedSurface3D.create()
    surface_ref = weakref.ref(surface)
    builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    del surface
    gc.collect()
    check(surface_ref() is not None, "Builder should keep its mesh alive")
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_triangle([0, 1, 2])
    check(surface_ref().nb_polygons() == 1, "Surface should have 1 triangle")
    surface_ref().enable_edges()
    edges_builder = builder.edges_builder()
    del builder
    gc.collect()
    check(surface_ref() is not None, "Edges builder should keep its mesh alive")
    edges_builder.delete_isolated_edges()
    crs_builder = mesh.TriangulatedSurfaceBuilder3D.create(
        surface_ref()
    ).main_coordinate_reference_system_manager_builder()
    del edges_builder
    gc.collect()
    check(surface_ref() is not None, "CRS builder should keep its mesh alive")
    del crs_builder
    gc.collect()
    check(surface_ref() is None, "Surface should be released with its builders")


def test_solid_sub_builders_keep_mesh_alive():
    solid = mesh.TetrahedralSolid3D.create()
    solid_ref = weakref.ref(solid)
    solid.enable_edges()
    solid.enable_facets()
    edges_builder = mesh.TetrahedralSolidBuilder3D.create(solid).edges_builder()
    facets_builder = mesh.TetrahedralSolidBuilder3D.create(solid).facets_builder()
    del solid
    gc.collect()
    check(solid_ref() is not None, "Solid sub-builders should keep the mesh alive")
    edges_builder.delete_isolated_edges()
    facets_builder.delete_isolated_facets()


def create_curve():
    curve = mesh.EdgedCurve3D.create()
    builder = mesh.EdgedCurveBuilder3D.create(curve)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_edge_with_vertices(0, 1)
    return curve


def create_surface():
    surface = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_triangle([0, 1, 2])
    return surface


def create_solid():
    solid = mesh.TetrahedralSolid3D.create()
    builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_point(geom.Point3D([0, 0, 1]))
    builder.create_tetrahedron([0, 1, 2, 3])
    return solid


def test_mesh_elements_outlive_mesh():
    segment = create_curve().segment(0)
    triangle = create_surface().triangle(0)
    tetrahedron = create_solid().tetrahedron(0)
    solid_triangle = create_solid().triangle(mesh.PolyhedronFacet(0, 0))
    junk = allocate_junk()
    check(segment.length() == 1, "Mesh segment length should be 1")
    check(
        geom.triangle_area3D(triangle) == 0.5, "Mesh triangle area should be 0.5"
    )
    check(
        abs(geom.tetrahedron_volume(tetrahedron) - 1.0 / 6) < 1e-12,
        "Mesh tetrahedron volume should be 1/6",
    )
    check(
        geom.triangle_area3D(solid_triangle) > 0,
        "Solid triangle area should be positive",
    )


def test_texture_manager_keeps_mesh_alive():
    surface = create_surface()
    surface_ref = weakref.ref(surface)
    manager = surface.texture_manager()
    del surface
    gc.collect()
    check(surface_ref() is not None, "Texture manager should keep its mesh alive")
    manager.find_or_create_texture("texture")
    check(manager.texture_exists("texture"), "Texture should exist")


def test_crs_builder_returns_reference():
    curve = create_curve()
    curve_ref = weakref.ref(curve)
    crs = (
        mesh.EdgedCurveBuilder3D.create(curve)
        .main_coordinate_reference_system_manager_builder()
        .active_coordinate_reference_system()
    )
    crs.set_point(0, geom.Point3D([5, 5, 5]))
    check(
        curve.point(0) == geom.Point3D([5, 5, 5]),
        "Active CRS from builder should modify the mesh",
    )
    crs_name = (
        curve.main_coordinate_reference_system_manager()
        .active_coordinate_reference_system_name()
    )
    named_crs = (
        mesh.EdgedCurveBuilder3D.create(curve)
        .main_coordinate_reference_system_manager_builder()
        .coordinate_reference_system(crs_name)
    )
    named_crs.set_point(1, geom.Point3D([6, 6, 6]))
    check(
        curve.point(1) == geom.Point3D([6, 6, 6]),
        "Named CRS from builder should modify the mesh",
    )
    del curve
    gc.collect()
    check(curve_ref() is not None, "CRS from builder should keep its mesh alive")
    check(crs.point(0) == geom.Point3D([5, 5, 5]), "Wrong CRS point")


def test_texture_image_reference():
    surface = create_surface()
    surface_ref = weakref.ref(surface)
    texture = surface.texture_manager().find_or_create_texture("texture")
    texture.set_image(image.RasterImage2D([2, 2]))
    raster = texture.image()
    del surface
    del texture
    gc.collect()
    check(surface_ref() is not None, "Texture image should keep its mesh alive")
    check(raster.nb_cells() == 4, "Texture image should have 4 cells")


def coordinates(geometry, dimension):
    return [[point.value(axis) for axis in range(dimension)] for point in geometry.vertices()]


def snapshot_case(dimension, kind):
    point = getattr(geom, "Point%dD" % dimension)
    if kind == "segment":
        target = getattr(mesh, "EdgedCurve%dD" % dimension).create()
        builder = getattr(mesh, "EdgedCurveBuilder%dD" % dimension).create(target)
    else:
        target = getattr(mesh, "TriangulatedSurface%dD" % dimension).create()
        builder = getattr(mesh, "TriangulatedSurfaceBuilder%dD" % dimension).create(target)
    for values in [[0, 0, 0], [1, 0, 0], [0, 1, 0]]:
        builder.create_point(point(values[:dimension]))
    if kind == "segment":
        builder.create_edge_with_vertices(1, 2)
        return target, builder, point, lambda mesh_object: mesh_object.segment(0)
    builder.create_triangle([0, 1, 2])
    return target, builder, point, lambda mesh_object: mesh_object.triangle(0)


def solid_case(kind):
    target = create_solid()
    builder = mesh.TetrahedralSolidBuilder3D.create(target)
    if kind == "tetrahedron":
        return target, builder, geom.Point3D, lambda solid: solid.tetrahedron(0)
    return (
        target,
        builder,
        geom.Point3D,
        lambda solid: solid.triangle(mesh.PolyhedronFacet(0, 1)),
    )


def test_mesh_geometry_is_a_snapshot():
    # Geometry returned by a mesh is built from copies of its points: later
    # changes of the mesh (moving, adding or removing vertices, deleting the
    # mesh) do not modify it
    factories = [
        lambda dimension=dimension, kind=kind: snapshot_case(dimension, kind)
        for dimension in [2, 3]
        for kind in ["segment", "triangle"]
    ] + [lambda: solid_case("tetrahedron"), lambda: solid_case("triangle")]
    for factory in factories:
        target, builder, point, accessor = factory()
        geometry = accessor(target)
        label = type(geometry).__name__ + " of " + type(target).__name__
        dimension = int(point.__name__[len("Point")])
        expected = coordinates(geometry, dimension)
        check(expected == coordinates(accessor(target), dimension), label + " coordinates")
        check(
            all(isinstance(vertex, point) for vertex in geometry.vertices()),
            label + " vertices are points",
        )
        # Moving a vertex
        for vertex in range(target.nb_vertices()):
            builder.set_point(vertex, point([9.0] * dimension))
        check(coordinates(geometry, dimension) == expected, label + " after set_point")
        check(
            coordinates(accessor(target), dimension) == [[9.0] * dimension] * len(expected),
            label + " new geometry after set_point",
        )
        # Adding many vertices reallocates the coordinates of the mesh
        for _ in range(100000):
            builder.create_point(point([7.0] * dimension))
        junk = allocate_junk()
        check(coordinates(geometry, dimension) == expected, label + " after create_point")
        for elements in ["edges", "polygons", "polyhedra"]:
            if hasattr(builder, "delete_" + elements):
                getattr(builder, "delete_" + elements)(
                    [True] * getattr(target, "nb_" + elements)()
                )
        builder.delete_vertices([True] * target.nb_vertices())
        check(target.nb_vertices() == 0, label + " vertices deleted")
        junk = allocate_junk()
        check(coordinates(geometry, dimension) == expected, label + " after delete_vertices")
        # Deleting the mesh
        target_ref = weakref.ref(target)
        del target, builder
        gc.collect()
        check(target_ref() is None, label + " does not keep the mesh alive")
        junk = allocate_junk()
        check(coordinates(geometry, dimension) == expected, label + " after deleting the mesh")


if __name__ == "__main__":
    mesh.OpenGeodeMeshLibrary.initialize()
    test_builder_keeps_mesh_alive()
    test_solid_sub_builders_keep_mesh_alive()
    test_mesh_elements_outlive_mesh()
    test_mesh_geometry_is_a_snapshot()
    test_texture_manager_keeps_mesh_alive()
    test_crs_builder_returns_reference()
    test_texture_image_reference()
