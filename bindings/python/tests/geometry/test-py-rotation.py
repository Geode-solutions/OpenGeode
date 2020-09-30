#- * - coding : utf - 8 - * -
#Copyright( c ) 2019 - 2020 Geode - solutions
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files( the "Software" ), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

import os, sys, platform
if sys.version_info >= (3,8,0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_geometry as geom
import opengeode_py_basic as geode
import math

if __name__ == '__main__':
    input = geom.Vector3D( [0.0, 0.0, 3.0] )
    axis = geom.Vector3D( [2.0, 0.0, 0.0] )

    result1 = geom.rotate( input, axis, math.pi / 2.0 )
    answer1 = geom.Vector3D( [0.0, -3.0, 0.0] )
    if not result1.inexact_equal( answer1, geode.global_epsilon ):
        raise ValueError( "[Test] Wrong result for configuration 1" )

    result2 = geom.rotate( input, axis, -math.pi / 2.0 )
    answer2 = geom.Vector3D( [0.0, 3.0, 0.0] )
    if not result2.inexact_equal( answer2, geode.global_epsilon ):
        raise ValueError( "[Test] Wrong result for configuration 2" )

    result3 = geom.rotate( input, axis, 5*math.pi / 2.0 )
    answer3 = geom.Vector3D( [0.0, -3.0, 0.0] )
    if not result3.inexact_equal( answer3, geode.global_epsilon ):
        raise ValueError( "[Test] Wrong result for configuration 3" )