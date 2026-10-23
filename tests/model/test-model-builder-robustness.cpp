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
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/detail/solid_mesh_validity_fix.hpp>
#include <geode/model/helpers/detail/split_along_block_mesh_borders.hpp>
#include <geode/model/helpers/detail/split_along_surface_mesh_borders.hpp>
#include <geode/model/helpers/detail/surface_mesh_validity_fix.hpp>
#include <geode/model/mixin/builder/relationships_builder.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/relationships.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

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
    {
        // This unique vertex is only used by a surface, which is not
        // transferred: its copy in other should not end up isolated in model
        geode::BRepBuilder builder{ model };
        const auto& surface = *model.surfaces().begin();
        builder.set_unique_vertex(
            { surface.component_id(), 0 }, builder.create_unique_vertex() );
    }
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
    geode::OpenGeodeModelException::test( model.nb_unique_vertices() == 3,
        "[Test] Wrong number of unique vertices after partial transfer" );
    for( const auto uv : geode::Range{ model.nb_unique_vertices() } )
    {
        geode::OpenGeodeModelException::test(
            !model.is_unique_vertex_isolated( uv ),
            "[Test] No unique vertex should be isolated after partial "
            "transfer" );
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

template < typename Model >
void test_copy_into_model_with_collection()
{
    const auto source = [] {
        Model model;
        typename Model::Builder builder{ model };
        static_cast< void >( builder.add_corner() );
        return model;
    }();
    Model model;
    typename Model::Builder builder{ model };
    static_cast< void >( builder.add_corner_collection() );
    bool has_thrown{ false };
    try
    {
        static_cast< void >( builder.copy( source ) );
    }
    catch( const geode::OpenGeodeException& )
    {
        has_thrown = true;
    }
    geode::OpenGeodeModelException::test(
        has_thrown, "[Test] Copy into a model with collections should throw" );
}

template < typename Function >
bool throws_opengeode_exception( Function&& function )
{
    try
    {
        function();
    }
    catch( const geode::OpenGeodeException& )
    {
        return true;
    }
    return false;
}

void test_null_mesh_access()
{
    auto model = build_simple_brep();
    geode::BRepBuilder builder{ model };
    const auto& corner = *model.corners().begin();
    const auto stolen_mesh = builder.steal_corner_mesh( corner );
    geode::OpenGeodeModelException::test(
        stolen_mesh != nullptr, "[Test] Stolen mesh should not be null" );
    geode::OpenGeodeModelException::test(
        throws_opengeode_exception( [&corner] {
            static_cast< void >( corner.mesh() );
        } ),
        "[Test] Accessing a stolen mesh should throw" );
    const auto& surface = *model.surfaces().begin();
    geode::OpenGeodeModelException::test(
        throws_opengeode_exception( [&builder, &surface] {
            builder.update_surface_mesh( surface, nullptr );
        } ),
        "[Test] Setting a null mesh should throw" );
}

template < typename Component >
void check_mesh_id( const Component& component )
{
    geode::OpenGeodeModelException::test(
        component.mesh().id() == component.id(), "[Test] ",
        component.component_type().get(),
        " mesh should have the same id as the component" );
}

void test_mesh_id_on_creation_with_uuid()
{
    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const geode::uuid corner_id;
    builder.add_corner( corner_id );
    check_mesh_id( model.corner( corner_id ) );
    const geode::uuid corner_impl_id;
    builder.add_corner(
        corner_impl_id, model.corner( corner_id ).mesh().impl_name() );
    check_mesh_id( model.corner( corner_impl_id ) );
    const geode::uuid line_id;
    builder.add_line( line_id );
    check_mesh_id( model.line( line_id ) );
    const geode::uuid line_impl_id;
    builder.add_line( line_impl_id, model.line( line_id ).mesh().impl_name() );
    check_mesh_id( model.line( line_impl_id ) );
    const geode::uuid surface_id;
    builder.add_surface( surface_id );
    check_mesh_id( model.surface( surface_id ) );
    const geode::uuid surface_impl_id;
    builder.add_surface(
        surface_impl_id, model.surface( surface_id ).mesh().impl_name() );
    check_mesh_id( model.surface( surface_impl_id ) );
    const geode::uuid block_id;
    builder.add_block( block_id );
    check_mesh_id( model.block( block_id ) );
    const geode::uuid block_impl_id;
    builder.add_block(
        block_impl_id, model.block( block_id ).mesh().impl_name() );
    check_mesh_id( model.block( block_impl_id ) );
}

// Two triangles sharing only one vertex, without any unique vertex
const geode::Surface3D& add_bowtie_surface(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    const auto& surface = model.surface( builder.add_surface() );
    auto mesh_builder = builder.surface_mesh_builder( surface );
    mesh_builder->create_point( geode::Point3D{ { 0., 0., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 1., 0., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 0., 1., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { -1., 0., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 0., -1., 0. } } );
    mesh_builder->create_polygon( { 0, 1, 2 } );
    mesh_builder->create_polygon( { 0, 3, 4 } );
    mesh_builder->compute_polygon_adjacencies();
    return surface;
}

// Two tetrahedra sharing only one vertex, without any unique vertex
const geode::Block3D& add_bowtie_block(
    const geode::BRep& model, geode::BRepBuilder& builder )
{
    const auto& block = model.block( builder.add_block() );
    auto mesh_builder = builder.block_mesh_builder( block );
    mesh_builder->create_point( geode::Point3D{ { 0., 0., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 1., 0., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 0., 1., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 0., 0., 1. } } );
    mesh_builder->create_point( geode::Point3D{ { -1., 0., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 0., -1., 0. } } );
    mesh_builder->create_point( geode::Point3D{ { 0., 0., -1. } } );
    const std::vector< std::vector< geode::local_index_t > > facets{
        { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 }
    };
    mesh_builder->create_polyhedron( { 0, 1, 2, 3 }, facets );
    mesh_builder->create_polyhedron( { 0, 4, 5, 6 }, facets );
    mesh_builder->compute_polyhedron_adjacencies();
    return block;
}

template < typename Mapping >
void check_no_unique_vertex( const geode::BRep& model, Mapping&& mapping )
{
    geode::OpenGeodeModelException::test(
        !mapping.empty(), "[Test] Vertices should have been split" );
    for( const auto& [_, out] : mapping )
    {
        geode::OpenGeodeModelException::test(
            model.unique_vertex( out ) == geode::NO_ID,
            "[Test] Split vertex should not have a unique vertex" );
    }
}

void test_repair_surface_without_unique_vertices()
{
    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto& surface = add_bowtie_surface( model, builder );
    const auto mapping =
        geode::detail::repair_non_manifold_vertices( model, builder, surface );
    geode::OpenGeodeModelException::test( mapping.size_in() == 1,
        "[Test] Surface non-manifold vertex should have been repaired" );
    geode::OpenGeodeModelException::test(
        surface.mesh().nb_vertices() == 6, "[Test] Wrong surface size" );
    geode::OpenGeodeModelException::test(
        model.unique_vertex( { surface.component_id(), 5 } ) == geode::NO_ID,
        "[Test] Repaired surface vertex should not have a unique vertex" );
}

void test_repair_block_without_unique_vertices()
{
    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto& block = add_bowtie_block( model, builder );
    const auto mapping =
        geode::detail::repair_non_manifold_vertices( model, builder, block );
    geode::OpenGeodeModelException::test( mapping.size_in() == 1,
        "[Test] Block non-manifold vertex should have been repaired" );
    geode::OpenGeodeModelException::test(
        block.mesh().nb_vertices() == 8, "[Test] Wrong block size" );
    geode::OpenGeodeModelException::test(
        model.unique_vertex( { block.component_id(), 7 } ) == geode::NO_ID,
        "[Test] Repaired block vertex should not have a unique vertex" );
}

void test_split_surface_without_unique_vertices()
{
    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto& surface = add_bowtie_surface( model, builder );
    geode::detail::SplitAlongSurfaceMeshBorders< geode::BRep > splitter{ model,
        builder };
    check_no_unique_vertex( model, splitter.split_surface( surface ) );
}

void test_split_block_without_unique_vertices()
{
    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto& block = add_bowtie_block( model, builder );
    geode::detail::SplitAlongBlockMeshBorders splitter{ model, builder };
    check_no_unique_vertex( model, splitter.split_block( block ) );
}

void test_split_all_blocks()
{
    geode::BRep model;
    geode::BRepBuilder builder{ model };
    const auto& block0 = add_bowtie_block( model, builder );
    const auto& block1 = add_bowtie_block( model, builder );
    geode::detail::SplitAlongBlockMeshBorders splitter{ model, builder };
    const auto mapping = splitter.split_all_blocks();
    geode::OpenGeodeModelException::test(
        mapping.size() == 2, "[Test] Both blocks should have been split" );
    geode::OpenGeodeModelException::test(
        block0.mesh().nb_vertices() == 8 && block1.mesh().nb_vertices() == 8,
        "[Test] Wrong number of block vertices after split" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_remove_foreign_component();
    test_partial_mesh_transfer();
    test_copy_relationships_into_non_empty();
    test_copy_into_model_with_collection< geode::BRep >();
    test_copy_into_model_with_collection< geode::Section >();
    test_null_mesh_access();
    test_mesh_id_on_creation_with_uuid();
    test_repair_surface_without_unique_vertices();
    test_repair_block_without_unique_vertices();
    test_split_surface_without_unique_vertices();
    test_split_block_without_unique_vertices();
    test_split_all_blocks();
}

OPENGEODE_TEST( "model-builder-robustness" )
