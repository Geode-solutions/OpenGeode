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


def test_lexicographic_mapping():
    pt0 = geom.Point3D([0., 0., 0.])
    pt1 = geom.Point3D([1., 0., 0.])
    pt2 = geom.Point3D([2., 0., 0.])
    pt3 = geom.Point3D([2., 1., 0.])
    pt4 = geom.Point3D([1., 1., 0.])
    pt5 = geom.Point3D([0., 1., 0.])
    pt6 = geom.Point3D([0., 2., 0.])
    pt7 = geom.Point3D([1., 2., 0.])
    pt8 = geom.Point3D([2., 2., 0.])
    pt9 = geom.Point3D([0., 0., 1.])
    pt10 = geom.Point3D([1., 0., 1.])

    pts = [pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7,
           pt8, pt9, pt10]
    mapping = geom.lexicographic_mapping3D(pts)
    answer = [0, 9, 5, 6, 1, 10, 4, 7, 2, 3, 8]
    for m in range(len(mapping)):
        if mapping[m] != answer[m]:
            raise ValueError("[Test] Wrong result in lexicographic sort")


if __name__ == '__main__':
    test_lexicographic_mapping()
