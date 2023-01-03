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
        "builder/edged_curve_builder.h"
        "builder/graph_builder.h"
        "builder/hybrid_solid_builder.h"
        "builder/point_set_builder.h"
        "builder/polygonal_surface_builder.h"
        "builder/polyhedral_solid_builder.h"
        "builder/regular_grid_builder.h"
        "builder/solid_edges_builder.h"
        "builder/solid_facets_builder.h"
        "builder/solid_mesh_builder.h"
        "builder/surface_edges_builder.h"
        "builder/surface_mesh_builder.h"
        "builder/tetrahedral_solid_builder.h"
        "builder/triangulated_surface_builder.h"
        "builder/vertex_set_builder.h"
        "core/edged_curve.h"
        "core/graph.h"
        "core/hybrid_solid.h"
        "core/point_set.h"
        "core/polygonal_surface.h"
        "core/polyhedral_solid.h"
        "core/regular_grid.h"
        "core/solid_edges.h"
        "core/solid_facets.h"
        "core/solid_mesh.h"
        "core/surface_edges.h"
        "core/surface_mesh.h"
        "core/tetrahedral_solid.h"
        "core/triangulated_surface.h"
        "core/vertex_set.h"
        "helpers/convert_edged_curve.h"
        "helpers/convert_point_set.h"
        "helpers/convert_solid_mesh.h"
        "helpers/convert_surface_mesh.h"
        "helpers/geometrical_operations_on_mesh.h"
        "io/edged_curve.h"
        "io/graph.h"
        "io/hybrid_solid.h"
        "io/point_set.h"
        "io/polygonal_surface.h"
        "io/polyhedral_solid.h"
        "io/regular_grid.h"
        "io/tetrahedral_solid.h"
        "io/triangulated_surface.h"
        "io/vertex_set.h"
    DEPENDENCIES
        ${PROJECT_NAME}::mesh
)
