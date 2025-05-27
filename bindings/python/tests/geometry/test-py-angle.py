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
import math
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_geometry as geom


def test_factory_methods():
    degrees_radians = [
        (180.0, math.pi),
        (360.0, 2 * math.pi),
        (90.0, math.pi / 2),
        (100.0, 1.74533),
        (1.0, 0.01745),
        (540.0, 3 * math.pi),
    ]
    for degrees, radians in degrees_radians:
        angle_deg = geom.Angle.from_degrees(degrees)
        if not abs(angle_deg.degrees() - degrees) < EPSILON:
            raise ValueError(f"[Test] Wrong Angle from degree value {angle_deg.degrees()} should be {degrees}!")
        if not abs(angle_deg.radians() - radians) < EPSILON:
            raise ValueError(f"[Test] Wrong Angle from degree value {angle_deg.radians()} should be {radians}!")

        angle_rad = geom.Angle.from_radians(radians)
        if not abs(angle_rad.degrees() - degrees) < EPSILON:
            raise ValueError(f"[Test] Wrong Angle from radian value {angle_rad.degrees()} should be {degrees}!")
        if not abs(angle_rad.radians() - radians) < EPSILON:
            raise ValueError(f"[Test] Wrong Angle from radian value {angle_rad.radians()} should be {radians}!")


def test_comparison_operators():
    angle60 = Angle.from_degrees(60.00)
    angle_almost60 = Angle.from_degrees(60.05)
    angle_not60 = Angle.from_degrees(60.07)
    angle30 = Angle.from_degrees(30.00)

    if not angle60 == angle60:
        raise ValueError(f"[Test] Wrong == comparison: angles {angle60} and {angle60} should be equal!")
    if angle60 == angle_not60:
        raise ValueError(f"[Test] Wrong == comparison: angles {angle60} and {angle_not60} should not be equal!")
    if angle60 == angle_almost60:
        raise ValueError(f"[Test] Wrong == comparison: angles {angle60} and {angle_almost60} should not be equal!")

    if not angle60.inexact_equal(angle60):
        raise ValueError(f"[Test] Wrong inexact_equal: angles {angle60} and {angle60} should be equal!")
    if angle60.inexact_equal(angle_not60):
        raise ValueError(f"[Test] Wrong inexact_equal: angles {angle60} and {angle_not60} should not be equal!")
    if not angle60.inexact_equal(angle_almost60):
        raise ValueError(f"[Test] Wrong inexact_equal: angles {angle60} and {angle_almost60} should be equal!")

    if not angle_almost60 > angle60:
        raise ValueError(f"[Test] Wrong comparison: {angle_almost60} > {angle60} should be true!")
    if not angle_almost60 < angle_not60:
        raise ValueError(f"[Test] Wrong comparison: {angle_almost60} < {angle_not60} should be true!")

def test_arithmetic():
    angle30 = geom.Angle.from_degrees(30)
    angle60 = geom.Angle.from_degrees(60)

    sum_angle = angle30 + angle60
    if not sum_angle.inexact_equal(geom.Angle.from_degrees(90)):
        raise ValueError("[Test] Wrong sum of angles!")

    diff_angle = angle60 - angle30
    if not diff_angle.inexact_equal(angle30):
        raise ValueError("[Test] Wrong difference of angles!")

    scaled_angle = angle30 * 2
    if not scaled_angle.inexact_equal(angle60):
        raise ValueError("[Test] Wrong scaled angle!")

    divided_angle = angle60 / 2
    if not divided_angle.inexact_equal(angle30):
        raise ValueError("[Test] Wrong divided angle!")


def test_normalization():
    angle90 = geom.Angle.from_degrees(90)
    angle450 = geom.Angle.from_degrees(450)
    norm450 = angle450.normalized_0_twopi()
    if not norm450.inexact_equal(angle90):
        raise ValueError("[Test] Wrong normalization between 0 and 2pi!")

    angleminus90 = geom.Angle.from_degrees(-90)
    norm270 = angleminus90.normalized_minuspi_pi()
    if not norm270.inexact_equal(angleminus90):
        raise ValueError("[Test] Wrong normalization between -pi and pi!")

    angle10 = geom.Angle.from_degrees(10)
    angle370 = geom.Angle.from_degrees(370)
    norm370 = angle370.normalized_0_pi()
    if not angle10.inexact_equal(norm370):
        raise ValueError("[Test] Wrong normalization between 0 and pi!")
if __name__ == '__main__':
    test_factory_methods()
    test_comparison_operators()
    test_arithmetic()
    test_normalization()