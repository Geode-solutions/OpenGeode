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
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/external_time_series.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/brep_time_series_input.hpp>
#include <geode/model/representation/io/external_time_series_input.hpp>
#include <geode/model/representation/io/external_time_series_output.hpp>

#include <geode/tests/common.hpp>

namespace
{
    constexpr auto PRESSURE = "pressure";
    constexpr auto VELOCITY = "velocity";
    constexpr std::array< double, 2 > TIMES{ 0., 10. };
    // test_mesh3 coordinates go up to 10: the matching tolerance (single
    // precision rounding + GLOBAL_EPSILON) is about 2 * GLOBAL_EPSILON
    constexpr double ROUNDING_TRANSLATION{ geode::GLOBAL_EPSILON };
    constexpr double WRONG_TRANSLATION{ 100 * geode::GLOBAL_EPSILON };
    // Georeferenced model: test_mesh3 barycenters are at least 0.19 apart,
    // scaled to stay farther apart than single precision rounding of
    // typical UTM coordinates (up to 0.28)
    constexpr double UTM_SCALE{ 10. };
    const geode::Vector3D UTM_TRANSLATION{ { 500000., 7000000., 3000. } };
    // Beyond the single precision rounding (about 0.25 here), below the
    // barycenter spacing
    constexpr double UTM_WRONG_TRANSLATION{ 1. };

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

    // One point per model unique vertex, in the order of the Block vertices
    std::unique_ptr< geode::PointSet3D > create_unique_vertex_points(
        const geode::BRep& model,
        std::vector< geode::index_t >& unique_vertices )
    {
        auto vertices = geode::PointSet3D::create();
        auto builder = geode::PointSetBuilder3D::create( *vertices );
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
        return vertices;
    }

