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

import opengeode_py_model as model

def test_convert_brep_section(data_dir):
    brep = model.load_brep(os.path.join(data_dir, "random_dfn.og_brep"))
    section, _ = model.convert_brep_into_section(brep, 2)

    if section.nb_corners() != 172:
        raise ValueError("[Test] Section should have 172 corners")
    if section.nb_lines() != 288:
        raise ValueError("[Test] Section should have 288 lines")
    if section.nb_surfaces() != 117:
        raise ValueError("[Test] Section should have 117 surfaces")
    
    brep2, _ = model.convert_section_into_brep(section, 2 , 10.)
    if brep2.nb_corners() != 172:
        raise ValueError("[Test] BRep should have 172 corners")
    if brep2.nb_lines() != 288:
        raise ValueError("[Test] BRep should have 288 lines")
    if brep2.nb_surfaces() != 117:
        raise ValueError("[Test] BRep should have 117 surfaces")

def test_extrusion_section_to_brep(data_dir):
    section = model.load_section(os.path.join(data_dir, "fractures.og_sctn"))
    options = model.SectionExtruderOptions
    options.axis_to_extrude = 2
    options.min_coordinate = 0.
    options.max_coordinate = 10.
    brep = model.extrude_section_to_brep(section, options)

    if brep.nb_corners() != 2*section.nb_corners():
        raise ValueError("[Test] Extruded BRep have wrong number of corners")
    if brep.nb_lines() != 2*section.nb_lines()+section.nb_corners():
        raise ValueError("[Test] Extruded BRep have wrong number of lines")
    if brep.nb_surfaces() != 2*section.nb_surfaces()+section.nb_lines():
        raise ValueError("[Test] Extruded BRep have wrong number of surfaces")
    if brep.nb_blocks() != section.nb_surfaces():
        raise ValueError("[Test] Extruded BRep have wrong number of blocks")
    
if __name__ == '__main__':
    model.OpenGeodeModelLibrary.initialize()
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    test_convert_brep_section(data_dir)
    test_extrusion_section_to_brep(data_dir)
