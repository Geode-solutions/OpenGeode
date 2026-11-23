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

#include <ctime>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/detail/file_content.hpp>
#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/detail/input_files.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

#include <geode/image/core/raster_image.hpp>
#include <geode/image/core/rgb_color.hpp>
#include <geode/image/io/raster_image_input.hpp>
#include <geode/image/io/raster_image_output.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/geode/geode_edged_curve.hpp>
#include <geode/mesh/core/geode/geode_graph.hpp>
#include <geode/mesh/core/geode/geode_hybrid_solid.hpp>
#include <geode/mesh/core/geode/geode_point_set.hpp>
#include <geode/mesh/core/geode/geode_polygonal_surface.hpp>
#include <geode/mesh/core/geode/geode_polyhedral_solid.hpp>
#include <geode/mesh/core/geode/geode_regular_grid_solid.hpp>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.hpp>
#include <geode/mesh/core/geode/geode_triangulated_surface.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/io/edged_curve_input.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/hybrid_solid_input.hpp>
#include <geode/mesh/io/light_regular_grid_input.hpp>
#include <geode/mesh/io/point_set_input.hpp>
#include <geode/mesh/io/polygonal_surface_input.hpp>
#include <geode/mesh/io/polyhedral_solid_input.hpp>
#include <geode/mesh/io/regular_grid_input.hpp>
#include <geode/mesh/io/tetrahedral_solid_input.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

#include <geode/tests/common.hpp>

namespace
{
    template < typename Mesh, typename = void >
    struct HasPoints : std::false_type
    {
    };

    template < typename Mesh >
    struct HasPoints< Mesh,
        std::void_t< decltype( std::declval< const Mesh& >().point( 0 ) ) > >
        : std::true_type
    {
    };

    std::string describe_attributes( const geode::AttributeManager& manager )
    {
        std::vector< std::string > ids;
        for( const auto& id : manager.attribute_ids() )
        {
            // Attributes of old files get a new uuid at each load
            ids.push_back( absl::StrCat( manager.attribute_type( id ), ":",
                manager.find_generic_attribute( id )->name().value_or( "" ) ) );
        }
        absl::c_sort( ids );
        return absl::StrCat(
            manager.nb_elements(), "[", absl::StrJoin( ids, "," ), "]" );
    }

    /*!
     * Text describing a mesh: its vertices, elements and attributes (not
     * its identifier: meshes of old files get a new one at each load).
     */
    template < typename Mesh >
    std::string describe( const Mesh& mesh )
    {
        auto text = absl::StrCat( mesh.nb_vertices(), " vertices ",
            describe_attributes( mesh.vertex_attribute_manager() ) );
        if constexpr( HasPoints< Mesh >::value )
        {
            for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
            {
                absl::StrAppend( &text, " ", mesh.point( vertex ).string() );
            }
        }
        if constexpr( std::is_base_of_v< geode::Graph, Mesh > )
        {
            absl::StrAppend( &text, " edges ",
                describe_attributes( mesh.edge_attribute_manager() ) );
            for( const auto edge : geode::Range{ mesh.nb_edges() } )
            {
                absl::StrAppend( &text, " ", mesh.edge_vertex( { edge, 0 } ),
                    "-", mesh.edge_vertex( { edge, 1 } ) );
            }
        }
        if constexpr( std::is_base_of_v< geode::SurfaceMesh< 2 >, Mesh >
                      || std::is_base_of_v< geode::SurfaceMesh< 3 >, Mesh > )
        {
            absl::StrAppend( &text, " polygons ",
                describe_attributes( mesh.polygon_attribute_manager() ) );
            for( const auto polygon : geode::Range{ mesh.nb_polygons() } )
            {
                absl::StrAppend( &text, " (" );
                for( const auto v :
                    geode::LRange{ mesh.nb_polygon_vertices( polygon ) } )
                {
                    absl::StrAppend(
                        &text, mesh.polygon_vertex( { polygon, v } ), "," );
                }
                absl::StrAppend( &text, ")" );
            }
        }
        if constexpr( std::is_base_of_v< geode::SolidMesh< 3 >, Mesh > )
        {
            absl::StrAppend( &text, " polyhedra ",
                describe_attributes( mesh.polyhedron_attribute_manager() ) );
            for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
            {
                absl::StrAppend( &text, " (" );
                for( const auto v :
                    geode::LRange{ mesh.nb_polyhedron_vertices( polyhedron ) } )
                {
                    absl::StrAppend( &text,
                        mesh.polyhedron_vertex( { polyhedron, v } ), "," );
                }
                absl::StrAppend( &text, ")" );
            }
        }
        return text;
    }

