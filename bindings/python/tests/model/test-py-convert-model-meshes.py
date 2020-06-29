# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2020 Geode-solutions
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

import opengeode_py_basic as basic
import opengeode_py_model as model

def run_test_brep():
    brep = model.BRep()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    model.load_brep(brep, os.path.join(data_dir, "test_v5.og_brep"))
    model.convert_brep_surface_meshes_into_triangulated_surfaces( brep )
    model.convert_brep_block_meshes_into_tetrahedral_solids( brep )

    file_io = "test_triangulated_surfaces." + brep.native_extension()
    model.save_brep(brep, file_io)
    brep2 = model.BRep()
    model.load_brep(brep2, file_io)

def run_test_section():
    section = model.Section()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    model.load_section(section, os.path.join(data_dir, "test_v5.og_sctn"))
    model.convert_section_surface_meshes_into_triangulated_surfaces( section )

    file_io = "test_triangulated_surfaces." + section.native_extension()
    model.save_section(section, file_io)
    section2 = model.Section()
    model.load_section(section2, file_io)

if __name__ == '__main__':
    run_test_brep()
    run_test_section()
