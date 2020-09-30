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

import os, sys, platform
if sys.version_info >= (3,8,0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_geometry as geom

def check_triangle_bary_coords( result, answer ):
    for i in range(3):
        if result[i] != answer[i]:
            print( "Lambda ", i, " is incorrect." )
            print( "Result = ", result[i], " - Answer = ", answer[i] )
            return False
    return True

def test_triangle_barycentric_coordinates():
    a = geom.Point2D( [0.0, 0.0] )
    b = geom.Point2D( [1.0, 0.0] )
    c = geom.Point2D( [1.0, 1.0] )
    triangle2D = geom.Triangle2D( a, b, c )

    if not check_triangle_bary_coords( geom.triangle_barycentric_coordinates2D( a, triangle2D ), [1, 0, 0] ):
        raise ValueError("[Test] Wrong result for triangle_barycentric_coordinates with query point a" )
    if not check_triangle_bary_coords( geom.triangle_barycentric_coordinates2D( b, triangle2D ), [0, 1, 0] ):
        raise ValueError("[Test] Wrong result for triangle_barycentric_coordinates with query point b" )
    if not check_triangle_bary_coords( geom.triangle_barycentric_coordinates2D( c, triangle2D ), [0, 0, 1] ):
        raise ValueError("[Test] Wrong result for triangle_barycentric_coordinates with query point c" )

    q1 = geom.Point2D( [0.25, 0.0] )
    if not check_triangle_bary_coords( geom.triangle_barycentric_coordinates2D( q1, triangle2D ), [0.75, 0.25, 0] ):
        raise ValueError("[Test] Wrong result for triangle_barycentric_coordinates with query point q1" )

    q2 = geom.Point2D( [0.5, 0.25] ) 
    if not check_triangle_bary_coords( geom.triangle_barycentric_coordinates2D( q2, triangle2D ), [0.5, 0.25, 0.25] ):
        raise ValueError("[Test] Wrong result for triangle_barycentric_coordinates with query point q2" )

    q3 = geom.Point2D( [0.0, 1.0] )
    if not check_triangle_bary_coords( geom.triangle_barycentric_coordinates2D( q3, triangle2D ), [1, -1, 1] ):
        raise ValueError("[Test] Wrong result for triangle_barycentric_coordinates with query point q3" )

if __name__ == '__main__':
    test_triangle_barycentric_coordinates()
