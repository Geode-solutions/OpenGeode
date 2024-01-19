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


def test_segment_segment_intersection_detection():
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
    segment_bf = geometry.Segment2D(b, f)
    segment_ae = geometry.Segment2D(a, e)

    result = geometry.segment_segment_intersection_detection2D(
        segment_cf, segment_de)
    if result != (geometry.Position.inside, geometry.Position.inside):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_cf and segment_de")

    result = geometry.segment_segment_intersection_detection2D(
        segment_bc, segment_af)
    if result != (geometry.Position.vertex0, geometry.Position.inside):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_bc and segment_af")

    result = geometry.segment_segment_intersection_detection2D(
        segment_cd, segment_df)
    if result != (geometry.Position.vertex1, geometry.Position.vertex0):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_cd and segment_df")

    result = geometry.segment_segment_intersection_detection2D(
        segment_ab, segment_cd)
    if result != (geometry.Position.outside, geometry.Position.outside):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_ab and segment_cd")

    result = geometry.segment_segment_intersection_detection2D(
        segment_ab, segment_ab2)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_ab and segment_ab2")
    result = geometry.colinear_segment_segment_intersection_detection2D(
        segment_ab, segment_ab2)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for colinear_segment_segment_intersection_detection2D with query segment_ab and segment_ab2")

    result = geometry.segment_segment_intersection_detection2D(
        segment_ab, segment_ef)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_ab and segment_ef")
    result = geometry.colinear_segment_segment_intersection_detection2D(
        segment_ab, segment_ef)
    if result != (geometry.Position.outside, geometry.Position.outside):
        raise ValueError(
            "[Test] Wrong result for colinear_segment_segment_intersection_detection2D with query segment_ab and segment_ef")

    result = geometry.segment_segment_intersection_detection2D(
        segment_ae, segment_bf)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_ae and segment_bf")
    result = geometry.colinear_segment_segment_intersection_detection2D(
        segment_ae, segment_bf)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for colinear_segment_segment_intersection_detection2D with query segment_ae and segment_bf")

    result = geometry.segment_segment_intersection_detection2D(
        segment_ae, segment_ef)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_ae and segment_ef")
    result = geometry.colinear_segment_segment_intersection_detection2D(
        segment_ae, segment_ef)
    if result != (geometry.Position.vertex1, geometry.Position.vertex0):
        raise ValueError(
            "[Test] Wrong result for colinear_segment_segment_intersection_detection2D with query segment_ae and segment_ef")

    result = geometry.segment_segment_intersection_detection2D(
        segment_bc, segment_df)
    if result != (geometry.Position.outside, geometry.Position.outside):
        raise ValueError(
            "[Test] Wrong result for segment_segment_intersection_detection2D with query segment_bc and segment_df")


def test_segment_line_intersection_detection():
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

    result = geometry.segment_line_intersection_detection2D(
        segment_cf, geometry.InfiniteLine2D(segment_de))
    if result != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection_detection2D with query segment_cf and segment_de")

    result = geometry.segment_line_intersection_detection2D(
        segment_bc, geometry.InfiniteLine2D(segment_af))
    if result != geometry.Position.vertex0:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection_detection2D with query segment_bc and segment_af")

    result = geometry.segment_line_intersection_detection2D(
        segment_cd, geometry.InfiniteLine2D(segment_df))
    if result != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection_detection2D with query segment_cd and segment_df")

    result = geometry.segment_line_intersection_detection2D(
        segment_ab, geometry.InfiniteLine2D(segment_cd))
    if result != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection_detection2D with query segment_ab and segment_cd")

    result = geometry.segment_line_intersection_detection2D(
        segment_ab, geometry.InfiniteLine2D(segment_ab2))
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection_detection2D with query segment_ab and segment_ab2")

    result = geometry.segment_line_intersection_detection2D(
        segment_ab, geometry.InfiniteLine2D(segment_ef))
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection_detection2D with query segment_ab and segment_ef")

    result = geometry.segment_line_intersection_detection2D(
        segment_bc, geometry.InfiniteLine2D(segment_df))
    if result != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for segment_line_intersection_detection2D with query segment_bc and segment_df")


def test_segment_triangle_intersection_detection():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    c = geometry.Point3D([1.0, 1.0, 0.0])
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
    o = geometry.Point3D([0.75, 0.25, 0.0])
    p = geometry.Point3D([0.75, -0.25, 0.0])

    triangle = geometry.Triangle3D(a, b, c)

    segment_ab = geometry.Segment3D(a, b)
    segment_da = geometry.Segment3D(d, a)
    segment_fg = geometry.Segment3D(f, g)
    segment_hi = geometry.Segment3D(h, i)
    segment_hj = geometry.Segment3D(h, j)
    segment_ki = geometry.Segment3D(k, i)
    segment_li = geometry.Segment3D(l, i)
    segment_kb = geometry.Segment3D(k, b)
    segment_mn = geometry.Segment3D(m, n)
    segment_op = geometry.Segment3D(o, p)

    result = geometry.segment_triangle_intersection_detection3D(
        segment_ab, triangle)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_ab")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_da, triangle)
    if result != (geometry.Position.vertex1, geometry.Position.vertex0):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_da")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_fg, triangle)
    if result != (geometry.Position.outside, geometry.Position.outside):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_fg")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_hi, triangle)
    if result != (geometry.Position.inside, geometry.Position.vertex0):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_hi")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_hj, triangle)
    if result != (geometry.Position.inside, geometry.Position.edge2):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_hj")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_ki, triangle)
    if result != (geometry.Position.inside, geometry.Position.inside):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_ki")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_li, triangle)
    if result != (geometry.Position.outside, geometry.Position.outside):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_li")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_kb, triangle)
    if result != (geometry.Position.vertex1, geometry.Position.vertex1):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_kb")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_mn, triangle)
    if result != (geometry.Position.outside, geometry.Position.outside):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_mn")

    result = geometry.segment_triangle_intersection_detection3D(
        segment_op, triangle)
    if result != (geometry.Position.parallel, geometry.Position.parallel):
        raise ValueError(
            "[Test] Wrong result for segment_triangle_intersection_detection3D with query segment_op")


