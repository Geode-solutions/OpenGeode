# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2022 Geode-solutions
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

import os, sys, platform
if sys.version_info >= (3,8,0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as geode_basic
import opengeode_py_mesh as geode_mesh
import opengeode_py_model as geode_model

def run_test_brep():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    brep = geode_model.load_brep(os.path.join(data_dir, "test_mesh3.og_brep"))

    for block in brep.blocks():
        for polyhedron_id in range( block.mesh().nb_polyhedra() ):
            for facet_id in range( block.mesh().nb_polyhedron_facets( polyhedron_id ) ):
                facet_vertices = block.mesh().polyhedron_facet_vertices( geode_mesh.PolyhedronFacet( polyhedron_id, facet_id) )
                facet_unique_vertices = [brep.unique_vertex( geode_model.MeshComponentVertex( block.component_id(), facet_vertices[i] ) ) for i in range(len(facet_vertices))]
                polyhedra_around_facet = geode_model.block_mesh_polyhedra_from_unique_vertices_facet( brep, block, facet_unique_vertices )
                if geode_mesh.PolyhedronFacet([polyhedron_id,facet_id]) not in polyhedra_around_facet :
                    raise ValueError( "[Test] Facet of polyhedron could not be linked to the polyhedron through its unique vertices." )

if __name__ == '__main__':
    run_test_brep()
