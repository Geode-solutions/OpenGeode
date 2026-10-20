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

#include <functional>
#include <memory>
#include <vector>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/logger_client.hpp>
#include <geode/basic/logger_manager.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/detail/transfer_collections.hpp>
#include <geode/model/representation/core/detail/transfer_metadata.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/tests/common.hpp>

namespace
{
    std::unique_ptr< geode::SurfaceMesh2D > create_surface_mesh(
        std::string_view name )
    {
        auto mesh = geode::SurfaceMesh2D::create();
        if( !name.empty() )
        {
            geode::SurfaceMeshBuilder2D::create( *mesh )->set_name( name );
        }
        return mesh;
    }

    void check_name( const geode::Surface2D& surface,
        std::optional< std::string_view > expected_name )
    {
        const auto& name = surface.name();
        if( !expected_name )
        {
            geode::OpenGeodeModelException::test( !name.has_value(),
                "Surface should not have a name, got ", name.value_or( "" ) );
            return;
        }
        geode::OpenGeodeModelException::test(
            name.has_value() && name.value() == expected_name.value(),
            "Wrong surface name: got ", name.value_or( "<none>" ),
            " instead of ", expected_name.value() );
    }

    void test_surfaces_metadata()
    {
        geode::Section section;
        geode::SectionBuilder builder{ section };
        const auto mesh_a = create_surface_mesh( "A" );
        const auto mesh_b = create_surface_mesh( "B" );
        const auto mesh_c = create_surface_mesh( "C" );
        const auto mesh_unnamed = create_surface_mesh( "" );
        const auto mesh_a2 = create_surface_mesh( "A" );
        const std::vector<
            std::reference_wrapper< const geode::SurfaceMesh2D > >
            meshes{ *mesh_a, *mesh_b, *mesh_c, *mesh_unnamed, *mesh_a2 };

        std::vector< geode::uuid > surfaces;
        for( const auto s : geode::Range{ 6 } )
        {
            geode_unused( s );
            surfaces.push_back( builder.add_surface() );
        }
        geode::GenericMapping< geode::uuid > mapping;
        mapping.map( mesh_a->id(), surfaces[0] );
        mapping.map( mesh_b->id(), surfaces[0] );
        mapping.map( mesh_c->id(), surfaces[1] );
        mapping.map( mesh_unnamed->id(), surfaces[2] );
        // Unnamed input between named ones
        mapping.map( mesh_a->id(), surfaces[3] );
        mapping.map( mesh_unnamed->id(), surfaces[3] );
        mapping.map( mesh_b->id(), surfaces[3] );
        // Unnamed last input
        mapping.map( mesh_a->id(), surfaces[4] );
        mapping.map( mesh_unnamed->id(), surfaces[4] );
        // Duplicated names
        mapping.map( mesh_a->id(), surfaces[5] );
        mapping.map( mesh_a2->id(), surfaces[5] );
        geode::ModelGenericMapping component_mapping;
        component_mapping.emplace(
            geode::Surface2D::component_type_static(), std::move( mapping ) );

        geode::detail::transfer_surfaces_metadata< geode::Section >(
            meshes, section, builder, component_mapping );
        check_name( section.surface( surfaces[0] ), "A+B" );
        check_name( section.surface( surfaces[1] ), "C" );
        check_name( section.surface( surfaces[2] ), std::nullopt );
        check_name( section.surface( surfaces[3] ), "A+B" );
        check_name( section.surface( surfaces[4] ), "A" );
        check_name( section.surface( surfaces[5] ), "A" );
    }

