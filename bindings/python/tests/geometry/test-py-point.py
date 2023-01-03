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
# all copies or substantial portions of the Software.
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

import opengeode_py_geometry as geom


def test_comparison():
    p = geom.Point3D([2, 1.0, 2.6])
    p2 = p
    if p != p2:
        raise ValueError("[Test] Points should be equal")

    P = geom.Point2D([15, 2.6])
    P2 = geom.Point2D([16, 2.6])
    if P == P2:
        raise ValueError("[Test] Points should be different")

    p_epsilon = geom.Point3D([2.0000000001, 1, 2.6])
    if not p.inexact_equal(p_epsilon, 0.0001):
        raise ValueError("[Test] Points should be almost equal")


def test_operators():
    p = geom.Point3D()
    p.set_value(0, 2)
    p.set_value(1, 1)
    p.set_value(2, 2.6)
    p2 = p
    answer = geom.Point3D([4, 2, 5.2])
    if p + p2 != answer:
        raise ValueError("[Test] Points should be equal")
    if p * 2 != answer:
        raise ValueError("[Test] Points should be equal")
    if p - p2 != geom.Point3D():
        raise ValueError("[Test] Points should be equal")
    if answer / 2 != p:
        raise ValueError("[Test] Points should be equal")


if __name__ == '__main__':
    test_comparison()
    test_operators()
