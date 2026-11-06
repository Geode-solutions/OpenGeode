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


def check_result(crss):
    if len(crss) != 1:
        raise ValueError("[Test] Wrong number of CRS")
    for crs_type, crs_name in crss:
        print(crs_type.get())
        if crs_name != "points":
            raise ValueError("[Test] Wrong CRS name")


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))

    brep_filename = os.path.join(data_dir, "random_dfn.og_brep")
    brep = opengeode.load_one(model.load_brep(brep_filename), brep_filename)
    check_result(model.brep_coordinate_reference_systems(brep))

    section_filename = os.path.join(data_dir, "fractures.og_sctn")
    section = opengeode.load_one(model.load_section(section_filename), section_filename)
    check_result(model.section_coordinate_reference_systems(section))
