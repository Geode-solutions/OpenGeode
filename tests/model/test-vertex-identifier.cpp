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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/geometry/point.h>

#include <geode/model/mixin/core/vertex_identifier.h>

#include <geode/model/mixin/builder/add_components_builders.h>
#include <geode/model/mixin/builder/corners_builder.h>
#include <geode/model/mixin/builder/topology_builder.h>

#include <geode/model/mixin/core/add_components.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/corners.h>
#include <geode/model/mixin/core/topology.h>

#include <geode/mesh/builder/point_set_builder.h>

class CornerProvider : public geode::Topology,
                       public geode::AddComponents< 2, geode::Corners >
{
public:
    CornerProvider() = default;
};

class CornerProviderBuilder
    : public geode::TopologyBuilder,
      public geode::AddComponentsBuilders< 2, geode::Corners >
{
public:
    CornerProviderBuilder( CornerProvider& corner_provider )
        : TopologyBuilder( corner_provider ),
          AddComponentsBuilders< 2, geode::Corners >( corner_provider ),
          corner_provider_( corner_provider )
    {
    }

    const geode::uuid& add_corner()
    {
        const auto& id = create_corner();
        register_mesh_component( corner_provider_.corner( id ) );
        return id;
    }

private:
    CornerProvider& corner_provider_;
};

void test_create_unique_vertices( geode::VertexIdentifier& vertex_identifier )
{
    geode::VertexIdentifierBuilder builder{ vertex_identifier };
    builder.create_unique_vertices( 4 );
    OPENGEODE_EXCEPTION( vertex_identifier.nb_unique_vertices() == 4,
        "[Test] Creation of unique vertices is not correct" );
}

void test_set_unique_vertices(
    geode::VertexIdentifier& vertex_identifier, const CornerProvider& provider )
{
    geode::index_t count{ 0 };
    std::vector< geode::uuid > uuids;
    geode::VertexIdentifierBuilder builder{ vertex_identifier };
    for( const auto& corner : provider.corners() )
    {
        builder.set_unique_vertex( { corner.component_id(), 0 }, count++ % 3 );
        uuids.push_back( corner.id() );
    }

    OPENGEODE_EXCEPTION( vertex_identifier.nb_unique_vertices() == 4,
        "[Test] Creation of unique vertices is not correct" );

    const auto& uvertices0 = vertex_identifier.mesh_component_vertices( 0 );
    OPENGEODE_EXCEPTION( uvertices0.size() == 2,
        "[Test] Search of unique vertices is not correct" );
    const auto& uvertices0_type = vertex_identifier.mesh_component_vertices(
        0, geode::Corner2D::component_type_static() );
    OPENGEODE_EXCEPTION( uvertices0_type.size() == 2,
        "[Test] Search of unique vertices by "
        "MeshComponentType is not correct" );
    OPENGEODE_EXCEPTION(
        vertex_identifier.mesh_component_vertices( 0, uuids[0] ).size() == 1
            && vertex_identifier.mesh_component_vertices( 0, uuids[0] ).front()
                   == 0,
        "[Test] Search of unique vertices by MeshComponent UUID is not "
        "correct" );

    const auto& uvertices3 = vertex_identifier.mesh_component_vertices( 3 );
    OPENGEODE_EXCEPTION( uvertices3.size() == 0,
        "[Test] Search of unique vertices is not correct" );
}

