# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2026 Geode-solutions
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


import gc

import geode_test_env

import opengeode_py_geometry as geom


def allocate_junk():
    gc.collect()
    junk = [geom.Point3D([1e6 + i, -1e6 - i, 42.0]) for i in range(10000)]
    junk2 = [geom.Point2D([1e6 + i, -1e6 - i]) for i in range(10000)]
    return junk, junk2


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def test_segment_from_temporaries():
    segment3 = geom.Segment3D(geom.Point3D([0, 0, 0]), geom.Point3D([1, 0, 0]))
    segment2 = geom.Segment2D(geom.Point2D([0, 0]), geom.Point2D([0, 1]))
    junk = allocate_junk()
    check(segment3.length() == 1, "Segment3D length should be 1")
    check(segment2.length() == 1, "Segment2D length should be 1")
    vertices = segment3.vertices()
    check(vertices[1] == geom.Point3D([1, 0, 0]), "Wrong Segment3D vertex")
    check(
        geom.point_segment_distance3D(geom.Point3D([0, 1, 0]), segment3) == 1,
        "Wrong point segment distance",
    )


def test_triangle_from_temporaries():
    triangle = geom.Triangle3D(
        geom.Point3D([0, 0, 0]), geom.Point3D([1, 0, 0]), geom.Point3D([0, 1, 0])
    )
    triangle2 = geom.Triangle2D(
        geom.Point2D([0, 0]), geom.Point2D([1, 0]), geom.Point2D([0, 1])
    )
    junk = allocate_junk()
    check(
        triangle.barycenter() == geom.Point3D([1.0 / 3, 1.0 / 3, 0]),
        "Wrong Triangle3D barycenter",
    )
    check(
        triangle2.barycenter() == geom.Point2D([1.0 / 3, 1.0 / 3]),
        "Wrong Triangle2D barycenter",
    )
    check(geom.triangle_area3D(triangle) == 0.5, "Wrong Triangle3D area")
    plane = geom.Triangle3D(
        geom.Point3D([0, 0, 1]), geom.Point3D([1, 0, 1]), geom.Point3D([0, 1, 1])
    ).plane()
    junk = allocate_junk()
    check(plane.origin() == geom.Point3D([0, 0, 1]), "Wrong Triangle3D plane")


def test_tetrahedron_from_temporaries():
    tetra = geom.Tetrahedron(
        geom.Point3D([0, 0, 0]),
        geom.Point3D([1, 0, 0]),
        geom.Point3D([0, 1, 0]),
        geom.Point3D([0, 0, 1]),
    )
    junk = allocate_junk()
    check(
        abs(geom.tetrahedron_volume(tetra) - 1.0 / 6) < 1e-12,
        "Wrong Tetrahedron volume",
    )
    check(tetra.vertices()[3] == geom.Point3D([0, 0, 1]), "Wrong Tetrahedron vertex")


def test_others_from_temporaries():
    plane = geom.Plane(geom.Vector3D([0, 0, 1]), geom.Point3D([0, 0, 2]))
    sphere = geom.Sphere3D(geom.Point3D([0, 0, 3]), 1)
    ray = geom.Ray3D(geom.Vector3D([1, 0, 0]), geom.Point3D([0, 0, 4]))
    line = geom.InfiniteLine3D(geom.Vector3D([1, 0, 0]), geom.Point3D([0, 0, 5]))
    segment_ray = geom.Ray3D(
        geom.Segment3D(geom.Point3D([0, 0, 6]), geom.Point3D([1, 0, 6]))
    )
    segment_line = geom.InfiniteLine3D(
        geom.Segment3D(geom.Point3D([0, 0, 7]), geom.Point3D([1, 0, 7]))
    )
    circle = geom.Circle(
        geom.Plane(geom.Vector3D([0, 0, 1]), geom.Point3D([0, 0, 8])), 1
    )
    circle_plane = geom.Circle(
        geom.Plane(geom.Vector3D([0, 0, 1]), geom.Point3D([0, 0, 9])), 1
    ).plane()
    cylinder = geom.Cylinder(
        geom.Segment3D(geom.Point3D([0, 0, 10]), geom.Point3D([0, 0, 11])), 1
    )
    cylinder_axis = geom.Cylinder(
        geom.Segment3D(geom.Point3D([0, 0, 12]), geom.Point3D([0, 0, 13])), 1
    ).axis()
    junk = allocate_junk()
    check(plane.origin() == geom.Point3D([0, 0, 2]), "Wrong Plane origin")
    check(sphere.origin() == geom.Point3D([0, 0, 3]), "Wrong Sphere origin")
    check(ray.origin() == geom.Point3D([0, 0, 4]), "Wrong Ray origin")
    check(line.origin() == geom.Point3D([0, 0, 5]), "Wrong InfiniteLine origin")
    check(
        segment_ray.origin() == geom.Point3D([0, 0, 6]),
        "Wrong Ray from Segment origin",
    )
    check(
        segment_line.origin() == geom.Point3D([0, 0, 7]),
        "Wrong InfiniteLine from Segment origin",
    )
    check(circle.plane().origin() == geom.Point3D([0, 0, 8]), "Wrong Circle plane")
    check(
        circle_plane.origin() == geom.Point3D([0, 0, 9]),
        "Wrong plane from temporary Circle",
    )
    check(cylinder.axis().length() == 1, "Wrong Cylinder axis")
    check(cylinder_axis.length() == 1, "Wrong axis from temporary Cylinder")


if __name__ == "__main__":
    test_segment_from_temporaries()
    test_triangle_from_temporaries()
    test_tetrahedron_from_temporaries()
    test_others_from_temporaries()
