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

set(folder "geode/model")
set(lib_source_dir ${PROJECT_SOURCE_DIR}/src/${folder})
set(lib_include_dir ${PROJECT_SOURCE_DIR}/include/${folder})

add_geode_library(
    NAME model
    FOLDER "${folder}"
    SOURCES
        "${lib_source_dir}/common.cpp"
        "${lib_source_dir}/mixin/builder/blocks_builder.cpp"
        "${lib_source_dir}/mixin/builder/corners_builder.cpp"
        "${lib_source_dir}/mixin/builder/lines_builder.cpp"
        "${lib_source_dir}/mixin/builder/model_boundaries_builder.cpp"
        "${lib_source_dir}/mixin/builder/surfaces_builder.cpp"
        "${lib_source_dir}/mixin/builder/relationships_builder.cpp"
        "${lib_source_dir}/mixin/builder/vertex_identifier_builder.cpp"
        "${lib_source_dir}/mixin/core/bitsery_archive.cpp"
        "${lib_source_dir}/mixin/core/block.cpp"
        "${lib_source_dir}/mixin/core/blocks.cpp"
        "${lib_source_dir}/mixin/core/component.cpp"
        "${lib_source_dir}/mixin/core/corner.cpp"
        "${lib_source_dir}/mixin/core/corners.cpp"
        "${lib_source_dir}/mixin/core/line.cpp"
        "${lib_source_dir}/mixin/core/lines.cpp"
        "${lib_source_dir}/mixin/core/model_boundaries.cpp"
        "${lib_source_dir}/mixin/core/model_boundary.cpp"
        "${lib_source_dir}/mixin/core/relationships.cpp"
        "${lib_source_dir}/mixin/core/surface.cpp"
        "${lib_source_dir}/mixin/core/surfaces.cpp"
        "${lib_source_dir}/mixin/core/vertex_identifier.cpp"
        "${lib_source_dir}/representation/builder/brep_builder.cpp"
        "${lib_source_dir}/representation/builder/section_builder.cpp"
        "${lib_source_dir}/representation/core/brep.cpp"
        "${lib_source_dir}/representation/core/section.cpp"
        "${lib_source_dir}/representation/io/brep_input.cpp"
        "${lib_source_dir}/representation/io/section_input.cpp"
        "${lib_source_dir}/representation/io/brep_output.cpp"
        "${lib_source_dir}/representation/io/section_output.cpp"
    PUBLIC_HEADERS
        "${lib_include_dir}/common.h"
        "${lib_include_dir}/mixin/builder/add_components_builders.h"
        "${lib_include_dir}/mixin/builder/blocks_builder.h"
        "${lib_include_dir}/mixin/builder/corners_builder.h"
        "${lib_include_dir}/mixin/builder/lines_builder.h"
        "${lib_include_dir}/mixin/builder/model_boundaries_builder.h"
        "${lib_include_dir}/mixin/builder/surfaces_builder.h"
        "${lib_include_dir}/mixin/builder/relationships_builder.h"
        "${lib_include_dir}/mixin/builder/topology_builder.h"
        "${lib_include_dir}/mixin/builder/vertex_identifier_builder.h"
        "${lib_include_dir}/mixin/core/add_components.h"
        "${lib_include_dir}/mixin/core/bitsery_archive.h"
        "${lib_include_dir}/mixin/core/block.h"
        "${lib_include_dir}/mixin/core/blocks.h"
        "${lib_include_dir}/mixin/core/component.h"
        "${lib_include_dir}/mixin/core/component_type.h"
        "${lib_include_dir}/mixin/core/corner.h"
        "${lib_include_dir}/mixin/core/corners.h"
        "${lib_include_dir}/mixin/core/line.h"
        "${lib_include_dir}/mixin/core/lines.h"
        "${lib_include_dir}/mixin/core/model_boundary.h"
        "${lib_include_dir}/mixin/core/model_boundaries.h"
        "${lib_include_dir}/mixin/core/relationships.h"
        "${lib_include_dir}/mixin/core/surface.h"
        "${lib_include_dir}/mixin/core/surfaces.h"
        "${lib_include_dir}/mixin/core/topology.h"
        "${lib_include_dir}/mixin/core/vertex_identifier.h"
        "${lib_include_dir}/representation/builder/brep_builder.h"
        "${lib_include_dir}/representation/builder/section_builder.h"
        "${lib_include_dir}/representation/core/brep.h"
        "${lib_include_dir}/representation/core/section.h"
        "${lib_include_dir}/representation/io/brep_input.h"
        "${lib_include_dir}/representation/io/section_input.h"
        "${lib_include_dir}/representation/io/brep_output.h"
        "${lib_include_dir}/representation/io/section_output.h"
    ADVANCED_HEADERS
        "${lib_include_dir}/mixin/core/detail/components_storage.h"
        "${lib_include_dir}/mixin/core/detail/count_relationships.h"
        "${lib_include_dir}/mixin/core/detail/mesh_storage.h"
        "${lib_include_dir}/mixin/core/detail/uuid_to_index.h"
        "${lib_include_dir}/representation/builder/detail/copy.h"
        "${lib_include_dir}/representation/io/detail/geode_brep_input.h"
        "${lib_include_dir}/representation/io/detail/geode_section_input.h"
        "${lib_include_dir}/representation/io/detail/geode_brep_output.h"
        "${lib_include_dir}/representation/io/detail/geode_section_output.h"
    PUBLIC_DEPENDENCIES
        Bitsery::bitsery
        ghcFilesystem::ghc_filesystem
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
)

