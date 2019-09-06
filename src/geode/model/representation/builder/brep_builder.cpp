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

#include <geode/model/representation/builder/brep_builder.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    BRepBuilder::BRepBuilder( BRep& brep )
        : GeoRepresentationBuilder( brep ),
          AddComponentsBuilders< 3, Corners, Lines, Surfaces, Blocks >( brep ),
          brep_( brep )
    {
    }

    const uuid& BRepBuilder::add_corner()
    {
        const auto& id = create_corner();
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.corner( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_corner( const MeshType& type )
    {
        const auto& id = create_corner( type );
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.corner( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_line()
    {
        const auto& id = create_line();
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.line( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_line( const MeshType& type )
    {
        const auto& id = create_line( type );
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.line( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_surface()
    {
        const auto& id = create_surface();
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.surface( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_surface( const MeshType& type )
    {
        const auto& id = create_surface( type );
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.surface( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_block()
    {
        const auto& id = create_block();
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.block( id ) );
        return id;
    }

    const uuid& BRepBuilder::add_block( const MeshType& type )
    {
        const auto& id = create_block( type );
        relationships().register_component( id );
        unique_vertices().register_mesh_component( brep_.block( id ) );
        return id;
    }

    void BRepBuilder::remove_corner( const Corner3D& corner )
    {
        relationships().unregister_component( corner.id() );
        unique_vertices().unregister_mesh_component( corner );
        delete_corner( corner );
    }

    void BRepBuilder::remove_line( const Line3D& line )
    {
        relationships().unregister_component( line.id() );
        unique_vertices().unregister_mesh_component( line );
        delete_line( line );
    }

    void BRepBuilder::remove_surface( const Surface3D& surface )
    {
        relationships().unregister_component( surface.id() );
        unique_vertices().unregister_mesh_component( surface );
        delete_surface( surface );
    }

    void BRepBuilder::remove_block( const Block3D& block )
    {
        relationships().unregister_component( block.id() );
        unique_vertices().unregister_mesh_component( block );
        delete_block( block );
    }

    void BRepBuilder::add_boundary_relation(
        const Corner3D& corner, const Line3D& line )
    {
        relationships().add_boundary_relation( corner.id(), line.id() );
    }

    void BRepBuilder::add_boundary_relation(
        const Line3D& line, const Surface3D& surface )
    {
        relationships().add_boundary_relation( line.id(), surface.id() );
    }

    void BRepBuilder::add_boundary_relation(
        const Surface3D& surface, const Block3D& block )
    {
        relationships().add_boundary_relation( surface.id(), block.id() );
    }
} // namespace geode
