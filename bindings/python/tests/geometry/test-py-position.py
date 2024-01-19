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


def test_point_side_to_segment():
    a = geometry.Point2D([0.0, 0.0])
    b = geometry.Point2D([1.0, 0.0])
    segment2D = geometry.Segment2D(a, b)

    side_a = geometry.point_side_to_segment2D(a, segment2D)
    if side_a != geometry.Side.zero:
        raise ValueError(
            "[Test] Wrong result for point_side_to_segment2D with query point a")

    side_b = geometry.point_side_to_segment2D(b, segment2D)
    if side_b != geometry.Side.zero:
        raise ValueError(
            "[Test] Wrong result for point_side_to_segment2D with query point b")

    side_q1 = geometry.point_side_to_segment2D(
        geometry.Point2D([0.5, 0]), segment2D)
    if side_q1 != geometry.Side.zero:
        raise ValueError(
            "[Test] Wrong result for point_side_to_segment2D with query point q1")

    side_q2 = geometry.point_side_to_segment2D(
        geometry.Point2D([10.0, 0]), segment2D)
    if side_q2 != geometry.Side.zero:
        raise ValueError(
            "[Test] Wrong result for point_side_to_segment2D with query point q2")

    side_q3 = geometry.point_side_to_segment2D(
        geometry.Point2D([0.3, 2.0]), segment2D)
    if side_q3 != geometry.Side.positive:
        raise ValueError(
            "[Test] Wrong result for point_side_to_segment2D with query point q3")

    side_q4 = geometry.point_side_to_segment2D(
        geometry.Point2D([12.0, -6.0]), segment2D)
    if side_q4 != geometry.Side.negative:
        raise ValueError(
            "[Test] Wrong result for point_side_to_segment2D with query point q4")

    side_q5 = geometry.point_side_to_segment2D(
        geometry.Point2D([0.0, -1e-10]), segment2D)
    if side_q5 != geometry.Side.negative:
        raise ValueError(
            "[Test] Wrong result for point_side_to_segment2D with query point q5")


def test_point_side_to_plane():
    origin = geometry.Point3D([0.0, 0.0, 2.0])
    normal = geometry.Vector3D([0.0, 0.0, 4.0])
    plane = geometry.Plane(normal, origin)

    side_q1 = geometry.point_side_to_plane3D(
        geometry.Point3D([0.5, 1.0, 2.0]), plane)
    if side_q1 != geometry.Side.zero:
        raise ValueError(
            "[Test] Wrong result for point_side_to_plane3D with query point q1")

    side_q2 = geometry.point_side_to_plane3D(
        geometry.Point3D([0.5, 1.0, 3.0]), plane)
    if side_q2 != geometry.Side.positive:
        raise ValueError(
            "[Test] Wrong result for point_side_to_plane3D with query point q2")

    side_q3 = geometry.point_side_to_plane3D(
        geometry.Point3D([0.5, 1.0, 1.0]), plane)
    if side_q3 != geometry.Side.negative:
        raise ValueError(
            "[Test] Wrong result for point_side_to_plane3D with query point q3")

    side_q4 = geometry.point_side_to_plane3D(
        geometry.Point3D([0.5, 1.0, 2.0 + 1e-10]), plane)
    if side_q4 != geometry.Side.positive:
        raise ValueError(
            "[Test] Wrong result for point_side_to_plane3D with query point q4")

    side_q5 = geometry.point_side_to_plane3D(
        geometry.Point3D([0.5, 1.0, 2.0 - 1e-10]), plane)
    if side_q5 != geometry.Side.negative:
        raise ValueError(
            "[Test] Wrong result for point_side_to_plane3D with query point q5")


