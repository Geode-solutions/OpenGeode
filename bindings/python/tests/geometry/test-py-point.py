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

import OpenGeode_py_geometry as geom

def test_comparison():
    p = geom.Point3D( [2, 1.0, 2.6] )
    p2 = p
    if p != p2:
        raise ValueError( "[Test] Points should be equal" )
    
    P = geom.Point2D( [15, 2.6] )
    P2 = geom.Point2D( [16, 2.6] )
    if P == P2:
        raise ValueError( "[Test] Points should be different" )

    p_epsilon = geom.Point3D( [2.0000000001, 1, 2.6] )
    if not p.inexact_equal( p_epsilon, 0.0001 ):
        raise ValueError( "[Test] Points should be almost equal" )

def test_operators():
    p = geom.Point3D()
    p.set_value( 0, 2 )
    p.set_value( 1, 1 )
    p.set_value( 2, 2.6 )
    p2 = p
    answer = geom.Point3D( [4, 2, 5.2] )
    if p.add( p2 ).different( answer ):
        raise ValueError( "[Test] Points should be equal" )
    if p.multiply( 2 ).different( answer ):
        raise ValueError( "[Test] Points should be equal" )
    if p.subtract( p2 ).different( geom.Point3D() ):
        raise ValueError( "[Test] Points should be equal" )
    if answer.divide( 2 ).different( p ):
        raise ValueError( "[Test] Points should be equal" )

test_comparison()
test_operators()
