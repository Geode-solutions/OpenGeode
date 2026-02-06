/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/model/representation/builder/section_builder.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/mesh_id.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/detail/copy.hpp>
#include <geode/model/representation/builder/detail/register.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    SectionBuilder::SectionBuilder( Section& section )
        : TopologyBuilder( section ),
          CornersBuilder2D( section ),
          LinesBuilder2D( section ),
          SurfacesBuilder2D( section ),
          ModelBoundariesBuilder2D( section ),
          CornerCollectionsBuilder2D( section ),
          LineCollectionsBuilder2D( section ),
          SurfaceCollectionsBuilder2D( section ),
          IdentifierBuilder( section ),
          section_( section )
    {
    }

    ModelCopyMapping SectionBuilder::copy( const Section& section )
    {
        auto nb_components =
            section_.nb_corners() + section_.nb_lines() + section_.nb_surfaces()
            + section_.nb_model_boundaries() + section_.nb_unique_vertices();
        OPENGEODE_EXCEPTION( nb_components == 0,
            "[SectionBuild::copy] Section should be empty before copy. To add "
            "Section components in a Section which is not empty, use "
            "ModelConcatener." );
        set_name( section.name() );
        auto mapping = copy_components( section );
        copy_relationships( mapping, section );
        copy_component_geometry( mapping, section );
        return mapping;
    }

    ModelCopyMapping SectionBuilder::copy_components( const Section& section )
    {
        ModelCopyMapping mappings;
        detail::copy_corner_components(
            section, *this, mappings[Corner2D::component_type_static()] );
        detail::copy_line_components(
            section, *this, mappings[Line2D::component_type_static()] );
        detail::copy_surface_components(
            section, *this, mappings[Surface2D::component_type_static()] );
        detail::copy_model_boundary_components( section, *this,
            mappings[ModelBoundary2D::component_type_static()] );
        detail::copy_corner_collection_components( section, *this,
            mappings[CornerCollection2D::component_type_static()] );
        detail::copy_line_collection_components( section, *this,
            mappings[LineCollection2D::component_type_static()] );
        detail::copy_surface_collection_components( section, *this,
            mappings[SurfaceCollection2D::component_type_static()] );
        return mappings;
    }

    void SectionBuilder::copy_components(
        ModelCopyMapping& mapping, const Section& section )
    {
        detail::copy_corner_components(
            section, *this, mapping[Corner2D::component_type_static()] );
        detail::copy_line_components(
            section, *this, mapping[Line2D::component_type_static()] );
        detail::copy_surface_components(
            section, *this, mapping[Surface2D::component_type_static()] );
        detail::copy_model_boundary_components(
            section, *this, mapping[ModelBoundary2D::component_type_static()] );
        detail::copy_corner_collection_components( section, *this,
            mapping[CornerCollection2D::component_type_static()] );
        detail::copy_line_collection_components( section, *this,
            mapping[LineCollection2D::component_type_static()] );
        detail::copy_surface_collection_components( section, *this,
            mapping[SurfaceCollection2D::component_type_static()] );
    }

    void SectionBuilder::copy_component_geometry(
        const ModelCopyMapping& mappings, const Section& section )
    {
        detail::copy_corner_geometry( section, section_, *this,
            mappings.at( Corner2D::component_type_static() ) );
        detail::copy_line_geometry( section, section_, *this,
            mappings.at( Line2D::component_type_static() ) );
        detail::copy_surface_geometry( section, section_, *this,
            mappings.at( Surface2D::component_type_static() ) );
        const auto first_new_unique_vertex_id =
            create_unique_vertices( section.nb_unique_vertices() );
        detail::copy_vertex_identifier_components(
            section, *this, first_new_unique_vertex_id, mappings );
    }

    const uuid& SectionBuilder::add_corner()
    {
        const auto& id = create_corner();
        detail::add_mesh_component( *this, section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_corner( const MeshImpl& impl )
    {
        const auto& id = create_corner( impl );
        detail::add_mesh_component( *this, section_.corner( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line()
    {
        const auto& id = create_line();
        detail::add_mesh_component( *this, section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line( const MeshImpl& impl )
    {
        const auto& id = create_line( impl );
        detail::add_mesh_component( *this, section_.line( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface()
    {
        const auto& id = create_surface();
        detail::add_mesh_component( *this, section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface( const MeshImpl& impl )
    {
        const auto& id = create_surface( impl );
        detail::add_mesh_component( *this, section_.surface( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_model_boundary()
    {
        const auto& id = create_model_boundary();
        detail::add_collection_component(
            *this, section_.model_boundary( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_corner_collection()
    {
        const auto& id = create_corner_collection();
        detail::add_collection_component(
            *this, section_.corner_collection( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_line_collection()
    {
        const auto& id = create_line_collection();
        detail::add_collection_component(
            *this, section_.line_collection( id ) );
        return id;
    }

    const uuid& SectionBuilder::add_surface_collection()
    {
        const auto& id = create_surface_collection();
        detail::add_collection_component(
            *this, section_.surface_collection( id ) );
        return id;
    }

    void SectionBuilder::add_corner( const uuid& corner_id )
    {
        create_corner( corner_id );
        detail::add_mesh_component( *this, section_.corner( corner_id ) );
    }

    void SectionBuilder::add_corner(
        const uuid& corner_id, const MeshImpl& impl )
    {
        create_corner( corner_id, impl );
        detail::add_mesh_component( *this, section_.corner( corner_id ) );
    }

    void SectionBuilder::add_line( const uuid& line_id )
    {
        create_line( line_id );
        detail::add_mesh_component( *this, section_.line( line_id ) );
    }

    void SectionBuilder::add_line( const uuid& line_id, const MeshImpl& impl )
    {
        create_line( line_id, impl );
        detail::add_mesh_component( *this, section_.line( line_id ) );
    }

    void SectionBuilder::add_surface( const uuid& surface_id )
    {
        create_surface( surface_id );
        detail::add_mesh_component( *this, section_.surface( surface_id ) );
    }

    void SectionBuilder::add_surface(
        const uuid& surface_id, const MeshImpl& impl )
    {
        create_surface( surface_id, impl );
        detail::add_mesh_component( *this, section_.surface( surface_id ) );
    }

    void SectionBuilder::add_model_boundary( const uuid& model_boundary_id )
    {
        create_model_boundary( model_boundary_id );
        detail::add_collection_component(
            *this, section_.model_boundary( model_boundary_id ) );
    }

    void SectionBuilder::add_corner_collection(
        const uuid& corner_collection_id )
    {
        create_corner_collection( corner_collection_id );
        detail::add_collection_component(
            *this, section_.corner_collection( corner_collection_id ) );
    }

    void SectionBuilder::add_line_collection( const uuid& line_collection_id )
    {
        create_line_collection( line_collection_id );
        detail::add_collection_component(
            *this, section_.line_collection( line_collection_id ) );
    }

    void SectionBuilder::add_surface_collection(
        const uuid& surface_collection_id )
    {
        create_surface_collection( surface_collection_id );
        detail::add_collection_component(
            *this, section_.surface_collection( surface_collection_id ) );
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
        detail::remove_mesh_component( *this, corner );
        delete_corner( corner );
    }

    void SectionBuilder::remove_line( const Line2D& line )
    {
        detail::remove_mesh_component( *this, line );
        delete_line( line );
    }

    void SectionBuilder::remove_surface( const Surface2D& surface )
    {
        detail::remove_mesh_component( *this, surface );
        delete_surface( surface );
    }

    void SectionBuilder::remove_model_boundary(
        const ModelBoundary2D& boundary )
    {
        detail::remove_collection_component( *this, boundary );
        delete_model_boundary( boundary );
    }

    void SectionBuilder::remove_corner_collection(
        const CornerCollection2D& collection )
    {
        detail::remove_collection_component( *this, collection );
        delete_corner_collection( collection );
    }

    void SectionBuilder::remove_line_collection(
        const LineCollection2D& collection )
    {
        detail::remove_collection_component( *this, collection );
        delete_line_collection( collection );
    }

    void SectionBuilder::remove_surface_collection(
        const SurfaceCollection2D& collection )
    {
        detail::remove_collection_component( *this, collection );
        delete_surface_collection( collection );
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

    void SectionBuilder::add_corner_in_corner_collection(
        const Corner2D& corner, const CornerCollection2D& collection )
    {
        add_item_in_collection(
            corner.component_id(), collection.component_id() );
    }

    void SectionBuilder::add_line_in_line_collection(
        const Line2D& line, const LineCollection2D& collection )
    {
        add_item_in_collection(
            line.component_id(), collection.component_id() );
    }

    void SectionBuilder::add_surface_in_surface_collection(
        const Surface2D& surface, const SurfaceCollection2D& collection )
    {
        add_item_in_collection(
            surface.component_id(), collection.component_id() );
    }

    void SectionBuilder::set_point(
        index_t unique_vertex, const Point2D& point )
    {
        for( const auto& cmv :
            section_.component_mesh_vertices( unique_vertex ) )
        {
            if( cmv.component_id.type() == Surface2D::component_type_static() )
            {
                surface_mesh_builder( cmv.component_id.id() )
                    ->set_point( cmv.vertex, point );
            }
            else if( cmv.component_id.type()
                     == Line2D::component_type_static() )
            {
                line_mesh_builder( cmv.component_id.id() )
                    ->set_point( cmv.vertex, point );
            }
            else if( cmv.component_id.type()
                     == Corner2D::component_type_static() )
            {
                corner_mesh_builder( cmv.component_id.id() )
                    ->set_point( cmv.vertex, point );
            }
        }
    }
} // namespace geode
