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


"""Naming conventions of the public API: dimensions are suffixed with 2D or 3D
(never 2d or 3d) and the enums have CamelCase class names."""

import enum
import inspect
import re

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_image as image
import opengeode_py_mesh as mesh
import opengeode_py_model as model

MODULES = (basic, geometry, image, mesh, model)
RENAMED_FUNCTIONS = {
    "convert_point_set3d_into_2d": "convert_point_set3D_into_2D",
    "convert_edged_curve3d_into_2d": "convert_edged_curve3D_into_2D",
    "convert_surface_mesh2d_into_3d": "convert_surface_mesh2D_into_3D",
    "convert_surface_mesh3d_into_2d": "convert_surface_mesh3D_into_2D",
    "convert_polygonal_surface2d_into_3d": "convert_polygonal_surface2D_into_3D",
    "convert_polygonal_surface3d_into_2d": "convert_polygonal_surface3D_into_2D",
    "convert_triangulated_surface2d_into_3d": "convert_triangulated_surface2D_into_3D",
    "convert_triangulated_surface3d_into_2d": "convert_triangulated_surface3D_into_2D",
}
LOWERCASE_DIMENSION = re.compile(r"[a-z_][23]d(_|$)")


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def public_members():
    for module in MODULES:
        for name, member in vars(module).items():
            if name.startswith("_"):
                continue
            yield module, None, name, member
            if inspect.isclass(member) and member.__module__ == module.__name__:
                for member_name, value in vars(member).items():
                    if not member_name.startswith("_"):
                        yield module, member, member_name, value


def test_renamed_functions():
    for old_name, new_name in RENAMED_FUNCTIONS.items():
        check(hasattr(mesh, new_name), f"{new_name} should exist")
        check(not hasattr(mesh, old_name), f"{old_name} should have been renamed")
        check(
            inspect.getdoc(getattr(mesh, new_name)),
            f"{new_name} should have a docstring",
        )


def test_dimension_suffix():
    # 2d and 3d in the name of a function or a class (argument names such as
    # surface3d are variables, not part of this convention)
    for module, owner, name, member in public_members():
        check(
            not LOWERCASE_DIMENSION.search(name),
            f"{module.__name__}.{(owner.__name__ + '.') if owner else ''}{name}: "
            "dimensions are suffixed with 2D or 3D",
        )


def test_enum_class_names():
    count = 0
    for module, owner, name, member in public_members():
        if inspect.isclass(member) and issubclass(member, enum.Enum):
            count += 1
            check(
                re.fullmatch(r"[A-Z][a-z0-9]+([A-Z][a-z0-9]+)*", name),
                f"{name} should be a CamelCase class name",
            )
    check(count >= 6, f"{count} enums found, expected at least 6")


if __name__ == "__main__":
    test_renamed_functions()
    test_dimension_suffix()
    test_enum_class_names()
