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

import opengeode
import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh
import opengeode_py_model as model


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def test_brep_builder_keeps_brep_alive():
    brep = model.BRep()
    brep_ref = weakref.ref(brep)
    builder = model.BRepBuilder(brep)
    del brep
    gc.collect()
    check(brep_ref() is not None, "BRepBuilder should keep its BRep alive")
    corner_id = builder.add_corner()
    check(brep_ref().nb_corners() == 1, "BRep should have 1 corner")
    corner_builder = builder.corner_mesh_builder(brep_ref().corner(corner_id))
    del builder
    gc.collect()
    check(brep_ref() is not None, "Corner mesh builder should keep the BRep alive")
    corner_builder.create_point(geom.Point3D([0, 0, 0]))
    check(
        brep_ref().corner(corner_id).mesh().nb_vertices() == 1,
        "Corner should have 1 vertex",
    )
    del corner_builder
    gc.collect()
    check(brep_ref() is None, "BRep should be released with its builders")


def test_section_builder_keeps_section_alive():
    section = model.Section()
    section_ref = weakref.ref(section)
    builder = model.SectionBuilder(section)
    del section
    gc.collect()
    check(section_ref() is not None, "SectionBuilder should keep its Section alive")
    line_id = builder.add_line()
    line_builder = builder.line_mesh_builder(section_ref().line(line_id))
    del builder
    gc.collect()
    check(section_ref() is not None, "Line mesh builder should keep the Section alive")
    line_builder.create_point(geom.Point2D([0, 0]))


def test_helpers_keep_brep_alive():
    box = geom.BoundingBox3D()
    box.add_point(geom.Point3D([0, 0, 0]))
    box.add_point(geom.Point3D([1, 1, 1]))
    brep = model.create_brep_from_bounding_box(box)
    brep_ref = weakref.ref(brep)
    ray_tracing = model.BRepRayTracing(brep)
    concatener = model.BRepConcatener(brep)
    registry_builder = model.ComponentRegistryBuilder(brep)
    del brep
    gc.collect()
    check(brep_ref() is not None, "Helpers should keep the BRep alive")
    check(
        ray_tracing.block_containing_point(geom.Point3D([0.5, 0.5, 0.5]))
        is not None,
        "Point should be inside the block",
    )
    del ray_tracing
    del concatener
    del registry_builder
    gc.collect()
    check(brep_ref() is None, "BRep should be released with its helpers")


def check_raises(function, exception, message):
    try:
        function()
    except exception:
        return
    raise ValueError("[Test] " + message + " should raise " + exception.__name__)


