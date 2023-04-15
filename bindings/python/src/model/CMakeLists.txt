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
    NAME "py_model"
    SOURCES
        "model.cpp"
        "helpers/component_mesh_polygons.cpp"
        "helpers/convert_brep_section.cpp"
        "helpers/convert_model_meshes.cpp"
        "helpers/convert_to_mesh.cpp"
        "helpers/model_component_filter.cpp"
        "helpers/model_concatener.cpp"
        "mixin/builder/blocks_builder.cpp"
        "mixin/builder/corners_builder.cpp"
        "mixin/builder/lines_builder.cpp"
        "mixin/builder/model_boundaries_builder.cpp"
        "mixin/builder/relationships_builder.cpp"
        "mixin/builder/surfaces_builder.cpp"
        "mixin/builder/topology_builder.cpp"
        "mixin/builder/vertex_identifier_builder.cpp"
        "mixin/core/block.cpp"
        "mixin/core/blocks.cpp"
        "mixin/core/component.cpp"
        "mixin/core/component_mesh_element.cpp"
        "mixin/core/component_type.cpp"
        "mixin/core/corner.cpp"
        "mixin/core/corners.cpp"
        "mixin/core/line.cpp"
        "mixin/core/lines.cpp"
        "mixin/core/model_boundary.cpp"
        "mixin/core/model_boundaries.cpp"
        "mixin/core/relationships.cpp"
        "mixin/core/surface.cpp"
        "mixin/core/surfaces.cpp"
        "mixin/core/topology.cpp"
        "mixin/core/vertex_identifier.cpp"
        "representation/builder/brep_builder.cpp"
        "representation/builder/copy.cpp"
        "representation/builder/section_builder.cpp"
        "representation/core/brep.cpp"
        "representation/core/section.cpp"
        "representation/io/brep.cpp"
        "representation/io/section.cpp"
    DEPENDENCIES
        ${PROJECT_NAME}::model
)
