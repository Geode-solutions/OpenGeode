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
import opengeode_py_geometry
import opengeode_py_mesh as mesh
import opengeode_py_model as model


def run_test_brep():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(
        test_dir, "../../../../tests/data"))
    brep = model.load_brep(os.path.join(data_dir, "test_mesh3.og_brep"))

    for block in brep.blocks():
        block_mesh = block.mesh()
        for surface in brep.surfaces():
            if not brep.is_block_boundary(surface, block) and not brep.is_surface_in_block_internals(surface, block):
                continue
            surface_mesh = surface.mesh()
            for polygon_id in range(surface_mesh.nb_polygons()):
                block_facets_vertices = model.block_vertices_from_surface_polygon(
                    brep, block, surface, polygon_id)
                oriented_block_facets_vertices = model.oriented_block_vertices_from_surface_polygon(
                    brep, block, surface, polygon_id)
                if brep.is_block_boundary(surface, block):
                    if not oriented_block_facets_vertices.nb_facets() == 1:
                        raise ValueError("[Test] " + oriented_block_facets_vertices.nb_facets(
                        ) + " polyhedra were found from boundary surface polygon.")
                elif brep.is_surface_in_block_internals(surface, block):
                    if not oriented_block_facets_vertices.nb_facets() == 2:
                        raise ValueError("[Test] " + oriented_block_facets_vertices.nb_facets(
                        ) + " polyhedra were found from internal surface polygon.")
                for polygon_vertex_id in range(len(block_facets_vertices[0].vertices)):
                    if not surface_mesh.point(surface_mesh.polygon_vertex(mesh.PolygonVertex(polygon_id, polygon_vertex_id))).inexact_equal(block_mesh.point(block_facets_vertices[0].vertices[polygon_vertex_id]), 1e-7):
                        raise ValueError(
                            "[Test] Point on the edge and on the surface have different positions.")

    for surface in brep.surfaces():
        surface_mesh = surface.mesh()
        for line in brep.lines():
            if not brep.is_surface_boundary(line, surface) and not brep.is_line_in_surface_internals(line, surface):
                continue
            line_mesh = line.mesh()
            for edge_id in range(line_mesh.nb_edges()):
                surface_edge_vertices = model.brep_surface_vertices_from_line_edge(
                    brep, surface, line, edge_id)
                oriented_surface_edge_vertices = model.brep_oriented_surface_vertices_from_line_edge(
                    brep, surface, line, edge_id)
                if len(surface_edge_vertices) != oriented_surface_edge_vertices.nb_edges():
                    raise ValueError(
                        "[Test] Different number of polygons for surface_vertices_from_line_edge and oriented_surface_vertices_from_line_edge functions.")
                if brep.is_surface_boundary(line, surface):
                    if len(surface_edge_vertices) != 1:
                        raise ValueError(
                            "[Test] " + len(surface_edge_vertices) + " polygons were found from boundary line edge.")
                elif brep.is_line_in_surface_internals(line, surface):
                    if len(surface_edge_vertices) != 2:
                        raise ValueError(
                            "[Test] " + len(surface_edge_vertices) + " polygons were found from internal line edge.")
                for edge_vertex_id in range(2):
                    if not line_mesh.point(line_mesh.edge_vertex(mesh.EdgeVertex(edge_id, edge_vertex_id))).inexact_equal(surface_mesh.point(surface_edge_vertices[0].vertices[edge_vertex_id]), 1e-7):
                        raise ValueError(
                            "[Test] Point on the edge and on the surface have different positions.")


if __name__ == '__main__':
    model.OpenGeodeModel.initialize()
    run_test_brep()