void test_modify_unique_vertices( geode::VertexIdentifier& vertex_identifier )
{
    geode::VertexIdentifierBuilder builder{ vertex_identifier };
    OPENGEODE_EXCEPTION( builder.create_unique_vertex() == 4,
        "[Test] Creation of an unique vertices is not correct" );

    const auto mesh_component_vertex0 =
        vertex_identifier.mesh_component_vertices( 0 ).front();

    builder.set_unique_vertex( mesh_component_vertex0, 0 );
    OPENGEODE_EXCEPTION(
        vertex_identifier.mesh_component_vertices( 0 ).size() == 2
            && vertex_identifier.mesh_component_vertices( 3 ).size() == 0,
        "[Test] Reset of a unique vertex (to similar value) is not correct" );

    builder.set_unique_vertex( mesh_component_vertex0, 3 );
    OPENGEODE_EXCEPTION(
        vertex_identifier.mesh_component_vertices( 0 ).size() == 1
            && vertex_identifier.mesh_component_vertices( 3 ).size() == 1,
        "[Test] Reset of a unique vertex is not correct - sizes" );
    OPENGEODE_EXCEPTION(
        vertex_identifier.unique_vertex( mesh_component_vertex0 ) == 3,
        "[Test] Reset of a unique vertex is not correct - values" );
}

void test_save_and_load_unique_vertices(
    geode::VertexIdentifier& vertex_identifier )
{
    vertex_identifier.save_unique_vertices( "." );
    geode::VertexIdentifier vertex_identifier2;
    geode::VertexIdentifierBuilder loader{ vertex_identifier2 };
    loader.load_unique_vertices( "." );
    OPENGEODE_EXCEPTION( vertex_identifier2.nb_unique_vertices()
                             == vertex_identifier.nb_unique_vertices(),
        "[Test] Save/Load of unique vertices are not correct (different number "
        "of "
        "unique vertices)" );
    for( auto v_id : geode::Range{ vertex_identifier2.nb_unique_vertices() } )
    {
        OPENGEODE_EXCEPTION(
            vertex_identifier.mesh_component_vertices( v_id ).size()
                == vertex_identifier2.mesh_component_vertices( v_id ).size(),
            "[Test] Save/Load of unique vertices are not correct (different "
            "number of mesh component vertices)" );
        for( auto mcv : geode::Range{
                 vertex_identifier.mesh_component_vertices( v_id ).size() } )
        {
            OPENGEODE_EXCEPTION(
                vertex_identifier.mesh_component_vertices( v_id )[mcv]
                    == vertex_identifier2.mesh_component_vertices( v_id )[mcv],
                "[Test] Save/Load of unique vertices are not correct "
                "(different "
                "mesh component vertices)" );
        }
    }
}

int main()
{
    using namespace geode;

    try
    {
        VertexIdentifier vertex_identifier;
        OPENGEODE_EXCEPTION( vertex_identifier.nb_unique_vertices() == 0,
            "[Test] Initialization of VertexIdentifier is not correct" );

        CornerProvider provider;
        CornerProviderBuilder builder( provider );

        const auto& corner0_id = builder.add_corner();
        const auto& corner1_id = builder.add_corner();
        const auto& corner2_id = builder.add_corner();
        const auto& corner3_id = builder.add_corner();
        builder.corner_mesh_builder( corner0_id )
            ->create_point( geode::Point2D{ { 0.1, 2.3 } } );
        builder.corner_mesh_builder( corner1_id )
            ->create_point( geode::Point2D{ { 1.1, 4.3 } } );
        builder.corner_mesh_builder( corner2_id )
            ->create_point( geode::Point2D{ { 0.9, -3.7 } } );
        builder.corner_mesh_builder( corner3_id )
            ->create_point( geode::Point2D{ { 0.1, 2.3 } } );

        VertexIdentifierBuilder vertex_id_builder{ vertex_identifier };
        vertex_id_builder.register_mesh_component(
            provider.corner( corner0_id ) );
        vertex_id_builder.register_mesh_component(
            provider.corner( corner1_id ) );
        vertex_id_builder.register_mesh_component(
            provider.corner( corner2_id ) );
        vertex_id_builder.register_mesh_component(
            provider.corner( corner3_id ) );

        test_create_unique_vertices( vertex_identifier );
        test_set_unique_vertices( vertex_identifier, provider );
        test_modify_unique_vertices( vertex_identifier );
        test_save_and_load_unique_vertices( vertex_identifier );

        builder.unregister_mesh_component( provider.corner( corner2_id ) );
        builder.register_mesh_component( provider.corner( corner2_id ) );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
