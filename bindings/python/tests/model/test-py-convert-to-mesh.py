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
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    brep_filename = os.path.join(data_dir, "test_mesh3.og_brep")
    brep = opengeode.load_one(model.load_brep(brep_filename), brep_filename)

    curve, curve_mappings = model.convert_brep_into_curve(brep)
    surface, surface_mappings = model.convert_brep_into_surface(brep)
    solid, solid_mappings = model.convert_brep_into_solid(brep)
    if curve.nb_vertices() != 154:
        raise ValueError("[Test] BRep - Wrong number of curve vertices")
    if curve.nb_edges() != 162:
        raise ValueError("[Test] BRep - Wrong number of curve edges")

    if surface.nb_vertices() != 840:
        raise ValueError("[Test] BRep - Wrong number of surface vertices")
    if surface.nb_polygons() != 1716:
        raise ValueError("[Test] BRep - Wrong number of surface polygons")

    if solid.nb_vertices() != 1317:
        raise ValueError("[Test] BRep - Wrong number of solid vertices")
    if solid.nb_polyhedra() != 5709:
        raise ValueError("[Test] BRep - Wrong number of solid polyhedra")


    if curve_mappings.line_edges_mapping.size() != curve.nb_edges():
        raise ValueError("[Test] BRep - Wrong number of curve edge mappings")
    if surface_mappings.surface_polygons_mapping.size() != surface.nb_polygons():
        raise ValueError("[Test] BRep - Wrong number of surface polygon mappings")
    if solid_mappings.solid_polyhedra_mapping.size() != solid.nb_polyhedra():
        raise ValueError("[Test] BRep - Wrong number of solid polyhedron mappings")
    if solid_mappings.unique_vertices_mapping.size() == 0:
        raise ValueError("[Test] BRep - Missing unique vertices mapping")

    element = solid_mappings.solid_polyhedra_mapping.out2in(0)
    if solid_mappings.solid_polyhedra_mapping.in2out(element) != 0:
        raise ValueError("[Test] BRep - Mapping should be bijective")
    if brep.block(element.mesh_id).id() != element.mesh_id:
        raise ValueError("[Test] BRep - Mapped element should come from a block")


def run_test_section():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    section_filename = os.path.join(data_dir, "quad.og_sctn")
    section = opengeode.load_one(model.load_section(section_filename), section_filename)

    curve, curve_mappings = model.convert_section_into_curve(section)
    surface, surface_mappings = model.convert_section_into_surface(section)
    if curve.nb_vertices() != 0:
        raise ValueError("[Test] Section - Wrong number of curve vertices")
    if curve.nb_edges() != 0:
        raise ValueError("[Test] Section - Wrong number of curve edges")

    if surface.nb_vertices() != 4:
        raise ValueError("[Test] Section - Wrong number of surface vertices")
    if surface.nb_polygons() != 1:
        raise ValueError("[Test] Section - Wrong number of surface polygons")
    if surface_mappings.surface_polygons_mapping.size() != 1:
        raise ValueError("[Test] Section - Wrong number of polygon mappings")
    if curve_mappings.line_edges_mapping.size() != 0:
        raise ValueError("[Test] Section - Wrong number of edge mappings")


if __name__ == "__main__":
    model.OpenGeodeModelLibrary.initialize()
    run_test_brep()
    run_test_section()
