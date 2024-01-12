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

add_geode_python_binding(
    NAME "py_mesh"
    SOURCES
        "mesh.cpp"
        "builder/crs_manager_builder.cpp"
        "builder/crs_managers_builder.cpp"
        "builder/edged_curve_builder.cpp"
        "builder/graph_builder.cpp"
        "builder/hybrid_solid_builder.cpp"
        "builder/point_set_builder.cpp"
        "builder/polygonal_surface_builder.cpp"
        "builder/polyhedral_solid_builder.cpp"
        "builder/regular_grid_builder.cpp"
        "builder/solid_edges_builder.cpp"
        "builder/solid_facets_builder.cpp"
        "builder/solid_mesh_builder.cpp"
        "builder/surface_edges_builder.cpp"
        "builder/surface_mesh_builder.cpp"
        "builder/tetrahedral_solid_builder.cpp"
        "builder/triangulated_surface_builder.cpp"
        "builder/vertex_set_builder.cpp"
        "core/crs.cpp"
        "core/crs_manager.cpp"
        "core/crs_managers.cpp"
        "core/edged_curve.cpp"
        "core/graph.cpp"
        "core/grid.cpp"
        "core/hybrid_solid.cpp"
        "core/light_regular_grid.cpp"
        "core/point_set.cpp"
        "core/polygonal_surface.cpp"
        "core/polyhedral_solid.cpp"
        "core/regular_grid.cpp"
        "core/solid_edges.cpp"
        "core/solid_facets.cpp"
        "core/solid_mesh.cpp"
        "core/surface_edges.cpp"
        "core/surface_mesh.cpp"
        "core/tetrahedral_solid.cpp"
        "core/triangulated_surface.cpp"
        "core/vertex_set.cpp"
        "core/texture.cpp"
        "core/texture_manager.cpp"
        "helpers/convert_edged_curve.cpp"
        "helpers/convert_point_set.cpp"
        "helpers/convert_solid_mesh.cpp"
        "helpers/convert_surface_mesh.cpp"
        "helpers/crs_helper.cpp"
        "helpers/euclidean_distance_transform.cpp"
        "helpers/geometrical_operations_on_mesh.cpp"
        "helpers/repair_polygon_orientations.cpp"
        "io/edged_curve.cpp"
        "io/graph.cpp"
        "io/hybrid_solid.cpp"
        "io/light_regular_grid.cpp"
        "io/point_set.cpp"
        "io/polygonal_surface.cpp"
        "io/polyhedral_solid.cpp"
        "io/regular_grid.cpp"
        "io/tetrahedral_solid.cpp"
        "io/triangulated_surface.cpp"
        "io/vertex_set.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::image
        ${PROJECT_NAME}::mesh
)
