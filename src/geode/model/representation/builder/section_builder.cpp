/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < typename Component >
    void register_new_component(
        geode::SectionBuilder& builder, const Component& component )
    {
        builder.register_component( component.component_id() );
        builder.register_mesh_component( component );
    }
} // namespace

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
        const auto mapping = copy_components( section );
        copy_component_relationships( mapping, section );
        copy_component_geometry( mapping, section );
    }

    SectionBuilder::ComponentMapping SectionBuilder::copy_components(
        const Section& section )
    {
        ComponentMapping mapping;
        mapping.corners =
            detail::copy_corner_components( section, section_, *this );
        mapping.lines =
            detail::copy_line_components( section, section_, *this );
        mapping.surfaces =
            detail::copy_surface_components( section, section_, *this );
        mapping.model_boundaries =
            detail::copy_model_boundary_components( section, *this );
        return mapping;
    }

    void SectionBuilder::copy_component_relationships(
        const ComponentMapping& mapping, const Section& section )
    {
        detail::copy_corner_line_relationships(
            section, section_, *this, mapping.corners, mapping.lines );
        detail::copy_corner_surface_relationships(
            section, section_, *this, mapping.corners, mapping.surfaces );
        detail::copy_line_surface_relationships(
            section, section_, *this, mapping.lines, mapping.surfaces );
        for( const auto& model_boundary : section.model_boundaries() )
        {
            const auto& new_model_boundary = section_.model_boundary(
                mapping.model_boundaries.at( model_boundary.id() ) );
            for( const auto& line : section.items( model_boundary ) )
            {
                const auto& new_line =
                    section_.line( mapping.lines.at( line.id() ) );
                add_line_in_model_boundary( new_line, new_model_boundary );
            }
        }
    }

    void SectionBuilder::copy_component_geometry(
        const ComponentMapping& mapping, const Section& section )
    {
        detail::copy_corner_geometry(
            section, section_, *this, mapping.corners );
        detail::copy_line_geometry( section, section_, *this, mapping.lines );
        detail::copy_surface_geometry(
            section, section_, *this, mapping.surfaces );
        create_unique_vertices( section.nb_unique_vertices() );
        detail::copy_vertex_identifier_components( section, *this,
            Corner2D::component_type_static(), mapping.corners );
        detail::copy_vertex_identifier_components(
            section, *this, Line2D::component_type_static(), mapping.lines );
        detail::copy_vertex_identifier_components( section, *this,
            Surface2D::component_type_static(), mapping.surfaces );
    }

    const uuid& SectionBuilder::add_corner()
    {
        const auto& id = create_corner();
        register_new_component( *this, section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_corner( const MeshType& type )
    {
        const auto& id = create_corner( type );
        register_new_component( *this, section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line()
    {
        const auto& id = create_line();
        register_new_component( *this, section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line( const MeshType& type )
    {
        const auto& id = create_line( type );
        register_new_component( *this, section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface()
    {
        const auto& id = create_surface();
        register_new_component( *this, section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface( const MeshType& type )
    {
        const auto& id = create_surface( type );
        register_new_component( *this, section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_model_boundary()
    {
        const auto& id = create_model_boundary();
        register_component( section_.model_boundary( id ).component_id() );
        return id;
    }

    void SectionBuilder::update_corner_mesh(
        const Corner2D& corner, std::unique_ptr< PointSet2D > mesh )
    {
        unregister_mesh_component( corner );
        set_corner_mesh( corner.id(), std::move( mesh ) );
        register_mesh_component( corner );
    }

    void SectionBuilder::update_line_mesh(
        const Line2D& line, std::unique_ptr< EdgedCurve2D > mesh )
    {
        unregister_mesh_component( line );
        set_line_mesh( line.id(), std::move( mesh ) );
        register_mesh_component( line );
    }

    void SectionBuilder::update_surface_mesh(
        const Surface2D& surface, std::unique_ptr< PolygonalSurface2D > mesh )
    {
        unregister_mesh_component( surface );
        set_surface_mesh( surface.id(), std::move( mesh ) );
        register_mesh_component( surface );
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

    void SectionBuilder::add_corner_line_boundary_relationship(
        const Corner2D& corner, const Line2D& line )
    {
        add_boundary_relation( corner.id(), line.id() );
    }

    void SectionBuilder::add_line_surface_boundary_relationship(
        const Line2D& line, const Surface2D& surface )
    {
        add_boundary_relation( line.id(), surface.id() );
    }

    void SectionBuilder::add_corner_surface_internal_relationship(
        const Corner2D& corner, const Surface2D& surface )
    {
        add_internal_relation( corner.id(), surface.id() );
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
