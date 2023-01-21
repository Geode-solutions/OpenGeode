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

add_geode_python_test(
    SOURCE "test-py-vertex-set.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-graph.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-edged-curve.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-euclidean-distance-transform.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-point-set.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-polygonal-surface.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-triangulated-surface.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-polyhedral-solid.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-tetrahedral-solid.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-regular-grid.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)
add_geode_python_test(
    SOURCE "test-py-geometrical-opeations-on-mesh.py"
    DEPENDENCIES 
        ${PROJECT_NAME}::py_basic
        ${PROJECT_NAME}::py_geometry
        ${PROJECT_NAME}::py_mesh
)

