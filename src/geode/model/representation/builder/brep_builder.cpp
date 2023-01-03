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

#include <geode/model/representation/builder/brep_builder.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_id.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>
#include <geode/model/representation/core/brep.h>

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
          IdentifierBuilder( brep ),
          brep_( brep )
    {
    }

    ModelCopyMapping BRepBuilder::copy( const BRep& brep )
    {
        set_name( brep.name() );
        const auto mapping = copy_components( brep );
        copy_relationships( mapping, brep );
        copy_component_geometry( mapping, brep );
        return mapping;
    }

    ModelCopyMapping BRepBuilder::copy_components( const BRep& brep )
    {
        ModelCopyMapping mappings;
        mappings.emplace( Corner3D::component_type_static(),
            detail::copy_corner_components( brep, brep_, *this ) );
        mappings.emplace( Line3D::component_type_static(),
            detail::copy_line_components( brep, brep_, *this ) );
        mappings.emplace( Surface3D::component_type_static(),
            detail::copy_surface_components( brep, brep_, *this ) );
        mappings.emplace( Block3D::component_type_static(),
            detail::copy_block_components( brep, brep_, *this ) );
        mappings.emplace( ModelBoundary3D::component_type_static(),
            detail::copy_model_boundary_components( brep, *this ) );
        return mappings;
    }

    void BRepBuilder::copy_component_geometry(
        const ModelCopyMapping& mappings, const BRep& brep )
    {
        detail::copy_corner_geometry< 3 >( brep, brep_, *this,
            mappings.at( Corner3D::component_type_static() ) );
        detail::copy_line_geometry< 3 >( brep, brep_, *this,
            mappings.at( Line3D::component_type_static() ) );
        detail::copy_surface_geometry< 3 >( brep, brep_, *this,
            mappings.at( Surface3D::component_type_static() ) );
        detail::copy_block_geometry< 3 >( brep, brep_, *this,
            mappings.at( Block3D::component_type_static() ) );
        create_unique_vertices( brep.nb_unique_vertices() );
        detail::copy_vertex_identifier_components( brep, *this, mappings );
    }

    const uuid& BRepBuilder::add_corner()
    {
        const auto& id = create_corner();
        register_mesh_component( brep_.corner( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_corner( const MeshImpl& impl )
    {
        const auto& id = create_corner( impl );
        register_mesh_component( brep_.corner( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_line()
    {
        const auto& id = create_line();
        register_mesh_component( brep_.line( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_line( const MeshImpl& impl )
    {
        const auto& id = create_line( impl );
        register_mesh_component( brep_.line( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_surface()
    {
        const auto& id = create_surface();
        register_mesh_component( brep_.surface( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_surface( const MeshImpl& impl )
    {
        const auto& id = create_surface( impl );
        register_mesh_component( brep_.surface( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_block()
    {
        const auto& id = create_block();
        register_mesh_component( brep_.block( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_block( const MeshImpl& impl )
    {
        const auto& id = create_block( impl );
        register_mesh_component( brep_.block( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_model_boundary()
    {
        const auto& id = create_model_boundary();
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
        const Surface3D& surface, std::unique_ptr< SurfaceMesh3D > mesh )
    {
        unregister_mesh_component( surface );
        set_surface_mesh( surface.id(), std::move( mesh ) );
        register_mesh_component( surface );
    }

    void BRepBuilder::update_block_mesh(
        const Block3D& block, std::unique_ptr< SolidMesh3D > mesh )
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
        add_boundary_relation( corner.component_id(), line.component_id() );
    }

    void BRepBuilder::add_line_surface_boundary_relationship(
        const Line3D& line, const Surface3D& surface )
    {
        add_boundary_relation( line.component_id(), surface.component_id() );
    }

    void BRepBuilder::add_surface_block_boundary_relationship(
        const Surface3D& surface, const Block3D& block )
    {
        add_boundary_relation( surface.component_id(), block.component_id() );
    }

    void BRepBuilder::add_line_surface_internal_relationship(
        const Line3D& line, const Surface3D& surface )
    {
        add_internal_relation( line.component_id(), surface.component_id() );
    }

    void BRepBuilder::add_corner_surface_internal_relationship(
        const Corner3D& corner, const Surface3D& surface )
    {
        add_internal_relation( corner.component_id(), surface.component_id() );
    }

    void BRepBuilder::add_corner_block_internal_relationship(
        const Corner3D& corner, const Block3D& block )
    {
        add_internal_relation( corner.component_id(), block.component_id() );
    }

    void BRepBuilder::add_line_block_internal_relationship(
        const Line3D& line, const Block3D& block )
    {
        add_internal_relation( line.component_id(), block.component_id() );
    }

    void BRepBuilder::add_surface_block_internal_relationship(
        const Surface3D& surface, const Block3D& block )
    {
        add_internal_relation( surface.component_id(), block.component_id() );
    }

    void BRepBuilder::add_surface_in_model_boundary(
        const Surface3D& surface, const ModelBoundary3D& boundary )
    {
        add_item_in_collection(
            surface.component_id(), boundary.component_id() );
    }
} // namespace geode
