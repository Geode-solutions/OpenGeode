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


def check_brep_prism(brep):
    if brep.nb_corners() != 7:
        raise ValueError("[Test] Filtered prism model has ", brep.nb_corners(),
                         " Corners, should have 7 Corners")
    if brep.nb_lines() != 10:
        raise ValueError("[Test] Filtered prism model has ", brep.nb_lines(),
                         " Lines, should have 10 Lines")
    if brep.nb_surfaces() != 5:
        raise ValueError("[Test] Filtered prism model has ", brep.nb_surfaces(),
                         " Surfaces, should have 5 Surfaces")
    if brep.nb_blocks() != 1:
        raise ValueError("[Test] Filtered prism model has ", brep.nb_blocks(),
                         " Blocks, should have 1 Block")


def check_brep_dangling(brep):
    if brep.nb_corners() != 8:
        raise ValueError("[Test] Filtered dangling model has ", brep.nb_corners(),
                         " Corners, should have 8 Corners")
    if brep.nb_lines() != 12:
        raise ValueError("[Test] Filtered dangling model has ", brep.nb_lines(),
                         " Lines, should have 12 Lines")
    if brep.nb_surfaces() != 6:
        raise ValueError("[Test] Filtered dangling model has ", brep.nb_surfaces(),
                         " Surfaces, should have 6 Surfaces")
    if brep.nb_blocks() != 1:
        raise ValueError("[Test] Filtered dangling model has ", brep.nb_blocks(),
                         " Blocks, should have 1 Block")


if __name__ == '__main__':
    model.OpenGeodeModel.initialize()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    brep = model.load_brep(os.path.join(data_dir, "prism_curve.og_brep"))
    model.filter_brep_components_with_regards_to_blocks(brep)
    check_brep_prism(brep)

    brep2 = model.load_brep(os.path.join(data_dir, "dangling.og_brep"))
    model.filter_brep_components_with_regards_to_blocks(brep2)
    check_brep_dangling(brep2)
