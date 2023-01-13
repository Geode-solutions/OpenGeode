# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2023 Geode-solutions
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
import math

import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh


def test_distance_transform_2D(cell_length):
    grid = mesh.RegularGrid2D.create()
    builder = mesh.RegularGridBuilder2D.create(grid)
    builder.initialize_cartesian_grid(geom.Point2D([ 0, 0]), [
                            10,10], cell_length)
    objects_raster = [[0,0],[9,9]]
    map_distance = mesh.euclidean_distance_transform2D(grid,objects_raster,"test_edt")
    values = [
        ([0, 1 ], cell_length * 1. ),
        ([0, 2 ], cell_length * 2. ),
        ([0, 3 ], cell_length * 3. ),
        ([0, 4 ], cell_length * 4. ),
        ([0, 5 ], cell_length * 5. ),
        ([0, 6 ], cell_length * 6. ),
        ([0, 7 ], cell_length * 7. ),
        ([0, 8 ], cell_length * 8. ),
        ([0, 9 ], cell_length * 9. ),
        ([1, 0 ], cell_length * 1. ),
        ([2, 0 ], cell_length * 2. ),
        ([3, 0 ], cell_length * 3. ),
        ([4, 0 ], cell_length * 4. ),
        ([5, 0 ], cell_length * 5. ),
        ([6, 0 ], cell_length * 6. ),
        ([7, 0 ], cell_length * 7. ),
        ([8, 0 ], cell_length * 8. ),
        ([9, 0 ], cell_length * 9. ),
        ([0, 0 ], cell_length * 0. ),
        ([1, 1 ], cell_length * math.sqrt(2) ),
        ([2, 2 ], cell_length * math.sqrt(8) ),
        ([3, 3 ], cell_length * math.sqrt(18)),
        ([4, 4 ], cell_length * math.sqrt(32)),
        ([5, 5 ], cell_length * math.sqrt(32)),
        ([6, 6 ], cell_length * math.sqrt(18)),
        ([7, 7 ], cell_length * math.sqrt(8) ),
        ([8, 8 ], cell_length * math.sqrt(2) ),
        ([9, 9 ], cell_length * 0. ),      
        ([8, 1 ], cell_length * math.sqrt( 65 )),
        ([7, 2 ], cell_length * math.sqrt( 53 )),
        ([6, 3 ], cell_length * math.sqrt( 45 )),
        ([5, 4 ], cell_length * math.sqrt( 41 )),
        ([4, 5 ], cell_length * math.sqrt( 41 )),
        ([3, 6 ], cell_length * math.sqrt( 45 )),
        ([2, 7 ], cell_length * math.sqrt( 53 )),
        ([1, 8 ], cell_length * math.sqrt( 65 ))
        ]
    for value in values:
        if math.fabs(map_distance.value(grid.cell_index(value[0])) - value[1] > basic.global_epsilon):
            raise ValueError("[Test] Wrong 2D euclidean distance map" )

def test_distance_transform_3D(cell_length):
    grid = mesh.RegularGrid3D.create()
    builder = mesh.RegularGridBuilder3D.create(grid)
    builder.initialize_cartesian_grid(geom.Point3D([ 0, 0,0]), [
                            10,10,10], cell_length)
    objects_raster = [[0,0,0],[9,9,9]]
    map_distance = mesh.euclidean_distance_transform3D(grid,objects_raster,"test_edt")
    values = [
        ([ 0, 9, 0 ], cell_length * 9.),
        ([ 0, 9, 9 ], cell_length * 9.),
        ([ 0, 0, 9 ], cell_length * 9.),
        ([ 9, 0, 0 ], cell_length * 9.),
        ([ 9, 0, 9 ], cell_length * 9.),
        ([ 9, 9, 0 ], cell_length * 9.),
        ([ 0, 0, 0 ], cell_length * 0.),
        ([ 1, 1, 1 ], cell_length * math.sqrt( 3 )),
        ([ 2, 2, 2 ], cell_length * math.sqrt(12)),
        ([ 3, 3, 3 ], cell_length * math.sqrt(27)),
        ([ 4, 4, 4 ], cell_length * math.sqrt(48)),
        ([ 5, 5, 5 ], cell_length * math.sqrt(48)),
        ([ 6, 6, 6 ], cell_length * math.sqrt(27)),
        ([ 7, 7, 7 ], cell_length * math.sqrt(12)),
        ([ 8, 8, 8 ], cell_length * math.sqrt(3)),
        ([ 9, 9, 9 ], cell_length * 0.)
        ]

    for value in values:
        if math.fabs(map_distance.value(grid.cell_index(value[0])) - value[1]) > basic.global_epsilon:
            raise ValueError("[Test] Wrong 3D euclidean distance map" )

if __name__ == '__main__':
    mesh.OpenGeodeMesh.initialize()
    test_distance_transform_2D(1.)
    test_distance_transform_3D(4.5)
    