def test_point_segment_position_2d():
    a = geometry.Point2D([0.0, 0.0])
    b = geometry.Point2D([1.0, 0.0])
    segment2D = geometry.Segment2D(a, b)

    position_a = geometry.point_segment_position2D(a, segment2D)
    if position_a != geometry.Position.vertex0:
        raise ValueError(
            "[Test] Wrong result for point_segment_position2D_2d with query point a")

    position_b = geometry.point_segment_position2D(b, segment2D)
    if position_b != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for point_segment_position2D_2d with query point b")

    position_q1 = geometry.point_segment_position2D(
        geometry.Point2D([0.5, 0]), segment2D)
    if position_q1 != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position2D_2d with query point q1")

    position_q2 = geometry.point_segment_position2D(
        geometry.Point2D([10.0, 0]), segment2D)
    if position_q2 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position2D_2d with query point q2")

    position_q3 = geometry.point_segment_position2D(
        geometry.Point2D([0.3, 2.0]), segment2D)
    if position_q3 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position2D_2d with query point q3")

    position_q4 = geometry.point_segment_position2D(
        geometry.Point2D([12.0, -6.0]), segment2D)
    if position_q4 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position2D_2d with query point q4")

    position_q5 = geometry.point_segment_position2D(
        geometry.Point2D([0.5, -1e-10]), segment2D)
    if position_q5 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position2D_2d with query point q5")


def test_point_segment_position_3d():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    segment3D = geometry.Segment3D(a, b)

    position_a = geometry.point_segment_position3D(a, segment3D)
    if position_a != geometry.Position.vertex0:
        raise ValueError(
            "[Test] Wrong result for point_segment_position3D_3d with query point a")

    position_b = geometry.point_segment_position3D(b, segment3D)
    if position_b != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for point_segment_position3D_3d with query point b")

    position_q1 = geometry.point_segment_position3D(
        geometry.Point3D([0.5, 0, 0]), segment3D)
    if position_q1 != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position3D_3d with query point q1")

    position_q2 = geometry.point_segment_position3D(
        geometry.Point3D([10.0, 0, 0]), segment3D)
    if position_q2 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position3D_3d with query point q2")

    position_q3 = geometry.point_segment_position3D(
        geometry.Point3D([0.3, 2.0, 0]), segment3D)
    if position_q3 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position3D_3d with query point q3")

    position_q4 = geometry.point_segment_position3D(
        geometry.Point3D([12.0, -6.0, 0]), segment3D)
    if position_q4 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position3D_3d with query point q4")

    position_q5 = geometry.point_segment_position3D(
        geometry.Point3D([0.5, -1e-10, 0]), segment3D)
    if position_q5 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_segment_position3D_3d with query point q5")


def test_point_segment_position():
    test_point_segment_position_2d()
    test_point_segment_position_3d()


def test_point_triangle_position_2d():
    a = geometry.Point2D([0.0, 0.0])
    b = geometry.Point2D([1.0, 0.0])
    c = geometry.Point2D([1.0, 1.0])
    triangle2D = geometry.Triangle2D(a, b, c)

    position_a = geometry.point_triangle_position2D(a, triangle2D)
    if position_a != geometry.Position.vertex0:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position2D_2d with query point a")

    position_b = geometry.point_triangle_position2D(b, triangle2D)
    if position_b != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position2D_2d with query point b")

    position_q1 = geometry.point_triangle_position2D(
        geometry.Point2D([0.5, 0]), triangle2D)
    if position_q1 != geometry.Position.edge0:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position2D_2d with query point q1")

    position_q2 = geometry.point_triangle_position2D(
        geometry.Point2D([0.0, 1.0]), triangle2D)
    if position_q2 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position2D_2d with query point q2")

    position_q3 = geometry.point_triangle_position2D(
        geometry.Point2D([2.0, 1.0]), triangle2D)
    if position_q3 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position2D_2d with query point q3")

    position_q4 = geometry.point_triangle_position2D(
        geometry.Point2D([0.5, 1e-10]), triangle2D)
    if position_q4 != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position2D_2d with query point q4")

    position_q5 = geometry.point_triangle_position2D(
        geometry.Point2D([0.5, -1e-10]), triangle2D)
    if position_q5 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position2D_2d with query point q5")


