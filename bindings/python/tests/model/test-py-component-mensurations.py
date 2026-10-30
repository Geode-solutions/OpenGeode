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
import opengeode_py_geometry as geometry
import opengeode_py_model as model


def check_value(value, expected, message):
    if abs(value - expected) > 1e-6:
        raise ValueError(
            "[Test] " + message + ": " + str(value) + " instead of " + str(expected)
        )


def check_point(point, expected, message):
    for d in range(3):
        check_value(point.value(d), expected[d], message)


def run_test_meshed_blocks(data_dir):
    brep = model.load_brep(os.path.join(data_dir, "test_mesh3.og_brep"))
    for block in brep.blocks():
        mesh = block.mesh()
        mesh_volume = sum(mesh.polyhedron_volume(p) for p in range(mesh.nb_polyhedra()))
        check_value(model.block_volume(brep, block), mesh_volume, "Wrong block volume")
        check_value(mesh_volume, 500, "Wrong block mesh volume")
        box = model.block_bounding_box(brep, block)
        mesh_box = mesh.bounding_box()
        check_point(box.min(), [mesh_box.min().value(d) for d in range(3)], "Wrong min")
        check_point(box.max(), [mesh_box.max().value(d) for d in range(3)], "Wrong max")
    for surface in brep.surfaces():
        mesh = surface.mesh()
        mesh_area = sum(mesh.polygon_area(p) for p in range(mesh.nb_polygons()))
        check_value(model.surface_area3D(surface), mesh_area, "Wrong surface area")


def run_test_block_boundaries(data_dir):
    brep = model.load_brep(os.path.join(data_dir, "box_brep.og_brep"))
    if brep.nb_blocks() != 1:
        raise ValueError("[Test] Wrong number of blocks")
    for block in brep.blocks():
        if block.mesh().nb_polyhedra() != 0:
            raise ValueError("[Test] Block should not be meshed")
        check_value(model.block_volume(brep, block), 64000, "Wrong block volume")
        box = model.block_bounding_box(brep, block)
        check_point(box.min(), [0, 0, 0], "Wrong block bounding box min")
        check_point(box.max(), [40, 40, 40], "Wrong block bounding box max")
    for surface in brep.surfaces():
        check_value(model.surface_area3D(surface), 1600, "Wrong surface area")


def run_test_section(data_dir):
    section = model.load_section(os.path.join(data_dir, "fractures.og_sctn"))
    for surface in section.surfaces():
        mesh = surface.mesh()
        mesh_area = sum(mesh.polygon_area(p) for p in range(mesh.nb_polygons()))
        check_value(model.surface_area2D(surface), mesh_area, "Wrong surface area")


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    run_test_meshed_blocks(data_dir)
    run_test_block_boundaries(data_dir)
    run_test_section(data_dir)