def check_removal(name, mod, builder, kinds, point):
    """Removing components by handle then by uuid; every later query goes
    through the uuids, never through the objects of the removed components."""
    ids = {kind: [getattr(builder, "add_" + kind)() for _ in range(4)] for kind in kinds}
    corner_ids = ids["corner"]
    builder.corner_mesh_builder(mod.corner(corner_ids[0])).create_point(point)
    builder.create_unique_vertices(1)
    builder.set_unique_vertex(
        model.ComponentMeshVertex(mod.corner(corner_ids[0]).component_id(), 0), 0
    )
    builder.add_corner_line_boundary_relationship(
        mod.corner(corner_ids[0]), mod.line(ids["line"][0])
    )
    check(mod.nb_boundaries(ids["line"][0]) == 1, f"{name}: line has a boundary")
    for kind in kinds:
        nb = getattr(mod, "nb_%ss" % kind)
        has = getattr(mod, "has_" + kind)
        get = getattr(mod, kind)
        kind_ids = ids[kind]
        # By handle: the object is fetched and dropped around the call
        getattr(builder, "remove_" + kind)(get(kind_ids[0]))
        check(not has(kind_ids[0]), f"{name}: {kind} removed by handle")
        check(nb() == 3, f"{name}: 3 {kind}s left")
        check_raises(lambda: get(kind_ids[0]), opengeode.OpenGeodeDataError, f"{name}.{kind}(removed)")
        # By uuid: a repeated uuid is removed once
        remove_many = getattr(builder, "remove_%ss" % kind)
        remove_many([kind_ids[1], kind_ids[1], kind_ids[2]])
        check(not has(kind_ids[1]) and not has(kind_ids[2]), f"{name}: {kind}s removed by uuid")
        check(nb() == 1 and has(kind_ids[3]), f"{name}: the other {kind} is kept")
        check(get(kind_ids[3]).id() == kind_ids[3], f"{name}: fetch the kept {kind} again")
        # An unknown uuid: error before removing anything
        check_raises(
            lambda: remove_many([kind_ids[3], basic.uuid()]),
            opengeode.OpenGeodeDataError,
            f"{name}.remove_{kind}s with an unknown uuid",
        )
        check(has(kind_ids[3]) and nb() == 1, f"{name}: nothing removed on error")
        check_raises(lambda: remove_many([kind_ids[0]]), opengeode.OpenGeodeDataError, f"{name}: removed twice")
        remove_many([])
        remove_many(ids=[kind_ids[3]])
        check(nb() == 0, f"{name}: every {kind} removed")
        doc = remove_many.__doc__
        check("uuid" in doc and "before removing" in doc, f"{name}.remove_{kind}s docstring")
    # The removals cleaned the relationships and the unique vertices
    check(mod.is_unique_vertex_isolated(0), f"{name}: unique vertex isolated")
    check(mod.nb_boundaries(ids["line"][0]) == 0, f"{name}: no relationship of a removed line")



def check_removal_by_uuid_cleanup(name, mod, builder, kinds, point):
    """The uuid removals clean the relationships and the unique vertices of
    the removed components, and an unknown uuid, wherever it is in the list,
    leaves the model untouched."""
    ids = {kind: getattr(builder, "add_" + kind)() for kind in kinds}
    builder.corner_mesh_builder(mod.corner(ids["corner"])).create_point(point)
    first_unique = builder.create_unique_vertices(1)
    builder.set_unique_vertex(
        model.ComponentMeshVertex(mod.corner(ids["corner"]).component_id(), 0),
        first_unique,
    )
    for boundary, incidence in zip(kinds, kinds[1:]):
        getattr(builder, "add_%s_%s_boundary_relationship" % (boundary, incidence))(
            getattr(mod, boundary)(ids[boundary]), getattr(mod, incidence)(ids[incidence])
        )
    check(not mod.is_unique_vertex_isolated(first_unique), f"{name}: unique vertex used")
    for boundary, incidence in zip(kinds, kinds[1:]):
        check(mod.nb_boundaries(ids[incidence]) == 1, f"{name}: {incidence} has a boundary")
    for kind in kinds:
        remove_many = getattr(builder, "remove_%ss" % kind)
        has = getattr(mod, "has_" + kind)
        nb = getattr(mod, "nb_%ss" % kind)
        for unknown_first in (True, False):
            unknown = [basic.uuid()]
            check_raises(
                lambda: remove_many(unknown + [ids[kind]] if unknown_first else [ids[kind]] + unknown),
                opengeode.OpenGeodeDataError,
                f"{name}.remove_{kind}s with an unknown uuid",
            )
            check(has(ids[kind]) and nb() == 1, f"{name}: {kind} kept on error")
        index = kinds.index(kind)
        if index + 1 < len(kinds):
            incidence = kinds[index + 1]
            check(mod.nb_boundaries(ids[incidence]) == 1, f"{name}: {kind} relationship kept on error")
        remove_many([ids[kind], ids[kind]])
        check(not has(ids[kind]) and nb() == 0, f"{name}: {kind} removed by uuid")
        if index + 1 < len(kinds):
            check(mod.nb_boundaries(ids[incidence]) == 0, f"{name}: relationship of the removed {kind}")
        if kind == "corner":
            check(mod.is_unique_vertex_isolated(first_unique), f"{name}: unique vertex of the removed corner")
            check(mod.component_mesh_vertices(first_unique) == [], f"{name}: no component vertex left")


