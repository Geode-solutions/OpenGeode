# Copyright (c) 2019 Geode-solutions
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

add_geode_test(test-edged-curve.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-graph.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-point-set.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-polygonal-surface.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-polyhedral-solid.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-register-builder.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-register-input.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-register-mesh.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-register-output.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-tetrahedral-solid.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-triangulated-surface.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-vertex-cycle.cpp ${PROJECT_NAME}::mesh)
add_geode_test(test-vertex-set.cpp ${PROJECT_NAME}::mesh)
