/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/image/core/raster_image.h>
#include <geode/image/core/rgb_color.h>
#include <geode/image/io/raster_image_input.h>
#include <geode/image/io/raster_image_output.h>

#include <geode/tests/common.h>

void test_raster( const geode::RasterImage2D& raster )
{
    OPENGEODE_EXCEPTION(
        raster.nb_cells() == 100, "[Test] Wrong number of cells" );
    OPENGEODE_EXCEPTION( raster.nb_cells_in_direction( 0 ) == 10,
        "[Test] Wrong number of cells in direction 0" );
    OPENGEODE_EXCEPTION( raster.nb_cells_in_direction( 1 ) == 10,
        "[Test] Wrong number of cells in direction 1" );
    for( const auto i : geode::LRange{ raster.nb_cells() } )
    {
        const geode::RGBColor color{ i, i, i };
        OPENGEODE_EXCEPTION(
            raster.color( i ) == color, "[Test] Wrong color for ", i );
    }
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
    auto raster2 = std::move( raster );
    OPENGEODE_EXCEPTION(
        raster2.nb_cells() == 100, "[Test] Wrong number of cells after move" );
}

OPENGEODE_TEST( "raster" )