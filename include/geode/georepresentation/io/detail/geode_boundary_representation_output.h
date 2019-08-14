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

#pragma once

#include <fstream>

#include <ghc/filesystem.hpp>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <geode/basic/uuid.h>

#include <geode/georepresentation/core/boundary_representation.h>
#include <geode/georepresentation/io/boundary_representation_output.h>

namespace geode
{
    class OpenGeodeBRepOutput final : public BRepOutput
    {
    public:
        OpenGeodeBRepOutput( const BRep& brep, std::string filename )
            : BRepOutput( brep, std::move( filename ) )
        {
            mz_zip_writer_create( &writer_ );
            mz_zip_writer_set_compress_method(
                writer_, MZ_COMPRESS_METHOD_STORE );
        }

        ~OpenGeodeBRepOutput()
        {
            mz_zip_writer_delete( &writer_ );
        }

        static std::string extension()
        {
            return BRep::native_extension_static();
        }

        void write() const final
        {
            auto status = mz_zip_writer_open_file(
                writer_, this->filename().c_str(), 0, 0 );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "Error opening zip for writing" );
            ghc::filesystem::path directory{ uuid{}.string() };
            ghc::filesystem::create_directory( directory );
            archive_file(
                brep().relationships().save_relationships( directory ) );
            archive_file( brep().unique_vertices().save_unique_vertices(
                directory ) );
            archive_files( brep().save_corners( directory ) );
            archive_files( brep().save_lines( directory ) );
            archive_files( brep().save_surfaces( directory ) );
            archive_files( brep().save_blocks( directory ) );
            ghc::filesystem::remove( directory );
            status = mz_zip_writer_close( writer_ );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "Error closing zip for writing" );
        }

    private:
        void archive_files( const std::vector< std::string >& files ) const
        {
            for( const auto& file : files )
            {
                archive_file( file );
            }
        }

        void archive_file( const std::string& file ) const
        {
            ghc::filesystem::path file_path{ file };
            auto status = mz_zip_writer_add_path(
                writer_, file_path.c_str(), NULL, 0, 1 );
            OPENGEODE_EXCEPTION( status == MZ_OK, "Error adding path to zip" );
            ghc::filesystem::remove( file_path );
        }

    private:
        void* writer_{ nullptr };
    };
} // namespace geode
