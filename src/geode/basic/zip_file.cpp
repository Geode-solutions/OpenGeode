/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/basic/zip_file.h>

#include <fstream>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <ghc/filesystem.hpp>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class ZipFile::Impl
    {
    public:
        Impl( absl::string_view file, absl::string_view archive_temp_filename )
        {
            directory_ = ghc::filesystem::current_path()
                         / to_string( archive_temp_filename );
            ghc::filesystem::create_directory( directory_ );
            mz_zip_writer_create( &writer_ );
            mz_zip_writer_set_compress_method(
                writer_, MZ_COMPRESS_METHOD_STORE );
            const auto status = mz_zip_writer_open_file(
                writer_, to_string( file ).c_str(), 0, 0 );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "[ZipFile] Error opening zip for writing" );
        }

        ~Impl()
        {
            ghc::filesystem::remove( directory_ );
            const auto status = mz_zip_writer_close( writer_ );
            if( status != MZ_OK )
            {
                Logger::error( "[ZipFile] Error closing zip for writing" );
            }
            mz_zip_writer_delete( &writer_ );
        }

        void archive_files( absl::Span< const absl::string_view >& files ) const
        {
            for( const auto& file : files )
            {
                archive_file( file );
            }
        }

        void archive_file( absl::string_view file ) const
        {
            const ghc::filesystem::path file_path{ to_string( file ) };
            const auto status = mz_zip_writer_add_path(
                writer_, file_path.c_str(), NULL, 0, 1 );
            OPENGEODE_EXCEPTION( status == MZ_OK,
                "[ZipFile::archive_file] Error adding path to zip" );
            ghc::filesystem::remove( file_path );
        }

        absl::string_view directory() const
        {
            return directory_.native();
        }

    private:
        ghc::filesystem::path directory_;
        void* writer_{ nullptr };
    };

    ZipFile::ZipFile(
        absl::string_view file, absl::string_view archive_temp_filename )
        : impl_{ file, archive_temp_filename }
    {
    }

    ZipFile::~ZipFile() {}

    void ZipFile::archive_file( absl::string_view file ) const
    {
        impl_->archive_file( file );
    }

    void ZipFile::archive_files(
        absl::Span< const absl::string_view >& files ) const
    {
        impl_->archive_files( files );
    }

    absl::string_view ZipFile::directory() const
    {
        return impl_->directory();
    }

    class UnzipFile::Impl
    {
    public:
        Impl(
            absl::string_view file, absl::string_view unarchive_temp_filename )
        {
            directory_ = ghc::filesystem::current_path()
                         / to_string( unarchive_temp_filename );
            ghc::filesystem::create_directory( directory_ );
            mz_zip_reader_create( &reader_ );
            const auto status =
                mz_zip_reader_open_file( reader_, to_string( file ).c_str() );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "[UnzipFile] Error opening zip for reading" );
        }

        ~Impl()
        {
            ghc::filesystem::remove_all( directory_ );
            mz_zip_reader_delete( &reader_ );
        }

        void extract_all() const
        {
            auto status = mz_zip_reader_goto_first_entry( reader_ );
            while( status == MZ_OK )
            {
                mz_zip_file* file_info{ nullptr };
                status = mz_zip_reader_entry_get_info( reader_, &file_info );
                OPENGEODE_EXCEPTION( status == MZ_OK, "[UnzipFile::extract_all]"
                                                      " Error getting entry "
                                                      "info in zip file" );

                auto file = directory_ / file_info->filename;
                status = mz_zip_reader_entry_save_file( reader_, file.c_str() );
                OPENGEODE_EXCEPTION( status == MZ_OK,
                    "[UnzipFile::extract_all] Error extracting entry file" );
                status = mz_zip_reader_goto_next_entry( reader_ );
            }
        }

        absl::string_view directory() const
        {
            return directory_.native();
        }

    private:
        ghc::filesystem::path directory_;
        void* reader_{ nullptr };
    };

    UnzipFile::UnzipFile(
        absl::string_view filename, absl::string_view unarchive_temp_filename )
        : impl_{ filename, unarchive_temp_filename }
    {
    }

    UnzipFile::~UnzipFile() {}

    void UnzipFile::extract_all() const
    {
        impl_->extract_all();
    }

    absl::string_view UnzipFile::directory() const
    {
        return impl_->directory();
    }

} // namespace geode
