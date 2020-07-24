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

add_geode_python_binding(
    NAME "py_model"
    SOURCES
        "model.cpp"
        "helpers/convert_model_meshes.h"
        "mixin/builder/blocks_builder.h"
        "mixin/builder/corners_builder.h"
        "mixin/builder/lines_builder.h"
        "mixin/builder/model_boundaries_builder.h"
        "mixin/builder/relationships_builder.h"
        "mixin/builder/surfaces_builder.h"
        "mixin/builder/topology_builder.h"
        "mixin/builder/vertex_identifier_builder.h"
        "mixin/core/block.h"
        "mixin/core/blocks.h"
        "mixin/core/component.h"
        "mixin/core/component_type.h"
        "mixin/core/corner.h"
        "mixin/core/corners.h"
        "mixin/core/line.h"
        "mixin/core/lines.h"
        "mixin/core/model_boundary.h"
        "mixin/core/model_boundaries.h"
        "mixin/core/relationships.h"
        "mixin/core/surface.h"
        "mixin/core/surfaces.h"
        "mixin/core/topology.h"
        "mixin/core/vertex_identifier.h"
        "representation/builder/brep_builder.h"
        "representation/builder/copy.h"
        "representation/builder/section_builder.h"
        "representation/core/brep.h"
        "representation/core/section.h"
        "representation/io/brep.h"
        "representation/io/section.h"
    DEPENDENCIES
        ${PROJECT_NAME}::model
)
