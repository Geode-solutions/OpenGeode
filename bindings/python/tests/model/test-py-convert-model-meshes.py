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
import opengeode_py_model as model
import opengeode


def run_test_brep():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    brep_filename = os.path.join(data_dir, "layers.og_brep")
    brep = opengeode.load_one(model.load_brep(brep_filename), brep_filename)
    model.triangulate_brep_surface_meshes(brep)
    model.convert_brep_surface_meshes_into_triangulated_surfaces(brep)
    model.convert_brep_block_meshes_into_tetrahedral_solids(brep)

    file_io = "test_triangulated_surfaces." + brep.native_extension()
    model.save_brep(brep, file_io)
    brep2 = opengeode.load_one(model.load_brep(file_io), file_io)


def run_test_section():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    section_filename = os.path.join(data_dir, "quad.og_sctn")
    section = opengeode.load_one(model.load_section(section_filename), section_filename)
    model.triangulate_section_surface_meshes(section)
    model.convert_section_surface_meshes_into_triangulated_surfaces(section)

    file_io = "test_triangulated_surfaces." + section.native_extension()
    model.save_section(section, file_io)
    section2 = opengeode.load_one(model.load_section(file_io), file_io)


if __name__ == '__main__':
    model.OpenGeodeModelLibrary.initialize()
    run_test_brep()
    run_test_section()
