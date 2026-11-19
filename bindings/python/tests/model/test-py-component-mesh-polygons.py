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
# all copies and substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry
import opengeode_py_mesh as mesh
import opengeode_py_model as model
import opengeode


def run_test_brep():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    brep_filename = os.path.join(data_dir, "test_mesh3.og_brep")
    brep = opengeode.load_one(model.load_brep(brep_filename), brep_filename)

    for block in brep.blocks():
        block_mesh = block.mesh()
        for surface in brep.surfaces():
            if not brep.is_block_boundary(surface, block) and not brep.is_surface_in_block_internals(surface, block):
                continue
            surface_mesh = surface.mesh()
            for polygon_id in range(surface_mesh.nb_polygons()):
                block_facets_vertices = model.block_vertices_from_surface_polygon(
                    brep, block, surface, polygon_id)
                oriented_block_facets_vertices = model.oriented_block_vertices_from_surface_polygon(
                    brep, block, surface, polygon_id)
                if brep.is_block_boundary(surface, block):
                    if not oriented_block_facets_vertices.nb_facets() == 1:
                        raise ValueError("[Test] " + oriented_block_facets_vertices.nb_facets(
                        ) + " polyhedra were found from boundary surface polygon.")
                elif brep.is_surface_in_block_internals(surface, block):
                    if not oriented_block_facets_vertices.nb_facets() == 2:
                        raise ValueError("[Test] " + oriented_block_facets_vertices.nb_facets(
                        ) + " polyhedra were found from internal surface polygon.")
                for polygon_vertex_id in range(len(block_facets_vertices[0].vertices)):
                    if not surface_mesh.point(surface_mesh.polygon_vertex(mesh.PolygonVertex(polygon_id, polygon_vertex_id))).inexact_equal(block_mesh.point(block_facets_vertices[0].vertices[polygon_vertex_id])):
                        raise ValueError(
                            "[Test] Point on the edge and on the surface have different positions.")

    for surface in brep.surfaces():
        surface_mesh = surface.mesh()
        for line in brep.lines():
            if not brep.is_surface_boundary(line, surface) and not brep.is_line_in_surface_internals(line, surface):
                continue
            line_mesh = line.mesh()
            for edge_id in range(line_mesh.nb_edges()):
                surface_edge_vertices = model.brep_surface_vertices_from_line_edge(
                    brep, surface, line, edge_id)
                oriented_surface_edge_vertices = model.brep_oriented_surface_vertices_from_line_edge(
                    brep, surface, line, edge_id)
                if len(surface_edge_vertices) != oriented_surface_edge_vertices.nb_edges():
                    raise ValueError(
                        "[Test] Different number of polygons for surface_vertices_from_line_edge and oriented_surface_vertices_from_line_edge functions.")
                if brep.is_surface_boundary(line, surface):
                    if len(surface_edge_vertices) != 1:
                        raise ValueError(
                            "[Test] " + len(surface_edge_vertices) + " polygons were found from boundary line edge.")
                elif brep.is_line_in_surface_internals(line, surface):
                    if len(surface_edge_vertices) != 2:
                        raise ValueError(
                            "[Test] " + len(surface_edge_vertices) + " polygons were found from internal line edge.")
                for edge_vertex_id in range(2):
                    if not line_mesh.point(line_mesh.edge_vertex(mesh.EdgeVertex(edge_id, edge_vertex_id))).inexact_equal(surface_mesh.point(surface_edge_vertices[0].vertices[edge_vertex_id])):
                        raise ValueError(
                            "[Test] Point on the edge and on the surface have different positions.")



def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def check_raises(function, exception, message):
    try:
        function()
    except exception:
        return
    raise ValueError("[Test] " + message + " should raise " + exception.__name__)


def check_optional_field_live(result, field, make, read, mutate):
    """An optional field of a bound class is None or a live reference to the
    stored value: modifying it modifies the result, reassigning the field
    keeps the storage (the references held stay valid and show the new
    value), and None is refused."""
    name = type(result).__name__ + "." + field
    check(getattr(result, field) is None, name + " is None by default")
    check_raises(lambda: setattr(result, field, None), TypeError, name + " = None when disengaged")
    check(getattr(result, field) is None, name + " stays None after a refused None")
    setattr(result, field, make(1))
    live = getattr(result, field)
    check(read(live) == 1, name + " assigned when disengaged")
    # In-place edit through the getter changes the result
    mutate(getattr(result, field), 2)
    check(read(getattr(result, field)) == 2, name + " modified in place through the getter")
    check(read(live) == 2, name + " reference sees the in-place edit")
    # None is refused and leaves the field unchanged
    check_raises(lambda: setattr(result, field, None), TypeError, name + " = None")
    check(read(getattr(result, field)) == 2, name + " unchanged by a refused None")
    check(read(live) == 2, name + " reference valid after a refused None")
    # Reassigning keeps the storage: the reference shows the new value
    other = make(3)
    setattr(result, field, other)
    check(read(live) == 3, name + " reference shows the reassigned value")
    mutate(other, 4)
    check(read(getattr(result, field)) == 3, name + " keeps its own copy of the assigned value")
    mutate(live, 5)
    check(read(getattr(result, field)) == 5, name + " modified through an older reference")
    # Churn: many reassignments while holding the reference
    for value in range(100):
        setattr(result, field, make(value))
        check(read(live) == value, name + " reference valid during reassignments")
    # The reference keeps its object alive
    del result
    check(read(live) == 99, name + " reference keeps its object alive")


