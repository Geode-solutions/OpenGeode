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


def test_perpendicular():
    v = geom.Vector2D([1.578, 1e-10])
    perp = geom.perpendicular(v).normalize()

    dot_product = v.value(0) * perp.value(0) + v.value(1) * perp.value(1)

    if perp.length() != 1 or dot_product != 0.:
        raise ValueError("[Test] Wrong result for normalized_perpendicular")


def test_dot_perpendicular():
    v1 = geom.Vector2D([0, 1])
    v2 = geom.Vector2D([1, 0])
    dot_perp = geom.dot_perpendicular(v1, v2)
    dot = geom.dot_perpendicular(geom.perpendicular(v1), v2)

    if dot_perp != -1 or dot != 0:
        raise ValueError("[Test] Wrong result for dot_perpendicular")


if __name__ == '__main__':
    test_perpendicular()
    test_dot_perpendicular()
