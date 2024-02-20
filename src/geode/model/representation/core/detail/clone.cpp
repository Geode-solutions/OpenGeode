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

#include <geode/model/representation/core/detail/clone.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/block_collection.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/corner_collection.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/line_collection.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/mixin/core/surface_collection.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_corner_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > corner_clone_mapping;
        for( const auto& corner : model.corners() )
        {
            corner_clone_mapping.map( corner.id(), corner.id() );
        }
        return corner_clone_mapping;
    }

    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_corner_collection_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > corner_collection_clone_mapping;
        for( const auto& corner_collection : model.corner_collections() )
        {
            corner_collection_clone_mapping.map(
                corner_collection.id(), corner_collection.id() );
        }
        return corner_collection_clone_mapping;
    }

    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_line_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > line_clone_mapping;
        for( const auto& line : model.lines() )
        {
            line_clone_mapping.map( line.id(), line.id() );
        }
        return line_clone_mapping;
    }

    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_line_collection_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > line_collection_clone_mapping;
        for( const auto& line_collection : model.line_collections() )
        {
            line_collection_clone_mapping.map(
                line_collection.id(), line_collection.id() );
        }
        return line_collection_clone_mapping;
    }

    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_surface_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > surface_clone_mapping;
        for( const auto& surface : model.surfaces() )
        {
            surface_clone_mapping.map( surface.id(), surface.id() );
        }
        return surface_clone_mapping;
    }

    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_surface_collection_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > surface_collection_clone_mapping;
        for( const auto& surface_collection : model.surface_collections() )
        {
            surface_collection_clone_mapping.map(
                surface_collection.id(), surface_collection.id() );
        }
        return surface_collection_clone_mapping;
    }

    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_model_boundary_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > model_boundary_clone_mapping;
        for( const auto& model_boundary : model.model_boundaries() )
        {
            model_boundary_clone_mapping.map(
                model_boundary.id(), model_boundary.id() );
        }
        return model_boundary_clone_mapping;
    }

    geode::BijectiveMapping< geode::uuid > clone_block_mapping(
        const geode::BRep& model )
    {
        geode::BijectiveMapping< geode::uuid > block_clone_mapping;
        for( const auto& block : model.blocks() )
        {
            block_clone_mapping.map( block.id(), block.id() );
        }
        return block_clone_mapping;
    }

    geode::BijectiveMapping< geode::uuid > clone_block_collection_mapping(
        const geode::BRep& model )
    {
        geode::BijectiveMapping< geode::uuid > block_collection_clone_mapping;
        for( const auto& block_collection : model.block_collections() )
        {
            block_collection_clone_mapping.map(
                block_collection.id(), block_collection.id() );
        }
        return block_collection_clone_mapping;
    }

    template < typename Model >
    geode::ModelCopyMapping model_clone_mappings( const Model& model )
    {
        geode::ModelCopyMapping clone_mapping;
        clone_mapping.emplace(
            geode::Corner< Model::dim >::component_type_static(),
            clone_corner_mapping( model ) );
        clone_mapping.emplace(
            geode::CornerCollection< Model::dim >::component_type_static(),
            clone_corner_collection_mapping( model ) );
        clone_mapping.emplace(
            geode::Line< Model::dim >::component_type_static(),
            clone_line_mapping( model ) );
        clone_mapping.emplace(
            geode::LineCollection< Model::dim >::component_type_static(),
            clone_line_collection_mapping( model ) );
        clone_mapping.emplace(
            geode::Surface< Model::dim >::component_type_static(),
            clone_surface_mapping( model ) );
        clone_mapping.emplace(
            geode::SurfaceCollection< Model::dim >::component_type_static(),
            clone_surface_collection_mapping( model ) );
        clone_mapping.emplace(
            geode::ModelBoundary< Model::dim >::component_type_static(),
            clone_model_boundary_mapping( model ) );
        return clone_mapping;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        ModelCopyMapping section_clone_mapping( const Section& section )
        {
            return model_clone_mappings( section );
        }

        ModelCopyMapping brep_clone_mapping( const BRep& brep )
        {
            auto clone_mappings = model_clone_mappings( brep );
            clone_mappings.emplace( geode::Block3D::component_type_static(),
                clone_block_mapping( brep ) );
            clone_mappings.emplace(
                geode::BlockCollection3D::component_type_static(),
                clone_block_collection_mapping( brep ) );
            return clone_mappings;
        }
    } // namespace detail
} // namespace geode
