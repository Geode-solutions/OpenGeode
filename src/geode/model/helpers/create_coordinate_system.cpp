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

#include <geode/model/helpers/create_coordinate_system.hpp>

#include <string_view>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/create_coordinate_system.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Builder >
    void set_generic_active_coordinate_system(
        Builder& builder, std::string_view coordinate_system_name )
    {
        builder.main_coordinate_reference_system_manager_builder()
            .set_active_coordinate_reference_system( coordinate_system_name );
    }

    template < typename Model >
    void set_generic_model_active_coordinate_system( const Model& model,
        typename Model::Builder& builder,
        std::string_view coordinate_system_name )
    {
        for( const auto& corner : model.corners() )
        {
            auto mesh_builder = builder.corner_mesh_builder( corner.id() );
            set_generic_active_coordinate_system(
                *mesh_builder, coordinate_system_name );
        }
        for( const auto& line : model.lines() )
        {
            auto mesh_builder = builder.line_mesh_builder( line.id() );
            set_generic_active_coordinate_system(
                *mesh_builder, coordinate_system_name );
        }
        for( const auto& surface : model.surfaces() )
        {
            auto mesh_builder = builder.surface_mesh_builder( surface.id() );
            set_generic_active_coordinate_system(
                *mesh_builder, coordinate_system_name );
        }
    }

    template < typename Model >
    void create_generic_model_coordinate_system( const Model& model,
        typename Model::Builder& builder,
        std::string_view new_coordinate_system_name,
        const geode::CoordinateSystem2D& input,
        const geode::CoordinateSystem2D& output )
    {
        for( const auto& corner : model.corners() )
        {
            const auto& mesh = corner.mesh();
            auto mesh_builder = builder.corner_mesh_builder( corner.id() );
            create_point_set_coordinate_system( mesh, *mesh_builder,
                new_coordinate_system_name, input, output );
        }
        for( const auto& line : model.lines() )
        {
            const auto& mesh = line.mesh();
            auto mesh_builder = builder.line_mesh_builder( line.id() );
            create_edged_curve_coordinate_system( mesh, *mesh_builder,
                new_coordinate_system_name, input, output );
        }
        for( const auto& surface : model.surfaces() )
        {
            const auto& mesh = surface.mesh();
            auto mesh_builder = builder.surface_mesh_builder( surface.id() );
            create_surface_mesh_coordinate_system( mesh, *mesh_builder,
                new_coordinate_system_name, input, output );
        }
    }
} // namespace

namespace geode
{
    void create_brep_coordinate_system( const BRep& model,
        BRepBuilder& builder,
        std::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output )
    {
        create_generic_model_coordinate_system(
            model, builder, new_coordinate_system_name, input, output );
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            auto mesh_builder = builder.block_mesh_builder( block.id() );
            create_solid_mesh_coordinate_system( mesh, *mesh_builder,
                new_coordinate_system_name, input, output );
        }
    }

    void create_section_coordinate_system( const Section& model,
        SectionBuilder& builder,
        std::string_view new_coordinate_system_name,
        const CoordinateSystem2D& input,
        const CoordinateSystem2D& output )
    {
        create_generic_model_coordinate_system(
            model, builder, new_coordinate_system_name, input, output );
    }

    void set_brep_active_coordinate_system( const BRep& model,
        BRepBuilder& builder,
        std::string_view coordinate_system_name )
    {
        set_generic_model_active_coordinate_system(
            model, builder, coordinate_system_name );
        for( const auto& block : model.blocks() )
        {
            auto mesh_builder = builder.block_mesh_builder( block.id() );
            set_generic_active_coordinate_system(
                *mesh_builder, coordinate_system_name );
        }
    }

    void set_section_active_coordinate_system( const Section& model,
        SectionBuilder& builder,
        std::string_view coordinate_system_name )
    {
        set_generic_model_active_coordinate_system(
            model, builder, coordinate_system_name );
    }
} // namespace geode