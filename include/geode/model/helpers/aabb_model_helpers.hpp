/*
 * Copyright (c) 2019 - 2024 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <absl/container/fixed_array.h>
#include <absl/container/flat_hash_map.h>

#include <geode/basic/uuid.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( AABBTree );
    ALIAS_2D_AND_3D( AABBTree );
    class BRep;
    class Section;
} // namespace geode

namespace geode
{
    std::tuple< AABBTree3D, absl::FixedArray< uuid > >
        opengeode_model_api create_lines_aabb_tree( const BRep& model );

    std::tuple< AABBTree3D, absl::FixedArray< uuid > >
        opengeode_model_api create_surfaces_aabb_tree( const BRep& model );

    std::tuple< AABBTree3D, absl::FixedArray< uuid > >
        opengeode_model_api create_blocks_aabb_tree( const BRep& model );

    std::tuple< AABBTree2D, absl::FixedArray< uuid > >
        opengeode_model_api create_lines_aabb_tree( const Section& model );

    std::tuple< AABBTree2D, absl::FixedArray< uuid > >
        opengeode_model_api create_surfaces_aabb_tree( const Section& model );

    template < index_t dimension >
    struct ModelMeshesAABBTree
    {
        explicit ModelMeshesAABBTree( index_t nb_components )
            : mesh_trees_( nb_components ), uuids_( nb_components )
        {
            mesh_tree_ids_.reserve( nb_components );
        }

        AABBTree< dimension > components_tree_;
        absl::FixedArray< AABBTree< dimension > > mesh_trees_;
        absl::FixedArray< uuid > uuids_;
        absl::flat_hash_map< uuid, index_t > mesh_tree_ids_;
    };
    ALIAS_2D_AND_3D( ModelMeshesAABBTree );

    ModelMeshesAABBTree3D opengeode_model_api create_line_meshes_aabb_trees(
        const BRep& model );

    ModelMeshesAABBTree3D opengeode_model_api create_surface_meshes_aabb_trees(
        const BRep& model );

    ModelMeshesAABBTree3D opengeode_model_api create_block_meshes_aabb_trees(
        const BRep& model );

    ModelMeshesAABBTree2D opengeode_model_api create_line_meshes_aabb_trees(
        const Section& model );

    ModelMeshesAABBTree2D opengeode_model_api create_surface_meshes_aabb_trees(
        const Section& model );

} // namespace geode