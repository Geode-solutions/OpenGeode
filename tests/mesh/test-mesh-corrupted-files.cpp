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

#include <cstdint>
#include <fstream>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <string>

#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/coordinate_reference_system_managers_builder.hpp>
#include <geode/mesh/builder/graph_builder.hpp>
#include <geode/mesh/builder/hybrid_solid_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/polygonal_surface_builder.hpp>
#include <geode/mesh/builder/polyhedral_solid_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/texture2d.hpp>
#include <geode/mesh/core/texture_manager.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/edged_curve_input.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/graph_output.hpp>
#include <geode/mesh/io/hybrid_solid_input.hpp>
#include <geode/mesh/io/hybrid_solid_output.hpp>
#include <geode/mesh/io/light_regular_grid_input.hpp>
#include <geode/mesh/io/light_regular_grid_output.hpp>
#include <geode/mesh/io/point_set_input.hpp>
#include <geode/mesh/io/point_set_output.hpp>
#include <geode/mesh/io/polygonal_surface_input.hpp>
#include <geode/mesh/io/polygonal_surface_output.hpp>
#include <geode/mesh/io/polyhedral_solid_input.hpp>
#include <geode/mesh/io/polyhedral_solid_output.hpp>
#include <geode/mesh/io/regular_grid_input.hpp>
#include <geode/mesh/io/tetrahedral_solid_input.hpp>
#include <geode/mesh/io/tetrahedral_solid_output.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>
#include <geode/mesh/io/vertex_set_input.hpp>

#include <geode/tests/common.hpp>

namespace
{
    using Loader = std::function< void( std::string_view ) >;

    /*
     * Serialized values, as written by bitsery: little endian integers,
     * sizes and Growable versions are compact values (one byte below 128).
     */
    class Bytes
    {
    public:
        Bytes& u8( std::uint8_t value )
        {
            bytes_.push_back( static_cast< char >( value ) );
            return *this;
        }

        Bytes& u32( std::uint32_t value )
        {
            for( const auto i : geode::LRange{ 4 } )
            {
                u8( static_cast< std::uint8_t >( value >> ( 8 * i ) ) );
            }
            return *this;
        }

        Bytes& size( std::size_t value )
        {
            geode::OpenGeodeMeshException::test(
                value < 128, "[Test] Only small sizes are supported" );
            return u8( static_cast< std::uint8_t >( value ) );
        }

        Bytes& u32s( absl::Span< const std::uint32_t > values )
        {
            for( const auto value : values )
            {
                u32( value );
            }
            return *this;
        }

        Bytes& append( const Bytes& other )
        {
            bytes_ += other.bytes_;
            return *this;
        }

        /* Growable element of two fields: index_t and local_index_t */
        Bytes& element( geode::index_t id, geode::local_index_t local_id )
        {
            return u8( 2 ).u32( id ).u8( local_id );
        }

        const std::string& string() const
        {
            return bytes_;
        }

    private:
        std::string bytes_;
    };

    std::string read_file( std::string_view filename )
    {
        std::ifstream file{ geode::to_string( filename ),
            std::ifstream::binary };
        return { std::istreambuf_iterator< char >{ file },
            std::istreambuf_iterator< char >{} };
    }

    /*
     * Replace the unique occurrence of the given bytes in the file and check
     * that loading the modified file throws an OpenGeodeException.
     */
    void test_corrupted_file( std::string_view filename,
        const Bytes& from,
        const Bytes& to,
        const Loader& loader )
    {
        loader( filename );
        auto content = read_file( filename );
        const auto position = content.find( from.string() );
        geode::OpenGeodeMeshException::test( position != std::string::npos,
            "[Test] Bytes to corrupt not found in ", filename );
        geode::OpenGeodeMeshException::test(
            content.find( from.string(), position + 1 ) == std::string::npos,
            "[Test] Bytes to corrupt are not unique in ", filename );
        content.replace( position, from.string().size(), to.string() );
        const auto corrupted = absl::StrCat( "corrupted_", filename );
        {
            std::ofstream file{ corrupted, std::ofstream::binary };
            file << content;
        }
        try
        {
            loader( corrupted );
        }
        catch( const geode::OpenGeodeException& exception )
        {
            geode::Logger::info( "Corrupted ", filename,
                " throws as expected: ", exception.what() );
            return;
        }
        throw geode::OpenGeodeMeshException{ nullptr,
            geode::OpenGeodeException::TYPE::internal, "[Test] Corrupted ",
            filename, " should not be loaded" };
    }

