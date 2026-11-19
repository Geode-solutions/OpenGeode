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

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_image as image
import opengeode_py_mesh as mesh
import opengeode_py_model as model


def check_hashable(make_equal_pair, make_different):
    first, second = make_equal_pair()
    other = make_different()
    if first != second:
        raise ValueError("[Test] Objects should be equal")
    if hash(first) != hash(second):
        raise ValueError("[Test] Equal objects should have equal hashes")
    if len({first, second}) != 1:
        raise ValueError("[Test] Equal objects should collapse in a set")
    if len({first, other}) != 2:
        raise ValueError("[Test] Different objects should be distinct")
    if {first: 1}[second] != 1:
        raise ValueError("[Test] Equal objects should be the same dict key")


def test_geometry():
    check_hashable(lambda: (geometry.Point1D([1.0]), geometry.Point1D([1.0])),
                   lambda: geometry.Point1D([2.0]))
    check_hashable(lambda: (geometry.Point2D([1.0, 2.0]),
                            geometry.Point2D([1.0, 2.0])),
                   lambda: geometry.Point2D([1.0, 3.0]))
    check_hashable(lambda: (geometry.Point3D([1.0, 2.0, 3.0]),
                            geometry.Point3D([1.0, 2.0, 3.0])),
                   lambda: geometry.Point3D([1.0, 2.0, 4.0]))
    check_hashable(lambda: (geometry.Vector3D([1.0, 2.0, 3.0]),
                            geometry.Vector3D([1.0, 2.0, 3.0])),
                   lambda: geometry.Vector3D([1.0, 2.0, 4.0]))
    check_hashable(lambda: (geometry.Vector2D([1.0, 2.0]),
                            geometry.Vector2D([1.0, 2.0])),
                   lambda: geometry.Vector2D([1.0, 3.0]))
    check_hashable(lambda: (geometry.Vector1D([1.0]),
                            geometry.Vector1D([1.0])),
                   lambda: geometry.Vector1D([2.0]))
    if hash(geometry.Point2D([0.0, 0.0])) != hash(geometry.Point2D([-0.0, 0.0])):
        raise ValueError("[Test] 0.0 and -0.0 points must hash equally")
    check_hashable(lambda: (geometry.Angle.create_from_degrees(30.0),
                            geometry.Angle.create_from_degrees(30.0)),
                   lambda: geometry.Angle.create_from_degrees(40.0))


def test_image():
    check_hashable(lambda: (image.RGBColor(1, 2, 3), image.RGBColor(1, 2, 3)),
                   lambda: image.RGBColor(1, 2, 4))
    check_hashable(lambda: (image.GreyscaleColor(5), image.GreyscaleColor(5)),
                   lambda: image.GreyscaleColor(6))


def test_mesh_elements():
    check_hashable(lambda: (mesh.EdgeVertex(1, 0), mesh.EdgeVertex(1, 0)),
                   lambda: mesh.EdgeVertex(1, 1))
    check_hashable(lambda: (mesh.PolygonVertex(1, 0), mesh.PolygonVertex(1, 0)),
                   lambda: mesh.PolygonVertex(1, 1))
    check_hashable(lambda: (mesh.PolygonEdge(1, 0), mesh.PolygonEdge(1, 0)),
                   lambda: mesh.PolygonEdge(1, 1))
    check_hashable(lambda: (mesh.PolyhedronVertex(1, 0),
                            mesh.PolyhedronVertex(1, 0)),
                   lambda: mesh.PolyhedronVertex(1, 1))
    check_hashable(lambda: (mesh.PolyhedronFacet(1, 0),
                            mesh.PolyhedronFacet(1, 0)),
                   lambda: mesh.PolyhedronFacet(1, 1))
    check_hashable(lambda: (mesh.PolyhedronFacetVertex(mesh.PolyhedronFacet(1, 0), 2),
                            mesh.PolyhedronFacetVertex(mesh.PolyhedronFacet(1, 0), 2)),
                   lambda: mesh.PolyhedronFacetVertex(mesh.PolyhedronFacet(1, 0), 3))
    check_hashable(lambda: (mesh.PolyhedronFacetEdge(mesh.PolyhedronFacet(1, 0), 2),
                            mesh.PolyhedronFacetEdge(mesh.PolyhedronFacet(1, 0), 2)),
                   lambda: mesh.PolyhedronFacetEdge(mesh.PolyhedronFacet(1, 0), 1))


def test_component_mesh_vertex():
    cid = model.ComponentID(model.ComponentType("Corner"), basic.uuid())
    other = model.ComponentID(model.ComponentType("Line"), basic.uuid())
    check_hashable(lambda: (model.ComponentMeshVertex(cid, 1),
                            model.ComponentMeshVertex(cid, 1)),
                   lambda: model.ComponentMeshVertex(other, 1))


def test_hash():
    test_geometry()
    test_image()
    test_mesh_elements()
    test_component_mesh_vertex()


if __name__ == '__main__':
    test_hash()
