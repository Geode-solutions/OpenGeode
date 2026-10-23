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

#include <array>
#include <memory>
#include <string_view>
#include <vector>

#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/attribute_time_series.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/helpers/external_time_series_helpers.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/brep_time_series_input.hpp>
#include <geode/model/representation/io/external_time_series_helpers_input.hpp>
#include <geode/model/representation/io/external_time_series_helpers_output.hpp>

#include <geode/tests/common.hpp>

namespace
{
    constexpr auto PRESSURE = "pressure";
    constexpr auto VELOCITY = "velocity";
    constexpr std::array< double, 2 > TIMES{ 0., 10. };
    // test_mesh3 coordinates go up to 10: the matching tolerance is
    // 10 * GLOBAL_EPSILON
    constexpr double ROUNDING_TRANSLATION{ geode::GLOBAL_EPSILON };
    constexpr double WRONG_TRANSLATION{ 100 * geode::GLOBAL_EPSILON };

    double pressure( geode::index_t unique_vertex, double time )
    {
        return unique_vertex + time;
    }

    std::array< double, 3 > velocity( geode::index_t polyhedron )
    {
        return { static_cast< double >( polyhedron ), 1., 2. };
    }

    template < typename T >
    std::shared_ptr< geode::VariableAttribute< T > > create_time_step(
        geode::AttributeManager& manager, std::string_view name, double time )
    {
        const auto attribute_id =
            manager.create_time_step_attribute< geode::VariableAttribute, T >(
                name, time, geode::AttributeValues< T >{},
                geode::AttributeProperties{} );
        return manager.find_attribute< geode::VariableAttribute, T >(
            attribute_id );
    }

