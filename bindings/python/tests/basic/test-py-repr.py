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


"""repr is unambiguous: the type and the value for the value types (eval-able
when possible), the type, the id and the main counts for meshes and models.
str keeps returning the readable string() of the object."""

import re

import geode_test_env

import opengeode
import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_image as image
import opengeode_py_mesh as mesh
import opengeode_py_model as model

NAMESPACE = {}
for module in (basic, geometry, image, mesh, model):
    NAMESPACE.update(vars(module))


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_repr(value, expected):
    check(repr(value) == expected, f"repr is {repr(value)}, expected {expected}")


def check_roundtrip(value):
    """The repr of a value type rebuilds an equal object"""
    rebuilt = eval(repr(value), NAMESPACE)
    check(type(rebuilt) is type(value), f"{repr(value)} rebuilds another type")
    if type(value).__eq__ is not object.__eq__:
        check(rebuilt == value, f"{repr(value)} does not rebuild an equal object")
    check(repr(rebuilt) == repr(value), f"{repr(value)} is not stable")


def check_str_unchanged(value):
    check(str(value) == value.string(), f"str of {repr(value)} is string()")
    check(str(value) != repr(value), f"str and repr of {repr(value)} are the same")


def test_points_and_vectors():
    check_repr(geometry.Point3D([1.0, 2.0, 3.0]), "Point3D([1.0, 2.0, 3.0])")
    check_repr(geometry.Point2D(), "Point2D([0.0, 0.0])")
    check_repr(geometry.Point1D([4]), "Point1D([4.0])")
    # a double that is not representable with a few digits is not truncated
    check_repr(geometry.Point2D([0.1, 1e-20]), "Point2D([0.1, 1e-20])")
    third = geometry.Point2D([1 / 3, 2 / 3])
    check_repr(third, "Point2D([0.3333333333333333, 0.6666666666666666])")
    check_roundtrip(third)
    check(
        eval(repr(geometry.Point3D([1.1, -2.2, 3.3e300])), NAMESPACE).value(2) == 3.3e300,
        "exact coordinates",
    )
    check_repr(geometry.Vector3D([1, 2, 3]), "Vector3D([1.0, 2.0, 3.0])")
    check_repr(geometry.Vector2D(), "Vector2D([0.0, 0.0])")
    check_repr(geometry.Vector1D([-1.5]), "Vector1D([-1.5])")
    check_roundtrip(geometry.Vector3D([0.1, 0.2, 0.3]))
    for point in (geometry.Point3D([1, 2, 3]), geometry.Point2D([1, 2])):
        check_str_unchanged(point)
        check_roundtrip(point)
    # a Vector is not mistaken for a Point
    check(not repr(geometry.Vector3D()).startswith("Point"), "Vector repr")


