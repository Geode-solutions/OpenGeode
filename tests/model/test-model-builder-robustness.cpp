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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/builder/relationships_builder.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/relationships.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/tests/common.hpp>

void test_remove_foreign_component()
{
    geode::BRep other;
    geode::BRepBuilder other_builder{ other };
    const auto& foreign_id = other_builder.add_corner_collection();
    const auto& foreign = other.corner_collection( foreign_id );

    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto& collection_id = builder.add_corner_collection();
    bool has_thrown{ false };
    try
    {
        builder.remove_corner_collection( foreign );
    }
    catch( const geode::OpenGeodeException& )
    {
        has_thrown = true;
    }
    geode::OpenGeodeModelException::test( has_thrown,
        "[Test] Removing a component from another model should throw" );
    geode::OpenGeodeModelException::test(
        model.nb_corner_collections() == 1
            && model.has_corner_collection( collection_id ),
        "[Test] Model should still have its corner collection" );
    geode::OpenGeodeModelException::test( other.nb_corner_collections() == 1,
        "[Test] Other model should still have its corner collection" );
}

geode::BRep build_simple_brep()
{
    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto first_unique_vertex = builder.create_unique_vertices( 2 );
    for( const auto c : geode::LRange{ 2 } )
    {
        const auto& corner = model.corner( builder.add_corner() );
        builder.corner_mesh_builder( corner )->create_point(
            geode::Point3D{ { static_cast< double >( c ), 0., 0. } } );
        builder.set_unique_vertex(
            { corner.component_id(), 0 }, first_unique_vertex + c );
    }
    const auto& surface = model.surface( builder.add_surface() );
    auto surface_builder = builder.surface_mesh_builder( surface );
    surface_builder->create_point( geode::Point3D{ { 0., 0., 0. } } );
    surface_builder->create_point( geode::Point3D{ { 1., 0., 0. } } );
    surface_builder->create_point( geode::Point3D{ { 0., 1., 0. } } );
    surface_builder->create_polygon( { 0, 1, 2 } );
    const auto& block = model.block( builder.add_block() );
    auto block_builder = builder.block_mesh_builder( block );
    block_builder->create_point( geode::Point3D{ { 0., 0., 0. } } );
    block_builder->create_point( geode::Point3D{ { 1., 0., 0. } } );
    block_builder->create_point( geode::Point3D{ { 0., 1., 0. } } );
    block_builder->create_point( geode::Point3D{ { 0., 0., 1. } } );
    return model;
}

void test_partial_mesh_transfer()
{
    auto model = build_simple_brep();
    geode::BRep other;
    geode::BRepBuilder other_builder{ other };
    const auto full_mapping = other_builder.copy( model );
    absl::flat_hash_map< geode::uuid, geode::index_t > block_nb_vertices;
    for( const auto& block : model.blocks() )
    {
        block_nb_vertices.emplace( block.id(), block.mesh().nb_vertices() );
    }
    absl::flat_hash_map< geode::uuid, geode::index_t > surface_nb_vertices;
    for( const auto& surface : model.surfaces() )
    {
        surface_nb_vertices.emplace(
            surface.id(), surface.mesh().nb_vertices() );
    }
    // Only corner meshes are transferred: other types are absent
    geode::ModelCopyMapping partial_mapping;
    partial_mapping.emplace( geode::Corner3D::component_type_static(),
        full_mapping.at( geode::Corner3D::component_type_static() ) );
    geode::BRepBuilder builder{ model };
    builder.replace_components_meshes_by_others(
        std::move( other ), partial_mapping );
    for( const auto& block : model.blocks() )
    {
        geode::OpenGeodeModelException::test(
            block.mesh().nb_vertices() == block_nb_vertices.at( block.id() ),
            "[Test] Block mesh should not be modified by a partial transfer" );
    }
    for( const auto& surface : model.surfaces() )
    {
        geode::OpenGeodeModelException::test(
            surface.mesh().nb_vertices()
                == surface_nb_vertices.at( surface.id() ),
            "[Test] Surface mesh should not be modified by a partial "
            "transfer" );
    }
    for( const auto& corner : model.corners() )
    {
        geode::OpenGeodeModelException::test( corner.mesh().nb_vertices() == 1,
            "[Test] Corner mesh should be transferred" );
        geode::OpenGeodeModelException::test( corner.mesh().id() == corner.id(),
            "[Test] Transferred corner mesh should have the corner id" );
        geode::OpenGeodeModelException::test(
            model.unique_vertex( { corner.component_id(), 0 } ) != geode::NO_ID,
            "[Test] Transferred corner vertex should have a unique vertex" );
    }
}

void test_copy_relationships_into_non_empty()
{
    const geode::ComponentType type{ "dummy" };
    std::vector< geode::uuid > target_ids( 6 );
    geode::Relationships target;
    geode::RelationshipsBuilder target_builder{ target };
    for( const auto i : geode::LRange{ 3 } )
    {
        target_builder.add_boundary_relation(
            { type, target_ids[2 * i] }, { type, target_ids[2 * i + 1] } );
    }
    const geode::uuid x;
    const geode::uuid y;
    const geode::uuid unmapped;
    geode::Relationships source;
    geode::RelationshipsBuilder source_builder{ source };
    source_builder.add_boundary_relation( { type, x }, { type, y } );
    source_builder.add_boundary_relation( { type, unmapped }, { type, x } );
    geode::ModelCopyMapping mapping;
    auto& type_mapping = mapping[type];
    type_mapping.map( x, x );
    type_mapping.map( y, y );
    target_builder.copy_relationships( mapping, source );

    geode::OpenGeodeModelException::test(
        target.nb_components_with_relations() == 2,
        "[Test] Wrong number of components with relations after copy" );
    for( const auto& id : target_ids )
    {
        geode::OpenGeodeModelException::test( target.nb_relations( id ) == 0,
            "[Test] Previous components should have no relation after "
            "copy" );
    }
    geode::OpenGeodeModelException::test(
        target.nb_relations( x ) == 1 && target.nb_relations( y ) == 1,
        "[Test] Copied components should have one relation" );
    geode::OpenGeodeModelException::test( target.is_boundary( x, y ),
        "[Test] Copied relation should be a boundary relation" );
    geode::OpenGeodeModelException::test( target.nb_relations( unmapped ) == 0,
        "[Test] Unmapped component should not be copied" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_remove_foreign_component();
    test_partial_mesh_transfer();
    test_copy_relationships_into_non_empty();
}

OPENGEODE_TEST( "model-builder-robustness" )