    template < typename Builder >
    void create_points( Builder& builder, geode::index_t nb_points )
    {
        for( const auto p : geode::Range{ nb_points } )
        {
            builder.create_point(
                geode::Point3D{ { p * 1.5, p * 2., p * 3. } } );
        }
    }

    template < typename Mesh >
    Bytes polygon_around_vertex( const Mesh& mesh, geode::index_t nb_vertices )
    {
        Bytes bytes;
        for( const auto v : geode::Range{ nb_vertices } )
        {
            const auto polygon_vertex = mesh.polygon_around_vertex( v ).value();
            bytes.element(
                polygon_vertex.polygon_id, polygon_vertex.vertex_id );
        }
        return bytes;
    }

    template < typename Mesh >
    Bytes polyhedron_around_vertex(
        const Mesh& mesh, geode::index_t nb_vertices )
    {
        Bytes bytes;
        for( const auto v : geode::Range{ nb_vertices } )
        {
            const auto polyhedron_vertex =
                mesh.polyhedron_around_vertex( v ).value();
            bytes.element(
                polyhedron_vertex.polyhedron_id, polyhedron_vertex.vertex_id );
        }
        return bytes;
    }

    void test_triangulated_surface()
    {
        auto surface = geode::TriangulatedSurface3D::create();
        auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
        create_points( *builder, 4 );
        builder->create_triangle( { 0, 1, 2 } );
        builder->create_triangle( { 1, 3, 2 } );
        const auto filename =
            absl::StrCat( "corrupt.", surface->native_extension() );
        geode::save_triangulated_surface( *surface, filename );
        const Loader loader = []( std::string_view file ) {
            geode_unused( geode::load_triangulated_surface< 3 >( file ) );
        };
        // Triangle vertices of one triangle only
        test_corrupted_file( filename,
            Bytes{}.size( 2 ).u32s( { 0, 1, 2, 1, 3, 2 } ),
            Bytes{}.size( 1 ).u32s( { 0, 1, 2 } ), loader );
        // Polygon around vertex of 3 vertices only
        test_corrupted_file( filename,
            Bytes{}.size( 4 ).append( polygon_around_vertex( *surface, 4 ) ),
            Bytes{}.size( 3 ).append( polygon_around_vertex( *surface, 3 ) ),
            loader );
    }

    void test_polygonal_surface()
    {
        auto surface = geode::PolygonalSurface3D::create();
        auto builder = geode::PolygonalSurfaceBuilder3D::create( *surface );
        create_points( *builder, 5 );
        builder->create_polygon( { 0, 1, 2, 3 } );
        builder->create_polygon( { 1, 4, 2 } );
        const auto filename =
            absl::StrCat( "corrupt.", surface->native_extension() );
        geode::save_polygonal_surface( *surface, filename );
        const Loader loader = []( std::string_view file ) {
            geode_unused( geode::load_polygonal_surface< 3 >( file ) );
        };
        // Polygon pointers of one polygon only
        test_corrupted_file( filename, Bytes{}.size( 3 ).u32s( { 0, 4, 7 } ),
            Bytes{}.size( 2 ).u32s( { 0, 4 } ), loader );
        // Polygon vertices of one polygon only
        test_corrupted_file( filename,
            Bytes{}.size( 7 ).u32s( { 0, 1, 2, 3, 1, 4, 2 } ),
            Bytes{}.size( 4 ).u32s( { 0, 1, 2, 3 } ), loader );
    }