def test_geometry_objects():
    box = geometry.BoundingBox3D()
    box.add_point(geometry.Point3D([0, 1, 2]))
    box.add_point(geometry.Point3D([3, 4, 5]))
    check_repr(
        box,
        "BoundingBox3D(min=Point3D([0.0, 1.0, 2.0]), max=Point3D([3.0, 4.0, 5.0]))",
    )
    box2 = geometry.BoundingBox2D()
    box2.add_point(geometry.Point2D([-1, 0.5]))
    check_repr(
        box2, "BoundingBox2D(min=Point2D([-1.0, 0.5]), max=Point2D([-1.0, 0.5]))"
    )
    check_str_unchanged(box)
    frame = geometry.Frame2D()
    check_repr(frame, "Frame2D([Vector2D([1.0, 0.0]), Vector2D([0.0, 1.0])])")
    check_roundtrip(frame)
    system = geometry.CoordinateSystem3D()
    check_repr(
        system,
        "CoordinateSystem3D(directions=[Vector3D([1.0, 0.0, 0.0]), "
        "Vector3D([0.0, 1.0, 0.0]), Vector3D([0.0, 0.0, 1.0])], "
        "origin=Point3D([0.0, 0.0, 0.0]))",
    )
    rebuilt = eval(repr(system), NAMESPACE)
    check(repr(rebuilt) == repr(system), "CoordinateSystem3D rebuilds")
    check_repr(
        geometry.CoordinateSystem2D(),
        "CoordinateSystem2D(directions=[Vector2D([1.0, 0.0]), Vector2D([0.0, 1.0])], "
        "origin=Point2D([0.0, 0.0]))",
    )
    check_repr(
        geometry.Frame3D(),
        "Frame3D([Vector3D([1.0, 0.0, 0.0]), Vector3D([0.0, 1.0, 0.0]), "
        "Vector3D([0.0, 0.0, 1.0])])",
    )
    points = [geometry.Point3D([0, 0, 0]), geometry.Point3D([1, 0, 0]), geometry.Point3D([0, 1, 0])]
    triangle = geometry.Triangle3D(*points)
    check_repr(
        triangle,
        "Triangle3D(Point3D([0.0, 0.0, 0.0]), Point3D([1.0, 0.0, 0.0]), "
        "Point3D([0.0, 1.0, 0.0]))",
    )
    check(repr(eval(repr(triangle), NAMESPACE)) == repr(triangle), "Triangle3D rebuilds")
    points2 = [geometry.Point2D([0, 0]), geometry.Point2D([2, 0]), geometry.Point2D([0, 2])]
    check_repr(
        geometry.Triangle2D(*points2),
        "Triangle2D(Point2D([0.0, 0.0]), Point2D([2.0, 0.0]), Point2D([0.0, 2.0]))",
    )
    check_str_unchanged(triangle)


def test_basic_values():
    identifier = basic.uuid()
    check_repr(identifier, f"uuid('{identifier.string()}')")
    check_roundtrip(identifier)
    check(str(identifier) == identifier.string(), "str of uuid")
    check_repr(basic.Percentage(0.5), "Percentage(0.5)")
    check_repr(basic.Percentage(1), "Percentage(1.0)")
    check(str(basic.Percentage(0.25)) == basic.Percentage(0.25).string(), "str of Percentage")
    check_repr(image.GreyscaleColor(128), "GreyscaleColor(128)")
    check_repr(image.GreyscaleColor(), "GreyscaleColor(0)")
    check_repr(image.RGBColor(3, 254, 68), "RGBColor(3, 254, 68)")
    check_repr(image.RGBColor(), "RGBColor(0, 0, 0)")
    check_roundtrip(image.RGBColor(1, 2, 3))
    check_roundtrip(image.GreyscaleColor(7))
    check_str_unchanged(image.RGBColor(3, 254, 68))
    check_str_unchanged(image.GreyscaleColor(9))


