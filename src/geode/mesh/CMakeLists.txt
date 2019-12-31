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

set(folder "geode/mesh")
set(lib_source_dir ${PROJECT_SOURCE_DIR}/src/${folder})
set(lib_include_dir ${PROJECT_SOURCE_DIR}/include/${folder})

add_geode_library(
    NAME mesh
    FOLDER "${folder}"
    SOURCES
        "${lib_source_dir}/builder/edged_curve_builder.cpp"
        "${lib_source_dir}/builder/geode_point_set_builder.cpp"
        "${lib_source_dir}/builder/geode_edged_curve_builder.cpp"
        "${lib_source_dir}/builder/geode_graph_builder.cpp"
        "${lib_source_dir}/builder/geode_polygonal_surface_builder.cpp"
        "${lib_source_dir}/builder/geode_polyhedral_solid_builder.cpp"
        "${lib_source_dir}/builder/geode_tetrahedral_solid_builder.cpp"
        "${lib_source_dir}/builder/geode_triangulated_surface_builder.cpp"
        "${lib_source_dir}/builder/geode_vertex_set_builder.cpp"
        "${lib_source_dir}/builder/graph_builder.cpp"
        "${lib_source_dir}/builder/point_set_builder.cpp"
        "${lib_source_dir}/builder/polygonal_surface_builder.cpp"
        "${lib_source_dir}/builder/polyhedral_solid_builder.cpp"
        "${lib_source_dir}/builder/register_builder.cpp"
        "${lib_source_dir}/builder/tetrahedral_solid_builder.cpp"
        "${lib_source_dir}/builder/triangulated_surface_builder.cpp"
        "${lib_source_dir}/builder/vertex_set_builder.cpp"
        "${lib_source_dir}/common.cpp"
        "${lib_source_dir}/core/bitsery_archive.cpp"
        "${lib_source_dir}/core/edged_curve.cpp"
        "${lib_source_dir}/core/geode_point_set.cpp"
        "${lib_source_dir}/core/geode_edged_curve.cpp"
        "${lib_source_dir}/core/geode_graph.cpp"
        "${lib_source_dir}/core/geode_polygonal_surface.cpp"
        "${lib_source_dir}/core/geode_polyhedral_solid.cpp"
        "${lib_source_dir}/core/register_mesh.cpp"
        "${lib_source_dir}/core/geode_tetrahedral_solid.cpp"
        "${lib_source_dir}/core/geode_triangulated_surface.cpp"
        "${lib_source_dir}/core/graph.cpp"
        "${lib_source_dir}/core/point_set.cpp"
        "${lib_source_dir}/core/polygonal_surface.cpp"
        "${lib_source_dir}/core/polyhedral_solid.cpp"
        "${lib_source_dir}/core/tetrahedral_solid.cpp"
        "${lib_source_dir}/core/triangulated_surface.cpp"
        "${lib_source_dir}/core/vertex_set.cpp"
        "${lib_source_dir}/io/edged_curve_input.cpp"
        "${lib_source_dir}/io/edged_curve_output.cpp"
        "${lib_source_dir}/io/graph_input.cpp"
        "${lib_source_dir}/io/graph_output.cpp"
        "${lib_source_dir}/io/io.cpp"
        "${lib_source_dir}/io/point_set_input.cpp"
        "${lib_source_dir}/io/point_set_output.cpp"
        "${lib_source_dir}/io/polygonal_surface_input.cpp"
        "${lib_source_dir}/io/polygonal_surface_output.cpp"
        "${lib_source_dir}/io/polyhedral_solid_input.cpp"
        "${lib_source_dir}/io/polyhedral_solid_output.cpp"
        "${lib_source_dir}/io/register_input.cpp"
        "${lib_source_dir}/io/register_output.cpp"
        "${lib_source_dir}/io/tetrahedral_solid_input.cpp"
        "${lib_source_dir}/io/tetrahedral_solid_output.cpp"
        "${lib_source_dir}/io/triangulated_surface_input.cpp"
        "${lib_source_dir}/io/triangulated_surface_output.cpp"
        "${lib_source_dir}/io/vertex_set_input.cpp"
        "${lib_source_dir}/io/vertex_set_output.cpp"
    PUBLIC_HEADERS
        "${lib_include_dir}/common.h"
        "${lib_include_dir}/builder/edged_curve_builder.h"
        "${lib_include_dir}/builder/geode_edged_curve_builder.h"
        "${lib_include_dir}/builder/geode_graph_builder.h"
        "${lib_include_dir}/builder/geode_point_set_builder.h"
        "${lib_include_dir}/builder/geode_polygonal_surface_builder.h"
        "${lib_include_dir}/builder/geode_polyhedral_solid_builder.h"
        "${lib_include_dir}/builder/geode_tetrahedral_solid_builder.h"
        "${lib_include_dir}/builder/geode_triangulated_surface_builder.h"
        "${lib_include_dir}/builder/geode_vertex_set_builder.h"
        "${lib_include_dir}/builder/graph_builder.h"
        "${lib_include_dir}/builder/point_set_builder.h"
        "${lib_include_dir}/builder/polygonal_surface_builder.h"
        "${lib_include_dir}/builder/polyhedral_solid_builder.h"
        "${lib_include_dir}/builder/tetrahedral_solid_builder.h"
        "${lib_include_dir}/builder/triangulated_surface_builder.h"
        "${lib_include_dir}/builder/vertex_set_builder.h"
        "${lib_include_dir}/core/bitsery_archive.h"
        "${lib_include_dir}/core/edged_curve.h"
        "${lib_include_dir}/core/geode_edged_curve.h"
        "${lib_include_dir}/core/geode_graph.h"
        "${lib_include_dir}/core/geode_point_set.h"
        "${lib_include_dir}/core/geode_polygonal_surface.h"
        "${lib_include_dir}/core/geode_polyhedral_solid.h"
        "${lib_include_dir}/core/geode_tetrahedral_solid.h"
        "${lib_include_dir}/core/geode_triangulated_surface.h"
        "${lib_include_dir}/core/geode_vertex_set.h"
        "${lib_include_dir}/core/graph.h"
        "${lib_include_dir}/core/mesh_type.h"
        "${lib_include_dir}/core/point_set.h"
        "${lib_include_dir}/core/polygonal_surface.h"
        "${lib_include_dir}/core/polyhedral_solid.h"
        "${lib_include_dir}/core/tetrahedral_solid.h"
        "${lib_include_dir}/core/triangulated_surface.h"
        "${lib_include_dir}/core/vertex_set.h"
        "${lib_include_dir}/io/edged_curve_input.h"
        "${lib_include_dir}/io/edged_curve_output.h"
        "${lib_include_dir}/io/graph_input.h"
        "${lib_include_dir}/io/graph_output.h"
        "${lib_include_dir}/io/input.h"
        "${lib_include_dir}/io/io.h"
        "${lib_include_dir}/io/output.h"
        "${lib_include_dir}/io/point_set_input.h"
        "${lib_include_dir}/io/point_set_output.h"
        "${lib_include_dir}/io/polygonal_surface_input.h"
        "${lib_include_dir}/io/polygonal_surface_output.h"
        "${lib_include_dir}/io/polyhedral_solid_input.h"
        "${lib_include_dir}/io/polyhedral_solid_output.h"
        "${lib_include_dir}/io/tetrahedral_solid_input.h"
        "${lib_include_dir}/io/tetrahedral_solid_output.h"
        "${lib_include_dir}/io/triangulated_surface_input.h"
        "${lib_include_dir}/io/triangulated_surface_output.h"
        "${lib_include_dir}/io/vertex_set_input.h"
        "${lib_include_dir}/io/vertex_set_output.h"
    ADVANCED_HEADERS
        "${lib_include_dir}/builder/detail/register_builder.h"
        "${lib_include_dir}/core/detail/edges_impl.h"
        "${lib_include_dir}/core/detail/facet_storage.h"
        "${lib_include_dir}/core/detail/points_impl.h"
        "${lib_include_dir}/core/detail/register_mesh.h"
        "${lib_include_dir}/core/detail/vertex_cycle.h"
        "${lib_include_dir}/io/detail/geode_bitsery_mesh_input.h"
        "${lib_include_dir}/io/detail/geode_bitsery_mesh_output.h"
        "${lib_include_dir}/io/detail/geode_edged_curve_input.h"
        "${lib_include_dir}/io/detail/geode_edged_curve_output.h"
        "${lib_include_dir}/io/detail/geode_graph_input.h"
        "${lib_include_dir}/io/detail/geode_graph_output.h"
        "${lib_include_dir}/io/detail/geode_point_set_input.h"
        "${lib_include_dir}/io/detail/geode_point_set_output.h"
        "${lib_include_dir}/io/detail/geode_polygonal_surface_input.h"
        "${lib_include_dir}/io/detail/geode_polygonal_surface_output.h"
        "${lib_include_dir}/io/detail/geode_polyhedral_solid_input.h"
        "${lib_include_dir}/io/detail/geode_polyhedral_solid_output.h"
        "${lib_include_dir}/io/detail/geode_tetrahedral_solid_input.h"
        "${lib_include_dir}/io/detail/geode_tetrahedral_solid_output.h"
        "${lib_include_dir}/io/detail/geode_triangulated_surface_input.h"
        "${lib_include_dir}/io/detail/geode_triangulated_surface_output.h"
        "${lib_include_dir}/io/detail/geode_vertex_set_input.h"
        "${lib_include_dir}/io/detail/register_input.h"
        "${lib_include_dir}/io/detail/register_output.h"
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
)
