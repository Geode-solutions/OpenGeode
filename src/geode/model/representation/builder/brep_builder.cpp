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

#include <geode/model/representation/builder/brep_builder.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    template < typename Component >
    void register_new_component(
        geode::BRepBuilder& builder, const Component& component )
    {
        builder.register_component( component.component_id() );
        builder.register_mesh_component( component );
    }
} // namespace

namespace geode
{
    BRepBuilder::BRepBuilder( BRep& brep )
        : TopologyBuilder( brep ),
          AddComponentsBuilders< 3,
              Corners,
              Lines,
              Surfaces,
              Blocks,
              ModelBoundaries >( brep ),
          brep_( brep )
    {
    }

    void BRepBuilder::copy( const BRep& brep )
    {
        const auto mapping = copy_components( brep );
        copy_component_relationships( mapping, brep );
        copy_component_geometry( mapping, brep );
    }

    BRepBuilder::ComponentMapping BRepBuilder::copy_components(
        const BRep& brep )
    {
        ComponentMapping mapping;
        mapping.corners = detail::copy_corner_components( brep, brep_, *this );
        mapping.lines = detail::copy_line_components( brep, brep_, *this );
        mapping.surfaces =
            detail::copy_surface_components( brep, brep_, *this );
        mapping.blocks = detail::copy_block_components( brep, brep_, *this );
        mapping.collections =
            detail::copy_model_boundary_components( brep, *this );
        return mapping;
    }

    void BRepBuilder::copy_component_relationships(
        const ComponentMapping& mapping, const BRep& brep )
    {
        detail::copy_corner_line_relationships(
            brep, brep_, *this, mapping.corners, mapping.lines );
        detail::copy_corner_surface_relationships(
            brep, brep_, *this, mapping.corners, mapping.surfaces );
        detail::copy_line_surface_relationships(
            brep, brep_, *this, mapping.lines, mapping.surfaces );
        detail::copy_corner_block_relationships(
            brep, brep_, *this, mapping.corners, mapping.blocks );
        detail::copy_line_block_relationships(
            brep, brep_, *this, mapping.lines, mapping.blocks );
        detail::copy_surface_block_relationships(
            brep, brep_, *this, mapping.surfaces, mapping.blocks );
        for( const auto& model_boundary : brep.model_boundaries() )
        {
            const auto& new_model_boundary = brep_.model_boundary(
                mapping.collections.in2out( model_boundary.id() ) );
            for( const auto& surface : brep.items( model_boundary ) )
            {
                const auto& new_surface =
                    brep_.surface( mapping.surfaces.in2out( surface.id() ) );
                add_surface_in_model_boundary(
                    new_surface, new_model_boundary );
            }
        }
    }

    void BRepBuilder::copy_component_geometry(
        const ComponentMapping& mapping, const BRep& brep )
    {
        detail::copy_corner_geometry( brep, brep_, *this, mapping.corners );
        detail::copy_line_geometry( brep, brep_, *this, mapping.lines );
        detail::copy_surface_geometry( brep, brep_, *this, mapping.surfaces );
        detail::copy_block_geometry( brep, brep_, *this, mapping.blocks );
        create_unique_vertices( brep.nb_unique_vertices() );
        detail::copy_vertex_identifier_components(
            brep, *this, Corner3D::component_type_static(), mapping.corners );
        detail::copy_vertex_identifier_components(
            brep, *this, Line3D::component_type_static(), mapping.lines );
        detail::copy_vertex_identifier_components(
            brep, *this, Surface3D::component_type_static(), mapping.surfaces );
        detail::copy_vertex_identifier_components(
            brep, *this, Block3D::component_type_static(), mapping.blocks );
    }

