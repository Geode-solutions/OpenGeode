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
#include <filesystem>
#include <string>
#include <vector>

#include <absl/strings/str_cat.h>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <geode/basic/assert.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>

#include <geode/tests/common.hpp>

void rezip_with_deflate( const std::string& input, const std::string& output )
{
    void* reader = mz_zip_reader_create();
    geode::OpenGeodeModelException::test(
        mz_zip_reader_open_file( reader, input.c_str() ) == MZ_OK,
        "Cannot open ", input );
    void* writer = mz_zip_writer_create();
    mz_zip_writer_set_compress_method( writer, MZ_COMPRESS_METHOD_DEFLATE );
    auto write_status = mz_zip_writer_open_file( writer, output.c_str(), 0, 0 );
    auto status = mz_zip_reader_goto_first_entry( reader );
    geode::index_t nb_entries{ 0 };
    while( status == MZ_OK && write_status == MZ_OK )
    {
        mz_zip_file* info = nullptr;
        geode::OpenGeodeModelException::test(
            mz_zip_reader_entry_get_info( reader, &info ) == MZ_OK,
            "Cannot read entry of ", input );
        std::vector< uint8_t > buffer(
            static_cast< size_t >( info->uncompressed_size ) );
        const auto length = static_cast< int32_t >( buffer.size() );
        geode::OpenGeodeModelException::test(
            mz_zip_reader_entry_save_buffer( reader, buffer.data(), length )
                == MZ_OK,
            "Cannot read entry ", info->filename, " of ", input );
        mz_zip_file deflated = *info;
        deflated.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
        write_status = mz_zip_writer_add_buffer(
            writer, buffer.data(), length, &deflated );
        nb_entries++;
        status = mz_zip_reader_goto_next_entry( reader );
    }
    const auto close_status = mz_zip_writer_close( writer );
    mz_zip_writer_delete( &writer );
    mz_zip_reader_close( reader );
    mz_zip_reader_delete( &reader );
    geode::OpenGeodeModelException::test(
        status == MZ_END_OF_LIST && write_status == MZ_OK
            && close_status == MZ_OK && nb_entries > 0,
        "Cannot re-zip ", input, " with DEFLATE (", status, ", ", write_status,
        ", ", close_status, ")" );
}

void check_same_brep( const geode::BRep& brep, const geode::BRep& reference )
{
    geode::OpenGeodeModelException::test(
        brep.nb_corners() == reference.nb_corners()
            && brep.nb_lines() == reference.nb_lines()
            && brep.nb_surfaces() == reference.nb_surfaces()
            && brep.nb_blocks() == reference.nb_blocks()
            && brep.nb_unique_vertices() == reference.nb_unique_vertices(),
        "BRep loaded from the DEFLATE archive differs from the reference" );
    for( const auto& surface : reference.surfaces() )
    {
        const auto& mesh = brep.surface( surface.id() ).mesh();
        geode::OpenGeodeModelException::test(
            mesh.nb_vertices() == surface.mesh().nb_vertices()
                && mesh.nb_polygons() == surface.mesh().nb_polygons(),
            "Surface mesh loaded from the DEFLATE archive differs" );
    }
    for( const auto& block : reference.blocks() )
    {
        const auto& mesh = brep.block( block.id() ).mesh();
        geode::OpenGeodeModelException::test(
            mesh.nb_vertices() == block.mesh().nb_vertices()
                && mesh.nb_polyhedra() == block.mesh().nb_polyhedra(),
            "Block mesh loaded from the DEFLATE archive differs" );
    }
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    const auto filename = absl::StrCat( geode::DATA_PATH, "layers.og_brep" );
    const std::string deflated{ "layers_deflate.og_brep" };
    rezip_with_deflate( filename, deflated );
    const auto reference =
        geode::load_one( geode::load_brep( filename ), filename );
    const auto brep = geode::load_one( geode::load_brep( deflated ), deflated );
    check_same_brep( brep, reference );
    std::filesystem::remove( deflated );
}

OPENGEODE_TEST( "model-deflate-archive" )
