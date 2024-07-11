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

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry


def test_line_sphere_intersection():
    sphere_origin = geometry.Point2D([2., 0.])
    sphere = geometry.Sphere2D(sphere_origin, 1.)
    line1_origin = geometry.Point2D([0., 0.5])
    line1_dest = geometry.Point2D([1., 0.5])
    line1 = geometry.InfiniteLine2D(
        geometry.Segment2D(line1_origin, line1_dest))
    line2_origin = geometry.Point2D([0., 1.])
    line2_dest = geometry.Point2D([1., 1.])
    line2 = geometry.InfiniteLine2D(
        geometry.Segment2D(line2_origin, line2_dest))
    line3_origin = geometry.Point2D([0., 2.])
    line3_dest = geometry.Point2D([1., 2.])
    line3 = geometry.InfiniteLine2D(
        geometry.Segment2D(line3_origin, line3_dest))

    intersection_points1 = geometry.line_sphere_intersection2D(line1, sphere)
    if not intersection_points1.has_intersection() or len(intersection_points1.result) != 2:
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query line1")

    intersection_points2 = geometry.line_sphere_intersection2D(line2, sphere)
    if not intersection_points2.has_intersection() or len(intersection_points2.result) != 1:
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query line 2")
    if intersection_points2.result[0] != geometry.Point2D([2.0, 1.0]):
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query line 2 (coords)")

    intersection_points3 = geometry.line_sphere_intersection2D(line3, sphere)
    if intersection_points3.has_intersection():
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query line 3")


def test_segment_sphere_intersection():
    origin = geometry.Point2D([0.0, 0.0])
    a = geometry.Point2D([2.0, 0.0])
    b = geometry.Point2D([1.5, 0.0])
    c = geometry.Point2D([2.5, 0.0])
    d = geometry.Point2D([3.0, 0.0])
    e = geometry.Point2D([4.0, 0.0])
    f = geometry.Point2D([5.0, 0.0])
    g = geometry.Point2D([4.0, 4.0])
    sphere = geometry.Sphere2D(a, 1.)
    segment1 = geometry.Segment2D(b, c)
    segment2 = geometry.Segment2D(a, d)
    segment3 = geometry.Segment2D(d, g)
    segment4 = geometry.Segment2D(origin, e)
    segment5 = geometry.Segment2D(e, f)

    intersection_points1 = geometry.segment_sphere_intersection2D(
        segment1, sphere)
    if intersection_points1.has_intersection():
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query segment 1")

    intersection_points2 = geometry.segment_sphere_intersection2D(
        segment2, sphere)
    if not intersection_points2.has_intersection() or len(intersection_points2.result) != 1:
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query segment 2")

    intersection_points3 = geometry.segment_sphere_intersection2D(
        segment3, sphere)
    if not intersection_points3.has_intersection() or len(intersection_points3.result) != 1:
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query segment 3")

    intersection_points4 = geometry.segment_sphere_intersection2D(
        segment4, sphere)
    if not intersection_points4.has_intersection() or len(intersection_points4.result) != 2:
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query segment 4")

    intersection_points5 = geometry.segment_sphere_intersection2D(
        segment5, sphere)
    if intersection_points5.has_intersection():
        raise ValueError(
            "[Test] Wrong result for line_sphere_intersection2D with query segment 5")


def test_line_line_intersection():
    a = geometry.Point2D([0.0, 0.0])
    b = geometry.Point2D([1.0, 0.0])
    c = geometry.Point2D([1.0, 1.0])
    d = geometry.Point2D([2.0, 1.0])
    line1 = geometry.InfiniteLine2D(geometry.Segment2D(a, b))
    line2 = geometry.InfiniteLine2D(geometry.Segment2D(a, b))
    line3 = geometry.InfiniteLine2D(geometry.Segment2D(b, c))
    line4 = geometry.InfiniteLine2D(geometry.Segment2D(c, d))
    line5 = geometry.InfiniteLine2D(geometry.Segment2D(a, c))

    intersection_point = geometry.line_line_intersection2D(line1, line2)
    if intersection_point.has_intersection():
        raise ValueError(
            "[Test] Wrong result for line_line_intersection2D with query lines 1 and 2")

    intersection_point1 = geometry.line_line_intersection2D(line1, line3)
    if not intersection_point1.has_intersection() or intersection_point1.result != b:
        raise ValueError(
            "[Test] Wrong result for line_line_intersection2D with query lines 1 and 3")

    intersection_point2 = geometry.line_line_intersection2D(line1, line4)
    if intersection_point2.has_intersection():
        raise ValueError(
            "[Test] Wrong result for line_line_intersection2D with query lines 1 and 4")

    intersection_point3 = geometry.line_line_intersection2D(line1, line5)
    if not intersection_point3.has_intersection() or intersection_point3.result != a:
        raise ValueError(
            "[Test] Wrong result for line_line_intersection2D with query lines 1 and 5")