def test_point_triangle_position_3d():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    c = geometry.Point3D([1.0, 1.0, 0.0])
    triangle3D = geometry.Triangle3D(a, b, c)

    position_qout = geometry.point_triangle_position3D(
        geometry.Point3D([0.75, -0.25, 0.0]), triangle3D)
    if position_qout != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point qout")

    position_q0 = geometry.point_triangle_position3D(
        geometry.Point3D([0.75, 0.25, 0.0]), triangle3D)
    if position_q0 != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point q0")

    position_a = geometry.point_triangle_position3D(a, triangle3D)
    if position_a != geometry.Position.vertex0:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point a")

    position_b = geometry.point_triangle_position3D(b, triangle3D)
    if position_b != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point b")

    position_q1 = geometry.point_triangle_position3D(
        geometry.Point3D([0.5, 0.0, 0.0]), triangle3D)
    if position_q1 != geometry.Position.edge0:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point q1")

    position_q2 = geometry.point_triangle_position3D(
        geometry.Point3D([0.0, 1.0, 0.0]), triangle3D)
    if position_q2 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point q2")

    position_q3 = geometry.point_triangle_position3D(
        geometry.Point3D([2.0, 1.0, 0.0]), triangle3D)
    if position_q3 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point q3")

    position_q4 = geometry.point_triangle_position3D(
        geometry.Point3D([0.5, 1e-10, 0.0]), triangle3D)
    if position_q4 != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point q4")

    position_q5 = geometry.point_triangle_position3D(
        geometry.Point3D([0.5, -1e-10, 0.0]), triangle3D)
    if position_q5 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point q5")

    position_q6 = geometry.point_triangle_position3D(
        geometry.Point3D([0.5, 1e-10, 1e-10]), triangle3D)
    if position_q6 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_triangle_position3D_3d with query point q6")


def test_point_triangle_position():
    test_point_triangle_position_2d()
    test_point_triangle_position_3d()


def test_point_tetrahedron_position():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    c = geometry.Point3D([0.0, 1.0, 0.0])
    d = geometry.Point3D([0.0, 0.0, 1.0])
    tetra = geometry.Tetrahedron(a, b, c, d)

    position_a = geometry.point_tetrahedron_position3D(a, tetra)
    if position_a != geometry.Position.vertex0:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point a")

    position_b = geometry.point_tetrahedron_position3D(b, tetra)
    if position_b != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point b")

    position_c = geometry.point_tetrahedron_position3D(c, tetra)
    if position_c != geometry.Position.vertex2:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point c")

    position_d = geometry.point_tetrahedron_position3D(d, tetra)
    if position_d != geometry.Position.vertex3:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point d")

    position_q1 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.5, 1e-10, 1e-10]), tetra)
    if position_q1 != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q1")

    position_q2 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.5, 1e-10, -1e-10]), tetra)
    if position_q2 != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q2")

    position_q3 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.5, 1e-10, 0.0]), tetra)
    if position_q3 != geometry.Position.facet3:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q3")

    position_q4 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.5, 0.0, 0.0]), tetra)
    if position_q4 != geometry.Position.edge01:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q4")

    position_q5 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.0, 0.5, 0.0]), tetra)
    if position_q5 != geometry.Position.edge02:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q5")

    position_q6 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.0, 0.0, 0.5]), tetra)
    if position_q6 != geometry.Position.edge03:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q6")

    position_q7 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.5, 0.5, 0.0]), tetra)
    if position_q7 != geometry.Position.edge12:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q7")

    position_q8 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.5, 0.0, 0.5]), tetra)
    if position_q8 != geometry.Position.edge13:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q8")

    position_q9 = geometry.point_tetrahedron_position3D(
        geometry.Point3D([0.0, 0.5, 0.5]), tetra)
    if position_q9 != geometry.Position.edge23:
        raise ValueError(
            "[Test] Wrong result for point_tetrahedron_position3D with query point q9")


if __name__ == '__main__':
    test_point_side_to_segment()
    test_point_side_to_plane()
    test_point_segment_position()
    test_point_triangle_position()
    test_point_tetrahedron_position()
