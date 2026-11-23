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

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string_view>

#include <absl/container/flat_hash_map.h>
#include <absl/strings/str_cat.h>

#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_mem.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <zlib.h>

#include <geode/basic/detail/input_files.hpp>
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

    constexpr size_t LOCAL_HEADER_SIZE{ 30 };
    constexpr uint32_t LOCAL_HEADER_SIGNATURE{ 0x04034b50 };

    uint32_t read_little_endian( const char* bytes, size_t nb_bytes )
    {
        uint32_t value{ 0 };
        for( size_t byte = nb_bytes; byte > 0; byte-- )
        {
            value = ( value << 8U )
                    | static_cast< unsigned char >( bytes[byte - 1] );
        }
        return value;
    }

    class ZipReader
    {
    public:
        explicit ZipReader( const std::filesystem::path& file )
            : reader_{ mz_zip_reader_create() }
        {
            status_ = reader_ == nullptr ? MZ_MEM_ERROR
                                         : mz_zip_reader_open_file( reader_,
                                               file.u8string().c_str() );
        }

        ~ZipReader()
        {
            if( reader_ )
            {
                mz_zip_reader_close( reader_ );
                mz_zip_reader_delete( &reader_ );
            }
        }

        ZipReader( const ZipReader& ) = delete;
        ZipReader& operator=( const ZipReader& ) = delete;

        [[nodiscard]] bool is_open() const
        {
            return status_ == MZ_OK;
        }

        [[nodiscard]] void* get() const
        {
            return reader_;
        }

    private:
        void* reader_;
        int32_t status_;
    };

    geode::detail::ByteBuffer inflate_entry(
        geode::ByteSpan compressed, size_t uncompressed_size, bool& success )
    {
        geode::detail::ByteBuffer buffer{ uncompressed_size };
        z_stream stream{};
        success = false;
        if( inflateInit2_( &stream, -MAX_WBITS, ZLIB_VERSION,
                static_cast< int >( sizeof( z_stream ) ) )
            != Z_OK )
        {
            return buffer;
        }
        using Chunk = decltype( stream.avail_in );
        constexpr auto MAX_CHUNK =
            static_cast< size_t >( std::numeric_limits< Chunk >::max() );
        const auto* input =
            reinterpret_cast< const unsigned char* >( compressed.data() );
        auto input_left = compressed.size();
        auto* output = reinterpret_cast< unsigned char* >( buffer.data() );
        auto output_left = buffer.size();
        auto status = Z_OK;
        while( status == Z_OK )
        {
            const auto input_chunk = std::min( input_left, MAX_CHUNK );
            const auto output_chunk = std::min( output_left, MAX_CHUNK );
            stream.next_in = const_cast< unsigned char* >( input );
            stream.avail_in = static_cast< Chunk >( input_chunk );
            stream.next_out = output;
            stream.avail_out = static_cast< Chunk >( output_chunk );
            status = inflate( &stream, Z_NO_FLUSH );
            const auto nb_read = input_chunk - stream.avail_in;
            const auto nb_written = output_chunk - stream.avail_out;
            input += nb_read;
            input_left -= nb_read;
            output += nb_written;
            output_left -= nb_written;
        }
        inflateEnd( &stream );
        success = status == Z_STREAM_END && output_left == 0;
        return buffer;
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

    namespace detail
    {
        class ArchiveInputFiles::Impl
        {
            struct ArchiveEntry
            {
                uint64_t header_offset;
                uint64_t compressed_size;
                uint64_t uncompressed_size;
                uint32_t crc;
                uint16_t method;
            };

        public:
            explicit Impl( std::string_view filename )
                : filename_{ to_string( filename ) }
            {
                OpenGeodeBasicException::check_exception(
                    std::filesystem::exists( to_path( filename ) ), nullptr,
                    OpenGeodeException::TYPE::data, "[ArchiveInputFiles] File ",
                    filename, " doesn't exist" );
                read_entries();
                mapping_ = MappedFile::map( filename_ );
                if( !mapping_ )
                {
                    Logger::debug( "[ArchiveInputFiles] Cannot map ", filename_,
                        ", reading its entries from disk" );
                }
            }

            bool has( std::string_view name ) const
            {
                return entries_.contains( name );
            }

            std::vector< std::string > names() const
            {
                return names_;
            }

            FileContent content( std::string_view name ) const
            {
                const auto entry = entries_.find( name );
                OpenGeodeBasicException::check_exception(
                    entry != entries_.end(), nullptr,
                    OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Missing entry ", location( name ) );
                auto content = entry->second.method == MZ_COMPRESS_METHOD_STORE
                                   ? stored_content( name, entry->second )
                                   : inflated_content( name, entry->second );
                const auto bytes = content.span();
                const auto crc = crc32_z( 0,
                    reinterpret_cast< const unsigned char* >( bytes.data() ),
                    bytes.size() );
                OpenGeodeBasicException::check_exception(
                    static_cast< uint32_t >( crc ) == entry->second.crc,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Corrupted entry ", location( name ),
                    " (CRC-32 mismatch)" );
                return content;
            }

            std::string location( std::string_view name ) const
            {
                return absl::StrCat( filename_, "/", name );
            }

        private:
            void read_entries()
            {
                const ZipReader reader{ to_path( filename_ ) };
                OpenGeodeBasicException::check_exception( reader.is_open(),
                    nullptr, OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Error opening zip for reading: ",
                    filename_ );
                auto status = mz_zip_reader_goto_first_entry( reader.get() );
                while( status == MZ_OK )
                {
                    read_entry( reader.get() );
                    status = mz_zip_reader_goto_next_entry( reader.get() );
                }
                OpenGeodeBasicException::check_exception(
                    status == MZ_END_OF_LIST, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Error while reading zip entries of ",
                    filename_, " (", status, ")" );
            }

            void read_entry( void* reader )
            {
                mz_zip_file* info = nullptr;
                const auto info_status =
                    mz_zip_reader_entry_get_info( reader, &info );
                OpenGeodeBasicException::check_exception(
                    info_status == MZ_OK && info != nullptr, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Cannot read zip entry information "
                    "of ",
                    filename_, " (", info_status, ")" );
                std::string name{ info->filename };
                OpenGeodeBasicException::check_exception(
                    is_safe_entry_path( to_path( name ) ), nullptr,
                    OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Unsafe zip entry path: ", name );
                if( mz_zip_reader_entry_is_dir( reader ) == MZ_OK )
                {
                    return;
                }
                OpenGeodeBasicException::check_exception(
                    ( info->flag & MZ_ZIP_FLAG_ENCRYPTED ) == 0, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Encrypted zip entry: ",
                    location( name ) );
                OpenGeodeBasicException::check_exception(
                    info->compression_method == MZ_COMPRESS_METHOD_STORE
                        || info->compression_method
                               == MZ_COMPRESS_METHOD_DEFLATE,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Unsupported compression method ",
                    info->compression_method, " of zip entry ",
                    location( name ) );
                OpenGeodeBasicException::check_exception(
                    info->disk_offset >= 0 && info->compressed_size >= 0
                        && info->uncompressed_size >= 0,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Invalid zip entry ",
                    location( name ) );
                ArchiveEntry entry{ static_cast< uint64_t >(
                                        info->disk_offset ),
                    static_cast< uint64_t >( info->compressed_size ),
                    static_cast< uint64_t >( info->uncompressed_size ),
                    info->crc, info->compression_method };
                const auto [_, inserted] =
                    entries_.insert_or_assign( name, entry );
                if( inserted )
                {
                    names_.emplace_back( std::move( name ) );
                }
            }

            /*!
             * Return the offset of the data of an entry, after its local
             * header.
             */
            uint64_t data_offset(
                std::string_view name, const ArchiveEntry& entry ) const
            {
                std::array< char, LOCAL_HEADER_SIZE > header_buffer;
                const char* header = header_buffer.data();
                if( mapping_ )
                {
                    const auto archive = mapping_->span();
                    check_inside_archive(
                        name, entry.header_offset, LOCAL_HEADER_SIZE );
                    header = archive.data() + entry.header_offset;
                }
                else
                {
                    read_at( name, entry.header_offset, header_buffer.data(),
                        LOCAL_HEADER_SIZE );
                }
                OpenGeodeBasicException::check_exception(
                    read_little_endian( header, 4 ) == LOCAL_HEADER_SIGNATURE,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Invalid local header of zip entry ",
                    location( name ) );
                const auto name_length = read_little_endian( header + 26, 2 );
                const auto extra_length = read_little_endian( header + 28, 2 );
                return entry.header_offset + LOCAL_HEADER_SIZE + name_length
                       + extra_length;
            }

            /*!
             * Return the raw (compressed) bytes of an entry: a slice of the
             * mapped archive, or a buffer read from the file.
             */
            FileContent raw_content(
                std::string_view name, const ArchiveEntry& entry ) const
            {
                const auto offset = data_offset( name, entry );
                if( mapping_ )
                {
                    check_inside_archive( name, offset, entry.compressed_size );
                    return { mapping_, mapping_->span().subspan(
                                           offset, entry.compressed_size ) };
                }
                ByteBuffer buffer{ entry.compressed_size };
                read_at( name, offset, buffer.data(), buffer.size() );
                return FileContent{ std::move( buffer ) };
            }

            FileContent stored_content(
                std::string_view name, const ArchiveEntry& entry ) const
            {
                OpenGeodeBasicException::check_exception(
                    entry.compressed_size == entry.uncompressed_size, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Invalid size of stored zip entry ",
                    location( name ) );
                return raw_content( name, entry );
            }

            FileContent inflated_content(
                std::string_view name, const ArchiveEntry& entry ) const
            {
                const auto compressed = raw_content( name, entry );
                bool success{ false };
                auto buffer = inflate_entry(
                    compressed.span(), entry.uncompressed_size, success );
                OpenGeodeBasicException::check_exception( success, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Cannot decompress zip entry ",
                    location( name ) );
                return FileContent{ std::move( buffer ) };
            }

            void check_inside_archive(
                std::string_view name, uint64_t offset, uint64_t size ) const
            {
                const auto archive_size = mapping_->span().size();
                OpenGeodeBasicException::check_exception(
                    offset <= archive_size && size <= archive_size - offset,
                    nullptr, OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Truncated zip entry ",
                    location( name ) );
            }

            /*!
             * Positional read, used when the archive cannot be mapped.
             */
            void read_at( std::string_view name,
                uint64_t offset,
                char* data,
                size_t size ) const
            {
                std::ifstream file{ to_path( filename_ ), std::ios::binary };
                file.seekg( static_cast< std::streamoff >( offset ) );
                file.read( data, static_cast< std::streamsize >( size ) );
                OpenGeodeBasicException::check_exception(
                    file.good()
                        && file.gcount()
                               == static_cast< std::streamsize >( size ),
                    nullptr, OpenGeodeException::TYPE::data,
                    "[ArchiveInputFiles] Cannot read zip entry ",
                    location( name ) );
            }

        private:
            std::string filename_;
            absl::flat_hash_map< std::string, ArchiveEntry > entries_;
            std::vector< std::string > names_;
            std::shared_ptr< const MappedFile > mapping_;
        };

        ArchiveInputFiles::ArchiveInputFiles( std::string_view filename )
            : impl_{ filename }
        {
        }

        ArchiveInputFiles::~ArchiveInputFiles() = default;

        bool ArchiveInputFiles::has( std::string_view name ) const
        {
            return impl_->has( name );
        }

        std::vector< std::string > ArchiveInputFiles::names() const
        {
            return impl_->names();
        }

        FileContent ArchiveInputFiles::content( std::string_view name ) const
        {
            return impl_->content( name );
        }

        std::optional< std::string > ArchiveInputFiles::path(
            std::string_view /*name*/ ) const
        {
            return std::nullopt;
        }

        std::string ArchiveInputFiles::location( std::string_view name ) const
        {
            return impl_->location( name );
        }
    } // namespace detail

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
