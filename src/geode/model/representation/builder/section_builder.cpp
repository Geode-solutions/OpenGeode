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

    const uuid& SectionBuilder::add_corner_collection()
    {
        const auto& id = create_corner_collection();
        return id;
    }

    const uuid& SectionBuilder::add_line_collection()
    {
        const auto& id = create_line_collection();
        return id;
    }

    const uuid& SectionBuilder::add_surface_collection()
    {
        const auto& id = create_surface_collection();
        return id;
    }

    void SectionBuilder::add_corner( uuid corner_id )
    {
        create_corner( std::move( corner_id ) );
    }

    void SectionBuilder::add_corner( uuid corner_id, const MeshImpl& impl )
    {
        create_corner( std::move( corner_id ), impl );
    }

    void SectionBuilder::add_line( uuid line_id )
    {
        create_line( std::move( line_id ) );
    }

    void SectionBuilder::add_line( uuid line_id, const MeshImpl& impl )
    {
        create_line( std::move( line_id ), impl );
    }

    void SectionBuilder::add_surface( uuid surface_id )
    {
        create_surface( std::move( surface_id ) );
    }

    void SectionBuilder::add_surface( uuid surface_id, const MeshImpl& impl )
    {
        create_surface( std::move( surface_id ), impl );
    }

    void SectionBuilder::add_model_boundary( uuid model_boundary_id )
    {
        create_model_boundary( std::move( model_boundary_id ) );
    }

    void SectionBuilder::add_corner_collection( uuid corner_collection_id )
    {
        create_corner_collection( std::move( corner_collection_id ) );
    }

    void SectionBuilder::add_line_collection( uuid line_collection_id )
    {
        create_line_collection( std::move( line_collection_id ) );
    }

    void SectionBuilder::add_surface_collection( uuid surface_collection_id )
    {
        create_surface_collection( std::move( surface_collection_id ) );
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

    void SectionBuilder::remove_corner_collection(
        const CornerCollection2D& collection )
    {
        unregister_component( collection.id() );
        delete_corner_collection( collection );
    }

    void SectionBuilder::remove_line_collection(
        const LineCollection2D& collection )
    {
        unregister_component( collection.id() );
        delete_line_collection( collection );
    }

    void SectionBuilder::remove_surface_collection(
        const SurfaceCollection2D& collection )
    {
        unregister_component( collection.id() );
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
