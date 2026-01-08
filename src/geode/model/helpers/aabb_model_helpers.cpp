/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/helpers/aabb_model_helpers.hpp>

#include <async++.h>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/bounding_box.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/aabb_edged_curve_helpers.hpp>
#include <geode/mesh/helpers/aabb_solid_helpers.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Range >
    geode::index_t count_non_empty_elements(
        const Range& range, geode::index_t nb_elements )
    {
        geode::index_t nb_non_empty_elements{ 0 };
        for( const auto& element : range )
        {
            if( element.mesh().nb_vertices() > 0 )
            {
                nb_non_empty_elements++;
            }
        }
        if( nb_non_empty_elements < nb_elements )
        {
            geode::Logger::warn( absl::StrCat( "[create_model_aabb] ",
                nb_elements - nb_non_empty_elements, " out of ", nb_elements,
                " components have empty meshes, not included in the "
                "AABBTree." ) );
        }
        return nb_non_empty_elements;
    }

    template < geode::index_t dimension, typename Range >
    std::tuple< geode::AABBTree< dimension >, absl::FixedArray< geode::uuid > >
        create_aabb( const Range& range, geode::index_t nb_elements )
    {
        const auto nb_non_empty_elements =
            count_non_empty_elements( range, nb_elements );
        absl::FixedArray< geode::BoundingBox< dimension > > boxes(
            nb_non_empty_elements );
        absl::FixedArray< geode::uuid > mapping( nb_non_empty_elements );
        absl::FixedArray< async::task< void > > tasks( nb_non_empty_elements );
        geode::index_t id{ 0 };
        for( const auto& element : range )
        {
            if( element.mesh().nb_vertices() == 0 )
            {
                continue;
            }
            tasks[id] = async::spawn( [id, &mapping, &boxes, &element] {
                mapping[id] = element.id();
                boxes[id] = element.mesh().bounding_box();
            } );
            id++;
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
        return std::make_tuple(
            geode::AABBTree< dimension >{ boxes }, std::move( mapping ) );
    }

    template < geode::index_t dimension, typename Range >
    geode::ModelMeshesAABBTree< dimension > create_aabbs(
        const Range& range, geode::index_t nb_elements )
    {
        const auto nb_non_empty_elements =
            count_non_empty_elements( range, nb_elements );
        geode::ModelMeshesAABBTree< dimension > result{ nb_non_empty_elements };
        absl::FixedArray< geode::BoundingBox< dimension > > boxes(
            nb_non_empty_elements );
        absl::FixedArray< async::task< void > > tasks( nb_non_empty_elements );
        geode::index_t id{ 0 };
        for( const auto& element : range )
        {
            if( element.mesh().nb_vertices() == 0 )
            {
                continue;
            }
            tasks[id] = async::spawn( [id, &result, &boxes, &element] {
                result.mesh_trees_[id] =
                    geode::create_aabb_tree( element.mesh() );
                result.uuids_[id] = element.id();
                boxes[id] = result.mesh_trees_[id].bounding_box();
            } );
            result.mesh_tree_ids_.emplace( element.id(), id );
            id++;
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
        result.components_tree_ = geode::AABBTree< dimension >{ boxes };
        return result;
    }
} // namespace

namespace geode
{
    std::tuple< AABBTree3D, absl::FixedArray< uuid > > create_lines_aabb_tree(
        const BRep& model )
    {
        return create_aabb< 3 >(
            model.active_lines(), model.nb_active_lines() );
    }

    std::tuple< AABBTree3D, absl::FixedArray< uuid > >
        create_surfaces_aabb_tree( const BRep& model )
    {
        return create_aabb< 3 >(
            model.active_surfaces(), model.nb_active_surfaces() );
    }

    std::tuple< AABBTree3D, absl::FixedArray< uuid > > create_blocks_aabb_tree(
        const BRep& model )
    {
        return create_aabb< 3 >(
            model.active_blocks(), model.nb_active_blocks() );
    }

    std::tuple< AABBTree2D, absl::FixedArray< uuid > > create_lines_aabb_tree(
        const Section& model )
    {
        return create_aabb< 2 >(
            model.active_lines(), model.nb_active_lines() );
    }

    std::tuple< AABBTree2D, absl::FixedArray< uuid > >
        create_surfaces_aabb_tree( const Section& model )
    {
        return create_aabb< 2 >(
            model.active_surfaces(), model.nb_active_surfaces() );
    }

    ModelMeshesAABBTree3D create_line_meshes_aabb_trees( const BRep& model )
    {
        return create_aabbs< 3 >(
            model.active_lines(), model.nb_active_lines() );
    }

    ModelMeshesAABBTree3D create_surface_meshes_aabb_trees( const BRep& model )
    {
        return create_aabbs< 3 >(
            model.active_surfaces(), model.nb_active_surfaces() );
    }

    ModelMeshesAABBTree3D create_block_meshes_aabb_trees( const BRep& model )
    {
        return create_aabbs< 3 >(
            model.active_blocks(), model.nb_active_blocks() );
    }

    ModelMeshesAABBTree2D create_line_meshes_aabb_trees( const Section& model )
    {
        return create_aabbs< 2 >(
            model.active_lines(), model.nb_active_lines() );
    }

    ModelMeshesAABBTree2D create_surface_meshes_aabb_trees(
        const Section& model )
    {
        return create_aabbs< 2 >(
            model.active_surfaces(), model.nb_active_surfaces() );
    }
} // namespace geode
