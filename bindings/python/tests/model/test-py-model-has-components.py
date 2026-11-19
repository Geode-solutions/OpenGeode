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


"""The has_<component> lookups of the models (BRep, Section) and the
unique vertex lookups of the VertexIdentifier: they answer from a uuid or an
index, so that the code removing components never needs a component handle."""

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_model as model


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def check_raises(function, exception, message):
    try:
        function()
    except exception:
        return
    raise ValueError("[Test] " + message + " should raise " + exception.__name__)


BREP_KINDS = [
    "corner",
    "line",
    "surface",
    "block",
    "model_boundary",
    "corner_collection",
    "line_collection",
    "surface_collection",
    "block_collection",
]
SECTION_KINDS = [
    "corner",
    "line",
    "surface",
    "model_boundary",
    "corner_collection",
    "line_collection",
    "surface_collection",
]


def check_has(name, mod, builder, kinds):
    unknown = basic.uuid()
    ids = {}
    for kind in kinds:
        has = getattr(mod, "has_" + kind)
        check(not has(unknown), f"{name}.has_{kind} of an unknown uuid")
        check(not has(id=unknown), f"{name}.has_{kind} by keyword")
        ids[kind] = getattr(builder, "add_" + kind)()
        check(has(ids[kind]), f"{name}.has_{kind} of an added component")
        doc = has.__doc__
        check("uuid" in doc and "True" in doc, f"{name}.has_{kind} docstring: {doc}")
    # Each lookup only knows its own kind of component
    for kind in kinds:
        for other in kinds:
            if other != kind:
                check(
                    not getattr(mod, "has_" + other)(ids[kind]),
                    f"{name}.has_{other} of a {kind}",
                )
    # Removed components are not found anymore; the removal takes a handle
    # fetched just before (see the removal docstrings), the lookup a uuid
    for kind in kinds:
        component = getattr(mod, kind)(ids[kind])
        getattr(builder, "remove_" + kind)(component)
        del component
        check(not getattr(mod, "has_" + kind)(ids[kind]), f"{name}.has_{kind} after removal")
    check_raises(lambda: mod.has_corner(None), TypeError, "has_corner(None)")
    check_raises(lambda: mod.has_corner("not a uuid"), TypeError, "has_corner(str)")


def check_vertex_identifier(name, mod, builder, point):
    corner_id = builder.add_corner()
    corner = mod.corner(corner_id)
    builder.corner_mesh_builder(corner).create_point(point)
    builder.create_unique_vertices(2)
    builder.set_unique_vertex(model.ComponentMeshVertex(corner.component_id(), 0), 0)
    corner_type = corner.component_type()
    line_type = model.ComponentType("Line")
    check(not mod.is_unique_vertex_isolated(0), f"{name}: unique vertex 0 is used")
    check(mod.is_unique_vertex_isolated(1), f"{name}: unique vertex 1 is isolated")
    check(
        mod.is_unique_vertex_isolated(unique_vertex_id=1),
        f"{name}: is_unique_vertex_isolated by keyword",
    )
    check(mod.has_component_mesh_vertices(0, corner_type), f"{name}: corner type")
    check(not mod.has_component_mesh_vertices(0, line_type), f"{name}: line type")
    check(not mod.has_component_mesh_vertices(1, corner_type), f"{name}: isolated")
    check(mod.has_component_mesh_vertices(0, corner_id), f"{name}: corner uuid")
    check(
        mod.has_component_mesh_vertices(unique_vertex_id=0, component_id=corner_id),
        f"{name}: has_component_mesh_vertices by keyword",
    )
    check(
        mod.has_component_mesh_vertices(unique_vertex_id=0, type=corner_type),
        f"{name}: has_component_mesh_vertices type by keyword",
    )
    check(not mod.has_component_mesh_vertices(0, basic.uuid()), f"{name}: other uuid")
    for function, arguments in [
        (mod.is_unique_vertex_isolated, (2,)),
        (mod.has_component_mesh_vertices, (2, corner_type)),
        (mod.has_component_mesh_vertices, (2, corner_id)),
    ]:
        check_raises(lambda: function(*arguments), IndexError, f"{name}: index 2")
    check_raises(
        lambda: mod.is_unique_vertex_isolated(-1), TypeError, f"{name}: negative index"
    )
    # After the removal of the corner, its unique vertex is isolated
    builder.remove_corner(corner)
    del corner
    check(mod.is_unique_vertex_isolated(0), f"{name}: isolated after removal")
    check(not mod.has_component_mesh_vertices(0, corner_id), f"{name}: removed corner")
    check(not mod.has_corner(corner_id), f"{name}: corner removed")


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    brep = model.BRep()
    check_has("BRep", brep, model.BRepBuilder(brep), BREP_KINDS)
    section = model.Section()
    check_has("Section", section, model.SectionBuilder(section), SECTION_KINDS)
    brep = model.BRep()
    check_vertex_identifier("BRep", brep, model.BRepBuilder(brep), geometry.Point3D([1, 2, 3]))
    section = model.Section()
    check_vertex_identifier(
        "Section", section, model.SectionBuilder(section), geometry.Point2D([1, 2])
    )
