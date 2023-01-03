# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2023 Geode-solutions
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
    model.triangulate_brep_surface_meshes(brep)
    model.convert_brep_surface_meshes_into_triangulated_surfaces(brep)
    model.convert_brep_block_meshes_into_tetrahedral_solids(brep)

    file_io = "test_triangulated_surfaces." + brep.native_extension()
    model.save_brep(brep, file_io)
    brep2 = model.load_brep(file_io)


def run_test_section():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    section = model.load_section(os.path.join(data_dir, "quad.og_sctn"))
    model.triangulate_section_surface_meshes(section)
    model.convert_section_surface_meshes_into_triangulated_surfaces(section)

    file_io = "test_triangulated_surfaces." + section.native_extension()
    model.save_section(section, file_io)
    section2 = model.load_section(file_io)


if __name__ == '__main__':
    model.OpenGeodeModel.initialize()
    run_test_brep()
    run_test_section()