def test_segment_segment_intersection():
    a = geometry.Point2D([0.0, 0.0])
    b = geometry.Point2D([1.0, 0.0])
    c = geometry.Point2D([2.0, 1.0])
    d = geometry.Point2D([3.0, 1.0])
    e = geometry.Point2D([2.0, 0.0])
    f = geometry.Point2D([3.0, 0.0])

    segment_ab = geometry.Segment2D(a, b)
    segment_ab2 = geometry.Segment2D(a, b)
    segment_af = geometry.Segment2D(a, f)
    segment_bc = geometry.Segment2D(b, c)
    segment_cd = geometry.Segment2D(c, d)
    segment_cf = geometry.Segment2D(c, f)
    segment_de = geometry.Segment2D(d, e)
    segment_df = geometry.Segment2D(d, f)
    segment_ef = geometry.Segment2D(e, f)

    intersection_point = geometry.segment_segment_intersection2D(
        segment_cf, segment_de)
    if not intersection_point.has_intersection() or intersection_point.result != geometry.Point2D([2.5, 0.5]):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection2D with query segment_cf and segment_de")

    intersection_point1 = geometry.segment_segment_intersection2D(
        segment_bc, segment_af)
    if not intersection_point1.has_intersection() or intersection_point1.result != b:
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection2D with query segment_bc and segment_af")

    intersection_point2 = geometry.segment_segment_intersection2D(
        segment_cd, segment_df)
    if not intersection_point2.has_intersection() or intersection_point2.result != d:
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection2D with query segment_cd and segment_df")

    intersection_point3 = geometry.segment_segment_intersection2D(
        segment_ab, segment_cd)
    if intersection_point3.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection2D with query segment_ab and segment_cd")

    intersection_point4 = geometry.segment_segment_intersection2D(
        segment_ab, segment_ab2)
    if intersection_point4.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection2D with query segment_ab and segment_ab2")

    intersection_point5 = geometry.segment_segment_intersection2D(
        segment_ab, segment_ef)
    if intersection_point5.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection2D with query segment_ab and segment_ef")

    intersection_point6 = geometry.segment_segment_intersection2D(
        segment_bc, segment_df)
    if intersection_point6.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection2D with query segment_bc and segment_df")


def test_segment_line_intersection():
    a = geometry.Point2D([0.0, 0.0])
    b = geometry.Point2D([1.0, 0.0])
    c = geometry.Point2D([2.0, 1.0])
    d = geometry.Point2D([3.0, 1.0])
    e = geometry.Point2D([2.0, 0.0])
    e_epsilon = geometry.Point2D([2.0 - 1e-10, 0.0])
    f = geometry.Point2D([3.0, 0.0])

    segment_ab = geometry.Segment2D(a, b)
    segment_cd = geometry.Segment2D(c, d)
    segment_de = geometry.Segment2D(d, e)
    segment_ef = geometry.Segment2D(e, f)

    line_cf = geometry.InfiniteLine2D(geometry.Segment2D(c, f))
    line_df = geometry.InfiniteLine2D(geometry.Segment2D(d, f))
    line_ef = geometry.InfiniteLine2D(geometry.Segment2D(e, f))
    line_ce = geometry.InfiniteLine2D(geometry.Segment2D(c, e))

    intersection_point = geometry.segment_line_intersection2D(
        segment_de, line_cf)
    if not intersection_point.has_intersection() or intersection_point.result != geometry.Point2D([2.5, 0.5]):
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection2D with query segment_de and line_cf")

    intersection_point1 = geometry.segment_line_intersection2D(
        segment_ef, line_df)
    if not intersection_point1.has_intersection() or intersection_point1.result != f:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection2D with query segment_ef and line_df")

    intersection_point2 = geometry.segment_line_intersection2D(
        segment_cd, line_ef)
    if intersection_point2.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection2D with query segment_cd and line_ef")

    intersection_point3 = geometry.segment_line_intersection2D(
        segment_ab, line_ef)
    if intersection_point3.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection2D with query segment_ab and line_ef")

    intersection_point4 = geometry.segment_line_intersection2D(
        segment_ab, line_ce)
    if intersection_point4.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection2D with query segment_ab and line_ce")