    const uuid& BRepBuilder::add_corner()
    {
        const auto& id = create_corner();
        register_new_component( *this, brep_.corner( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_corner( const MeshType& type )
    {
        const auto& id = create_corner( type );
        register_new_component( *this, brep_.corner( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_line()
    {
        const auto& id = create_line();
        register_new_component( *this, brep_.line( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_line( const MeshType& type )
    {
        const auto& id = create_line( type );
        register_new_component( *this, brep_.line( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_surface()
    {
        const auto& id = create_surface();
        register_new_component( *this, brep_.surface( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_surface( const MeshType& type )
    {
        const auto& id = create_surface( type );
        register_new_component( *this, brep_.surface( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_block()
    {
        const auto& id = create_block();
        register_new_component( *this, brep_.block( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_block( const MeshType& type )
    {
        const auto& id = create_block( type );
        register_new_component( *this, brep_.block( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_model_boundary()
    {
        const auto& id = create_model_boundary();
        register_component( brep_.model_boundary( id ).component_id() );
        return id;
    }

    void BRepBuilder::update_corner_mesh(
        const Corner3D& corner, std::unique_ptr< PointSet3D > mesh )
    {
        unregister_mesh_component( corner );
        set_corner_mesh( corner.id(), std::move( mesh ) );
        register_mesh_component( corner );
    }

    void BRepBuilder::update_line_mesh(
        const Line3D& line, std::unique_ptr< EdgedCurve3D > mesh )
    {
        unregister_mesh_component( line );
        set_line_mesh( line.id(), std::move( mesh ) );
        register_mesh_component( line );
    }

    void BRepBuilder::update_surface_mesh(
        const Surface3D& surface, std::unique_ptr< PolygonalSurface3D > mesh )
    {
        unregister_mesh_component( surface );
        set_surface_mesh( surface.id(), std::move( mesh ) );
        register_mesh_component( surface );
    }

    void BRepBuilder::update_block_mesh(
        const Block3D& block, std::unique_ptr< PolyhedralSolid3D > mesh )
    {
        unregister_mesh_component( block );
        set_block_mesh( block.id(), std::move( mesh ) );
        register_mesh_component( block );
    }

    void BRepBuilder::remove_corner( const Corner3D& corner )
    {
        unregister_component( corner.id() );
        unregister_mesh_component( corner );
        delete_corner( corner );
    }

    void BRepBuilder::remove_line( const Line3D& line )
    {
        unregister_component( line.id() );
        unregister_mesh_component( line );
        delete_line( line );
    }

    void BRepBuilder::remove_surface( const Surface3D& surface )
    {
        unregister_component( surface.id() );
        unregister_mesh_component( surface );
        delete_surface( surface );
    }

    void BRepBuilder::remove_block( const Block3D& block )
    {
        unregister_component( block.id() );
        unregister_mesh_component( block );
        delete_block( block );
    }

    void BRepBuilder::remove_model_boundary( const ModelBoundary3D& boundary )
    {
        unregister_component( boundary.id() );
        delete_model_boundary( boundary );
    }

    void BRepBuilder::add_corner_line_boundary_relationship(
        const Corner3D& corner, const Line3D& line )
    {
        add_boundary_relation( corner.id(), line.id() );
    }

    void BRepBuilder::add_line_surface_boundary_relationship(
        const Line3D& line, const Surface3D& surface )
    {
        add_boundary_relation( line.id(), surface.id() );
    }

    void BRepBuilder::add_surface_block_boundary_relationship(
        const Surface3D& surface, const Block3D& block )
    {
        add_boundary_relation( surface.id(), block.id() );
    }

    void BRepBuilder::add_line_surface_internal_relationship(
        const Line3D& line, const Surface3D& surface )
    {
        add_internal_relation( line.id(), surface.id() );
    }

    void BRepBuilder::add_corner_surface_internal_relationship(
        const Corner3D& corner, const Surface3D& surface )
    {
        add_internal_relation( corner.id(), surface.id() );
    }

    void BRepBuilder::add_corner_block_internal_relationship(
        const Corner3D& corner, const Block3D& block )
    {
        add_internal_relation( corner.id(), block.id() );
    }

    void BRepBuilder::add_line_block_internal_relationship(
        const Line3D& line, const Block3D& block )
    {
        add_internal_relation( line.id(), block.id() );
    }

    void BRepBuilder::add_surface_block_internal_relationship(
        const Surface3D& surface, const Block3D& block )
    {
        add_internal_relation( surface.id(), block.id() );
    }

    void BRepBuilder::add_surface_in_model_boundary(
        const Surface3D& surface, const ModelBoundary3D& boundary )
    {
        add_item_in_collection( surface.id(), boundary.id() );
    }
} // namespace geode
