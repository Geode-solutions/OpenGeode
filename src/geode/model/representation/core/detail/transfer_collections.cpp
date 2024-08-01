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

#include <geode/model/representation/core/detail/transfer_collections.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model >
    void transfer_corner_collections( const Model& old_model,
        const Model& new_model,
        typename Model::Builder& new_model_builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Corner< Model::dim >::component_type_static() ) )
        {
            return;
        }
        const auto& corners_mapping = component_mapping.at(
            geode::Corner< Model::dim >::component_type_static() );
        for( const auto& collection_in : old_model.corner_collections() )
        {
            bool exists_in_out{ false };
            for( const auto& corner_in_collection :
                old_model.corner_collection_items( collection_in ) )
            {
                if( corners_mapping.has_mapping_input(
                        corner_in_collection.id() ) )
                {
                    exists_in_out = true;
                    break;
                }
            }
            if( !exists_in_out )
            {
                continue;
            }
            new_model_builder.add_corner_collection( collection_in.id() );
            new_model_builder.set_corner_collection_name(
                collection_in.id(), collection_in.name() );
            const auto& new_collection =
                new_model.corner_collection( collection_in.id() );
            for( const auto& corner_in_collection :
                old_model.corner_collection_items( collection_in ) )
            {
                if( !corners_mapping.has_mapping_input(
                        corner_in_collection.id() ) )
                {
                    continue;
                }
                for( const auto& corner_out_id :
                    corners_mapping.in2out( corner_in_collection.id() ) )
                {
                    new_model_builder.add_corner_in_corner_collection(
                        new_model.corner( corner_out_id ), new_collection );
                }
            }
        }
    }

    template < typename Model >
    void transfer_line_collections( const Model& old_model,
        const Model& new_model,
        typename Model::Builder& new_model_builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Line< Model::dim >::component_type_static() ) )
        {
            return;
        }
        const auto& lines_mapping = component_mapping.at(
            geode::Line< Model::dim >::component_type_static() );
        for( const auto& collection_in : old_model.line_collections() )
        {
            bool exists_in_out{ false };
            for( const auto& line_in_collection :
                old_model.line_collection_items( collection_in ) )
            {
                if( lines_mapping.has_mapping_input( line_in_collection.id() ) )
                {
                    exists_in_out = true;
                    break;
                }
            }
            if( !exists_in_out )
            {
                continue;
            }
            new_model_builder.add_line_collection( collection_in.id() );
            new_model_builder.set_line_collection_name(
                collection_in.id(), collection_in.name() );
            const auto& new_collection =
                new_model.line_collection( collection_in.id() );
            for( const auto& line_in_collection :
                old_model.line_collection_items( collection_in ) )
            {
                if( !lines_mapping.has_mapping_input(
                        line_in_collection.id() ) )
                {
                    continue;
                }
                for( const auto& line_out_id :
                    lines_mapping.in2out( line_in_collection.id() ) )
                {
                    new_model_builder.add_line_in_line_collection(
                        new_model.line( line_out_id ), new_collection );
                }
            }
        }
    }

    template < typename Model >
    void transfer_surface_collections( const Model& old_model,
        const Model& new_model,
        typename Model::Builder& new_model_builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Surface< Model::dim >::component_type_static() ) )
        {
            return;
        }
        const auto& surfaces_mapping = component_mapping.at(
            geode::Surface< Model::dim >::component_type_static() );
        for( const auto& collection_in : old_model.surface_collections() )
        {
            bool exists_in_out{ false };
            for( const auto& surface_in_collection :
                old_model.surface_collection_items( collection_in ) )
            {
                if( surfaces_mapping.has_mapping_input(
                        surface_in_collection.id() ) )
                {
                    exists_in_out = true;
                    break;
                }
            }
            if( !exists_in_out )
            {
                continue;
            }
            new_model_builder.add_surface_collection( collection_in.id() );
            new_model_builder.set_surface_collection_name(
                collection_in.id(), collection_in.name() );
            const auto& new_collection =
                new_model.surface_collection( collection_in.id() );
            for( const auto& surface_in_collection :
                old_model.surface_collection_items( collection_in ) )
            {
                if( !surfaces_mapping.has_mapping_input(
                        surface_in_collection.id() ) )
                {
                    continue;
                }
                for( const auto& surface_out_id :
                    surfaces_mapping.in2out( surface_in_collection.id() ) )
                {
                    new_model_builder.add_surface_in_surface_collection(
                        new_model.surface( surface_out_id ), new_collection );
                }
            }
        }
    }

    void transfer_block_collections( const geode::BRep& old_model,
        const geode::BRep& new_model,
        typename geode::BRepBuilder& new_model_builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Block3D::component_type_static() ) )
        {
            return;
        }
        const auto& blocks_mapping =
            component_mapping.at( geode::Block3D::component_type_static() );
        for( const auto& collection_in : old_model.block_collections() )
        {
            bool exists_in_out{ false };
            for( const auto& block_in_collection :
                old_model.block_collection_items( collection_in ) )
            {
                if( blocks_mapping.has_mapping_input(
                        block_in_collection.id() ) )
                {
                    exists_in_out = true;
                    break;
                }
            }
            if( !exists_in_out )
            {
                continue;
            }
            new_model_builder.add_block_collection( collection_in.id() );
            new_model_builder.set_block_collection_name(
                collection_in.id(), collection_in.name() );
            const auto& new_collection =
                new_model.block_collection( collection_in.id() );
            for( const auto& block_in_collection :
                old_model.block_collection_items( collection_in ) )
            {
                if( !blocks_mapping.has_mapping_input(
                        block_in_collection.id() ) )
                {
                    continue;
                }
                for( const auto& block_out_id :
                    blocks_mapping.in2out( block_in_collection.id() ) )
                {
                    new_model_builder.add_block_in_block_collection(
                        new_model.block( block_out_id ), new_collection );
                }
            }
        }
    }

    void transfer_brep_model_boundaries( const geode::BRep& old_model,
        const geode::BRep& new_model,
        typename geode::BRepBuilder& new_model_builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Surface3D::component_type_static() ) )
        {
            return;
        }
        const auto& surfaces_mapping =
            component_mapping.at( geode::Surface3D::component_type_static() );
        for( const auto& collection_in : old_model.model_boundaries() )
        {
            bool exists_in_out{ false };
            for( const auto& surface_in_collection :
                old_model.model_boundary_items( collection_in ) )
            {
                if( surfaces_mapping.has_mapping_input(
                        surface_in_collection.id() ) )
                {
                    exists_in_out = true;
                    break;
                }
            }
            if( !exists_in_out )
            {
                continue;
            }
            new_model_builder.add_model_boundary( collection_in.id() );
            new_model_builder.set_model_boundary_name(
                collection_in.id(), collection_in.name() );
            const auto& new_collection =
                new_model.model_boundary( collection_in.id() );
            for( const auto& surface_in_collection :
                old_model.model_boundary_items( collection_in ) )
            {
                if( !surfaces_mapping.has_mapping_input(
                        surface_in_collection.id() ) )
                {
                    continue;
                }
                for( const auto& surface_out_id :
                    surfaces_mapping.in2out( surface_in_collection.id() ) )
                {
                    new_model_builder.add_surface_in_model_boundary(
                        new_model.surface( surface_out_id ), new_collection );
                }
            }
        }
    }

    void transfer_section_model_boundaries( const geode::Section& old_model,
        const geode::Section& new_model,
        typename geode::SectionBuilder& new_model_builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Line2D::component_type_static() ) )
        {
            return;
        }
        const auto& lines_mapping =
            component_mapping.at( geode::Line2D::component_type_static() );
        for( const auto& collection_in : old_model.model_boundaries() )
        {
            bool exists_in_out{ false };
            for( const auto& surface_in_collection :
                old_model.model_boundary_items( collection_in ) )
            {
                if( lines_mapping.has_mapping_input(
                        surface_in_collection.id() ) )
                {
                    exists_in_out = true;
                    break;
                }
            }
            if( !exists_in_out )
            {
                continue;
            }
            new_model_builder.add_model_boundary( collection_in.id() );
            new_model_builder.set_model_boundary_name(
                collection_in.id(), collection_in.name() );
            const auto& new_collection =
                new_model.model_boundary( collection_in.id() );
            for( const auto& line_in_collection :
                old_model.model_boundary_items( collection_in ) )
            {
                if( !lines_mapping.has_mapping_input(
                        line_in_collection.id() ) )
                {
                    continue;
                }
                for( const auto& line_out_id :
                    lines_mapping.in2out( line_in_collection.id() ) )
                {
                    new_model_builder.add_line_in_model_boundary(
                        new_model.line( line_out_id ), new_collection );
                }
            }
        }
    }
} // namespace

namespace geode
{
    namespace detail
    {
        void opengeode_model_api transfer_brep_collections(
            const BRep& old_brep,
            const BRep& new_brep,
            BRepBuilder& new_brep_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_corner_collections(
                old_brep, new_brep, new_brep_builder, component_mapping );
            transfer_line_collections(
                old_brep, new_brep, new_brep_builder, component_mapping );
            transfer_surface_collections(
                old_brep, new_brep, new_brep_builder, component_mapping );
            transfer_block_collections(
                old_brep, new_brep, new_brep_builder, component_mapping );
            transfer_brep_model_boundaries(
                old_brep, new_brep, new_brep_builder, component_mapping );
        }

        void opengeode_model_api transfer_section_collections(
            const Section& old_section,
            const Section& new_section,
            SectionBuilder& new_section_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_corner_collections( old_section, new_section,
                new_section_builder, component_mapping );
            transfer_line_collections( old_section, new_section,
                new_section_builder, component_mapping );
            transfer_surface_collections( old_section, new_section,
                new_section_builder, component_mapping );
            transfer_section_model_boundaries( old_section, new_section,
                new_section_builder, component_mapping );
        }
    } // namespace detail
} // namespace geode
