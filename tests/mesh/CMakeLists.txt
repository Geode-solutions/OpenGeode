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

add_geode_test(
    SOURCE "test-aabb-edged-curve-helpers.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-aabb-triangulated-surface-helpers.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-aabb-tetrahedral-solid-helpers.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-convert-surface.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-coordinate-reference-system.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-edged-curve.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-euclidean-distance-transform.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-generic-mesh-accessor.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-geometrical-operations-on-mesh.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-graph.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-hausdorff-distance.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-hybrid-solid.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-light-regular-grid.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-merge-curve.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-merge-solid.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-merge-surface.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
    ESSENTIAL
)
add_geode_test(
    SOURCE "test-mesh-factory.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-nnsearch-point-set.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-point-set.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-polygonal-surface.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
    ESSENTIAL
)
add_geode_test(
    SOURCE "test-polyhedral-solid.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-rasterize.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
    ESSENTIAL
)
add_geode_test(
    SOURCE "test-ray-tracing.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-register-builder.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-register-input.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-register-mesh.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-register-output.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-regular-grid.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-regular-grid-function.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-repair-polygon-orientations.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-tetrahedral-solid.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-tetrahedral-solid-function.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-texture-manager.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::image
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-triangulated-surface.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-triangulated-surface-function.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-vertex-cycle.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::mesh
)
add_geode_test(
    SOURCE "test-vertex-set.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)
