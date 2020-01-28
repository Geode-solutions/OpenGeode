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

import OpenGeode_py_geometry as geom

def test_length():
    p = geom.Vector3D( [1, 2, 4] )
    if p.length() != math.sqrt( 1 + 4 + 16 ):
        raise ValueError( "[Test] Wrong vector length" )

    p_normalized = p.normalize()
    if p_normalized.length() != 1:
        raise ValueError( "[Test] Wrong vector length" )

def test_operations():
    unit = geom.Vector3D( [0, 2, 0] )
    p = geom.Vector3D( [1, 2, 4] )
    if p.dot( unit ) != 4:
        raise ValueError( "[Test] Wrong dot product" )

    cross_unit = geom.Vector3D( [1, 0, 0] ).cross( geom.Vector3D( [0, 1, 0] ) )
    answer_unit = geom.Vector3D( [0, 0, 1] )
    if cross_unit != answer_unit:
        raise ValueError( "[Test] Wrong unit cross product" )

    answer = geom.Vector3D( [-8, 0, 2] )
    if p.cross( unit ) != answer:
        raise ValueError( "[Test] Wrong cross product" )

if __name__ == '__main__':
    test_length()
    test_operations()
