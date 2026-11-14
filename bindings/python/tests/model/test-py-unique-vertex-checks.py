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
import subprocess
import sys

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_model as model


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    raise ValueError(f"[Test] {message} should raise {error.__name__}")


def vertex(component_id, index):
    return model.ComponentMeshVertex(component_id, index)


def fill(mesh_builder, nb_vertices, dimension):
    for i in range(nb_vertices):
        coordinates = [float(i)] + [0.0] * (dimension - 1)
        mesh_builder.create_point(
            geometry.Point3D(coordinates)
            if dimension == 3
            else geometry.Point2D(coordinates)
        )


def build_brep():
    brep = model.BRep()
    builder = model.BRepBuilder(brep)
    corner = brep.corner(builder.add_corner())
    line = brep.line(builder.add_line())
    surface = brep.surface(builder.add_surface())
    block = brep.block(builder.add_block())
    fill(builder.corner_mesh_builder(corner), 1, 3)
    fill(builder.line_mesh_builder(line), 3, 3)
    fill(builder.surface_mesh_builder(surface), 4, 3)
    fill(builder.block_mesh_builder(block), 5, 3)
    builder.create_unique_vertices(2)
    return brep, builder, [corner, line, surface, block]


def build_section():
    section = model.Section()
    builder = model.SectionBuilder(section)
    corner = section.corner(builder.add_corner())
    line = section.line(builder.add_line())
    surface = section.surface(builder.add_surface())
    fill(builder.corner_mesh_builder(corner), 1, 2)
    fill(builder.line_mesh_builder(line), 3, 2)
    fill(builder.surface_mesh_builder(surface), 4, 2)
    builder.create_unique_vertices(2)
    return section, builder, [corner, line, surface]


def foreign_component(builder_class, model_class, add):
    other = model_class()
    other_builder = builder_class(other)
    return add(other, other_builder)


def check_model(name, mod, builder, components, nb_vertices, foreign):
    # Valid calls work for every component type
    for component, nb in zip(components, nb_vertices):
        component_id = component.component_id()
        if mod.unique_vertex(vertex(component_id, 0)) != basic.NO_ID:
            raise ValueError(f"[Test] {name}: a new vertex has no unique vertex")
        builder.set_unique_vertex(vertex(component_id, nb - 1), 1)
        expected_first = 1 if nb == 1 else basic.NO_ID
        if mod.unique_vertex(vertex(component_id, nb - 1)) != 1:
            raise ValueError(f"[Test] {name}: wrong unique vertex")
        check_raises(
            lambda: mod.unique_vertex(vertex(component_id, nb)),
            IndexError,
            f"{name} unique_vertex of the vertex {nb} of a mesh of {nb} vertices",
        )
        check_raises(
            lambda: mod.unique_vertex(vertex(component_id, basic.NO_ID)),
            IndexError,
            f"{name} unique_vertex of NO_ID",
        )
        check_raises(
            lambda: builder.set_unique_vertex(vertex(component_id, nb), 0),
            IndexError,
            f"{name} set_unique_vertex of a vertex out of the mesh",
        )
        check_raises(
            lambda: builder.set_unique_vertex(vertex(component_id, 0), 2),
            IndexError,
            f"{name} set_unique_vertex with a missing unique vertex",
        )
        check_raises(
            lambda: builder.set_unique_vertex(vertex(component_id, 0), basic.NO_ID),
            IndexError,
            f"{name} set_unique_vertex with NO_ID",
        )
        if mod.unique_vertex(vertex(component_id, 0)) != expected_first:
            raise ValueError(f"[Test] {name}: a rejected call must change nothing")
    if len(mod.component_mesh_vertices(1)) != len(components):
        raise ValueError(f"[Test] {name}: rejected calls must not identify vertices")
    # A component of another model
    check_raises(
        lambda: mod.unique_vertex(vertex(foreign, 0)),
        basic.OpenGeodeDataError,
        f"{name} unique_vertex of a component of another model",
    )
    check_raises(
        lambda: builder.set_unique_vertex(vertex(foreign, 0), 0),
        basic.OpenGeodeDataError,
        f"{name} set_unique_vertex of a component of another model",
    )
    if mod.nb_unique_vertices() != 2:
        raise ValueError(f"[Test] {name}: wrong number of unique vertices")


def test_brep():
    brep, builder, components = build_brep()
    foreign = foreign_component(
        model.BRepBuilder, model.BRep, lambda m, b: m.corner(b.add_corner()).component_id()
    )
    check_model("BRep", brep, builder, components, [1, 3, 4, 5], foreign)


def test_section():
    section, builder, components = build_section()
    foreign = foreign_component(
        model.SectionBuilder, model.Section, lambda m, b: m.corner(b.add_corner()).component_id()
    )
    check_model("Section", section, builder, components, [1, 3, 4], foreign)


