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
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
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
        const std::vector<
            std::reference_wrapper< const geode::SurfaceMesh2D > >
            meshes{ *mesh_a, *mesh_b, *mesh_c, *mesh_unnamed };

        std::vector< geode::uuid > surfaces;
        for( const auto s : geode::Range{ 3 } )
        {
            geode_unused( s );
            surfaces.push_back( builder.add_surface() );
        }
        geode::GenericMapping< geode::uuid > mapping;
        mapping.map( mesh_a->id(), surfaces[0] );
        mapping.map( mesh_b->id(), surfaces[0] );
        mapping.map( mesh_c->id(), surfaces[1] );
        mapping.map( mesh_unnamed->id(), surfaces[2] );
        geode::ModelGenericMapping component_mapping;
        component_mapping.emplace(
            geode::Surface2D::component_type_static(), std::move( mapping ) );

        geode::detail::transfer_surfaces_metadata< geode::Section >(
            meshes, section, builder, component_mapping );
        check_name( section.surface( surfaces[0] ), "A+B" );
        check_name( section.surface( surfaces[1] ), "C" );
        check_name( section.surface( surfaces[2] ), std::nullopt );
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
} // namespace

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_surfaces_metadata();
    test_solids_metadata();
}

OPENGEODE_TEST( "model-transfer" )
