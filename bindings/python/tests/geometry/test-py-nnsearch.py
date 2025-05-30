# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2025 Geode-solutions
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
    for path in [x.strip() for x in os.environ["PATH"].split(";") if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic
import opengeode_py_geometry as geom

if __name__ == "__main__":
    search = geom.NNSearch2D(
        [
            geom.Point2D([0.1, 4.2]),
            geom.Point2D([5.9, 7.3]),
            geom.Point2D([1.8, -5]),
            geom.Point2D([-7.3, -1.6]),
        ]
    )

    if search.closest_neighbor(geom.Point2D([0, 0])) != 0:
        raise ValueError("[Test] Error in closest neighbor")
    if search.closest_neighbor(geom.Point2D([1, -4])) != 2:
        raise ValueError("[Test] Error in closest neighbor")

    answer_radius = [0, 2]
    if search.radius_neighbors(geom.Point2D([0, 0]), 5.4) != answer_radius:
        raise ValueError("[Test] Error in radius neighbors")

    answer_neighbors = [2, 0]
    if search.neighbors(geom.Point2D([-1, -1]), 2) != answer_neighbors:
        raise ValueError("[Test] Error in neighbors")

    p0 = geom.Point3D([0.1, 2.9, 5.4])
    p1 = geom.Point3D([2.4, 8.1, 7.6])
    p2 = geom.Point3D([8.1, 4.2, 3.8])
    p3 = geom.Point3D([3.1, 9.4, 9.7])
    colocator = geom.NNSearch3D([p0, p0, p1, p0, p2, p1, p3])

    colocated_info = colocator.radius_colocated_index_mapping(basic.GLOBAL_EPSILON)
    if colocated_info.nb_colocated_points() != 3:
        raise ValueError("[Test] Should be 3 colocated points")
