# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2020 Geode-solutions
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

import math
import opengeode_py_geometry as geom

def test_point_segment_distance():
    a = geom.Point2D( [1.0, 5.0] )
    b = geom.Point2D( [-1.0, -5.0] )
    segment2D = geom.Segment2D( a, b )

    distance, closest_point = geom.point_segment_distance( a, segment2D )
    if distance != 0 or closest_point != a:
        raise ValueError( "[Test] Wrong result for point_segment_distance with query point a" )

    distance, closest_point = geom.point_segment_distance( b, segment2D )
    if distance != 0 or closest_point != b:
        raise ValueError( "[Test] Wrong result for point_segment_distance with query point b" )

    q1 = geom.Point2D( [0.0, 0.0] )
    distance, closest_point = geom.point_segment_distance( q1, segment2D )
    if distance != 0 or closest_point != q1:
        raise ValueError( "[Test] Wrong result for point_segment_distance with query point q1" )

    q2 = geom.Point2D( [ 10.0, 10.0] )
    distance, closest_point = geom.point_segment_distance( q2, segment2D )
    if distance != math.sqrt( 106 ) or closest_point != a:
        raise ValueError( "[Test] Wrong result for point_segment_distance with query point q2" )

    q3 = geom.Point2D( [ 5.0, -1.0] )
    distance, closest_point = geom.point_segment_distance( q3, segment2D )
    result_q3 = geom.Point2D( [ 0.0, 0.0] )
    if distance != math.sqrt( 26 ) or closest_point != result_q3:
        raise ValueError( "[Test] Wrong result for point_segment_distance with query point q3" )

    q4 = geom.Point2D( [ 5.5, 1.5] )
    distance, closest_point = geom.point_segment_distance( q4, segment2D )
    result_q4 = geom.Point2D( [ 0.5, 2.5] )
    if distance != math.sqrt( 26 ) or closest_point != result_q4:
        raise ValueError( "[Test] Wrong result for point_segment_distance with query point q4" )

def test_point_triangle_distance():
    a = geom.Point2D( [0.0, 0.0] )
    b = geom.Point2D( [1.0, 0.0] )
    c = geom.Point2D( [1.0, 1.0] )
    triangle2D = geom.Triangle2D( a, b, c )

    distance, closest_point = geom.point_triangle_distance( a, triangle2D )
    if distance != 0 or closest_point != a:
        raise ValueError( "[Test] Wrong result for point_triangle_distance with query point a" )

    distance, closest_point = geom.point_triangle_distance( b, triangle2D )
    if distance != 0 or closest_point != b:
        raise ValueError( "[Test] Wrong result for point_triangle_distance with query point b" )

    q1 = geom.Point2D( [0.5, 0.5] )
    distance, closest_point = geom.point_triangle_distance( q1, triangle2D )
    if distance != 0 or closest_point != q1:
        raise ValueError( "[Test] Wrong result for point_triangle_distance with query point q1" )

    q2 = geom.Point2D( [0.0, 1.0] )
    distance, closest_point = geom.point_triangle_distance( q2, triangle2D )
    result_q2 = geom.Point2D( [0.5, 0.5] )
    if distance != math.sqrt( 2 ) / 2. or closest_point != result_q2:
        raise ValueError( "[Test] Wrong result for point_triangle_distance with query point q2" )

    q3 = geom.Point2D( [2.0, 1.0] )
    distance, closest_point = geom.point_triangle_distance( q3, triangle2D )
    if distance != 1 or closest_point != c:
        raise ValueError( "[Test] Wrong result for point_triangle_distance with query point q3" )

    q4 = geom.Point2D( [0.5, 0.5] )
    distance, closest_point = geom.point_triangle_distance( q4, triangle2D )
    if distance != 0 or closest_point != q4:
        raise ValueError( "[Test] Wrong result for point_triangle_distance with query point q4" )

def test_point_plane_distance():
    a = geom.Point3D( [0.0, 0.0, 0.0] )
    b = geom.Point3D( [1.0, 0.0, 0.0] )
    c = geom.Point3D( [1.0, 1.0, 0.0] )
    triangle = geom.Triangle3D( a, b, c )
    plane = triangle.plane()

    q1 = geom.Point3D( [0.5, 0.5, 1] )
    distance, closest_point = geom.point_plane_distance( q1, plane )
    answer = geom.Point3D( [0.5, 0.5, 0.0] )
    if distance != 1.0 or closest_point != answer:
        raise ValueError( "[Test] Wrong result for point_plane_distance with query point q1" )
    distance, closest_point = geom.point_plane_signed_distance( q1, plane )
    answer = geom.Point3D( [0.5, 0.5, 0.0] )
    if distance != 1.0 or closest_point != answer:
        raise ValueError( "[Test] Wrong result for point_plane_signed_distance with query point q1" )

    q2 = geom.Point3D( [0.5, 0.5, -1] )
    distance, closest_point = geom.point_plane_distance( q2, plane )
    answer = geom.Point3D( [0.5, 0.5, 0.0] )
    if distance != 1.0 or closest_point != answer:
        raise ValueError( "[Test] Wrong result for point_plane_distance with query point q2" )
    distance, closest_point = geom.point_plane_signed_distance( q2, plane )
    answer = geom.Point3D( [0.5, 0.5, 0.0] )
    if distance != -1.0 or closest_point != answer:
        raise ValueError( "[Test] Wrong result for point_plane_signed_distance with query point q2" )

if __name__ != '__main__':
    test_point_segment_distance()
    test_point_triangle_distance()
    test_point_plane_distance()