    template < geode::index_t dimension >
    std::string describe_grid(
        const geode::LightRegularGrid< dimension >& grid )
    {
        auto text = absl::StrCat( grid.nb_cells(), " cells ",
            grid.grid_point( {} ).string(), " ",
            describe_attributes( grid.cell_attribute_manager() ),
            describe_attributes( grid.grid_vertex_attribute_manager() ) );
        for( const auto direction : geode::LRange{ dimension } )
        {
            absl::StrAppend( &text, " ",
                grid.nb_cells_in_direction( direction ), "x",
                grid.cell_length_in_direction( direction ) );
        }
        return text;
    }

    std::string describe_image( const geode::RasterImage2D& image )
    {
        auto text = absl::StrCat( image.nb_cells(), " cells" );
        for( const auto cell : geode::Range{ image.nb_cells() } )
        {
            absl::StrAppend( &text, " ", image.color( cell ).string() );
        }
        return text;
    }

    std::string data_file( std::string_view name )
    {
        return absl::StrCat( geode::DATA_PATH, name );
    }

    geode::detail::FileContent read_whole_file( std::string_view filename )
    {
        auto content = geode::detail::map_file_content( filename );
        geode::OpenGeodeMeshException::test(
            content.has_value(), "Cannot read ", filename );
        return std::move( content.value() );
    }

    /*!
     * Read a native mesh file by path, from its bytes, and through the
     * input files of its directory: the three meshes must be identical.
     */
    template < typename OpenGeodeMesh, typename Factory >
    void check_mesh( std::string_view filename )
    {
        const auto impl = OpenGeodeMesh::impl_name_static();
        const auto extension = geode::detail::lowercase_extension( filename );
        const auto by_path = geode::load_one(
            Factory::create( extension, filename )->read( impl ), filename );
        const auto reference = describe( *by_path );

        const auto bytes = read_whole_file( filename );
        auto from_bytes = Factory::create( extension, "name_only" )
                              ->read_bytes( bytes.span(), impl );
        geode::OpenGeodeMeshException::test( from_bytes.has_value(),
            "Native input of ", filename, " cannot read bytes" );
        geode::OpenGeodeMeshException::test(
            describe(
                *geode::load_one( std::move( from_bytes.value() ), filename ) )
                == reference,
            "Mesh read from bytes differs from the file ", filename );

        const std::filesystem::path path{ filename };
        const geode::detail::DirectoryInputFiles files{
            path.parent_path().u8string()
        };
        const auto from_files = geode::load_one(
            geode::detail::geode_object_input_from_files< Factory >(
                "Mesh", files, path.filename().u8string(), impl ),
            filename );
        geode::OpenGeodeMeshException::test(
            describe( *from_files ) == reference,
            "Mesh read from input files differs from the file ", filename );
    }

    template < typename Object, typename Factory, typename Describe >
    void check_object( std::string_view filename, const Describe& describe )
    {
        const auto extension = geode::detail::lowercase_extension( filename );
        const auto by_path = geode::load_one(
            Factory::create( extension, filename )->read(), filename );
        const auto bytes = read_whole_file( filename );
        auto from_bytes = Factory::create( extension, "name_only" )
                              ->read_bytes( bytes.span() );
        geode::OpenGeodeMeshException::test( from_bytes.has_value(),
            "Native input of ", filename, " cannot read bytes" );
        geode::OpenGeodeMeshException::test(
            describe(
                geode::load_one( std::move( from_bytes.value() ), filename ) )
                == describe( by_path ),
            "Object read from bytes differs from the file ", filename );
    }

