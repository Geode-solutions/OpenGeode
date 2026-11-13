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


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    test_brep_builder_keeps_brep_alive()
    test_section_builder_keeps_section_alive()
    test_helpers_keep_brep_alive()