    void test_solids_metadata()
    {
        geode::BRep brep;
        geode::BRepBuilder builder{ brep };
        auto solid = geode::SolidMesh3D::create();
        geode::SolidMeshBuilder3D::create( *solid )->set_name( "solid" );
        const std::vector< std::reference_wrapper< const geode::SolidMesh3D > >
            solids{ *solid };
        const auto& block_id = builder.add_block();
        geode::GenericMapping< geode::uuid > mapping;
        mapping.map( solid->id(), block_id );
        geode::ModelGenericMapping component_mapping;
        component_mapping.emplace(
            geode::Block3D::component_type_static(), std::move( mapping ) );

        geode::detail::transfer_solids_metadata(
            solids, brep, builder, component_mapping );
        geode::OpenGeodeModelException::test(
            brep.block( block_id ).name() == "solid", "Wrong block name" );
    }
    struct BRepCollections
    {
        geode::uuid corner;
        geode::uuid other_corner;
        geode::uuid line;
        geode::uuid surface;
        geode::uuid block;
        geode::uuid corner_collection;
        geode::uuid line_collection;
        geode::uuid surface_collection;
        geode::uuid block_collection;
        geode::uuid model_boundary;
    };

    BRepCollections create_brep_with_collections(
        const geode::BRep& model, geode::BRepBuilder& builder )
    {
        BRepCollections ids;
        ids.corner = builder.add_corner();
        ids.other_corner = builder.add_corner();
        ids.line = builder.add_line();
        ids.surface = builder.add_surface();
        ids.block = builder.add_block();
        ids.corner_collection = builder.add_corner_collection();
        ids.line_collection = builder.add_line_collection();
        ids.surface_collection = builder.add_surface_collection();
        ids.block_collection = builder.add_block_collection();
        ids.model_boundary = builder.add_model_boundary();
        const auto& corner_collection =
            model.corner_collection( ids.corner_collection );
        builder.add_corner_in_corner_collection(
            model.corner( ids.corner ), corner_collection );
        builder.add_corner_in_corner_collection(
            model.corner( ids.other_corner ), corner_collection );
        builder.add_line_in_line_collection( model.line( ids.line ),
            model.line_collection( ids.line_collection ) );
        builder.add_surface_in_surface_collection( model.surface( ids.surface ),
            model.surface_collection( ids.surface_collection ) );
        builder.add_block_in_block_collection( model.block( ids.block ),
            model.block_collection( ids.block_collection ) );
        builder.add_surface_in_model_boundary( model.surface( ids.surface ),
            model.model_boundary( ids.model_boundary ) );
        return ids;
    }

    geode::ModelUnchangedComponentMapping brep_unchanged_components(
        const BRepCollections& ids )
    {
        geode::ModelUnchangedComponentMapping unchanged;
        unchanged.emplace(
            geode::Corner3D::component_type_static(), { ids.corner } );
        unchanged.emplace(
            geode::Line3D::component_type_static(), { ids.line } );
        unchanged.emplace(
            geode::Surface3D::component_type_static(), { ids.surface } );
        unchanged.emplace(
            geode::Block3D::component_type_static(), { ids.block } );
        return unchanged;
    }

    void check_brep_collections(
        const geode::BRep& model, const BRepCollections& ids )
    {
        geode::OpenGeodeModelException::test(
            model.has_corner_collection( ids.corner_collection )
                && model.nb_items( ids.corner_collection ) == 1
                && model.is_corner_collection_item( model.corner( ids.corner ),
                    model.corner_collection( ids.corner_collection ) ),
            "Wrong transferred corner collection" );
        geode::OpenGeodeModelException::test(
            model.has_line_collection( ids.line_collection )
                && model.nb_items( ids.line_collection ) == 1,
            "Wrong transferred line collection" );
        geode::OpenGeodeModelException::test(
            model.has_surface_collection( ids.surface_collection )
                && model.nb_items( ids.surface_collection ) == 1,
            "Wrong transferred surface collection" );
        geode::OpenGeodeModelException::test(
            model.has_block_collection( ids.block_collection )
                && model.nb_items( ids.block_collection ) == 1,
            "Wrong transferred block collection" );
        geode::OpenGeodeModelException::test(
            model.has_model_boundary( ids.model_boundary )
                && model.nb_items( ids.model_boundary ) == 1,
            "Wrong transferred model boundary" );
    }