    std::unique_ptr< geode::PointSet3D > create_unique_vertices(
        const geode::BRep& model )
    {
        std::vector< geode::index_t > unique_vertices;
        auto vertices = create_unique_vertex_points( model, unique_vertices );
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

    geode::ExternalTimeSeries create_time_series( const geode::BRep& model,
        const geode::uuid& block_id,
        const geode::Vector3D& translation )
    {
        geode::ExternalTimeSeries time_series;
        time_series.set_unique_vertices( create_unique_vertices( model ) );
        time_series.set_cell_barycenters(
            create_cell_barycenters( model, block_id, translation ) );
        return time_series;
    }

    geode::ExternalTimeSeries create_time_series(
        const geode::BRep& model, const geode::uuid& block_id )
    {
        return create_time_series( model, block_id, geode::Vector3D{} );
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

    void check_other_model( const geode::ExternalTimeSeries& time_series )
    {
        const auto other_filename =
            absl::StrCat( geode::DATA_PATH, "layers.og_brep" );
        auto other = geode::load_one(
            geode::load_brep( other_filename ), other_filename );
        bool thrown{ false };
        try
        {
            time_series.apply_to( other );
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
        const auto model_filename =
            absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" );
        auto model = geode::load_one(
            geode::load_brep( model_filename ), model_filename );
        create_time_series(
            model, block_id, geode::Vector3D{ { ROUNDING_TRANSLATION, 0, 0 } } )
            .apply_to( model );
        check_velocities( model, block_id );
        const auto wrong_time_series = create_time_series(
            model, block_id, geode::Vector3D{ { WRONG_TRANSLATION, 0, 0 } } );
        bool thrown{ false };
        try
        {
            wrong_time_series.apply_to( model );
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
        const auto model_filename =
            absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" );
        auto model = geode::load_one(
            geode::load_brep( model_filename ), model_filename );
        auto barycenters =
            create_cell_barycenters( model, block_id, geode::Vector3D{} );
        geode::PointSetBuilder3D::create( *barycenters )
            ->create_point( barycenters->point( 0 ) );
        geode::ExternalTimeSeries time_series;
        time_series.set_cell_barycenters( std::move( barycenters ) );
        bool thrown{ false };
        try
        {
            time_series.apply_to( model );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test(
            thrown, "[Test] Applying colocated barycenters should throw" );
    }

    // Simulators often write single precision coordinates
    geode::Point3D round_to_float( const geode::Point3D& point )
    {
        geode::Point3D rounded;
        for( const auto axis : geode::LRange{ 3 } )
        {
            rounded.set_value(
                axis, static_cast< float >( point.value( axis ) ) );
        }
        return rounded;
    }

    std::unique_ptr< geode::PointSet3D > round_to_float(
        const geode::PointSet3D& point_set )
    {
        auto rounded = point_set.clone();
        auto builder = geode::PointSetBuilder3D::create( *rounded );
        for( const auto vertex : geode::Range{ rounded->nb_vertices() } )
        {
            builder->set_point(
                vertex, round_to_float( rounded->point( vertex ) ) );
        }
        return rounded;
    }

    // Unique vertices without values are not matched, even when they
    // could not be
    void check_unattributed_vertices( const geode::uuid& block_id )
    {
        const auto model_filename =
            absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" );
        auto model = geode::load_one(
            geode::load_brep( model_filename ), model_filename );
        std::vector< geode::index_t > unique_vertices;
        auto vertices = create_unique_vertex_points( model, unique_vertices );
        geode::PointSetBuilder3D::create( *vertices )
            ->create_point( vertices->point( 0 ) );
        geode::ExternalTimeSeries time_series;
        time_series.set_unique_vertices( std::move( vertices ) );
        time_series.set_cell_barycenters(
            create_cell_barycenters( model, block_id, geode::Vector3D{} ) );
        time_series.apply_to( model );
        check_velocities( model, block_id );
    }

    geode::BRep load_georeferenced_model()
    {
        const auto model_filename =
            absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" );
        auto model = geode::load_one(
            geode::load_brep( model_filename ), model_filename );
        geode::BRepBuilder model_builder{ model };
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            const auto mesh_builder = model_builder.block_mesh_builder( block );
            for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
            {
                mesh_builder->set_point( vertex,
                    mesh.point( vertex ) * UTM_SCALE + UTM_TRANSLATION );
            }
        }
        return model;
    }

    // Georeferenced model with only cell values written in single
    // precision: the unique vertices carry no value
    void check_georeferenced_cell_values( const geode::uuid& block_id )
    {
        auto model = load_georeferenced_model();
        std::vector< geode::index_t > unique_vertices;
        const auto vertices =
            create_unique_vertex_points( model, unique_vertices );
        geode::ExternalTimeSeries time_series;
        time_series.set_unique_vertices( round_to_float( *vertices ) );
        time_series.set_cell_barycenters( round_to_float(
            *create_cell_barycenters( model, block_id, geode::Vector3D{} ) ) );
        time_series.apply_to( model );
        check_velocities( model, block_id );
    }

    // Georeferenced model with vertex and cell values written in single
    // precision
    void check_georeferenced_values( const geode::uuid& block_id )
    {
        auto model = load_georeferenced_model();
        geode::ExternalTimeSeries time_series;
        time_series.set_unique_vertices(
            round_to_float( *create_unique_vertices( model ) ) );
        time_series.set_cell_barycenters( round_to_float(
            *create_cell_barycenters( model, block_id, geode::Vector3D{} ) ) );
        time_series.apply_to( model );
        check_pressures( model );
        check_velocities( model, block_id );
    }

    // Points translated beyond the single precision rounding of UTM
    // coordinates are rejected
    void check_georeferenced_tolerance( const geode::uuid& block_id )
    {
        auto model = load_georeferenced_model();
        geode::ExternalTimeSeries time_series;
        time_series.set_cell_barycenters(
            round_to_float( *create_cell_barycenters( model, block_id,
                geode::Vector3D{ { 0, UTM_WRONG_TRANSLATION, 0 } } ) ) );
        bool thrown{ false };
        try
        {
            time_series.apply_to( model );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test( thrown,
            "[Test] Applying georeferenced barycenters translated beyond the "
            "tolerance should throw" );
    }

    void check_null_point_set()
    {
        geode::ExternalTimeSeries time_series;
        bool thrown{ false };
        try
        {
            time_series.set_cell_barycenters( nullptr );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeModelException::test(
            thrown, "[Test] Setting a null point set should throw" );
    }

    void check_wrong_extension( const geode::ExternalTimeSeries& time_series )
    {
        bool thrown{ false };
        try
        {
            geode::save_external_time_series(
                time_series, "test_external_time_series.txt" );
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
    const auto model_filename =
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" );
    auto model =
        geode::load_one( geode::load_brep( model_filename ), model_filename );
    geode::OpenGeodeModelException::test(
        model.nb_blocks() > 0, "[Test] Model has no Block" );
    const auto block_id = ( *model.blocks().begin() ).id();
    const auto filename = absl::StrCat( "test_external_time_series.",
        geode::ExternalTimeSeries::native_extension_static() );
    geode::save_external_time_series(
        create_time_series( model, block_id ), filename );
    const auto time_series = geode::load_external_time_series( filename );

    time_series.apply_to( model );
    check_pressures( model );
    check_velocities( model, block_id );
    create_time_series( model, block_id ).apply_to( model );
    check_pressures( model );
    check_velocities( model, block_id );

    const auto reloaded_model_filename =
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" );
    auto reloaded_model = geode::load_one(
        geode::load_brep( reloaded_model_filename ), reloaded_model_filename );
    geode::load_brep_time_series( reloaded_model, filename );
    check_pressures( reloaded_model );
    check_velocities( reloaded_model, block_id );
    check_other_model( time_series );
    check_tolerance( block_id );
    check_colocated_points( block_id );
    check_unattributed_vertices( block_id );
    check_georeferenced_cell_values( block_id );
    check_georeferenced_values( block_id );
    check_georeferenced_tolerance( block_id );
    check_null_point_set();
    check_wrong_extension( time_series );
}

OPENGEODE_TEST( "external-time-series" )
