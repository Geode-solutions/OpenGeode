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
# all copies and substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import enum

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_model as model


def check_enum(module, enum_class, names):
    if not issubclass(enum_class, enum.Enum):
        raise ValueError(f"[Test] {enum_class} should be an enum.Enum")
    for name in names:
        if not hasattr(enum_class, name):
            raise ValueError(f"[Test] {enum_class} is missing {name}")
        if hasattr(module, name):
            raise ValueError(
                f"[Test] {name} should not be exported in the module namespace")


def test_enums():
    check_enum(geometry, geometry.IntersectionType,
               ["none", "intersect", "parallel", "incorrect"])
    check_enum(geometry, geometry.Side, ["positive", "negative", "zero"])
    check_enum(geometry, geometry.Position,
               ["outside", "inside", "vertex0", "edge01", "facet3", "parallel"])
    check_enum(model, model.RayTracingResult.Position, ["outside", "inside", "on_border"])
    check_enum(model, model.PhysicalPropertyName,
               ["porosity", "permeability", "density"])
    check_enum(basic, basic.Logger.Level, ["trace", "info", "off"])


def test_enum_names():
    # CamelCase class names, nested when the C++ enum is nested in a class
    if geometry.IntersectionType.__name__ != "IntersectionType":
        raise ValueError("[Test] Wrong name of IntersectionType")
    if model.PhysicalPropertyName.__name__ != "PhysicalPropertyName":
        raise ValueError("[Test] Wrong name of PhysicalPropertyName")
    if model.RayTracingResult.Position.__qualname__ != "RayTracingResult.Position":
        raise ValueError("[Test] Wrong name of RayTracingResult.Position")
    if basic.Logger.Level.__qualname__ != "Logger.Level":
        raise ValueError("[Test] Wrong name of Logger.Level")
    # the position of the geometry module and the one of the ray tracing
    # do not collide
    if model.RayTracingResult.Position is geometry.Position:
        raise ValueError("[Test] The two positions should be different enums")
    # the former names do not exist anymore
    for module, old_name in (
        (geometry, "INTERSECTION_TYPE"),
        (model, "PHYSICAL_PROPERTY_NAME"),
        (model, "POSITION"),
    ):
        if hasattr(module, old_name):
            raise ValueError(f"[Test] {old_name} should have been renamed")
    if hasattr(basic.Logger, "LEVEL"):
        raise ValueError("[Test] Logger.LEVEL should have been renamed")
    # the enums are the ones used by the functions
    result = model.RayTracingResult()
    if not isinstance(result.position, model.RayTracingResult.Position):
        raise ValueError("[Test] RayTracingResult.position should be a Position")
    if not isinstance(basic.Logger.level(), basic.Logger.Level):
        raise ValueError("[Test] Logger.level() should be a Logger.Level")


def test_enum_results():
    segment = geometry.Segment2D(geometry.Point2D([0.0, 0.0]),
                                 geometry.Point2D([1.0, 0.0]))
    other = geometry.Segment2D(geometry.Point2D([0.0, 1.0]),
                               geometry.Point2D([1.0, 1.0]))
    result = geometry.segment_segment_intersection2D(segment, other)
    if result.type != geometry.IntersectionType.parallel:
        raise ValueError("[Test] Parallel segments should be parallel")
    if geometry.IntersectionType.parallel == geometry.Position.parallel:
        raise ValueError("[Test] Different enums must not compare equal")


if __name__ == '__main__':
    test_enums()
    test_enum_names()
    test_enum_results()
