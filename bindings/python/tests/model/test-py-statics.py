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

import inspect

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_image as image
import opengeode_py_mesh as mesh
import opengeode_py_model as model


def check_static(python_class, name):
    attribute = inspect.getattr_static(python_class, name)
    if not isinstance(attribute, staticmethod):
        raise ValueError(f"[Test] {python_class.__name__}.{name} should be a "
                         "static method")


def test_static_methods():
    for library in [basic.OpenGeodeBasicLibrary,
                    geometry.OpenGeodeGeometryLibrary,
                    image.OpenGeodeImageLibrary,
                    mesh.OpenGeodeMeshLibrary,
                    model.OpenGeodeModelLibrary]:
        check_static(library, "initialize")
    for factory in [mesh.GraphInputFactory,
                    mesh.GraphOutputFactory,
                    mesh.PointSetInputFactory3D]:
        check_static(factory, "list_creators")
        check_static(factory, "has_creator")
    check_static(mesh.GridCoordinateReferenceSystem2D, "type_name_static")
    check_static(geometry.Angle, "create_from_degrees")
    check_static(geometry.Angle, "create_from_radians")


def test_static_call_on_instance():
    angle = geometry.Angle.create_from_degrees(90.0)
    other = angle.create_from_degrees(180.0)
    if other.degrees() != 180.0:
        raise ValueError("[Test] Static call on instance gave a wrong result")
    if not mesh.GridCoordinateReferenceSystem2D.type_name_static():
        raise ValueError("[Test] type_name_static should return a name")


if __name__ == '__main__':
    test_static_methods()
    test_static_call_on_instance()
