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

import os
import sys
import math
import platform

if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ["PATH"].split(";") if x]:
        os.add_dll_directory(path)

import opengeode_py_geometry as geom

EPSILON = 1e-3


def test_factory_methods():
    degrees_radians = [
        (-720.0, -4.0 * math.pi),
        (-360.0, -2.0 * math.pi),
        (-270.0, -3.0 * math.pi / 2.0),
        (-180.0, -math.pi),
        (-135.0, -3.0 * math.pi / 4.0),
        (-90.0, -math.pi / 2.0),
        (-45.0, -math.pi / 4.0),
        (0.0, 0.0),
        (22.5, math.pi / 8.0),
        (45.0, math.pi / 4.0),
        (90.0, math.pi / 2.0),
        (135.0, 3.0 * math.pi / 4.0),
        (180.0, math.pi),
        (270.0, 3.0 * math.pi / 2.0),
        (360.0, 2.0 * math.pi),
        (540.0, 3.0 * math.pi),
        (720.0, 4.0 * math.pi),
        (1080.0, 6.0 * math.pi),
    ]
    for degrees, radians in degrees_radians:
        angle_deg = geom.Angle.create_from_degrees(degrees)
        if not abs(angle_deg.degrees() - degrees) < EPSILON:
            raise ValueError(
                f"[Test] Wrong Angle from degree value {angle_deg.degrees()} should be {degrees}!"
            )
        if not abs(angle_deg.radians() - radians) < EPSILON:
            raise ValueError(
                f"[Test] Wrong Angle from degree value {angle_deg.radians()} should be {radians}!"
            )

        angle_rad = geom.Angle.create_from_radians(radians)
        if not abs(angle_rad.degrees() - degrees) < EPSILON:
            raise ValueError(
                f"[Test] Wrong Angle from radian value {angle_rad.degrees()} should be {degrees}!"
            )
        if not abs(angle_rad.radians() - radians) < EPSILON:
            raise ValueError(
                f"[Test] Wrong Angle from radian value {angle_rad.radians()} should be {radians}!"
            )


def test_comparison_operators():
    angle60 = geom.Angle.create_from_degrees(60.00)
    angle_almost60 = geom.Angle.create_from_degrees(60.05)
    angle_not60 = geom.Angle.create_from_degrees(60.07)
    angle30 = geom.Angle.create_from_degrees(30.00)

    if not angle60 == angle60:
        raise ValueError(
            f"[Test] Wrong == comparison: angles {angle60} and {angle60} should be equal!"
        )
    if angle60 == angle_not60:
        raise ValueError(
            f"[Test] Wrong == comparison: angles {angle60} and {angle_not60} should not be equal!"
        )
    if angle60 == angle_almost60:
        raise ValueError(
            f"[Test] Wrong == comparison: angles {angle60} and {angle_almost60} should not be equal!"
        )

    if not angle60.inexact_equal(angle60):
        raise ValueError(
            f"[Test] Wrong inexact_equal: angles {angle60} and {angle60} should be equal!"
        )
    if angle60.inexact_equal(angle_not60):
        raise ValueError(
            f"[Test] Wrong inexact_equal: angles {angle60} and {angle_not60} should not be equal!"
        )
    if not angle60.inexact_equal(angle_almost60):
        raise ValueError(
            f"[Test] Wrong inexact_equal: angles {angle60} and {angle_almost60} should be equal!"
        )

    if not angle_almost60 > angle60:
        raise ValueError(
            f"[Test] Wrong comparison: {angle_almost60} > {angle60} should be true!"
        )
    if not angle_almost60 < angle_not60:
        raise ValueError(
            f"[Test] Wrong comparison: {angle_almost60} < {angle_not60} should be true!"
        )


def test_arithmetic():
    angle30 = geom.Angle.create_from_degrees(30)
    angle60 = geom.Angle.create_from_degrees(60)

    sum_angle = angle30 + angle60
    if not sum_angle.inexact_equal(geom.Angle.create_from_degrees(90)):
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
    raw_angles = [
        geom.Angle.create_from_radians(rad)
        for rad in [
            -4.0 * math.pi,
            -2.0 * math.pi,
            -3.0 * math.pi / 2.0,
            -math.pi,
            -3.0 * math.pi / 4.0,
            -math.pi / 2.0,
            -math.pi / 4.0,
            0.0,
            math.pi / 8.0,
            math.pi / 4.0,
            math.pi / 2.0,
            3.0 * math.pi / 4.0,
            math.pi,
            3.0 * math.pi / 2.0,
            2.0 * math.pi,
            3.0 * math.pi,
            4.0 * math.pi,
            6.0 * math.pi,
        ]
    ]

    expected_0_2pi = [
        geom.Angle.create_from_radians(rad)
        for rad in [
            0.0,
            0.0,
            math.pi / 2.0,
            math.pi,
            5.0 * math.pi / 4.0,
            3.0 * math.pi / 2.0,
            7.0 * math.pi / 4.0,
            0.0,
            math.pi / 8.0,
            math.pi / 4.0,
            math.pi / 2.0,
            3.0 * math.pi / 4.0,
            math.pi,
            3.0 * math.pi / 2.0,
            0.0,
            math.pi,
            0.0,
            0.0,
        ]
    ]

    expected_minuspi_pi = [
        geom.Angle.create_from_radians(rad)
        for rad in [
            0.0,
            0.0,
            math.pi / 2.0,
            math.pi,
            -3.0 * math.pi / 4.0,
            -math.pi / 2.0,
            -math.pi / 4.0,
            0.0,
            math.pi / 8.0,
            math.pi / 4.0,
            math.pi / 2.0,
            3.0 * math.pi / 4.0,
            math.pi,
            -math.pi / 2.0,
            0.0,
            math.pi,
            0.0,
            0.0,
        ]
    ]

    expected_0_pi = [
        geom.Angle.create_from_radians(rad)
        for rad in [
            0.0,
            0.0,
            math.pi / 2.0,
            0.0,
            math.pi / 4.0,
            math.pi / 2.0,
            3.0 * math.pi / 4.0,
            0.0,
            math.pi / 8.0,
            math.pi / 4.0,
            math.pi / 2.0,
            3.0 * math.pi / 4.0,
            0.0,
            math.pi / 2.0,
            0.0,
            0.0,
            0.0,
            0.0,
        ]
    ]

    for i, angle in enumerate(raw_angles):
        if not angle.normalized_between_0_and_2pi().inexact_equal(expected_0_2pi[i]):
            raise ValueError(
                f"[Test] Wrong normalization 0–2pi at index {i}! - should be {angle.normalized_between_0_and_2pi().degrees()} and get {expected_0_2pi[i].degrees()} instead."
            )
        if not angle.normalized_between_minuspi_and_pi().inexact_equal(
            expected_minuspi_pi[i]
        ):
            raise ValueError(
                f"[Test] Wrong normalization -pi–pi at index {i}!- should be {angle.normalized_between_minuspi_and_pi().degrees()} and get {expected_minuspi_pi[i].degrees()} instead."
            )
        if not angle.normalized_between_0_and_pi().inexact_equal(expected_0_pi[i]):
            raise ValueError(
                f"[Test] Wrong normalization 0–pi at index {i}!- should be {angle.normalized_between_0_and_pi().degrees()} and get {expected_0_pi[i].degrees()} instead."
            )


if __name__ == "__main__":
    test_factory_methods()
    test_comparison_operators()
    test_arithmetic()
    test_normalization()