    // One point per model unique vertex
    std::unique_ptr< geode::PointSet3D > create_unique_vertices(
        const geode::BRep& model )
    {
        auto vertices = geode::PointSet3D::create();
        auto builder = geode::PointSetBuilder3D::create( *vertices );
        std::vector< geode::index_t > unique_vertices;
        std::vector< bool > visited( model.nb_unique_vertices(), false );
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
            {
                const auto unique_vertex =
                    model.unique_vertex( { block.component_id(), vertex } );
                if( visited[unique_vertex] )
                {
                    continue;
                }
                visited[unique_vertex] = true;
                builder->create_point( mesh.point( vertex ) );
                unique_vertices.push_back( unique_vertex );
            }
        }
        for( const auto time : TIMES )
        {
            auto attribute = create_time_step< double >(
                vertices->vertex_attribute_manager(), PRESSURE, time );
            for( const auto point : geode::Indices{ unique_vertices } )
            {
                attribute->set_value(
                    point, pressure( unique_vertices[point], time ) );
            }
        }
        return vertices;
    }

    // Barycenters of a single Block: the other Blocks get no velocity
    std::unique_ptr< geode::PointSet3D > create_cell_barycenters(
        const geode::BRep& model,
        const geode::uuid& block_id,
        const geode::Vector3D& translation )
    {
        auto barycenters = geode::PointSet3D::create();
        auto builder = geode::PointSetBuilder3D::create( *barycenters );
        const auto& mesh = model.block( block_id ).mesh();
        auto attribute = create_time_step< std::array< double, 3 > >(
            barycenters->vertex_attribute_manager(), VELOCITY, TIMES[1] );
        for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
        {
            const auto point = builder->create_point(
                mesh.polyhedron_barycenter( polyhedron ) + translation );
            attribute->set_value( point, velocity( polyhedron ) );
        }
        return barycenters;
    }

    geode::ExternalTimeSeriesHelpers create_helpers( const geode::BRep& model,
        const geode::uuid& block_id,
        const geode::Vector3D& translation )
    {
        geode::ExternalTimeSeriesHelpers helpers;
        helpers.set_unique_vertices( create_unique_vertices( model ) );
        helpers.set_cell_barycenters(
            create_cell_barycenters( model, block_id, translation ) );
        return helpers;
    }

    geode::ExternalTimeSeriesHelpers create_helpers(
        const geode::BRep& model, const geode::uuid& block_id )
    {
        return create_helpers( model, block_id, geode::Vector3D{} );
    }

    void check_pressures( const geode::BRep& model )
    {
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            const geode::AttributeTimeSeries< double > series{
                mesh.vertex_attribute_manager(), PRESSURE
            };
            geode::OpenGeodeModelException::test(
                series.nb_time_steps() == TIMES.size(),
                "[Test] Wrong number of pressure time steps on Block ",
                block.id().string(), ": ", series.nb_time_steps() );
            for( const auto step : geode::Range{ series.nb_time_steps() } )
            {
                geode::OpenGeodeModelException::test(
                    series.time( step ) == TIMES[step],
                    "[Test] Wrong pressure time" );
                for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
                {
                    const auto unique_vertex =
                        model.unique_vertex( { block.component_id(), vertex } );
                    geode::OpenGeodeModelException::test(
                        series.value( step, vertex )
                            == pressure( unique_vertex, TIMES[step] ),
                        "[Test] Wrong pressure on vertex ", vertex,
                        " of Block ", block.id().string() );
                }
            }
        }
    }

    void check_velocities(
        const geode::BRep& model, const geode::uuid& block_id )
    {
        const auto& mesh = model.block( block_id ).mesh();
        const geode::AttributeTimeSeries< std::array< double, 3 > > series{
            mesh.polyhedron_attribute_manager(), VELOCITY
        };
        geode::OpenGeodeModelException::test(
            series.nb_time_steps() == 1 && series.time( 0 ) == TIMES[1],
            "[Test] Wrong velocity time steps" );
        for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
        {
            geode::OpenGeodeModelException::test(
                series.value( 0, polyhedron ) == velocity( polyhedron ),
                "[Test] Wrong velocity on polyhedron ", polyhedron );
        }
        for( const auto& block : model.blocks() )
        {
            if( block.id() == block_id )
            {
                continue;
            }
            geode::OpenGeodeModelException::test(
                block.mesh()
                    .polyhedron_attribute_manager()
                    .time_steps( VELOCITY )
                    .empty(),
                "[Test] Block ", block.id().string(),
                " should have no velocity" );
        }
    }

    void check_other_model( const geode::ExternalTimeSeriesHelpers& helpers )
    {
        auto other = geode::load_brep(
            absl::StrCat( geode::DATA_PATH, "layers.og_brep" ) );
        bool thrown{ false };
        try
        {
            helpers.apply_to( other );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test(
            thrown, "[Test] Applying on another model should throw" );
    }

    // Translated barycenters are matched only with a large enough tolerance
    // Barycenters translated below the tolerance are matched, beyond it
    // they are rejected
    void check_tolerance( const geode::uuid& block_id )
    {
        auto model = geode::load_brep(
            absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
        create_helpers(
            model, block_id, geode::Vector3D{ { ROUNDING_TRANSLATION, 0, 0 } } )
            .apply_to( model );
        check_velocities( model, block_id );
        const auto wrong_helpers = create_helpers(
            model, block_id, geode::Vector3D{ { WRONG_TRANSLATION, 0, 0 } } );
        bool thrown{ false };
        try
        {
            wrong_helpers.apply_to( model );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test( thrown,
            "[Test] Applying barycenters translated beyond the tolerance "
            "should throw" );
    }

    // Values of colocated points cannot be told apart
    void check_colocated_points( const geode::uuid& block_id )
    {
        auto model = geode::load_brep(
            absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
        auto barycenters =
            create_cell_barycenters( model, block_id, geode::Vector3D{} );
        geode::PointSetBuilder3D::create( *barycenters )
            ->create_point( barycenters->point( 0 ) );
        geode::ExternalTimeSeriesHelpers helpers;
        helpers.set_cell_barycenters( std::move( barycenters ) );
        bool thrown{ false };
        try
        {
            helpers.apply_to( model );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test(
            thrown, "[Test] Applying colocated barycenters should throw" );
    }

    void check_null_point_set()
    {
        geode::ExternalTimeSeriesHelpers helpers;
        bool thrown{ false };
        try
        {
            helpers.set_cell_barycenters( nullptr );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test(
            thrown, "[Test] Setting a null point set should throw" );
    }

    void check_wrong_extension(
        const geode::ExternalTimeSeriesHelpers& helpers )
    {
        bool thrown{ false };
        try
        {
            geode::save_external_time_series_helpers(
                helpers, "test_external_time_series.txt" );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test(
            thrown, "[Test] Saving with another extension should throw" );
    }
} // namespace

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    auto model = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
    geode::OpenGeodeModelException::test(
        model.nb_blocks() > 0, "[Test] Model has no Block" );
    const auto block_id = ( *model.blocks().begin() ).id();
    const auto filename = absl::StrCat( "test_external_time_series.",
        geode::ExternalTimeSeriesHelpers::native_extension_static() );
    geode::save_external_time_series_helpers(
        create_helpers( model, block_id ), filename );
    const auto helpers = geode::load_external_time_series_helpers( filename );

    helpers.apply_to( model );
    check_pressures( model );
    check_velocities( model, block_id );
    create_helpers( model, block_id ).apply_to( model );
    check_pressures( model );
    check_velocities( model, block_id );

    auto reloaded_model = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
    geode::load_brep_time_series( reloaded_model, filename );
    check_pressures( reloaded_model );
    check_velocities( reloaded_model, block_id );
    check_other_model( helpers );
    check_tolerance( block_id );
    check_colocated_points( block_id );
    check_null_point_set();
    check_wrong_extension( helpers );
}

OPENGEODE_TEST( "external-time-series-helpers" )
