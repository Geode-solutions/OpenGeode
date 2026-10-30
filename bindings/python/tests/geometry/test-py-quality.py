# - * - coding : utf - 8 - * -
# Copyright( c ) 2019 - 2026 Geode - solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files( the "Software" ), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
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

import math

import opengeode_py_geometry as geom


def check(value, expected, message):
    if abs(value - expected) > 1e-6:
        raise ValueError("[Test] Wrong " + message + ": " + str(value))


def test_regular_tetrahedron():
    a = geom.Point3D([0.0, 0.0, 0.0])
    b = geom.Point3D([1.0, 0.0, 0.0])
    c = geom.Point3D([0.5, math.sqrt(3) / 2, 0.0])
    d = geom.Point3D([0.5, math.sqrt(3) / 6, math.sqrt(2.0 / 3.0)])
    tetra = geom.Tetrahedron(a, b, c, d)
    check(geom.tetrahedron_aspect_ratio(tetra), 1, "aspect ratio of regular tetrahedron")
    check(geom.tetrahedron_volume_to_edge_ratio(tetra), 1,
          "volume to edge ratio of regular tetrahedron")
    check(geom.tetrahedron_volume_to_facet_ratio(tetra), 1,
          "volume to facet ratio of regular tetrahedron")
    check(geom.tetrahedron_collapse_aspect_ratio(tetra), 1,
          "collapse aspect ratio of regular tetrahedron")


def test_other_tetrahedron():
    a = geom.Point3D([0.0, 0.0, 0.0])
    b = geom.Point3D([4.0, 0.0, 0.0])
    c = geom.Point3D([2.0, 1.0, 0.0])
    d = geom.Point3D([2.0, 1.0, 1.0])
    tetra = geom.Tetrahedron(a, b, c, d)
    check(geom.tetrahedron_aspect_ratio(tetra), 2.884068, "aspect ratio")
    check(geom.tetrahedron_volume_to_edge_ratio(tetra),
          0.341354, "volume to edge ratio")
    check(geom.tetrahedron_volume_to_facet_ratio(tetra),
          0.346732, "volume to facet ratio")
    check(geom.tetrahedron_collapse_aspect_ratio(tetra),
          4.618802, "collapse aspect ratio")


def test_flat_tetrahedron():
    a = geom.Point3D([0.0, 0.0, 0.0])
    b = geom.Point3D([1.0, 0.0, 0.0])
    c = geom.Point3D([0.0, 1.0, 0.0])
    d = geom.Point3D([1.0, 1.0, 0.0])
    tetra = geom.Tetrahedron(a, b, c, d)
    if geom.tetrahedron_aspect_ratio(tetra) < 1e300:
        raise ValueError("[Test] Wrong aspect ratio of flat tetrahedron")
    check(geom.tetrahedron_volume_to_facet_ratio(tetra), 0,
          "volume to facet ratio of flat tetrahedron")


def test_triangle_angle_based_quality():
    a2 = geom.Point2D([0.0, 0.0])
    b2 = geom.Point2D([1.0, 0.0])
    c2 = geom.Point2D([0.5, math.sqrt(3) / 2])
    triangle2D = geom.Triangle2D(a2, b2, c2)
    check(geom.triangle_angle_based_quality2D(triangle2D),
          1, "quality of equilateral triangle")
    a3 = geom.Point3D([0.0, 0.0, 0.0])
    b3 = geom.Point3D([1.0, 0.0, 0.0])
    c3 = geom.Point3D([0.0, 1.0, 0.0])
    triangle3D = geom.Triangle3D(a3, b3, c3)
    check(geom.triangle_angle_based_quality3D(triangle3D),
          2 / (1 + math.sqrt(2)), "quality of right triangle")
    d3 = geom.Point3D([2.0, 0.0, 0.0])
    flat = geom.Triangle3D(a3, b3, d3)
    check(geom.triangle_angle_based_quality3D(flat),
          0, "quality of flat triangle")


if __name__ == '__main__':
    test_regular_tetrahedron()
    test_other_tetrahedron()
    test_flat_tetrahedron()
    test_triangle_angle_based_quality()