def run_test_optional_fields():
    def make_facet(value):
        facet = model.BlockPolyhedronFacet(mesh.PolyhedronFacet(value, 0))
        facet.vertices = [value, value + 1, value + 2]
        return facet

    def read_facet(facet):
        check(list(facet.vertices) == [facet.facet.polyhedron_id + i for i in range(3)], "facet vertices")
        return facet.facet.polyhedron_id

    def mutate_facet(facet, value):
        facet.facet.polyhedron_id = value
        facet.vertices = [value, value + 1, value + 2]

    def make_edge(value):
        edge = model.SurfacePolygonEdge(mesh.PolygonEdge(value, 0))
        edge.vertices = [value, value + 1]
        return edge

    def read_edge(edge):
        check(list(edge.vertices) == [edge.facet.polygon_id, edge.facet.polygon_id + 1], "edge vertices")
        return edge.facet.polygon_id

    def mutate_edge(edge, value):
        edge.facet.polygon_id = value
        edge.vertices = [value, value + 1]

    for field in ["oriented_polyhedron_facet", "opposite_polyhedron_facet"]:
        check_optional_field_live(
            model.BlockPolyhedraFacetVertices(), field, make_facet, read_facet, mutate_facet
        )
    for field in ["oriented_edge", "opposite_edge"]:
        check_optional_field_live(
            model.SurfacePolygonsEdgeVertices(), field, make_edge, read_edge, mutate_edge
        )
    # The two optional fields of one result are independent
    result = model.SurfacePolygonsEdgeVertices()
    result.oriented_edge = make_edge(1)
    result.opposite_edge = make_edge(2)
    result.oriented_edge.facet.polygon_id = 7
    check(result.opposite_edge.facet.polygon_id == 2, "optional fields are independent")
    check(result.nb_edges() == 2, "nb_edges sees both assigned fields")
    # A non optional class field is a reference to the field of its object,
    # which it keeps alive
    edge = model.SurfacePolygonEdge(mesh.PolygonEdge(5, 1))
    facet = edge.facet
    facet.polygon_id = 6
    check(edge.facet.polygon_id == 6, "a class field is a reference")
    del edge
    check(facet.polygon_id == 6, "a class field keeps its object alive")
    # Docstrings: live optional fields, containers, scalars
    for cls, field in [
        (model.BlockPolyhedraFacetVertices, "oriented_polyhedron_facet"),
        (model.BlockPolyhedraFacetVertices, "opposite_polyhedron_facet"),
        (model.SurfacePolygonsEdgeVertices, "oriented_edge"),
        (model.SurfacePolygonsEdgeVertices, "opposite_edge"),
    ]:
        doc = " ".join(getattr(cls, field).__doc__.split())
        check("live reference" in doc and "modified in place" in doc and "cannot be set to None" in doc,
              cls.__name__ + "." + field + " docstring")
        check("copy" not in doc, cls.__name__ + "." + field + " docstring without copy")
    for cls, field in [
        (model.BlockPolyhedronFacet, "vertices"),
        (model.SurfacePolygonEdge, "vertices"),
    ]:
        doc = " ".join(getattr(cls, field).__doc__.split())
        check("returned as a copy" in doc and "assign" in doc, cls.__name__ + "." + field + " docstring")
    for cls, field in [
        (mesh.PolygonEdge, "polygon_id"),
        (mesh.PolyhedronFacet, "facet_id"),
        (basic.AttributeProperties, "assignable"),
        (basic.AttributeProperties, "time"),
        (model.BlockPolyhedronFacet, "facet"),
    ]:
        doc = getattr(cls, field).__doc__
        check("copy" not in doc and "live reference" not in doc, cls.__name__ + "." + field + " docstring")
    # A container field is a copy: modifying it does not modify the object
    edge = model.SurfacePolygonEdge(mesh.PolygonEdge(5, 1))
    edge.vertices = [1, 2]
    vertices = edge.vertices
    vertices[0] = 9
    check(list(edge.vertices) == [1, 2], "a container field is a copy")


def run_test_value_optional_fields():
    # Optional fields of scalars keep value semantics: None allowed
    properties = basic.AttributeProperties()
    check(properties.time is None, "time is None by default")
    properties.time = 2.5
    check(properties.time == 2.5, "time assigned")
    properties.time = None
    check(properties.time is None, "time reset to None")
    values = basic.AttributeValuesDouble()
    check(values.no_value is None, "no_value is None by default")
    values.no_value = -1.0
    check(values.no_value == -1.0, "no_value assigned")
    values.no_value = None
    check(values.no_value is None, "no_value reset to None")
    doc = basic.AttributeValuesDouble.no_value.__doc__
    check("copy" not in doc and "live reference" not in doc, "AttributeValuesDouble.no_value docstring")


if __name__ == '__main__':
    model.OpenGeodeModelLibrary.initialize()
    run_test_brep()
    run_test_optional_fields()
    run_test_value_optional_fields()