    void test_unchanged_collections_transfer()
    {
        geode::BRep old_model;
        geode::BRepBuilder old_builder{ old_model };
        const auto ids = create_brep_with_collections( old_model, old_builder );

        geode::BRep new_model;
        geode::BRepBuilder new_builder{ new_model };
        new_builder.add_corner( ids.corner );
        new_builder.add_line( ids.line );
        new_builder.add_surface( ids.surface );
        new_builder.add_block( ids.block );
        geode::detail::transfer_brep_collections( old_model, new_model,
            new_builder, brep_unchanged_components( ids ) );
        check_brep_collections( new_model, ids );
    }
    class WarningCounter : public geode::LoggerClient
    {
    public:
        explicit WarningCounter( geode::index_t& nb_warnings )
            : nb_warnings_( nb_warnings )
        {
        }

        void trace( const std::string& /*unused*/ ) override {}

        void debug( const std::string& /*unused*/ ) override {}

        void info( const std::string& /*unused*/ ) override {}

        void warning( const std::string& /*unused*/ ) override
        {
            nb_warnings_++;
        }

        void error( const std::string& /*unused*/ ) override {}

        void critical( const std::string& /*unused*/ ) override {}

    private:
        geode::index_t& nb_warnings_;
    };

    geode::index_t& nb_warnings()
    {
        static geode::index_t counter{ 0 };
        static const auto& client = geode::LoggerManager::register_client(
            std::make_unique< WarningCounter >( counter ) );
        geode_unused( client );
        return counter;
    }

    void test_unchanged_collections_transfer_existing_relations()
    {
        geode::BRep old_model;
        geode::BRepBuilder old_builder{ old_model };
        const auto ids = create_brep_with_collections( old_model, old_builder );

        auto new_model = old_model.clone();
        geode::BRepBuilder new_builder{ new_model };
        const auto nb_warnings_before = nb_warnings();
        geode::detail::transfer_brep_collections( old_model, new_model,
            new_builder, brep_unchanged_components( ids ) );
        const auto nb_new_warnings = nb_warnings() - nb_warnings_before;
        geode::OpenGeodeModelException::test( nb_new_warnings == 0,
            "Existing collection relations should not be added again (",
            nb_new_warnings, " warnings)" );
        geode::OpenGeodeModelException::test(
            new_model.nb_items( ids.corner_collection ) == 2
                && new_model.nb_items( ids.line_collection ) == 1
                && new_model.nb_items( ids.surface_collection ) == 1
                && new_model.nb_items( ids.block_collection ) == 1
                && new_model.nb_items( ids.model_boundary ) == 1,
            "Wrong collections after transfer on existing relations" );
    }
    void test_section_unchanged_collections_transfer_existing_relations()
    {
        geode::Section old_model;
        geode::SectionBuilder old_builder{ old_model };
        const auto& line_id = old_builder.add_line();
        const auto& collection_id = old_builder.add_line_collection();
        const auto& boundary_id = old_builder.add_model_boundary();
        old_builder.add_line_in_line_collection( old_model.line( line_id ),
            old_model.line_collection( collection_id ) );
        old_builder.add_line_in_model_boundary( old_model.line( line_id ),
            old_model.model_boundary( boundary_id ) );

        auto new_model = old_model.clone();
        geode::SectionBuilder new_builder{ new_model };
        geode::ModelUnchangedComponentMapping unchanged;
        unchanged.emplace(
            geode::Line2D::component_type_static(), { line_id } );
        const auto nb_warnings_before = nb_warnings();
        geode::detail::transfer_section_collections(
            old_model, new_model, new_builder, unchanged );
        geode::OpenGeodeModelException::test(
            nb_warnings() == nb_warnings_before,
            "Existing Section collection relations should not be added "
            "again" );
        geode::OpenGeodeModelException::test(
            new_model.nb_items( collection_id ) == 1
                && new_model.nb_items( boundary_id ) == 1,
            "Wrong Section collections after transfer on existing relations" );
    }
} // namespace

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_surfaces_metadata();
    test_solids_metadata();
    test_unchanged_collections_transfer();
    test_unchanged_collections_transfer_existing_relations();
    test_section_unchanged_collections_transfer_existing_relations();
}

OPENGEODE_TEST( "model-transfer" )