def check_removal_of_another_kind(name, mod, builder, kinds):
    """A uuid of another component kind is refused by every uuid removal,
    alone or with valid uuids, before removing anything."""
    ids = {kind: getattr(builder, "add_" + kind)() for kind in kinds}
    for boundary, incidence in zip(kinds, kinds[1:]):
        getattr(builder, "add_%s_%s_boundary_relationship" % (boundary, incidence))(
            getattr(mod, boundary)(ids[boundary]), getattr(mod, incidence)(ids[incidence])
        )

    def state():
        return (
            [getattr(mod, "nb_%ss" % kind)() for kind in kinds],
            [mod.nb_boundaries(ids[kind]) for kind in kinds],
            [mod.nb_incidences(ids[kind]) for kind in kinds],
        )

    before = state()
    check(before == ([1] * len(kinds), [0] + [1] * (len(kinds) - 1), [1] * (len(kinds) - 1) + [0]),
          f"{name}: initial state")
    for kind in kinds:
        remove_many = getattr(builder, "remove_%ss" % kind)
        for other in kinds:
            if other == kind:
                continue
            for wrong in ([ids[other]], [ids[kind], ids[other]], [ids[other], ids[kind]]):
                check_raises(
                    lambda: remove_many(wrong),
                    opengeode.OpenGeodeDataError,
                    f"{name}.remove_{kind}s with the uuid of a {other}",
                )
                check(state() == before, f"{name}.remove_{kind}s([{other}]) removed nothing")
                check(getattr(mod, "has_" + other)(ids[other]), f"{name}: the {other} is kept")
                check(getattr(mod, "has_" + kind)(ids[kind]), f"{name}: the {kind} is kept")


def check_removal_documentation(builder_class, kinds):
    # Every removal of components, by object or by uuids; the relationships
    # and the registry entries remove no component
    not_component_removals = {
        "remove_relation",
        "remove_mesh_component",
        "remove_collection_component",
    }
    checked = []
    for name in dir(builder_class):
        if not name.startswith("remove_") or name in not_component_removals:
            continue
        checked.append(name)
        doc = getattr(builder_class, name).__doc__
        check(
            "freed memory" in doc and "uuid" in doc,
            f"{builder_class.__name__}.{name} should warn about the removed objects",
        )
    for kind in kinds:
        check(hasattr(builder_class, "remove_%ss" % kind), f"remove_{kind}s")
        check("remove_%ss" % kind in checked, f"remove_{kind}s documentation checked")
    text = " ".join(opengeode.__doc__.split())
    check("freed memory" in text and "remove_corners" in text, "package documentation")


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    test_brep_builder_keeps_brep_alive()
    test_section_builder_keeps_section_alive()
    test_helpers_keep_brep_alive()
    brep = model.BRep()
    check_removal(
        "BRep", brep, model.BRepBuilder(brep),
        ["corner", "line", "surface", "block"], geom.Point3D([1, 2, 3]),
    )
    brep = model.BRep()
    check_removal_by_uuid_cleanup(
        "BRep", brep, model.BRepBuilder(brep),
        ["corner", "line", "surface", "block"], geom.Point3D([1, 2, 3]),
    )
    section = model.Section()
    check_removal_by_uuid_cleanup(
        "Section", section, model.SectionBuilder(section),
        ["corner", "line", "surface"], geom.Point2D([1, 2]),
    )
    section = model.Section()
    check_removal(
        "Section", section, model.SectionBuilder(section),
        ["corner", "line", "surface"], geom.Point2D([1, 2]),
    )
    brep = model.BRep()
    check_removal_of_another_kind(
        "BRep", brep, model.BRepBuilder(brep), ["corner", "line", "surface", "block"]
    )
    section = model.Section()
    check_removal_of_another_kind(
        "Section", section, model.SectionBuilder(section), ["corner", "line", "surface"]
    )
    check_removal_documentation(model.BRepBuilder, ["corner", "line", "surface", "block"])
    check_removal_documentation(model.SectionBuilder, ["corner", "line", "surface"])
