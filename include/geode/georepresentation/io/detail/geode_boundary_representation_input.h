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

#include <filesystem/path.h>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <geode/basic/uuid.h>

#include <geode/georepresentation/core/boundary_representation.h>
#include <geode/georepresentation/io/boundary_representation_input.h>

namespace geode
{
    class OpenGeodeBRepInput final : public BRepInput
    {
    public:
        OpenGeodeBRepInput( BRep& brep, std::string filename )
            : BRepInput( brep, std::move( filename ) )
        {
            mz_zip_reader_create( &reader_ );
        }

        ~OpenGeodeBRepInput()
        {
            mz_zip_reader_delete( &reader_ );
        }

        static std::string extension()
        {
            return BRep::native_extension_static();
        }

        void read() final
        {
            auto status =
                mz_zip_reader_open_file( reader_, this->filename().c_str() );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "Error opening zip for reading" );
            filesystem::path directory{ uuid{}.string() };
            filesystem::create_directory( directory );

            std::vector< filesystem::path > files;
            status = mz_zip_reader_goto_first_entry( reader_ );
            while( status == MZ_OK )
            {
                mz_zip_file* file_info{ nullptr };
                status = mz_zip_reader_entry_get_info( reader_, &file_info );
                OPENGEODE_EXCEPTION(
                    status == MZ_OK, "Error getting entry info in zip file" );

                files.emplace_back( directory / file_info->filename );
                status = mz_zip_reader_entry_save_file(
                    reader_, files.back().str().c_str() );
                OPENGEODE_EXCEPTION(
                    status == MZ_OK, "Error extracting entry file" );
                status = mz_zip_reader_goto_next_entry( reader_ );
            }

            BRepBuilder builder( brep() );

            builder.load_corners( directory.str() );
            builder.load_lines( directory.str() );
            builder.load_surfaces( directory.str() );
            builder.load_blocks( directory.str() );
            builder.load_relationships( directory.str() );
            builder.unique_vertices().load_unique_vertices( directory.str() );

            for( const auto& corner : brep().corners() )
            {
                builder.unique_vertices().register_component( corner );
            }
            for( const auto& line : brep().lines() )
            {
                builder.unique_vertices().register_component( line );
            }
            for( const auto& surface : brep().surfaces() )
            {
                builder.unique_vertices().register_component( surface );
            }
            for( const auto& block : brep().blocks() )
            {
                builder.unique_vertices().register_component( block );
            }

            for( auto& file : files )
            {
                file.remove_file();
            }
            directory.remove_file();
        }

    private:
        void* reader_{ nullptr };
    };
} // namespace geode
