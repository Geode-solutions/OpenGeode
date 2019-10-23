/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/model/representation/builder/section_builder.h>

#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/polygonal_surface.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    SectionBuilder::SectionBuilder( Section& section )
        : TopologyBuilder( section ),
          AddComponentsBuilders< 2, Corners, Lines, Surfaces, ModelBoundaries >(
              section ),
          section_( section )
    {
    }

    void SectionBuilder::copy( const Section& section )
    {
        auto mapping = copy_components( section );
        copy_component_relationships( mapping, section );
        copy_component_geometry( mapping, section );
    }

    SectionBuilder::ComponentMapping SectionBuilder::copy_components(
        const Section& section )
    {
        ComponentMapping mapping;
        for( const auto& corner : section.corners() )
        {
            mapping.corners.emplace(
                corner.id(), add_corner( corner.mesh().type_name() ) );
        }
        for( const auto& line : section.lines() )
        {
            mapping.lines.emplace(
                line.id(), add_line( line.mesh().type_name() ) );
        }
        for( const auto& surface : section.surfaces() )
        {
            mapping.surfaces.emplace(
                surface.id(), add_surface( surface.mesh().type_name() ) );
        }
        for( const auto& model_boundary : section.model_boundaries() )
        {
            mapping.model_boundaries.emplace(
                model_boundary.id(), add_model_boundary() );
        }
        return mapping;
    }

    void SectionBuilder::copy_component_relationships(
        const ComponentMapping& mapping, const Section& section )
    {
        for( const auto& line : section.lines() )
        {
            const auto& new_line = section_.line( mapping.lines.at( line.id() ) );
            for( const auto& corner : section.boundaries( line ) )
            {
                const auto& new_corner =
                    section_.corner( mapping.corners.at( corner.id() ) );
                add_corner_line_relationship( new_corner, new_line );
            }
        }
        for( const auto& surface : section.surfaces() )
        {
            const auto& new_surface =
                section_.surface( mapping.surfaces.at( surface.id() ) );
            for( const auto& line : section.boundaries( surface ) )
            {
                const auto& new_line =
                    section_.line( mapping.lines.at( line.id() ) );
                add_line_surface_relationship( new_line, new_surface );
            }
            for( const auto& line : section.internals( surface ) )
            {
                const auto& new_line =
                    section_.line( mapping.lines.at( line.id() ) );
                add_line_surface_internal_relationship( new_line, new_surface );
            }
        }
        for( const auto& model_boundary : section.model_boundaries() )
        {
            const auto& new_model_boundary = section_.model_boundary(
                mapping.model_boundaries.at( model_boundary.id() ) );
            for( const auto& line : section.items( model_boundary ) )
            {
                const auto& new_line =
                    section_.line( mapping.lines.at( line.id() ) );
                add_line_in_model_boundary(
                    new_line, new_model_boundary );
            }
        }
    }

    void SectionBuilder::copy_component_geometry(
        const ComponentMapping& mapping, const Section& section )
    {
        for( const auto& corner : section.corners() )
        {
            set_corner_mesh(
                mapping.corners.at( corner.id() ), corner.mesh().clone() );
        }
        for( const auto& line : section.lines() )
        {
            set_line_mesh(
                mapping.lines.at( line.id() ), line.mesh().clone() );
        }
        for( const auto& surface : section.surfaces() )
        {
            set_surface_mesh(
                mapping.surfaces.at( surface.id() ), surface.mesh().clone() );
        }
    }

    const uuid& SectionBuilder::add_corner()
    {
        const auto& id = create_corner();
        register_component( id );
        register_mesh_component( section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_corner( const MeshType& type )
    {
        const auto& id = create_corner( type );
        register_component( id );
        register_mesh_component( section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line()
    {
        const auto& id = create_line();
        register_component( id );
        register_mesh_component( section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line( const MeshType& type )
    {
        const auto& id = create_line( type );
        register_component( id );
        register_mesh_component( section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface()
    {
        const auto& id = create_surface();
        register_component( id );
        register_mesh_component( section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface( const MeshType& type )
    {
        const auto& id = create_surface( type );
        register_component( id );
        register_mesh_component( section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_model_boundary()
    {
        const auto& id = create_model_boundary();
        register_component( id );
        return id;
    }

    void SectionBuilder::remove_corner( const Corner2D& corner )
    {
        unregister_component( corner.id() );
        unregister_mesh_component( corner );
        delete_corner( corner );
    }

    void SectionBuilder::remove_line( const Line2D& line )
    {
        unregister_component( line.id() );
        unregister_mesh_component( line );
        delete_line( line );
    }

    void SectionBuilder::remove_surface( const Surface2D& surface )
    {
        unregister_component( surface.id() );
        unregister_mesh_component( surface );
        delete_surface( surface );
    }

    void SectionBuilder::remove_model_boundary(
        const ModelBoundary2D& boundary )
    {
        unregister_component( boundary.id() );
        delete_model_boundary( boundary );
    }

    void SectionBuilder::add_corner_line_relationship(
        const Corner2D& corner, const Line2D& line )
    {
        add_boundary_relation( corner.id(), line.id() );
    }

    void SectionBuilder::add_line_surface_relationship(
        const Line2D& line, const Surface2D& surface )
    {
        add_boundary_relation( line.id(), surface.id() );
    }

    void SectionBuilder::add_line_surface_internal_relationship(
        const Line2D& line, const Surface2D& surface )
    {
        add_internal_relation( line.id(), surface.id() );
    }

    void SectionBuilder::add_line_in_model_boundary(
        const Line2D& line, const ModelBoundary2D& boundary )
    {
        add_item_in_collection( line.id(), boundary.id() );
    }
} // namespace geode
