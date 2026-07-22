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

#include <geode/basic/zip_file.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>

#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_mem.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace
{
    std::filesystem::path create_directory(
        std::string_view file, std::string_view temp_filename )
    {
        const auto file_string = geode::to_string( file );
        auto directory = std::filesystem::path{ file_string }.parent_path()
                         / geode::to_string( temp_filename );
        std::filesystem::create_directory( directory );
        return directory;
    }
} // namespace

namespace geode
{
    class ZipFile::Impl
    {
    public:
        Impl( std::string_view file, std::string_view archive_temp_filename )
        {
            directory_ = create_directory( file, archive_temp_filename );
            writer_ = mz_zip_writer_create();
            mz_zip_writer_set_compress_method(
                writer_, MZ_COMPRESS_METHOD_STORE );
            const auto status = mz_zip_writer_open_file(
                writer_, to_string( file ).c_str(), 0, 0 );
            if( status != MZ_OK )
            {
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[ZipFile] Error opening zip for writing (", status, ")" );
            }
        }

        ~Impl()
        {
            if( writer_ )
            {
                mz_zip_writer_close( writer_ );
                mz_zip_writer_delete( &writer_ );
            }
            std::filesystem::remove_all( directory_ );
        }

        void archive_files( absl::Span< const std::string_view > files ) const
        {
            for( const auto& file : files )
            {
                archive_file( file );
            }
        }

        void archive_file( std::string_view file ) const
        {
            const std::filesystem::path file_path{ to_string( file ) };
            const auto status = mz_zip_writer_add_path(
                writer_, file_path.string().c_str(), nullptr, 0, 1 );
            if( status != MZ_OK )
            {
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[ZipFile::archive_file] Error adding path to zip (",
                    status, ")" );
            }
            std::filesystem::remove( file_path );
        }

        std::string directory() const
        {
            return directory_.string();
        }

    private:
        std::filesystem::path directory_;
        void* writer_{ nullptr };
    };

    ZipFile::ZipFile(
        std::string_view file, std::string_view archive_temp_filename )
        : impl_{ file, archive_temp_filename }
    {
    }

    ZipFile::~ZipFile() = default;

    void ZipFile::archive_file( std::string_view file ) const
    {
        impl_->archive_file( file );
    }

    void ZipFile::archive_files(
        absl::Span< const std::string_view >& files ) const
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
        Impl( std::string_view file, std::string_view unarchive_temp_filename )
        {
            directory_ = create_directory( file, unarchive_temp_filename );
            if( !load_zip_into_memory( file ) || !open_reader() )
            {
                Logger::info( "[UnzipFile] Couldn't open zip in memory, trying "
                              "to open on disk, this could take more time" );
                reader_ = nullptr;
                memory_stream_ = nullptr;
                if( !create_reader_from_disk( file ) )
                {
                    std::filesystem::remove_all( directory_ );
                    throw OpenGeodeBasicException( nullptr,
                        OpenGeodeException::TYPE::internal,
                        "[UnzipFile] Error opening zip for reading" );
                }
            }
        }

        ~Impl()
        {
            if( reader_ )
            {
                mz_zip_reader_close( reader_ );
                mz_zip_reader_delete( &reader_ );
                reader_ = nullptr;
            }
            if( memory_stream_ )
            {
                mz_stream_close( memory_stream_ );
                mz_stream_delete( &memory_stream_ );
                memory_stream_ = nullptr;
            }
            std::filesystem::remove_all( directory_ );
        }

        void extract_all() const
        {
            constexpr size_t BUF_SIZE = 1024 * 1024; // 1 MB
            std::vector< uint8_t > buffer( BUF_SIZE );
            int status = mz_zip_reader_goto_first_entry( reader_ );
            while( status == MZ_OK )
            {
                mz_zip_file* info = nullptr;
                mz_zip_reader_entry_get_info( reader_, &info );
                if( !info )
                {
                    status = mz_zip_reader_goto_next_entry( reader_ );
                    continue;
                }
                auto out_path = directory_ / info->filename;
                std::filesystem::create_directories( out_path.parent_path() );
                FILE* file = fopen( out_path.string().c_str(), "wb" );
                if( !file )
                {
                    status = mz_zip_reader_goto_next_entry( reader_ );
                    continue;
                }
                status = mz_zip_reader_entry_open( reader_ );
                if( status == MZ_OK )
                {
                    int32_t bytes_read = 0;
                    while( ( bytes_read = mz_zip_reader_entry_read(
                                 reader_, buffer.data(), BUF_SIZE ) )
                           > 0 )
                    {
                        fwrite( buffer.data(), 1, bytes_read, file );
                    }
                    mz_zip_reader_entry_close( reader_ );
                }
                fclose( file );
                status = mz_zip_reader_goto_next_entry( reader_ );
            }
        }

        std::string directory() const
        {
            return directory_.string();
        }

    private:
        bool load_zip_into_memory( std::string_view file )
        {
            std::ifstream ifs(
                to_string( file ), std::ios::binary | std::ios::ate );
            if( !ifs.is_open() )
            {
                return false;
            }
            auto size = ifs.tellg();
            zip_data_.resize( static_cast< size_t >( size ) );
            ifs.seekg( 0 );
            ifs.read( reinterpret_cast< char* >( zip_data_.data() ), size );
            return ifs.good();
        }

        bool open_reader()
        {
            memory_stream_ = mz_stream_mem_create();
            mz_stream_mem_set_buffer( memory_stream_,
                static_cast< void* >( zip_data_.data() ),
                static_cast< int32_t >( zip_data_.size() ) );
            reader_ = mz_zip_reader_create();
            return mz_zip_reader_open( reader_, memory_stream_ ) == MZ_OK;
        }

        bool create_reader_from_disk( std::string_view file )
        {
            reader_ = mz_zip_reader_create();
            return mz_zip_reader_open_file( reader_, to_string( file ).c_str() )
                   == MZ_OK;
        }

    private:
        std::filesystem::path directory_;
        std::vector< uint8_t > zip_data_;

        void* reader_{ nullptr };
        void* memory_stream_{ nullptr };
    };

    UnzipFile::UnzipFile(
        std::string_view filename, std::string_view unarchive_temp_filename )
        : impl_{ filename, unarchive_temp_filename }
    {
    }

    UnzipFile::~UnzipFile() = default;

    void UnzipFile::extract_all() const
    {
        impl_->extract_all();
    }

    std::string UnzipFile::directory() const
    {
        return impl_->directory();
    }

    bool is_zip_file( std::string_view file )
    {
        void* reader = mz_zip_reader_create();
        const auto status =
            mz_zip_reader_open_file( reader, to_string( file ).c_str() );
        mz_zip_reader_close( reader );
        mz_zip_reader_delete( &reader );
        return status == MZ_OK;
    }
} // namespace geode
