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
#include <limits>
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
    std::filesystem::path to_path( std::string_view utf8_path )
    {
        return std::filesystem::u8path( utf8_path.begin(), utf8_path.end() );
    }

    std::filesystem::path create_directory(
        std::string_view file, std::string_view temp_filename )
    {
        auto directory =
            to_path( file ).parent_path() / to_path( temp_filename );
        std::filesystem::create_directory( directory );
        return directory;
    }

    void remove_directory( const std::filesystem::path& directory ) noexcept
    {
        std::error_code error;
        std::filesystem::remove_all( directory, error );
    }

    bool is_safe_entry_path( const std::filesystem::path& entry )
    {
        if( entry.empty() || entry.has_root_path() )
        {
            return false;
        }
        for( const auto& part : entry.lexically_normal() )
        {
            if( part == ".." )
            {
                return false;
            }
        }
        return true;
    }
} // namespace

namespace geode
{
    class ZipFile::Impl
    {
    public:
        Impl( std::string_view file, std::string_view archive_temp_filename )
            : file_{ to_path( file ) },
              directory_{ create_directory( file, archive_temp_filename ) },
              archive_{ to_path( directory_.u8string() + ".zip" ) },
              nb_uncaught_exceptions_{ std::uncaught_exceptions() }
        {
            writer_ = mz_zip_writer_create();
            mz_zip_writer_set_compress_method(
                writer_, MZ_COMPRESS_METHOD_STORE );
            const auto status = mz_zip_writer_open_file(
                writer_, archive_.u8string().c_str(), 0, 0 );
            if( status != MZ_OK )
            {
                discard();
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[ZipFile] Error opening zip for writing ",
                    file_.u8string(), " (", status, ")" );
            }
        }

        ~Impl()
        {
            if( finalized_ )
            {
                return;
            }
            if( std::uncaught_exceptions() > nb_uncaught_exceptions_ )
            {
                discard();
                return;
            }
            try
            {
                finalize();
            }
            catch( ... )
            {
                try
                {
                    geode_lippincott();
                }
                catch( ... )
                {
                }
            }
        }

