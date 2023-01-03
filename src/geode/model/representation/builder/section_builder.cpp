/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_id.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    SectionBuilder::SectionBuilder( Section& section )
        : TopologyBuilder( section ),
          AddComponentsBuilders< 2, Corners, Lines, Surfaces, ModelBoundaries >(
              section ),
          IdentifierBuilder( section ),
          section_( section )
    {
    }

    ModelCopyMapping SectionBuilder::copy( const Section& section )
    {
        set_name( section.name() );
        const auto mapping = copy_components( section );
        copy_relationships( mapping, section );
        copy_component_geometry( mapping, section );
        return mapping;
    }

    ModelCopyMapping SectionBuilder::copy_components( const Section& section )
    {
        ModelCopyMapping mappings;
        mappings.emplace( Corner2D::component_type_static(),
            detail::copy_corner_components( section, section_, *this ) );
        mappings.emplace( Line2D::component_type_static(),
            detail::copy_line_components( section, section_, *this ) );
        mappings.emplace( Surface2D::component_type_static(),
            detail::copy_surface_components( section, section_, *this ) );
        mappings.emplace( ModelBoundary2D::component_type_static(),
            detail::copy_model_boundary_components( section, *this ) );
        return mappings;
    }

    void SectionBuilder::copy_component_geometry(
        const ModelCopyMapping& mappings, const Section& section )
    {
        detail::copy_corner_geometry< 2 >( section, section_, *this,
            mappings.at( Corner2D::component_type_static() ) );
        detail::copy_line_geometry< 2 >( section, section_, *this,
            mappings.at( Line2D::component_type_static() ) );
        detail::copy_surface_geometry< 2 >( section, section_, *this,
            mappings.at( Surface2D::component_type_static() ) );
        create_unique_vertices( section.nb_unique_vertices() );
        detail::copy_vertex_identifier_components( section, *this, mappings );
    }

    const uuid& SectionBuilder::add_corner()
    {
        const auto& id = create_corner();
        register_mesh_component( section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_corner( const MeshImpl& impl )
    {
        const auto& id = create_corner( impl );
        register_mesh_component( section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line()
    {
        const auto& id = create_line();
        register_mesh_component( section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line( const MeshImpl& impl )
    {
        const auto& id = create_line( impl );
        register_mesh_component( section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface()
    {
        const auto& id = create_surface();
        register_mesh_component( section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface( const MeshImpl& impl )
    {
        const auto& id = create_surface( impl );
        register_mesh_component( section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_model_boundary()
    {
        const auto& id = create_model_boundary();
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
        const Surface2D& surface, std::unique_ptr< SurfaceMesh2D > mesh )
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
        add_boundary_relation( corner.component_id(), line.component_id() );
    }

    void SectionBuilder::add_line_surface_boundary_relationship(
        const Line2D& line, const Surface2D& surface )
    {
        add_boundary_relation( line.component_id(), surface.component_id() );
    }

    void SectionBuilder::add_corner_surface_internal_relationship(
        const Corner2D& corner, const Surface2D& surface )
    {
        add_internal_relation( corner.component_id(), surface.component_id() );
    }

    void SectionBuilder::add_line_surface_internal_relationship(
        const Line2D& line, const Surface2D& surface )
    {
        add_internal_relation( line.component_id(), surface.component_id() );
    }

    void SectionBuilder::add_line_in_model_boundary(
        const Line2D& line, const ModelBoundary2D& boundary )
    {
        add_item_in_collection( line.component_id(), boundary.component_id() );
    }
} // namespace geode
