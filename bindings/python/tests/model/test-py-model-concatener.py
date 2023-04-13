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


def check_concatenation(
  brep, nb_components ):
    if brep.nb_corners() != nb_components[0]:
        raise ValueError( "[Test] Concatenated model has ", brep.nb_corners(), " Corners, should have ", nb_components[0], " Corners" )
    if brep.nb_lines() != nb_components[1]:
        raise ValueError( "[Test] Concatenated model has ", brep.nb_lines(), " Lines, should have ", nb_components[1], " Lines" )
    if brep.nb_surfaces() != nb_components[2]:
        raise ValueError( "[Test] Concatenated model has ", brep.nb_surfaces(), " Surfaces, should have ", nb_components[2], " Surfaces" )
    if brep.nb_blocks() != nb_components[3]:
        raise ValueError( "[Test] Concatenated model has ", brep.nb_blocks(), " Blocks, should have ", nb_components[3], " Blocks" )
    if brep.nb_model_boundaries() != nb_components[4]:
        raise ValueError( "[Test] Concatenated model has ", brep.nb_model_boundaries(), " ModelBoundaries, should have ", nb_components[4], " ModelBoundaries" )

if __name__ == '__main__':
    model.OpenGeodeModelLibrary.initialize()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    brep = model.load_brep(os.path.join(data_dir, "prism_curve.og_brep"))
    brep2 = model.load_brep(os.path.join(data_dir, "dangling.og_brep" ) )
    nb_components = [ brep.nb_corners() + brep2.nb_corners(), brep.nb_lines() + brep2.nb_lines(), brep.nb_surfaces() + brep2.nb_surfaces(), brep.nb_blocks() + brep2.nb_blocks(), brep.nb_model_boundaries() + brep2.nb_model_boundaries() ]
    concatener = model.BRepConcatener( brep )
    concatener.concatenate( brep2 )
    check_concatenation( brep, nb_components )
    model.save_brep( brep, "concatenated_brep.og_brep" )
