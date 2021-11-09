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
    NAME model
    FOLDER "geode/model"
    SOURCES
        "common.cpp"
        "helpers/convert_model_meshes.cpp"
        "helpers/convert_to_mesh.cpp"
        "helpers/simplicial_brep_creator.cpp"
        "helpers/simplicial_section_creator.cpp"
        "mixin/builder/blocks_builder.cpp"
        "mixin/builder/corners_builder.cpp"
        "mixin/builder/lines_builder.cpp"
        "mixin/builder/model_boundaries_builder.cpp"
        "mixin/builder/surfaces_builder.cpp"
        "mixin/builder/relationships_builder.cpp"
        "mixin/builder/vertex_identifier_builder.cpp"
        "mixin/core/bitsery_archive.cpp"
        "mixin/core/block.cpp"
        "mixin/core/blocks.cpp"
        "mixin/core/component.cpp"
        "mixin/core/corner.cpp"
        "mixin/core/corners.cpp"
        "mixin/core/line.cpp"
        "mixin/core/lines.cpp"
        "mixin/core/model_boundaries.cpp"
        "mixin/core/model_boundary.cpp"
        "mixin/core/relationships.cpp"
        "mixin/core/surface.cpp"
        "mixin/core/surfaces.cpp"
        "mixin/core/vertex_identifier.cpp"
        "representation/builder/brep_builder.cpp"
        "representation/builder/section_builder.cpp"
        "representation/core/brep.cpp"
        "representation/core/section.cpp"
        "representation/io/brep_input.cpp"
        "representation/io/section_input.cpp"
        "representation/io/brep_output.cpp"
        "representation/io/section_output.cpp"
    PUBLIC_HEADERS
        "common.h"
        "helpers/convert_model_meshes.h"
        "helpers/convert_to_mesh.h"
        "helpers/simplicial_brep_creator.h"
        "helpers/simplicial_creator_definitions.h"
        "helpers/simplicial_section_creator.h"
        "mixin/builder/add_components_builders.h"
        "mixin/builder/blocks_builder.h"
        "mixin/builder/corners_builder.h"
        "mixin/builder/lines_builder.h"
        "mixin/builder/model_boundaries_builder.h"
        "mixin/builder/surfaces_builder.h"
        "mixin/builder/relationships_builder.h"
        "mixin/builder/topology_builder.h"
        "mixin/builder/vertex_identifier_builder.h"
        "mixin/core/add_components.h"
        "mixin/core/bitsery_archive.h"
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
        "representation/builder/copy_mapping.h"
        "representation/builder/section_builder.h"
        "representation/core/brep.h"
        "representation/core/section.h"
        "representation/io/brep_input.h"
        "representation/io/section_input.h"
        "representation/io/brep_output.h"
        "representation/io/section_output.h"
    ADVANCED_HEADERS
        "mixin/core/detail/components_storage.h"
        "mixin/core/detail/count_relationships.h"
        "mixin/core/detail/mesh_storage.h"
        "mixin/core/detail/uuid_to_index.h"
        "representation/builder/detail/copy.h"
        "representation/io/detail/geode_brep_input.h"
        "representation/io/detail/geode_section_input.h"
        "representation/io/detail/geode_brep_output.h"
        "representation/io/detail/geode_section_output.h"
    PRIVATE_HEADERS
        "helpers/private/simplicial_model_creator.h"
    PUBLIC_DEPENDENCIES
        absl::flat_hash_map
        Bitsery::bitsery
        ghcFilesystem::ghc_filesystem
        ${PROJECT_NAME}::basic
        ${PROJECT_NAME}::geometry
        ${PROJECT_NAME}::mesh
    PRIVATE_DEPENDENCIES
        Async++
)