    void test_tetrahedral_solid()
    {
        auto solid = geode::TetrahedralSolid3D::create();
        auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
        create_points( *builder, 5 );
        builder->create_tetrahedron( { 0, 1, 2, 3 } );
        builder->create_tetrahedron( { 1, 2, 3, 4 } );
        const auto filename =
            absl::StrCat( "corrupt.", solid->native_extension() );
        geode::save_tetrahedral_solid( *solid, filename );
        const Loader loader = []( std::string_view file ) {
            geode_unused( geode::load_tetrahedral_solid< 3 >( file ) );
        };
        // Tetrahedron vertices of one tetrahedron only
        test_corrupted_file( filename,
            Bytes{}.size( 2 ).u32s( { 0, 1, 2, 3, 1, 2, 3, 4 } ),
            Bytes{}.size( 1 ).u32s( { 0, 1, 2, 3 } ), loader );
        // Polyhedron around vertex of 4 vertices only
        test_corrupted_file( filename,
            Bytes{}.size( 5 ).append( polyhedron_around_vertex( *solid, 5 ) ),
            Bytes{}.size( 4 ).append( polyhedron_around_vertex( *solid, 4 ) ),
            loader );
    }

    void test_polyhedral_solid()
    {
        auto solid = geode::PolyhedralSolid3D::create();
        auto builder = geode::PolyhedralSolidBuilder3D::create( *solid );
        create_points( *builder, 8 );
        builder->create_polyhedron( { 0, 1, 2, 3 },
            { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
        builder->create_polyhedron(
            { 1, 2, 3, 5, 6, 7 }, { { 0, 1, 2 }, { 3, 5, 4 }, { 0, 3, 4, 1 },
                                      { 1, 4, 5, 2 }, { 2, 5, 3, 0 } } );
        const auto filename =
            absl::StrCat( "corrupt.", solid->native_extension() );
        geode::save_polyhedral_solid( *solid, filename );
        const Loader loader = []( std::string_view file ) {
            geode_unused( geode::load_polyhedral_solid< 3 >( file ) );
        };
        // Polyhedron vertex pointers of one polyhedron only
        test_corrupted_file( filename, Bytes{}.size( 3 ).u32s( { 0, 4, 10 } ),
            Bytes{}.size( 2 ).u32s( { 0, 4 } ), loader );
        // Polyhedron adjacent pointers of one polyhedron only
        test_corrupted_file( filename, Bytes{}.size( 3 ).u32s( { 0, 4, 9 } ),
            Bytes{}.size( 2 ).u32s( { 0, 4 } ), loader );
    }

    void test_hybrid_solid()
    {
        auto solid = geode::HybridSolid3D::create();
        auto builder = geode::HybridSolidBuilder3D::create( *solid );
        create_points( *builder, 8 );
        builder->create_tetrahedron( { 0, 1, 2, 3 } );
        builder->create_prism( { 1, 2, 3, 5, 6, 7 } );
        const auto filename =
            absl::StrCat( "corrupt.", solid->native_extension() );
        geode::save_hybrid_solid( *solid, filename );
        const Loader loader = []( std::string_view file ) {
            geode_unused( geode::load_hybrid_solid< 3 >( file ) );
        };
        // Polyhedron vertex pointers of one polyhedron only
        test_corrupted_file( filename, Bytes{}.size( 3 ).u32s( { 0, 4, 10 } ),
            Bytes{}.size( 2 ).u32s( { 0, 4 } ), loader );
        // Polyhedron adjacent pointers of one polyhedron only
        test_corrupted_file( filename, Bytes{}.size( 3 ).u32s( { 0, 4, 9 } ),
            Bytes{}.size( 2 ).u32s( { 0, 4 } ), loader );
    }

    void test_graph()
    {
        auto graph = geode::Graph::create();
        auto builder = geode::GraphBuilder::create( *graph );
        builder->create_vertices( 3 );
        builder->create_edge( 0, 1 );
        builder->create_edge( 1, 2 );
        const auto filename =
            absl::StrCat( "corrupt.", graph->native_extension() );
        geode::save_graph( *graph, filename );
        const Loader loader = []( std::string_view file ) {
            geode_unused( geode::load_graph( file ) );
        };
        // Edge vertices of one edge only
        test_corrupted_file( filename, Bytes{}.size( 2 ).u32s( { 0, 1, 1, 2 } ),
            Bytes{}.size( 1 ).u32s( { 0, 1 } ), loader );
        // Edges around vertex of 2 vertices only
        Bytes around;
        for( const auto v : geode::Range{ 2 } )
        {
            const auto& edges = graph->edges_around_vertex( v );
            around.size( edges.size() );
            for( const auto& edge_vertex : edges )
            {
                around.element( edge_vertex.edge_id, edge_vertex.vertex_id );
            }
        }
        test_corrupted_file( filename,
            Bytes{}.size( 3 ).append( around ).size( 1 ).element( 1, 1 ),
            Bytes{}.size( 2 ).append( around ), loader );
    }

    void test_light_regular_grid()
    {
        const geode::LightRegularGrid3D grid{ geode::Point3D{ { 1.5, 0, 1 } },
            { 2, 3, 4 }, { 1, 2, 3 } };
        const auto filename =
            absl::StrCat( "corrupt.", grid.native_extension() );
        geode::save_light_regular_grid( grid, filename );
        const Loader loader = []( std::string_view file ) {
            geode_unused( geode::load_light_regular_grid< 3 >( file ) );
        };
        // Cell attribute manager with 23 elements instead of 24
        test_corrupted_file( filename, Bytes{}.u8( 2 ).u32( 24 ).size( 0 ),
            Bytes{}.u8( 2 ).u32( 23 ).size( 0 ), loader );
        // Vertex attribute manager with 59 elements instead of 60
        test_corrupted_file( filename, Bytes{}.u8( 2 ).u32( 60 ).size( 0 ),
            Bytes{}.u8( 2 ).u32( 59 ).size( 0 ), loader );
        // No cell in one direction
        test_corrupted_file( filename, Bytes{}.u32s( { 2, 3, 4 } ),
            Bytes{}.u32s( { 0, 3, 4 } ), loader );
        // Too many grid cells
        test_corrupted_file( filename, Bytes{}.u32s( { 2, 3, 4 } ),
            Bytes{}.u32s( { 65536, 65536, 1 } ), loader );
        // Too many grid vertices
        test_corrupted_file( filename, Bytes{}.u32s( { 2, 3, 4 } ),
            Bytes{}.u32s( { 65535, 65535, 1 } ), loader );
    }

    void test_legacy_grid_without_cells_number()
    {
        // In v12 files, Grid version 1 relies on the cells number stored by
        // Grid::Impl version 1: a Grid::Impl version 2 does not provide it
        const auto filename = absl::StrCat(
            geode::DATA_PATH, "backward_io/v12/test_v12.og_rgd3d" );
        auto content = read_file( filename );
        const auto from = Bytes{}.u8( 1 ).u32s( { 5, 10, 15 } ).string();
        const auto position = content.find( from );
        geode::OpenGeodeMeshException::test(
            position != std::string::npos, "[Test] Cells number not found" );
        content.replace( position, from.size(), Bytes{}.u8( 2 ).string() );
        const std::string corrupted{ "corrupted_v12.og_rgd3d" };
        {
            std::ofstream file{ corrupted, std::ofstream::binary };
            file << content;
        }
        try
        {
            const auto grid = geode::load_regular_grid< 3 >( corrupted );
            geode::Logger::error( "Grid loaded with cells ",
                grid->nb_cells_in_direction( 0 ), " ",
                grid->nb_cells_in_direction( 1 ), " ",
                grid->nb_cells_in_direction( 2 ) );
        }
        catch( const geode::OpenGeodeException& exception )
        {
            geode::Logger::info(
                corrupted, " throws as expected: ", exception.what() );
            return;
        }
        throw geode::OpenGeodeMeshException{ nullptr,
            geode::OpenGeodeException::TYPE::internal, "[Test] ", corrupted,
            " should not be loaded" };
    }

    template < typename Loader >
    void expect_load_failure( std::string_view filename, const Loader& loader )
    {
        try
        {
            loader( filename );
        }
        catch( const geode::OpenGeodeException& exception )
        {
            geode::Logger::info(
                filename, " throws as expected: ", exception.what() );
            return;
        }
        throw geode::OpenGeodeMeshException{ nullptr,
            geode::OpenGeodeException::TYPE::internal, "[Test] ", filename,
            " should not be loaded" };
    }

    std::unique_ptr< geode::PointSet3D > create_point_set_with_crs(
        std::string_view crs_name )
    {
        auto point_set = geode::PointSet3D::create();
        auto builder = geode::PointSetBuilder3D::create( *point_set );
        create_points( *builder, 3 );
        auto crs_builder =
            geode::CoordinateReferenceSystemManagersBuilder3D{ *point_set }
                .main_coordinate_reference_system_manager_builder();
        crs_builder.register_coordinate_reference_system( crs_name,
            std::make_shared< geode::AttributeCoordinateReferenceSystem3D >(
                point_set->vertex_attribute_manager(), "other_coordinates" ) );
        crs_builder.set_active_coordinate_reference_system( crs_name );
        return point_set;
    }

    void test_active_crs()
    {
        const std::string crs_name{ "ACTIVE_CRS_NAME" };
        const auto point_set = create_point_set_with_crs( crs_name );
        const auto filename =
            absl::StrCat( "active_crs.", point_set->native_extension() );
        geode::save_point_set( *point_set, filename );
        const auto loader = []( std::string_view file ) {
            geode_unused( geode::load_point_set< 3 >( file ) );
        };
        loader( filename );
        // Rename the active CRS (its name is written last) to an unknown one
        auto content = read_file( filename );
        const auto position = content.rfind( crs_name );
        geode::OpenGeodeMeshException::test(
            position != std::string::npos, "[Test] Active CRS name not found" );
        content[position + crs_name.size() - 1] = 'X';
        const auto corrupted = absl::StrCat( "corrupted_", filename );
        {
            std::ofstream file{ corrupted, std::ofstream::binary };
            file << content;
        }
        expect_load_failure( corrupted, loader );
    }

    /*
     * Saving a name longer than MAX_SERIALIZED_NAME_SIZE is not possible
     * (bitsery asserts on write), so a short name is saved and its size
     * prefix is patched above the limit.
     */
    template < typename Loader >
    void test_too_long_name(
        std::string_view filename, std::string_view name, const Loader& loader )
    {
        loader( filename );
        auto content = read_file( filename );
        const auto from =
            absl::StrCat( Bytes{}.size( name.size() ).string(), name );
        const auto position = content.find( from );
        geode::OpenGeodeMeshException::test( position != std::string::npos,
            "[Test] Name ", name, " not found in ", filename );
        // bitsery 4-byte size encoding: two high bytes, then a little endian
        // 16-bit word
        constexpr auto TOO_LONG = geode::MAX_SERIALIZED_NAME_SIZE + 1;
        const auto too_long_size =
            Bytes{}
                .u8( static_cast< std::uint8_t >( 0xC0u | ( TOO_LONG >> 24 ) ) )
                .u8( static_cast< std::uint8_t >( TOO_LONG >> 16 ) )
                .u8( static_cast< std::uint8_t >( TOO_LONG ) )
                .u8( static_cast< std::uint8_t >( TOO_LONG >> 8 ) );
        content.replace( position, 1, too_long_size.string() );
        const auto corrupted = absl::StrCat( "corrupted_", filename );
        {
            std::ofstream file{ corrupted, std::ofstream::binary };
            file << content;
        }
        expect_load_failure( corrupted, loader );
    }

    void test_long_names()
    {
        const std::string crs_name{ "LONG_CRS_NAME" };
        const auto point_set = create_point_set_with_crs( crs_name );
        const auto filename =
            absl::StrCat( "long_crs_name.", point_set->native_extension() );
        geode::save_point_set( *point_set, filename );
        test_too_long_name( filename, crs_name, []( std::string_view file ) {
            geode_unused( geode::load_point_set< 3 >( file ) );
        } );

        auto surface = geode::TriangulatedSurface2D::create();
        auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
        builder->create_point( geode::Point2D{ { 0, 0 } } );
        builder->create_point( geode::Point2D{ { 1, 0 } } );
        builder->create_point( geode::Point2D{ { 0, 1 } } );
        builder->create_triangle( { 0, 1, 2 } );
        const std::string texture_name{ "LONG_TEXTURE_NAME" };
        auto manager = surface->texture_manager();
        manager.find_or_create_texture( texture_name );
        const auto surface_filename =
            absl::StrCat( "long_texture_name.", surface->native_extension() );
        geode::save_triangulated_surface( *surface, surface_filename );
        test_too_long_name(
            surface_filename, texture_name, []( std::string_view file ) {
                geode_unused( geode::load_triangulated_surface< 2 >( file ) );
            } );
    }

    template < typename Base, typename... Args >
    class ThrowingInput final : public Base
    {
    public:
        explicit ThrowingInput( std::string_view filename ) : Base{ filename }
        {
        }

        typename Base::InputData read( const Args&... /*unused*/ ) override
        {
            throw std::runtime_error{ "[Test] Not an OpenGeodeException" };
        }

        geode::AdditionalFiles additional_files() const override
        {
            return {};
        }

        geode::index_t object_priority() const override
        {
            return 0;
        }

        geode::Percentage is_loadable() const override
        {
            return geode::Percentage{ 1 };
        }
    };

    template < typename Factory, typename Input, typename Loader >
    void test_std_exception( const Loader& loader )
    {
        const std::string extension{ "throwing_input" };
        Factory::template register_creator< Input >( extension );
        const auto filename = absl::StrCat( "std_exception.", extension );
        {
            std::ofstream file{ filename };
            file << "content";
        }
        expect_load_failure( filename, loader );
    }

    void test_std_exceptions()
    {
        using geode::MeshImpl;
        test_std_exception< geode::VertexSetInputFactory,
            ThrowingInput< geode::VertexSetInput, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_vertex_set( file ) );
            } );
        test_std_exception< geode::PointSetInputFactory3D,
            ThrowingInput< geode::PointSetInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_point_set< 3 >( file ) );
            } );
        test_std_exception< geode::GraphInputFactory,
            ThrowingInput< geode::GraphInput, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_graph( file ) );
            } );
        test_std_exception< geode::EdgedCurveInputFactory3D,
            ThrowingInput< geode::EdgedCurveInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_edged_curve< 3 >( file ) );
            } );
        test_std_exception< geode::PolygonalSurfaceInputFactory3D,
            ThrowingInput< geode::PolygonalSurfaceInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_polygonal_surface< 3 >( file ) );
            } );
        test_std_exception< geode::TriangulatedSurfaceInputFactory3D,
            ThrowingInput< geode::TriangulatedSurfaceInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_triangulated_surface< 3 >( file ) );
            } );
        test_std_exception< geode::PolyhedralSolidInputFactory3D,
            ThrowingInput< geode::PolyhedralSolidInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_polyhedral_solid< 3 >( file ) );
            } );
        test_std_exception< geode::TetrahedralSolidInputFactory3D,
            ThrowingInput< geode::TetrahedralSolidInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_tetrahedral_solid< 3 >( file ) );
            } );
        test_std_exception< geode::HybridSolidInputFactory3D,
            ThrowingInput< geode::HybridSolidInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_hybrid_solid< 3 >( file ) );
            } );
        test_std_exception< geode::RegularGridInputFactory3D,
            ThrowingInput< geode::RegularGridInput3D, MeshImpl > >(
            []( std::string_view file ) {
                geode_unused( geode::load_regular_grid< 3 >( file ) );
            } );
        test_std_exception< geode::LightRegularGridInputFactory3D,
            ThrowingInput< geode::LightRegularGridInput3D > >(
            []( std::string_view file ) {
                geode_unused( geode::load_light_regular_grid< 3 >( file ) );
            } );
    }

    void test_legacy_file_without_points()
    {
        // Rename the legacy vertex coordinates attribute
        auto content = read_file( absl::StrCat(
            geode::DATA_PATH, "backward_io/v12/test_v12.og_edc3d" ) );
        const auto position = content.find( "points" );
        geode::OpenGeodeMeshException::test( position != std::string::npos,
            "[Test] Points attribute not found" );
        content[position + 5] = 'z';
        const std::string corrupted{ "corrupted_v12.og_edc3d" };
        {
            std::ofstream file{ corrupted, std::ofstream::binary };
            file << content;
        }
        expect_load_failure( corrupted, []( std::string_view file ) {
            geode_unused( geode::load_edged_curve< 3 >( file ) );
        } );
    }
} // namespace

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_triangulated_surface();
    test_polygonal_surface();
    test_tetrahedral_solid();
    test_polyhedral_solid();
    test_hybrid_solid();
    test_graph();
    test_light_regular_grid();
    test_legacy_grid_without_cells_number();
    test_long_names();
    test_active_crs();
    test_legacy_file_without_points();
    test_std_exceptions();
}

OPENGEODE_TEST( "mesh-corrupted-files" )