    void test_native_meshes()
    {
        check_mesh< geode::OpenGeodeTriangulatedSurface3D,
            geode::TriangulatedSurfaceInputFactory3D >(
            data_file( "triangle.og_tsf3d" ) );
        check_mesh< geode::OpenGeodeTriangulatedSurface2D,
            geode::TriangulatedSurfaceInputFactory2D >(
            data_file( "3patches.og_tsf2d" ) );
        check_mesh< geode::OpenGeodeTetrahedralSolid3D,
            geode::TetrahedralSolidInputFactory3D >(
            data_file( "backward_io/v18_5/v18_5.og_tso3d" ) );
        check_mesh< geode::OpenGeodePolygonalSurface3D,
            geode::PolygonalSurfaceInputFactory3D >(
            data_file( "backward_io/v18_5/v18_5.og_psf3d" ) );
        check_mesh< geode::OpenGeodePolyhedralSolid3D,
            geode::PolyhedralSolidInputFactory3D >(
            data_file( "backward_io/v17/v17.og_pso3d" ) );
        check_mesh< geode::OpenGeodeHybridSolid3D,
            geode::HybridSolidInputFactory3D >(
            data_file( "backward_io/v17/v17.og_hso3d" ) );
        check_mesh< geode::OpenGeodeEdgedCurve3D,
            geode::EdgedCurveInputFactory3D >(
            data_file( "backward_io/v18_5/v18_5.og_edc3d" ) );
        check_mesh< geode::OpenGeodePointSet3D, geode::PointSetInputFactory3D >(
            data_file( "backward_io/v17/v17.og_pts3d" ) );
        check_mesh< geode::OpenGeodeRegularGrid3D,
            geode::RegularGridInputFactory3D >(
            data_file( "backward_io/v17/v17.og_rgd3d" ) );
        check_mesh< geode::OpenGeodeGraph, geode::GraphInputFactory >(
            data_file( "backward_io/v18_5/v18_5.og_grp" ) );
        check_object< geode::LightRegularGrid3D,
            geode::LightRegularGridInputFactory3D >(
            data_file( "backward_io/v19_8/v19_8.og_lrgd3d" ),
            describe_grid< 3 > );
        check_object< geode::LightRegularGrid2D,
            geode::LightRegularGridInputFactory2D >(
            data_file( "backward_io/v19_8/v19_8.og_lrgd2d" ),
            describe_grid< 2 > );
    }

    void test_raster_image()
    {
        geode::RasterImage2D image{ { 3, 2 } };
        for( const auto cell : geode::Range{ image.nb_cells() } )
        {
            image.set_color(
                cell, geode::RGBColor{
                          static_cast< geode::local_index_t >( cell ), 2, 3 } );
        }
        const std::string filename{ "bytes_raster.og_img2d" };
        geode::save_raster_image( image, filename );
        check_object< geode::RasterImage2D, geode::RasterImageInputFactory2D >(
            filename, describe_image );
        std::filesystem::remove( filename );
    }

    /*!
     * Input reading only files, to check the fallback of the input files.
     */
    class PathOnlyInput : public geode::TriangulatedSurfaceInput< 3 >
    {
    public:
        explicit PathOnlyInput( std::string_view filename )
            : geode::TriangulatedSurfaceInput< 3 >( filename )
        {
        }

        std::vector< std::unique_ptr< geode::TriangulatedSurface3D > > read(
            const geode::MeshImpl& impl ) final
        {
            read_paths().emplace_back( filename() );
            geode::OpenGeodeMeshException::test(
                std::filesystem::exists(
                    std::filesystem::u8path( geode::to_string( filename() ) ) ),
                "[PathOnlyInput] File ", filename(), " does not exist" );
            const auto bytes = read_whole_file( filename() );
            auto meshes = geode::TriangulatedSurfaceInputFactory3D::create(
                "og_tsf3d", filename() )
                              ->read_bytes( bytes.span(), impl );
            return std::move( meshes.value() );
        }