def test_mesh_elements():
    check_repr(mesh.PolygonVertex(1, 2), "PolygonVertex(polygon_id=1, vertex_id=2)")
    check_repr(mesh.PolygonEdge(3, 0), "PolygonEdge(polygon_id=3, edge_id=0)")
    check_repr(mesh.EdgeVertex(4, 1), "EdgeVertex(edge_id=4, vertex_id=1)")
    check_repr(mesh.PolyhedronVertex(5, 6), "PolyhedronVertex(polyhedron_id=5, vertex_id=6)")
    facet = mesh.PolyhedronFacet(7, 2)
    check_repr(facet, "PolyhedronFacet(polyhedron_id=7, facet_id=2)")
    check_repr(
        mesh.PolyhedronFacetVertex(facet, 3),
        "PolyhedronFacetVertex(facet=PolyhedronFacet(polyhedron_id=7, facet_id=2), vertex_id=3)",
    )
    check_repr(
        mesh.PolyhedronFacetEdge(facet, 1),
        "PolyhedronFacetEdge(facet=PolyhedronFacet(polyhedron_id=7, facet_id=2), edge_id=1)",
    )
    identifier = basic.uuid()
    check_repr(
        mesh.MeshElement(identifier, 8),
        f"MeshElement(mesh_id=uuid('{identifier.string()}'), element_id=8)",
    )
    for value in (
        mesh.PolygonVertex(1, 2),
        mesh.PolygonEdge(3, 0),
        mesh.EdgeVertex(4, 1),
        mesh.PolyhedronVertex(5, 6),
        facet,
        mesh.PolyhedronFacetVertex(facet, 3),
        mesh.PolyhedronFacetEdge(facet, 1),
        mesh.MeshElement(identifier, 8),
    ):
        check_roundtrip(value)
        check_str_unchanged(value)
    # invalid indices are shown as numbers
    check(
        re.fullmatch(r"PolygonVertex\(polygon_id=\d+, vertex_id=\d+\)", repr(mesh.PolygonVertex())),
        "default PolygonVertex",
    )
    check_repr(mesh.MeshImpl("OpenGeodeTriangulatedSurface3D"), "MeshImpl('OpenGeodeTriangulatedSurface3D')")
    check_repr(mesh.MeshType("TriangulatedSurface3D"), "MeshType('TriangulatedSurface3D')")
    check_roundtrip(mesh.MeshType("TriangulatedSurface3D"))
    check(str(mesh.MeshType("PointSet3D")) == "PointSet3D", "str of MeshType")
    check_repr(mesh.MeshImpl(""), "MeshImpl('')")


def test_model_identifiers():
    identifier = basic.uuid()
    corner = model.ComponentType("Corner")
    check_repr(corner, "ComponentType('Corner')")
    check_roundtrip(corner)
    check(str(corner) == "Corner", "str of ComponentType")
    component = model.ComponentID(corner, identifier)
    check_repr(component, f"ComponentID(ComponentType('Corner'), uuid('{identifier.string()}'))")
    check_roundtrip(component)
    check_str_unchanged(component)
    check_repr(
        model.ComponentMeshVertex(component, 4),
        f"ComponentMeshVertex(component_id={repr(component)}, vertex_id=4)",
    )
    check_repr(
        model.ComponentMeshElement(component, 5),
        f"ComponentMeshElement(component_id={repr(component)}, element_id=5)",
    )
    check_roundtrip(model.ComponentMeshVertex(component, 4))
    check_roundtrip(model.ComponentMeshElement(component, 5))


