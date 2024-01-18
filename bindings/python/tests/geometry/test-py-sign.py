# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2024 Geode-solutions
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

import opengeode_py_geometry as geometry


def test_triangle_sign_2d():
    a = geometry.Point2D([0.0, 0.0])
    b = geometry.Point2D([1.0, 0.0])
    c = geometry.Point2D([1.0, 1.0])
    triangle2D = geometry.Triangle2D(a, b, c)

    side = geometry.triangle_area_sign2D(triangle2D)
    if side != geometry.Side.positive:
        raise ValueError("[Test] Wrong result for triangle_area_sign2D")


def test_triangle_sign_3d():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 1.0])
    c = geometry.Point3D([1.0, 1.0, 2.0])
    triangle3D = geometry.Triangle3D(a, b, c)

    side_z = geometry.triangle_area_sign3D(triangle3D, 2)
    if side_z != geometry.Side.positive:
        raise ValueError(
            "[Test] Wrong result for triangle_area_sign3D and axis 2")

    side_y = geometry.triangle_area_sign3D(triangle3D, 1)
    if side_y != geometry.Side.negative:
        raise ValueError(
            "[Test] Wrong result for triangle_area_sign3D and axis 1")

    side_x = geometry.triangle_area_sign3D(triangle3D, 0)
    if side_x != geometry.Side.negative:
        raise ValueError(
            "[Test] Wrong result for triangle_area_sign3sD and axis 0")


def test_triangle_sign():
    test_triangle_sign_2d()
    test_triangle_sign_3d()


if __name__ == '__main__':
    test_triangle_sign()
