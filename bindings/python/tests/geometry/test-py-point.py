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

import geode_test_env

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
    if not p.inexact_equal(p_epsilon):
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


def test_string():
    p = geom.Point3D([1, 2, 3])
    if str(p) != p.string():
        raise ValueError("[Test] str of Point should return its string")
    if repr(p) != "Point3D([1.0, 2.0, 3.0])":
        raise ValueError("[Test] repr of Point should give its type and coordinates")
    if eval(repr(p), {"Point3D": geom.Point3D}) != p:
        raise ValueError("[Test] repr of Point should rebuild the same point")


def expect_index_error(function, expected, label):
    try:
        function()
    except IndexError as error:
        if str(error) != expected:
            raise ValueError(f"[Test] {label}: unexpected message '{error}'")
        return
    raise ValueError(f"[Test] {label}: IndexError not raised")


def test_value_index():
    # value and set_value check the axis against the dimension, for points
    # and vectors (which inherit them)
    for dimension in [1, 2, 3]:
        for prefix in ["Point", "Vector"]:
            cls = getattr(geom, f"{prefix}{dimension}D")
            coordinates = [float(axis + 1) for axis in range(dimension)]
            point = cls(coordinates)
            label = cls.__name__
            for axis in range(dimension):
                if point.value(axis) != coordinates[axis]:
                    raise ValueError(f"[Test] {label} value({axis})")
                point.set_value(axis, 10.0 * axis)
                if point.value(axis) != 10.0 * axis:
                    raise ValueError(f"[Test] {label} set_value({axis})")
            for axis in [dimension, dimension + 1, 200, 255]:
                expect_index_error(
                    lambda: point.value(axis),
                    f"[value] axis {axis} out of range [0, {dimension})",
                    f"{label} value({axis})",
                )
                expect_index_error(
                    lambda: point.set_value(axis, 1.0),
                    f"[set_value] axis {axis} out of range [0, {dimension})",
                    f"{label} set_value({axis})",
                )
                expect_index_error(
                    lambda: point.value(index=axis),
                    f"[value] axis {axis} out of range [0, {dimension})",
                    f"{label} value(index={axis})",
                )
            if point != cls([10.0 * axis for axis in range(dimension)]):
                raise ValueError(f"[Test] {label} modified by a failed set_value")
            for bad in [-1, 256, 1.5]:
                for call in [lambda: point.value(bad), lambda: point.set_value(bad, 1.0)]:
                    try:
                        call()
                    except TypeError:
                        continue
                    raise ValueError(f"[Test] {label} accepted the axis {bad}")


if __name__ == '__main__':
    test_comparison()
    test_operators()
    test_string()
    test_value_index()