        geode::AdditionalFiles additional_files() const final
        {
            return {};
        }

        geode::index_t object_priority() const final
        {
            return 0;
        }

        geode::Percentage is_loadable() const final
        {
            return geode::Percentage{ 1 };
        }

        static std::vector< std::string >& read_paths()
        {
            static std::vector< std::string > paths;
            return paths;
        }
    };

    void write_stored_zip( const std::string& zip,
        const std::string& name,
        geode::ByteSpan content )
    {
        void* writer = mz_zip_writer_create();
        mz_zip_writer_set_compress_method( writer, MZ_COMPRESS_METHOD_STORE );
        auto status = mz_zip_writer_open_file( writer, zip.c_str(), 0, 0 );
        if( status == MZ_OK )
        {
            mz_zip_file info{};
            info.filename = name.c_str();
            info.compression_method = MZ_COMPRESS_METHOD_STORE;
            info.flag = MZ_ZIP_FLAG_UTF8;
            info.modified_date = std::time( nullptr );
            status = mz_zip_writer_add_buffer( writer, content.data(),
                static_cast< int32_t >( content.size() ), &info );
        }
        const auto close_status = mz_zip_writer_close( writer );
        mz_zip_writer_delete( &writer );
        geode::OpenGeodeMeshException::test(
            status == MZ_OK && close_status == MZ_OK, "Cannot write zip" );
    }

    void test_fallback_to_file()
    {
        geode::TriangulatedSurfaceInputFactory3D::register_creator<
            PathOnlyInput >( "path_only" );
        const auto bytes = read_whole_file( data_file( "triangle.og_tsf3d" ) );
        const auto impl =
            geode::OpenGeodeTriangulatedSurface3D::impl_name_static();
        const auto reference =
            describe( *geode::load_one( geode::load_triangulated_surface< 3 >(
                                            data_file( "triangle.og_tsf3d" ) ),
                "triangle" ) );

        // Entry of an archive: written to a temporary file
        const std::string zip{ "path_only.zip" };
        write_stored_zip( zip, "mesh.path_only", bytes.span() );
        {
            const geode::detail::ArchiveInputFiles files{ zip };
            const auto mesh =
                geode::load_one( geode::detail::geode_object_input_from_files<
                                     geode::TriangulatedSurfaceInputFactory3D >(
                                     "Mesh", files, "mesh.path_only", impl ),
                    "mesh" );
            geode::OpenGeodeMeshException::test( describe( *mesh ) == reference,
                "Wrong mesh read from a temporary file" );
        }
        auto& paths = PathOnlyInput::read_paths();
        geode::OpenGeodeMeshException::test(
            paths.size() == 1, "The path only input should be called once" );
        const std::filesystem::path temporary{ paths.front() };
        geode::OpenGeodeMeshException::test(
            temporary.filename() == "mesh.path_only"
                && !std::filesystem::exists( temporary.parent_path() ),
            "The temporary file should be removed: ", paths.front() );
        std::filesystem::remove( zip );

        // File of a directory: read in place
        const std::filesystem::path directory{ "path_only_directory" };
        std::filesystem::create_directories( directory );
        {
            std::ofstream file{ directory / "mesh.path_only",
                std::ios::binary };
            file.write( bytes.span().data(),
                static_cast< std::streamsize >( bytes.span().size() ) );
        }
        const geode::detail::DirectoryInputFiles files{ directory.u8string() };
        geode_unused( geode::detail::geode_object_input_from_files<
            geode::TriangulatedSurfaceInputFactory3D >(
            "Mesh", files, "mesh.path_only", impl ) );
        geode::OpenGeodeMeshException::test(
            paths.size() == 2
                && paths.back() == files.location( "mesh.path_only" ),
            "A file of a directory should be read in place" );
        std::filesystem::remove_all( directory );
    }
} // namespace

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_native_meshes();
    test_raster_image();
    test_fallback_to_file();
}

OPENGEODE_TEST( "mesh-input-bytes" )
