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

#include <geode/basic/zip_file.h>

#include <fstream>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <ghc/filesystem.hpp>

#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class ZipFile::Impl
    {
    public:
        Impl(
            const std::string& file, const std::string& archive_temp_filename )
        {
            directory_ =
                ghc::filesystem::current_path() / archive_temp_filename;
            ghc::filesystem::create_directory( directory_ );
            mz_zip_writer_create( &writer_ );
            mz_zip_writer_set_compress_method(
                writer_, MZ_COMPRESS_METHOD_STORE );
            auto status =
                mz_zip_writer_open_file( writer_, file.c_str(), 0, 0 );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "Error opening zip for writing" );
        }

        ~Impl()
        {
            ghc::filesystem::remove( directory_ );
            auto status = mz_zip_writer_close( writer_ );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "Error closing zip for writing" );
            mz_zip_writer_delete( &writer_ );
        }

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

        std::string directory() const
        {
            return directory_;
        }

    private:
        ghc::filesystem::path directory_;
        void* writer_{ nullptr };
    };

    ZipFile::ZipFile(
        const std::string& file, const std::string& archive_temp_filename )
        : impl_{ file, archive_temp_filename }
    {
    }

    ZipFile::~ZipFile() {}

    void ZipFile::archive_file( const std::string& file ) const
    {
        impl_->archive_file( file );
    }

    void ZipFile::archive_files( const std::vector< std::string >& files ) const
    {
        impl_->archive_files( files );
    }

    std::string ZipFile::directory() const
    {
        return impl_->directory();
    }

    class UnzipFile::Impl
    {
    public:
        Impl( const std::string& file,
            const std::string& unarchive_temp_filename )
        {
            directory_ =
                ghc::filesystem::current_path() / unarchive_temp_filename;
            ghc::filesystem::create_directory( directory_ );
            mz_zip_reader_create( &reader_ );
            auto status = mz_zip_reader_open_file( reader_, file.c_str() );
            OPENGEODE_EXCEPTION(
                status == MZ_OK, "Error opening zip for reading" );
        }

        ~Impl()
        {
            ghc::filesystem::remove_all( directory_ );
            mz_zip_reader_delete( &reader_ );
        }

        void extract_all() const
        {
            std::vector< ghc::filesystem::path > files;
            auto status = mz_zip_reader_goto_first_entry( reader_ );
            while( status == MZ_OK )
            {
                mz_zip_file* file_info{ nullptr };
                status = mz_zip_reader_entry_get_info( reader_, &file_info );
                OPENGEODE_EXCEPTION(
                    status == MZ_OK, "Error getting entry info in zip file" );

                files.emplace_back( directory_ / file_info->filename );
                status = mz_zip_reader_entry_save_file(
                    reader_, files.back().c_str() );
                OPENGEODE_EXCEPTION(
                    status == MZ_OK, "Error extracting entry file" );
                status = mz_zip_reader_goto_next_entry( reader_ );
            }
        }

        std::string directory() const
        {
            return directory_;
        }

    private:
        ghc::filesystem::path directory_;
        void* reader_{ nullptr };
    };

    UnzipFile::UnzipFile(
        const std::string& filename, std::string unarchive_temp_filename )
        : impl_{ filename, std::move( unarchive_temp_filename ) }
    {
    }

    UnzipFile::~UnzipFile() {}

    void UnzipFile::extract_all() const
    {
        impl_->extract_all();
    }

    std::string UnzipFile::directory() const
    {
        return impl_->directory();
    }

} // namespace geode
