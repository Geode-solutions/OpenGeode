# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2024 Geode-solutions
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
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic
import opengeode_py_model as model


def run_test_brep():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    brep = model.load_brep(os.path.join(data_dir, "layers.og_brep"))

    curve, surface, solid = model.convert_brep_into_curve_and_polygonal_surface_and_polyhedral_solid(
        brep)
    if curve.nb_vertices() != 16:
        raise ValueError("[Test] BRep - Wrong number of curve vertices")
    if curve.nb_edges() != 28:
        raise ValueError("[Test] BRep - Wrong number of curve edges")

    if surface.nb_vertices() != 16:
        raise ValueError("[Test] BRep - Wrong number of surface vertices")
    if surface.nb_polygons() != 16:
        raise ValueError("[Test] BRep - Wrong number of surface polygons")

    if solid.nb_vertices() != 16:
        raise ValueError("[Test] BRep - Wrong number of solid vertices")
    if solid.nb_polyhedra() != 0:
        raise ValueError("[Test] BRep - Wrong number of solid polyhedra")


def run_test_section():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    section = model.load_section(os.path.join(data_dir, "quad.og_sctn"))

    curve, surface = model.convert_section_into_curve_and_polygonal_surface(
        section)
    if curve.nb_vertices() != 0:
        raise ValueError("[Test] Section - Wrong number of curve vertices")
    if curve.nb_edges() != 0:
        raise ValueError("[Test] Section - Wrong number of curve edges")

    if surface.nb_vertices() != 4:
        raise ValueError("[Test] Section - Wrong number of surface vertices")
    if surface.nb_polygons() != 1:
        raise ValueError("[Test] Section - Wrong number of surface polygons")


if __name__ == '__main__':
    model.OpenGeodeModelLibrary.initialize()
    run_test_brep()
    run_test_section()