def test_removed_component():
    brep, builder, components = build_brep()
    corner = components[0]
    component_id = corner.component_id()
    builder.set_unique_vertex(vertex(component_id, 0), 0)
    builder.remove_corner(corner)
    check_raises(
        lambda: brep.unique_vertex(vertex(component_id, 0)),
        basic.OpenGeodeDataError,
        "unique_vertex of a removed component",
    )
    check_raises(
        lambda: builder.set_unique_vertex(vertex(component_id, 0), 0),
        basic.OpenGeodeDataError,
        "set_unique_vertex of a removed component",
    )


def test_keyword_arguments_and_builder_attributes():
    brep, builder, components = build_brep()
    component_id = components[1].component_id()
    builder.set_unique_vertex(
        component_vertex_id=vertex(component_id, 2), unique_vertex_id=0
    )
    if brep.unique_vertex(component_vertex=vertex(component_id, 2)) != 0:
        raise ValueError("[Test] Keyword arguments should work")
    # The builder keeps the model alive and still builds the model it modifies
    other_builder = model.BRepBuilder(brep=brep)
    other_builder.set_unique_vertex(vertex(component_id, 1), 1)
    if brep.unique_vertex(vertex(component_id, 1)) != 1:
        raise ValueError("[Test] A second builder should modify the same model")
    check_raises(
        lambda: model.BRepBuilder(model.Section()),
        TypeError,
        "BRepBuilder of a Section",
    )


def test_standalone_identifier():
    # A VertexIdentifier alone does not know the meshes: valid calls work
    identifier = model.VertexIdentifier()
    if identifier.nb_unique_vertices() != 0:
        raise ValueError("[Test] A new identifier has no unique vertex")
    identifier_builder = model.VertexIdentifierBuilder(identifier)
    identifier_builder.create_unique_vertices(2)
    if identifier.nb_unique_vertices() != 2:
        raise ValueError("[Test] Wrong number of unique vertices")

IMPORT_SCRIPT = """
import geode_test_env
import opengeode_py_model
print("imported")
"""


def test_import_emits_no_warning():
    # An __init__ replaced by a plain function is an old-style constructor, which
    # pybind11 reports with a FutureWarning (in non-NDEBUG builds)
    result = subprocess.run(
        [sys.executable, "-W", "error::FutureWarning", "-c", IMPORT_SCRIPT],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=dict(os.environ),
        timeout=120,
    )
    if result.returncode != 0 or "imported" not in result.stdout:
        raise ValueError(
            f"[Test] Importing the model bindings should not warn:\n{result.stdout}"
        )


def test_builder_subclass():
    # A Python subclass calling the constructor of its base is checked too
    class MyBuilder(model.BRepBuilder):
        def __init__(self, brep):
            super().__init__(brep)

    brep = model.BRep()
    builder = MyBuilder(brep)
    corner = brep.corner(builder.add_corner())
    builder.create_unique_vertex()
    builder.corner_mesh_builder(corner).create_point(geometry.Point3D([0, 0, 0]))
    check_raises(
        lambda: builder.set_unique_vertex(vertex(corner.component_id(), 1), 0),
        IndexError,
        "set_unique_vertex of a subclass builder",
    )
    check_raises(
        lambda: builder.set_unique_vertex(vertex(corner.component_id(), 0), 5),
        IndexError,
        "set_unique_vertex of a subclass builder, missing unique vertex",
    )
    builder.set_unique_vertex(vertex(corner.component_id(), 0), 0)
    if brep.unique_vertex(vertex(corner.component_id(), 0)) != 0:
        raise ValueError("[Test] A subclass builder should build the model")


def test_derived_builder_without_model():
    # A class deriving from the builder without calling its constructor is not
    # an initialized builder
    class LazyBuilder(model.BRepBuilder):
        def __init__(self, brep):
            self.brep = brep

    brep = model.BRep()
    check_raises(
        lambda: LazyBuilder(brep).create_unique_vertex(),
        TypeError,
        "a builder that was not initialized",
    )


def test_model_is_the_given_object():
    brep = model.BRep()
    builder = model.BRepBuilder(brep)
    if builder._model is not brep:
        raise ValueError("[Test] The builder should remember the given BRep")
    section = model.Section()
    if model.SectionBuilder(section)._model is not section:
        raise ValueError("[Test] The builder should remember the given Section")


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    test_brep()
    test_section()
    test_removed_component()
    test_keyword_arguments_and_builder_attributes()
    test_standalone_identifier()
    test_import_emits_no_warning()
    test_builder_subclass()
    test_derived_builder_without_model()
    test_model_is_the_given_object()
