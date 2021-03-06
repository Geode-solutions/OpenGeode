# Copyright (c) 2019 - 2021 Geode-solutions
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

add_geode_library(
    NAME mesh
    FOLDER "geode/mesh"
    SOURCES
        "builder/edged_curve_builder.cpp"
        "builder/geode_point_set_builder.cpp"
        "builder/geode_edged_curve_builder.cpp"
        "builder/geode_graph_builder.cpp"
        "builder/geode_hybrid_solid_builder.cpp"
        "builder/geode_polygonal_surface_builder.cpp"
        "builder/geode_polyhedral_solid_builder.cpp"
        "builder/geode_tetrahedral_solid_builder.cpp"
        "builder/geode_triangulated_surface_builder.cpp"
        "builder/geode_vertex_set_builder.cpp"
        "builder/graph_builder.cpp"
        "builder/hybrid_solid_builder.cpp"
        "builder/point_set_builder.cpp"
        "builder/polygonal_surface_builder.cpp"
        "builder/polygonal_surface_view_builder.cpp"
        "builder/polyhedral_solid_builder.cpp"
        "builder/polyhedral_solid_view_builder.cpp"
        "builder/register_builder.cpp"
        "builder/surface_edges_builder.cpp"
        "builder/surface_mesh_builder.cpp"
        "builder/solid_edges_builder.cpp"
        "builder/solid_facets_builder.cpp"
        "builder/solid_mesh_builder.cpp"
        "builder/surface_mesh_builder.cpp"
        "builder/tetrahedral_solid_builder.cpp"
        "builder/tetrahedral_solid_view_builder.cpp"
        "builder/triangulated_surface_builder.cpp"
        "builder/triangulated_surface_view_builder.cpp"
        "builder/vertex_set_builder.cpp"
        "common.cpp"
        "core/bitsery_archive.cpp"
        "core/edged_curve.cpp"
        "core/geode_point_set.cpp"
        "core/geode_edged_curve.cpp"
        "core/geode_graph.cpp"
        "core/geode_hybrid_solid.cpp"
        "core/geode_polygonal_surface.cpp"
        "core/geode_polyhedral_solid.cpp"
        "core/geode_tetrahedral_solid.cpp"
        "core/geode_triangulated_surface.cpp"
        "core/graph.cpp"
        "core/hybrid_solid.cpp"
        "core/mesh_factory.cpp"
        "core/point_set.cpp"
        "core/polygonal_surface.cpp"
        "core/polygonal_surface_view.cpp"
        "core/polyhedral_solid.cpp"
        "core/polyhedral_solid_view.cpp"
        "core/register_mesh.cpp"
        "core/regular_grid.cpp"
        "core/solid_facets.cpp"
        "core/solid_edges.cpp"
        "core/solid_mesh.cpp"
        "core/surface_edges.cpp"
        "core/surface_mesh.cpp"
        "core/tetrahedral_solid.cpp"
        "core/tetrahedral_solid_view.cpp"
        "core/triangulated_surface.cpp"
        "core/triangulated_surface_view.cpp"
        "core/vertex_set.cpp"
        "helpers/aabb_edged_curve_helpers.cpp"
        "helpers/aabb_surface_helpers.cpp"
        "helpers/convert_surface_mesh.cpp"
        "helpers/convert_solid_mesh.cpp"
        "io/edged_curve_input.cpp"
        "io/edged_curve_output.cpp"
        "io/graph_input.cpp"
        "io/graph_output.cpp"
        "io/hybrid_solid_input.cpp"
        "io/hybrid_solid_output.cpp"
        "io/io.cpp"
        "io/point_set_input.cpp"
        "io/point_set_output.cpp"
        "io/polygonal_surface_input.cpp"
        "io/polygonal_surface_output.cpp"
        "io/polyhedral_solid_input.cpp"
        "io/polyhedral_solid_output.cpp"
        "io/register_input.cpp"
        "io/register_output.cpp"
        "io/regular_grid_input.cpp"
        "io/regular_grid_output.cpp"
        "io/tetrahedral_solid_input.cpp"
        "io/tetrahedral_solid_output.cpp"
        "io/triangulated_surface_input.cpp"
        "io/triangulated_surface_output.cpp"
        "io/vertex_set_input.cpp"
        "io/vertex_set_output.cpp"
    PUBLIC_HEADERS
        "common.h"
        "builder/edged_curve_builder.h"
        "builder/geode_edged_curve_builder.h"
        "builder/geode_graph_builder.h"
        "builder/geode_hybrid_solid_builder.h"
        "builder/geode_point_set_builder.h"
        "builder/geode_polygonal_surface_builder.h"
        "builder/geode_polyhedral_solid_builder.h"
        "builder/geode_tetrahedral_solid_builder.h"
        "builder/geode_triangulated_surface_builder.h"
        "builder/geode_vertex_set_builder.h"
        "builder/graph_builder.h"
        "builder/hybrid_solid_builder.h"
        "builder/mesh_builder_factory.h"
        "builder/point_set_builder.h"
        "builder/polygonal_surface_builder.h"
        "builder/polygonal_surface_view_builder.h"
        "builder/polyhedral_solid_builder.h"
        "builder/polyhedral_solid_view_builder.h"
        "builder/surface_edges_builder.h"
        "builder/surface_mesh_builder.h"
        "builder/solid_facets_builder.h"
        "builder/solid_edges_builder.h"
        "builder/solid_mesh_builder.h"
        "builder/tetrahedral_solid_builder.h"
        "builder/tetrahedral_solid_view_builder.h"
        "builder/triangulated_surface_builder.h"
        "builder/triangulated_surface_view_builder.h"
        "builder/vertex_set_builder.h"
        "core/bitsery_archive.h"
        "core/edged_curve.h"
        "core/geode_edged_curve.h"
        "core/geode_graph.h"
        "core/geode_hybrid_solid.h"
        "core/geode_point_set.h"
        "core/geode_polygonal_surface.h"
        "core/geode_polyhedral_solid.h"
        "core/geode_tetrahedral_solid.h"
        "core/geode_triangulated_surface.h"
        "core/geode_vertex_set.h"
        "core/graph.h"
        "core/hybrid_solid.h"
        "core/mesh_factory.h"
        "core/mesh_id.h"
        "core/point_set.h"
        "core/polygonal_surface.h"
        "core/polygonal_surface_view.h"
        "core/polyhedral_solid.h"
        "core/polyhedral_solid_view.h"
        "core/regular_grid.h"
        "core/solid_edges.h"
        "core/solid_facets.h"
        "core/solid_mesh.h"
        "core/surface_edges.h"
        "core/surface_mesh.h"
        "core/tetrahedral_solid.h"
        "core/tetrahedral_solid_view.h"
        "core/triangulated_surface.h"
        "core/triangulated_surface_view.h"
        "core/vertex_set.h"
        "helpers/aabb_edged_curve_helpers.h"
        "helpers/aabb_surface_helpers.h"
        "helpers/convert_surface_mesh.h"
        "helpers/convert_solid_mesh.h"
        "io/edged_curve_input.h"
        "io/edged_curve_output.h"
        "io/graph_input.h"
        "io/graph_output.h"
        "io/hybrid_solid_input.h"
        "io/hybrid_solid_output.h"
        "io/input.h"
        "io/io.h"
        "io/output.h"
        "io/point_set_input.h"
        "io/point_set_output.h"
        "io/polygonal_surface_input.h"
        "io/polygonal_surface_output.h"
        "io/polyhedral_solid_input.h"
        "io/polyhedral_solid_output.h"
        "io/regular_grid_input.h"
        "io/regular_grid_output.h"
        "io/tetrahedral_solid_input.h"
        "io/tetrahedral_solid_output.h"
        "io/triangulated_surface_input.h"
        "io/triangulated_surface_output.h"
        "io/vertex_set_input.h"
        "io/vertex_set_output.h"
    ADVANCED_HEADERS
        "builder/detail/register_builder.h"
        "core/detail/edges_impl.h"
        "core/detail/facet_edges_impl.h"
        "core/detail/facet_storage.h"
        "core/detail/geode_elements.h"
        "core/detail/points_impl.h"
        "core/detail/points_view_impl.h"
        "core/detail/register_mesh.h"
        "core/detail/solid_mesh_view_impl.h"
        "core/detail/surface_mesh_view_impl.h"
        "core/detail/vertex_cycle.h"
        "io/detail/geode_bitsery_mesh_input.h"
        "io/detail/geode_bitsery_mesh_output.h"
        "io/detail/geode_edged_curve_input.h"
        "io/detail/geode_edged_curve_output.h"
        "io/detail/geode_graph_input.h"
        "io/detail/geode_graph_output.h"
        "io/detail/geode_hybrid_solid_input.h"
        "io/detail/geode_hybrid_solid_output.h"
        "io/detail/geode_point_set_input.h"
        "io/detail/geode_point_set_output.h"
        "io/detail/geode_polygonal_surface_input.h"
        "io/detail/geode_polygonal_surface_output.h"
        "io/detail/geode_polyhedral_solid_input.h"
        "io/detail/geode_polyhedral_solid_output.h"
        "io/detail/geode_regular_grid_input.h"
        "io/detail/geode_regular_grid_output.h"
        "io/detail/geode_tetrahedral_solid_input.h"
        "io/detail/geode_tetrahedral_solid_output.h"
        "io/detail/geode_triangulated_surface_input.h"
        "io/detail/geode_triangulated_surface_output.h"
        "io/detail/geode_vertex_set_input.h"
        "io/detail/register_input.h"
        "io/detail/register_output.h"
    PUBLIC_DEPENDENCIES
        absl::flat_hash_map
        Bitsery::bitsery
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