def test_line_triangle_intersection_detection():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    c = geometry.Point3D([1.0, 1.0, 0.0])
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
    o = geometry.Point3D([0.75, 0.25, 0.0])
    p = geometry.Point3D([0.75, -0.25, 0.0])

    triangle = geometry.Triangle3D(a, b, c)

    line_ab = geometry.InfiniteLine3D(geometry.Segment3D(a, b))
    line_da = geometry.InfiniteLine3D(geometry.Segment3D(d, a))
    line_fg = geometry.InfiniteLine3D(geometry.Segment3D(f, g))
    line_hi = geometry.InfiniteLine3D(geometry.Segment3D(h, i))
    line_hj = geometry.InfiniteLine3D(geometry.Segment3D(h, j))
    line_ki = geometry.InfiniteLine3D(geometry.Segment3D(k, i))
    line_li = geometry.InfiniteLine3D(geometry.Segment3D(l, i))
    line_kb = geometry.InfiniteLine3D(geometry.Segment3D(k, b))
    line_mn = geometry.InfiniteLine3D(geometry.Segment3D(m, n))
    line_op = geometry.InfiniteLine3D(geometry.Segment3D(o, p))

    result = geometry.line_triangle_intersection_detection3D(line_ab, triangle)
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_ab")

    result = geometry.line_triangle_intersection_detection3D(line_da, triangle)
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_da")

    result = geometry.line_triangle_intersection_detection3D(line_fg, triangle)
    if result != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_fg")

    result = geometry.line_triangle_intersection_detection3D(line_hi, triangle)
    if result != geometry.Position.vertex0:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_hi")

    result = geometry.line_triangle_intersection_detection3D(line_hj, triangle)
    if result != geometry.Position.edge2:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_hj")

    result = geometry.line_triangle_intersection_detection3D(line_ki, triangle)
    if result != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_ki")

    result = geometry.line_triangle_intersection_detection3D(line_li, triangle)
    if result != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_li")

    result = geometry.line_triangle_intersection_detection3D(line_kb, triangle)
    if result != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_kb")

    result = geometry.line_triangle_intersection_detection3D(line_mn, triangle)
    if result != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_mn")

    result = geometry.line_triangle_intersection_detection3D(line_op, triangle)
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for line_triangle_intersection_detection3D with query line_op")


def test_segment_plane_intersection_detection():
    a = geometry.Point3D([0.0, 0.0, 0.0])
    b = geometry.Point3D([1.0, 0.0, 0.0])
    c = geometry.Point3D([1.0, 1.0, 0.0])
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
    o = geometry.Point3D([0.75, 0.25, 0.0])
    p = geometry.Point3D([0.75, -0.25, 0.0])

    plane = geometry.Plane(geometry.Vector3D([0.0, 0.0, 1.0]), a)

    segment_ab = geometry.Segment3D(a, b)
    segment_da = geometry.Segment3D(d, a)
    segment_fg = geometry.Segment3D(f, g)
    segment_hi = geometry.Segment3D(h, i)
    segment_hj = geometry.Segment3D(h, j)
    segment_ki = geometry.Segment3D(k, i)
    segment_li = geometry.Segment3D(l, i)
    segment_kb = geometry.Segment3D(k, b)
    segment_mn = geometry.Segment3D(m, n)
    segment_op = geometry.Segment3D(o, p)

    result = geometry.segment_plane_intersection_detection3D(segment_ab, plane)
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_ab")

    result = geometry.segment_plane_intersection_detection3D(segment_da, plane)
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_da")

    result = geometry.segment_plane_intersection_detection3D(segment_fg, plane)
    if result != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_fg")

    result = geometry.segment_plane_intersection_detection3D(segment_hi, plane)
    if result != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_hi")

    result = geometry.segment_plane_intersection_detection3D(segment_hj, plane)
    if result != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_hj")

    result = geometry.segment_plane_intersection_detection3D(segment_ki, plane)
    if result != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_ki")

    result = geometry.segment_plane_intersection_detection3D(segment_li, plane)
    if result != geometry.Position.inside:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_li")

    result = geometry.segment_plane_intersection_detection3D(segment_kb, plane)
    if result != geometry.Position.vertex1:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_kb")

    result = geometry.segment_plane_intersection_detection3D(segment_mn, plane)
    if result != geometry.Position.outside:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_mn")

    result = geometry.segment_plane_intersection_detection3D(segment_op, plane)
    if result != geometry.Position.parallel:
        raise ValueError(
            "[Test] Wrong result for segment_plane_intersection_detection3D with query segment_op")


if __name__ == '__main__':
    test_segment_segment_intersection_detection()
    test_segment_line_intersection_detection()
    test_segment_triangle_intersection_detection()
    test_line_triangle_intersection_detection()
    test_segment_plane_intersection_detection()