        void finalize()
        {
            if( finalized_ )
            {
                return;
            }
            const auto status = mz_zip_writer_close( writer_ );
            mz_zip_writer_delete( &writer_ );
            writer_ = nullptr;
            if( status != MZ_OK )
            {
                discard();
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[ZipFile::finalize] Error closing zip ", file_.u8string(),
                    " (", status, ")" );
            }
            std::error_code error;
            std::filesystem::rename( archive_, file_, error );
            if( error )
            {
                discard();
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[ZipFile::finalize] Error moving zip to ",
                    file_.u8string(), " (", error.message(), ")" );
            }
            finalized_ = true;
            remove_directory( directory_ );
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
            const auto file_path = to_path( file );
            const auto status = mz_zip_writer_add_path(
                writer_, file_path.u8string().c_str(), nullptr, 0, 1 );
            if( status != MZ_OK )
            {
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[ZipFile::archive_file] Error adding path ", file,
                    " to zip (", status, ")" );
            }
            std::error_code error;
            std::filesystem::remove_all( file_path, error );
        }

        std::string directory() const
        {
            return directory_.u8string();
        }

    private:
        void discard() noexcept
        {
            if( writer_ )
            {
                mz_zip_writer_close( writer_ );
                mz_zip_writer_delete( &writer_ );
                writer_ = nullptr;
            }
            remove_directory( directory_ );
            std::error_code error;
            std::filesystem::remove( archive_, error );
        }

    private:
        std::filesystem::path file_;
        std::filesystem::path directory_;
        std::filesystem::path archive_;
        int nb_uncaught_exceptions_;
        void* writer_{ nullptr };
        bool finalized_{ false };
    };

    ZipFile::ZipFile(
        std::string_view file, std::string_view archive_temp_filename )
        : impl_{ file, archive_temp_filename }
    {
    }

    ZipFile::~ZipFile() = default;

    void ZipFile::finalize()
    {
        impl_->finalize();
    }

    void ZipFile::archive_file( std::string_view file ) const
    {
        impl_->archive_file( file );
    }

    void ZipFile::archive_files(
        absl::Span< const std::string_view > files ) const
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
            : file_{ to_path( file ) }
        {
            if( !std::filesystem::exists( file_ ) )
            {
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::data,
                    "[UnzipFile] File to unzip ", file, " doesn't exist" );
            }
            directory_ = std::filesystem::temp_directory_path()
                         / to_path( unarchive_temp_filename );
            std::filesystem::create_directories( directory_ );
            try
            {
                open();
            }
            catch( ... )
            {
                release();
                remove_directory( directory_ );
                throw;
            }
        }

        ~Impl()
        {
            release();
            remove_directory( directory_ );
        }

        void extract_all() const
        {
            if( reader_ == nullptr )
            {
                open();
            }
            constexpr int32_t BUF_SIZE = 1024 * 1024; // 1 MB
            std::vector< uint8_t > buffer( BUF_SIZE );
            auto status = mz_zip_reader_goto_first_entry( reader_ );
            while( status == MZ_OK )
            {
                extract_entry( buffer );
                status = mz_zip_reader_goto_next_entry( reader_ );
            }
            OpenGeodeBasicException::check_exception( status == MZ_END_OF_LIST,
                nullptr, OpenGeodeException::TYPE::data,
                "[UnzipFile::extract_all] Error while reading zip entries (",
                status, ")" );
            // Archive content is on disk now, free the memory
            release();
        }

        std::string directory() const
        {
            return directory_.u8string();
        }

    private:
        void extract_entry( std::vector< uint8_t >& buffer ) const
        {
            mz_zip_file* info = nullptr;
            const auto info_status =
                mz_zip_reader_entry_get_info( reader_, &info );
            OpenGeodeBasicException::check_exception(
                info_status == MZ_OK && info != nullptr, nullptr,
                OpenGeodeException::TYPE::data,
                "[UnzipFile::extract_all] Cannot read zip entry information (",
                info_status, ")" );
            const auto entry = to_path( info->filename );
            OpenGeodeBasicException::check_exception(
                is_safe_entry_path( entry ), nullptr,
                OpenGeodeException::TYPE::data,
                "[UnzipFile::extract_all] Unsafe zip entry path: ",
                info->filename );
            if( mz_zip_reader_entry_is_dir( reader_ ) == MZ_OK )
            {
                std::filesystem::create_directories( directory_ / entry );
                return;
            }
            const auto out_path = directory_ / entry;
            std::filesystem::create_directories( out_path.parent_path() );
            std::ofstream file{ out_path, std::ios::binary };
            OpenGeodeBasicException::check_exception( file.is_open(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[UnzipFile::extract_all] Cannot create file ",
                out_path.u8string() );
            const auto open_status = mz_zip_reader_entry_open( reader_ );
            OpenGeodeBasicException::check_exception( open_status == MZ_OK,
                nullptr, OpenGeodeException::TYPE::data,
                "[UnzipFile::extract_all] Cannot open zip entry ",
                info->filename, " (", open_status, ")" );
            int32_t bytes_read{ 0 };
            while(
                ( bytes_read = mz_zip_reader_entry_read( reader_, buffer.data(),
                      static_cast< int32_t >( buffer.size() ) ) )
                > 0 )
            {
                if( !file.write(
                        reinterpret_cast< const char* >( buffer.data() ),
                        bytes_read ) )
                {
                    mz_zip_reader_entry_close( reader_ );
                    throw OpenGeodeBasicException( nullptr,
                        OpenGeodeException::TYPE::internal,
                        "[UnzipFile::extract_all] Cannot write file ",
                        out_path.u8string() );
                }
            }
            const auto close_status = mz_zip_reader_entry_close( reader_ );
            OpenGeodeBasicException::check_exception(
                bytes_read == 0 && close_status == MZ_OK, nullptr,
                OpenGeodeException::TYPE::data,
                "[UnzipFile::extract_all] Error while reading zip entry ",
                info->filename, " (", bytes_read, ", ", close_status, ")" );
        }

        void open() const
        {
            if( load_zip_into_memory() && open_reader_from_memory() )
            {
                return;
            }
            release();
            if( !open_reader_from_disk() )
            {
                release();
                throw OpenGeodeBasicException( nullptr,
                    OpenGeodeException::TYPE::data,
                    "[UnzipFile] Error opening zip for reading: ",
                    file_.u8string() );
            }
        }

        bool load_zip_into_memory() const
        {
            std::ifstream ifs( file_, std::ios::binary | std::ios::ate );
            if( !ifs.is_open() )
            {
                return false;
            }
            const auto size = static_cast< std::int64_t >( ifs.tellg() );
            if( size < 0 || size > std::numeric_limits< int32_t >::max() )
            {
                return false;
            }
            zip_data_.resize( static_cast< size_t >( size ) );
            ifs.seekg( 0, std::ios::beg );
            ifs.read( reinterpret_cast< char* >( zip_data_.data() ), size );
            return ifs.good();
        }

        bool open_reader_from_memory() const
        {
            memory_stream_ = mz_stream_mem_create();
            mz_stream_mem_set_buffer( memory_stream_,
                static_cast< void* >( zip_data_.data() ),
                static_cast< int32_t >( zip_data_.size() ) );
            reader_ = mz_zip_reader_create();
            return mz_zip_reader_open( reader_, memory_stream_ ) == MZ_OK;
        }

        bool open_reader_from_disk() const
        {
            Logger::debug( "[UnzipFile] Opening zip from disk" );
            reader_ = mz_zip_reader_create();
            return mz_zip_reader_open_file( reader_, file_.u8string().c_str() )
                   == MZ_OK;
        }

        void release() const noexcept
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
            zip_data_.clear();
            zip_data_.shrink_to_fit();
        }

    private:
        std::filesystem::path file_;
        std::filesystem::path directory_;
        mutable std::vector< uint8_t > zip_data_;

        mutable void* reader_{ nullptr };
        mutable void* memory_stream_{ nullptr };
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
        if( reader == nullptr )
        {
            return false;
        }
        const auto status =
            mz_zip_reader_open_file( reader, to_string( file ).c_str() );
        mz_zip_reader_close( reader );
        mz_zip_reader_delete( &reader );
        return status == MZ_OK;
    }
} // namespace geode