def test_segment_plane_intersection():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    c = geometry.Point3D([1.0, 1.0, 0.0])

    triangle = geometry.Triangle3D(a, b, c)
    plane = triangle.plane()

    d = geometry.Point3D([-1.0, -1.0, 0.0])
    e = geometry.Point3D([2.0, 2.0, 0.0])
    f = geometry.Point3D([-1.0, -1.0, 3.0])
    g = geometry.Point3D([2.0, 2.0, 3.0])
    h = geometry.Point3D([-1.0, -1.0, -1.0])
    i = geometry.Point3D([1.0, 1.0, 1.0])
    j = geometry.Point3D([2.0, 2.0, 1.0])
    k = geometry.Point3D([0.0, -0.5, -1.0])
    l = geometry.Point3D([-10.0, -1.0, -1.0])
    m = geometry.Point3D([0.5, 0.25, 1.0])
    n = geometry.Point3D([0.5, 0.25, 3.0])

    segment_de = geometry.Segment3D(a, b)
    segment_da = geometry.Segment3D(d, a)
    segment_fg = geometry.Segment3D(f, g)
    segment_hi = geometry.Segment3D(h, i)
    segment_hj = geometry.Segment3D(h, j)
    segment_ki = geometry.Segment3D(k, i)
    segment_li = geometry.Segment3D(l, i)
    segment_kb = geometry.Segment3D(k, b)
    segment_mn = geometry.Segment3D(m, n)

    intersect = geometry.segment_plane_intersection3D(segment_de, plane)
    if intersect.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_de")

    intersect1 = geometry.segment_plane_intersection3D(segment_da, plane)
    if intersect1.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_da")

    intersect2 = geometry.segment_plane_intersection3D(segment_fg, plane)
    if intersect2.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_fg")

    intersect3 = geometry.segment_plane_intersection3D(segment_hi, plane)
    if intersect3.result != a:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_hi")

    intersect4 = geometry.segment_plane_intersection3D(segment_hj, plane)
    distance_to_answer = geometry.Vector3D(
        intersect4.result, geometry.Point3D([0.5, 0.5, 0.0])).length()
    if distance_to_answer > basic.GLOBAL_EPSILON:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_hj")

    intersect5 = geometry.segment_plane_intersection3D(segment_ki, plane)
    distance_to_answer = geometry.Vector3D(
        intersect5.result, geometry.Point3D([0.5, 0.25, 0.0])).length()
    if distance_to_answer > basic.GLOBAL_EPSILON:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_ki")

    intersect6 = geometry.segment_plane_intersection3D(segment_li, plane)
    distance_to_answer = geometry.Vector3D(
        intersect6.result, geometry.Point3D([-4.5, 0.0, 0.0])).length()
    if distance_to_answer > basic.GLOBAL_EPSILON:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_li")

    intersect7 = geometry.segment_plane_intersection3D(segment_kb, plane)
    if intersect7.result != b:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_kb")

    intersect8 = geometry.segment_plane_intersection3D(segment_mn, plane)
    if intersect8.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection3D with query segment_mn")


def test_segment_triangle_intersection():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    c = geometry.Point3D([1.0, 1.0, 0.0])

    triangle = geometry.Triangle3D(a, b, c)

    d = geometry.Point3D([-1.0, -1.0, 0.0])
    e = geometry.Point3D([2.0, 2.0, 0.0])
    f = geometry.Point3D([-1.0, -1.0, 3.0])
    g = geometry.Point3D([2.0, 2.0, 3.0])
    h = geometry.Point3D([-1.0, -1.0, -1.0])
    i = geometry.Point3D([1.0, 1.0, 1.0])
    j = geometry.Point3D([2.0, 2.0, 1.0])
    k = geometry.Point3D([0.0, -0.5, -1.0])
    l = geometry.Point3D([-10.0, -1.0, -1.0])
    m = geometry.Point3D([0.5, 0.25, 1.0])
    n = geometry.Point3D([0.5, 0.25, 3.0])

    segment_de = geometry.Segment3D(a, b)
    segment_da = geometry.Segment3D(d, a)
    segment_fg = geometry.Segment3D(f, g)
    segment_hi = geometry.Segment3D(h, i)
    segment_hj = geometry.Segment3D(h, j)
    segment_ki = geometry.Segment3D(k, i)
    segment_li = geometry.Segment3D(l, i)
    segment_kb = geometry.Segment3D(k, b)
    segment_mn = geometry.Segment3D(m, n)

    intersection = geometry.segment_triangle_intersection3D(
        segment_de, triangle)
    if intersection.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_de")

    intersection1 = geometry.segment_triangle_intersection3D(
        segment_da, triangle)
    if intersection1.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_da")

    intersection2 = geometry.segment_triangle_intersection3D(
        segment_fg, triangle)
    if intersection2.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_fg")

    intersection3 = geometry.segment_triangle_intersection3D(
        segment_hi, triangle)
    if intersection3.result != a:
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_hi")

    intersection4 = geometry.segment_triangle_intersection3D(
        segment_hj, triangle)
    if intersection4.result != geometry.Point3D([0.5, 0.5, 0.0]):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_hj")

    intersection5 = geometry.segment_triangle_intersection3D(
        segment_ki, triangle)
    if intersection5.result != geometry.Point3D([0.5, 0.25, 0.0]):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_ki")

    intersection6 = geometry.segment_triangle_intersection3D(
        segment_li, triangle)
    if intersection6.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_li")

    intersection7 = geometry.segment_triangle_intersection3D(
        segment_kb, triangle)
    if intersection7.result != b:
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_kb")

    intersection8 = geometry.segment_triangle_intersection3D(
        segment_mn, triangle)
    if intersection8.has_intersection():
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection3D with query segment_mn")


if __name__ == '__main__':
    test_line_sphere_intersection()
    test_segment_sphere_intersection()
    test_line_line_intersection()
    test_segment_segment_intersection()
    test_segment_line_intersection()
    test_segment_plane_intersection()
    test_segment_triangle_intersection()
