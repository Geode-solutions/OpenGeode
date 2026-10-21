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

#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/attribute_time_series.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/helpers/external_time_series_helpers.hpp>
#include <geode/model/mixin/core/block.hpp>
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

    double pressure( geode::index_t unique_vertex, double time )
    {
        return unique_vertex + time;
    }

    std::array< double, 3 > velocity( geode::index_t polyhedron )
    {
        return { static_cast< double >( polyhedron ), 1., 2. };
    }

    geode::ExternalTimeSeriesHelpers create_helpers(
        const geode::BRep& model, const geode::uuid& block_id )
    {
        geode::ExternalTimeSeriesHelpers helpers{ model };
        auto& vertices = helpers.unique_vertex_attribute_manager();
        for( const auto time : TIMES )
        {
            const auto attribute_id =
                vertices.create_time_step_attribute< geode::VariableAttribute,
                    double >( PRESSURE, time,
                    geode::AttributeValues< double >{},
                    geode::AttributeProperties{} );
            auto attribute =
                vertices.find_attribute< geode::VariableAttribute, double >(
                    attribute_id );
            for( const auto unique_vertex :
                geode::Range{ model.nb_unique_vertices() } )
            {
                attribute->set_value(
                    unique_vertex, pressure( unique_vertex, time ) );
            }
        }
        auto& polyhedra =
            helpers.block_polyhedron_attribute_manager( block_id );
        const auto attribute_id =
            polyhedra.create_time_step_attribute< geode::VariableAttribute,
                std::array< double, 3 > >( VELOCITY, TIMES[1],
                geode::AttributeValues< std::array< double, 3 > >{},
                geode::AttributeProperties{} );
        auto attribute = polyhedra.find_attribute< geode::VariableAttribute,
            std::array< double, 3 > >( attribute_id );
        for( const auto polyhedron :
            geode::Range{ model.block( block_id ).mesh().nb_polyhedra() } )
        {
            attribute->set_value( polyhedron, velocity( polyhedron ) );
        }
        return helpers;
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
                block.id().string() );
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
    }

    void check_other_model( const geode::ExternalTimeSeriesHelpers& helpers,
        const geode::BRep& model )
    {
        auto other = geode::load_brep(
            absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
        geode::OpenGeodeModelException::test(
            other.nb_unique_vertices() != model.nb_unique_vertices(),
            "[Test] Models should have different unique vertices" );
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
    auto model =
        geode::load_brep( absl::StrCat( geode::DATA_PATH, "layers.og_brep" ) );
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

    auto reloaded_model =
        geode::load_brep( absl::StrCat( geode::DATA_PATH, "layers.og_brep" ) );
    geode::load_brep_time_series( reloaded_model, filename );
    check_pressures( reloaded_model );
    check_velocities( reloaded_model, block_id );
    check_other_model( helpers, model );
    check_wrong_extension( helpers );
}

OPENGEODE_TEST( "external-time-series-helpers" )
