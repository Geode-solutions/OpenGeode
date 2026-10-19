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

#include <filesystem>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>

#include <geode/image/core/raster_image.hpp>
#include <geode/image/core/rgb_color.hpp>
#include <geode/image/io/raster_image_input.hpp>
#include <geode/image/io/raster_image_output.hpp>

#include <geode/tests/common.hpp>

void test_raster( const geode::RasterImage2D& raster )
{
    geode::OpenGeodeImageException::test(
        raster.nb_cells() == 100, "Wrong number of cells" );
    geode::OpenGeodeImageException::test(
        raster.nb_cells_in_direction( 0 ) == 10,
        "Wrong number of cells in direction 0" );
    geode::OpenGeodeImageException::test(
        raster.nb_cells_in_direction( 1 ) == 10,
        "Wrong number of cells in direction 1" );
    for( const auto i : geode::LRange{ raster.nb_cells() } )
    {
        const geode::RGBColor color{ i, i, i };
        geode::OpenGeodeImageException::test(
            raster.color( i ) == color, "Wrong color for ", i );
    }
}

void test_save_failure( const geode::RasterImage2D& raster )
{
    const auto folder = "raster_folder.og_img2d";
    std::filesystem::create_directory( folder );
    bool thrown{ false };
    try
    {
        geode::save_raster_image( raster, folder );
    }
    catch( const geode::OpenGeodeException& )
    {
        thrown = true;
    }
    geode::OpenGeodeImageException::test(
        thrown, "[TEST] Saving into a directory should throw" );
}

void test_missing_colors()
{
    const auto raster = geode::load_raster_image< 2 >(
        absl::StrCat( geode::DATA_PATH, "raster_missing_colors.og_img2d" ) );
    geode::OpenGeodeImageException::test(
        raster.nb_cells() == 64, "[TEST] Wrong number of repaired cells" );
    for( const auto i : geode::Range{ 16 } )
    {
        const geode::RGBColor color{
            static_cast< geode::local_index_t >( i + 1 ), 0, 0
        };
        geode::OpenGeodeImageException::test(
            raster.color( i ) == color, "[TEST] Wrong stored color for ", i );
    }
    for( const auto i : geode::Range{ 16, 64 } )
    {
        geode::OpenGeodeImageException::test(
            raster.color( i ) == geode::RGBColor{},
            "[TEST] Wrong repaired color for ", i );
    }
}

bool load_throws( std::string_view filename )
{
    try
    {
        const auto raster = geode::load_raster_image< 2 >(
            absl::StrCat( geode::DATA_PATH, filename ) );
    }
    catch( const geode::OpenGeodeException& )
    {
        return true;
    }
    return false;
}

void test_too_many_cells()
{
    geode::OpenGeodeImageException::test(
        load_throws( "raster_too_many_cells.og_img2d" ),
        "[TEST] Loading a raster with too many cells should throw" );
}

void test_extra_colors()
{
    geode::OpenGeodeImageException::test(
        load_throws( "raster_extra_colors.og_img2d" ),
        "[TEST] Loading more colors than cells should throw" );
}

void test()
{
    geode::OpenGeodeImageLibrary::initialize();
    geode::RasterImage2D raster{ { 10, 10 } };
    for( const auto i : geode::LRange{ raster.nb_cells() } )
    {
        raster.set_color( i, { i, i, i } );
    }
    test_raster( raster );
    test_raster( raster.clone() );
    const auto filename_with_spaces = absl::StrCat( " ", "test.og_img2d", " " );
    geode::save_raster_image( raster, filename_with_spaces );
    const auto reload = geode::load_raster_image< 2 >( filename_with_spaces );
    test_raster( reload );
    test_save_failure( raster );
    test_missing_colors();
    test_too_many_cells();
    test_extra_colors();
    auto raster2 = std::move( raster );
    geode::OpenGeodeImageException::test(
        raster2.nb_cells() == 100, "Wrong number of cells after move" );
}

OPENGEODE_TEST( "raster" )