def test_meshes():
    def check_mesh(created, name, counts):
        match = re.fullmatch(r"(\w+)\(id='([0-9a-f-]{36})'((?:, \w+=\d+)*)\)", repr(created))
        check(match, f"repr of a {name} is {repr(created)}")
        check(match.group(1) == name, f"the repr of a {name} names {match.group(1)}")
        check(match.group(2) == created.id().string(), f"id in the repr of a {name}")
        check(match.group(3) == counts, f"counts in the repr of a {name}: {match.group(3)}")

    # the name of the most derived class, whatever the type it is seen as
    surface = mesh.TriangulatedSurface3D.create()
    check_mesh(surface, "TriangulatedSurface3D", ", nb_vertices=0, nb_polygons=0")
    builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    for coordinates in ([0, 0, 0], [1, 0, 0], [0, 1, 0]):
        builder.create_point(geometry.Point3D(coordinates))
    builder.create_triangle([0, 1, 2])
    check_repr(
        surface,
        f"TriangulatedSurface3D(id='{surface.id().string()}', nb_vertices=3, nb_polygons=1)",
    )
    # a name is shown when there is one
    surface_builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    surface_builder.set_name("my surface")
    check_repr(
        surface,
        f"TriangulatedSurface3D(id='{surface.id().string()}', name='my surface', "
        "nb_vertices=3, nb_polygons=1)",
    )
    surface_builder.set_name("it's")
    check("name=\"it's\"" in repr(surface), "name is quoted")
    # no data dump
    check(len(repr(surface)) < 200, "repr of a mesh is short")
    clone = surface.clone()
    check(clone.id() == surface.id() and repr(clone) == repr(surface), "clone repr")

    check_mesh(mesh.PointSet2D.create(), "PointSet2D", ", nb_vertices=0")
    check_mesh(mesh.PointSet3D.create(), "PointSet3D", ", nb_vertices=0")
    check_mesh(mesh.VertexSet.create(), "VertexSet", ", nb_vertices=0")
    check_mesh(mesh.Graph.create(), "Graph", ", nb_vertices=0, nb_edges=0")
    for dimension, curve_class in ((2, mesh.EdgedCurve2D), (3, mesh.EdgedCurve3D)):
        check_mesh(curve_class.create(), f"EdgedCurve{dimension}D", ", nb_vertices=0, nb_edges=0")
    for dimension in (2, 3):
        for kind in ("PolygonalSurface", "TriangulatedSurface"):
            created = getattr(mesh, f"{kind}{dimension}D").create()
            check_mesh(created, f"{kind}{dimension}D", ", nb_vertices=0, nb_polygons=0")
        check_mesh(
            getattr(mesh, f"RegularGrid{dimension}D").create(),
            f"RegularGrid{dimension}D",
            ", nb_vertices=0, nb_polygons=0" if dimension == 2 else ", nb_vertices=0, nb_polyhedra=0",
        )
    for kind in ("PolyhedralSolid", "TetrahedralSolid", "HybridSolid"):
        created = getattr(mesh, f"{kind}3D").create()
        check_mesh(created, f"{kind}3D", ", nb_vertices=0, nb_polyhedra=0")
    grid = mesh.LightRegularGrid3D(geometry.Point3D([1, 2, 3]), [2, 3, 4], [1.0, 2.0, 0.5])
    check_mesh(grid, "LightRegularGrid3D", ", nb_cells=24")
    check_mesh(
        mesh.LightRegularGrid2D(geometry.Point2D([0, 0]), [5, 2], [1.0, 1.0]),
        "LightRegularGrid2D",
        ", nb_cells=10",
    )
    check_mesh(image.RasterImage2D([3, 4]), "RasterImage2D", ", nb_cells=12")
    check_mesh(image.RasterImage3D([3, 4, 2]), "RasterImage3D", ", nb_cells=24")


def test_models():
    brep = model.BRep()
    check_repr(
        brep,
        f"BRep(id='{brep.id().string()}', nb_corners=0, nb_lines=0, nb_surfaces=0, nb_blocks=0)",
    )
    builder = model.BRepBuilder(brep)
    builder.add_corner()
    builder.add_corner()
    builder.add_line()
    builder.add_surface()
    builder.add_block()
    builder.set_name("my model")
    check_repr(
        brep,
        f"BRep(id='{brep.id().string()}', name='my model', nb_corners=2, nb_lines=1, "
        "nb_surfaces=1, nb_blocks=1)",
    )
    section = model.Section()
    check_repr(
        section,
        f"Section(id='{section.id().string()}', nb_corners=0, nb_lines=0, nb_surfaces=0)",
    )
    model.SectionBuilder(section).add_corner()
    check_repr(
        section,
        f"Section(id='{section.id().string()}', nb_corners=1, nb_lines=0, nb_surfaces=0)",
    )
    check(len(repr(brep)) < 200, "repr of a model is short")
    # the mesh of a component is named after its real type, not its static one
    surface = builder.add_surface(mesh.MeshImpl("OpenGeodeTriangulatedSurface3D"))
    component_mesh = brep.surface(surface).mesh()
    check(
        repr(component_mesh).startswith("TriangulatedSurface3D(id='"),
        f"repr of a component mesh: {repr(component_mesh)}",
    )
    check(
        repr(brep.surface(surface).triangulated_mesh()) == repr(component_mesh),
        "same mesh, same repr",
    )


if __name__ == "__main__":
    test_points_and_vectors()
    test_geometry_objects()
    test_basic_values()
    test_mesh_elements()
    test_model_identifiers()
    test_meshes()
    test_models()
