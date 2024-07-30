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

#include <geode/model/representation/core/detail/clone.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model >
    void transfer_model_corner_metadata( const Model& old_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Corner3D::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping.at( geode::Corner3D::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend(
                    &out_name, old_model.corner( in_uuid ).name() );
            }
            builder.set_corner_name( out2in_mapping.first, out_name );
        }
    }

    template < typename Model >
    void transfer_model_line_metadata( const Model& old_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Line3D::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping.at( geode::Line3D::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend( &out_name, old_model.line( in_uuid ).name() );
            }
            builder.set_line_name( out2in_mapping.first, out_name );
        }
    }

    template < typename Model >
    void transfer_model_surface_metadata( const Model& old_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Surface3D::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping.at( geode::Surface3D::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend(
                    &out_name, old_model.surface( in_uuid ).name() );
            }
            builder.set_surface_name( out2in_mapping.first, out_name );
        }
    }

    void transfer_model_block_metadata( const geode::BRep& old_model,
        typename geode::BRepBuilder& builder,
        const geode::ModelGenericMapping& component_mapping )
    {
        if( !component_mapping.has_mapping_type(
                geode::Block3D::component_type_static() ) )
        {
            return;
        }
        for( const auto& out2in_mapping :
            component_mapping.at( geode::Block3D::component_type_static() )
                .out2in_map() )
        {
            std::string out_name{ "" };
            bool first{ true };
            for( const auto& in_uuid : out2in_mapping.second )
            {
                if( !first )
                {
                    absl::StrAppend( &out_name, "+" );
                }
                first = false;
                absl::StrAppend( &out_name, old_model.block( in_uuid ).name() );
            }
            builder.set_block_name( out2in_mapping.first, out_name );
        }
    }
} // namespace

namespace geode
{
    namespace detail
    {
        void opengeode_model_api transfer_brep_metadata( const BRep& old_brep,
            BRepBuilder& new_brep_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_model_corner_metadata(
                old_brep, new_brep_builder, component_mapping );
            transfer_model_line_metadata(
                old_brep, new_brep_builder, component_mapping );
            transfer_model_surface_metadata(
                old_brep, new_brep_builder, component_mapping );
            transfer_model_block_metadata(
                old_brep, new_brep_builder, component_mapping );
        }

        void opengeode_model_api transfer_section_metadata(
            const Section& old_section,
            SectionBuilder& new_section_builder,
            const ModelGenericMapping& component_mapping )
        {
            transfer_model_corner_metadata(
                old_section, new_section_builder, component_mapping );
            transfer_model_line_metadata(
                old_section, new_section_builder, component_mapping );
            transfer_model_surface_metadata(
                old_section, new_section_builder, component_mapping );
        }
    } // namespace detail
} // namespace geode
