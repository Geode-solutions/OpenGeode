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
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_geometry as geom

if __name__ == '__main__':
    box = geom.BoundingBox2D()
    box.add_point(geom.Point2D([-1, -1]))
    box.add_point(geom.Point2D([1, 1]))

    box2 = box
    box2.add_point(geom.Point2D([-2, -2]))
    box2.add_point(geom.Point2D([0, 0]))

    box2.add_box(box)
    if box2.min() != geom.Point2D([-2, -2]):
        raise ValueError("[Test] Error in BoundingBox union computation")
    if box2.max() != geom.Point2D([1, 1]):
        raise ValueError("[Test] Error in BoundingBox union computation")

    if not box2.contains_point(geom.Point2D([0, 0])):
        raise ValueError("[Test] BBox should contain this point")
    if box2.contains_point(geom.Point2D([10, 0])):
        raise ValueError("[Test] BBox should not contain this point")

    # --- 3D Bounding Box and Ray/Line intersections ---
    bbox3 = geom.BoundingBox3D()
    bbox3.add_point(geom.Point3D([-1, -1, -1]))
    bbox3.add_point(geom.Point3D([1, 1, 1]))
    
    # Rays
    ray_inside = geom.Ray3D( geom.Vector3D([0, 0, 1]),geom.Point3D([0, 0, 0]))
    if not bbox3.intersects_ray(ray_inside):
        raise ValueError("[Test] Wrong result with ray_inside")

    ray_up = geom.Ray3D( geom.Vector3D([0, 0, 1]),geom.Point3D([0, 0, 2]))
    if bbox3.intersects_ray(ray_up):
        raise ValueError("[Test] Wrong result with ray_up")

    ray_down = geom.Ray3D( geom.Vector3D([0, 0, 1]),geom.Point3D([0, 0, -2]))
    if not bbox3.intersects_ray(ray_down):
        raise ValueError("[Test] Wrong result with ray_down")

    # Infinite lines
    line_inside = geom.InfiniteLine3D( geom.Vector3D([0, 0, 1]),geom.Point3D([0, 0, 0]))
    if not bbox3.intersects_infinite_line(line_inside):
        raise ValueError("[Test] Wrong result with line_inside")

    line_up = geom.InfiniteLine3D( geom.Vector3D([0, 0, 1]),geom.Point3D([0, 0, 2]))
    if not bbox3.intersects_infinite_line(line_up):
        raise ValueError("[Test] Wrong result with line_up")

    line_down = geom.InfiniteLine3D( geom.Vector3D([0, 0, 1]),geom.Point3D([0, 0, -2]))
    if not bbox3.intersects_infinite_line(line_down):
        raise ValueError("[Test] Wrong result with line_down")