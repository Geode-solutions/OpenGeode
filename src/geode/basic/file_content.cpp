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

#include <geode/basic/detail/file_content.hpp>

#include <filesystem>
#include <fstream>

#ifdef _WIN32
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <windows.h>
#else
#    include <fcntl.h>
#    include <sys/mman.h>
#    include <sys/stat.h>
#    include <unistd.h>
#endif

#include <geode/basic/logger.hpp>

namespace
{
    std::filesystem::path to_path( std::string_view utf8_path )
    {
        return std::filesystem::u8path( utf8_path.begin(), utf8_path.end() );
    }

    size_t page_size()
    {
#ifdef _WIN32
        SYSTEM_INFO info;
        GetSystemInfo( &info );
        return static_cast< size_t >( info.dwPageSize );
#else
        return static_cast< size_t >( sysconf( _SC_PAGESIZE ) );
#endif
    }

    struct MappedBytes
    {
        const char* data{ nullptr };
        size_t size{ 0 };
        bool mapped{ false };
    };

#ifdef _WIN32
    MappedBytes map_bytes( const std::filesystem::path& path )
    {
        const auto file =
            CreateFileW( path.wstring().c_str(), GENERIC_READ, FILE_SHARE_READ,
                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
        if( file == INVALID_HANDLE_VALUE )
        {
            return {};
        }
        MappedBytes result;
        LARGE_INTEGER size;
        if( GetFileType( file ) == FILE_TYPE_DISK
            && GetFileSizeEx( file, &size ) && size.QuadPart >= 0 )
        {
            if( size.QuadPart == 0 )
            {
                result.mapped = true;
            }
            else if( const auto mapping = CreateFileMappingW(
                         file, nullptr, PAGE_READONLY, 0, 0, nullptr ) )
            {
                if( const auto* view =
                        MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 ) )
                {
                    result.data = static_cast< const char* >( view );
                    result.size = static_cast< size_t >( size.QuadPart );
                    result.mapped = true;
                }
                CloseHandle( mapping );
            }
        }
        CloseHandle( file );
        return result;
    }

    void unmap_bytes( const char* data, size_t /*size*/ )
    {
        UnmapViewOfFile( data );
    }

    void release_pages( const char* data, size_t size )
    {
        // Unlocking pages that are not locked removes them from the working
        // set
        VirtualUnlock( const_cast< char* >( data ), size );
    }
#else
    MappedBytes map_bytes( const std::filesystem::path& path )
    {
        const auto file = open( path.c_str(), O_RDONLY | O_CLOEXEC );
        if( file < 0 )
        {
            return {};
        }
        MappedBytes result;
        struct stat status;
        if( fstat( file, &status ) == 0 && S_ISREG( status.st_mode )
            && status.st_size >= 0 )
        {
            if( status.st_size == 0 )
            {
                result.mapped = true;
            }
            else
            {
                const auto size = static_cast< size_t >( status.st_size );
                auto* data =
                    mmap( nullptr, size, PROT_READ, MAP_PRIVATE, file, 0 );
                if( data != MAP_FAILED )
                {
                    result.data = static_cast< const char* >( data );
                    result.size = size;
                    result.mapped = true;
                }
            }
        }
        close( file );
        return result;
    }

    void unmap_bytes( const char* data, size_t size )
    {
        munmap( const_cast< char* >( data ), size );
    }

    void release_pages( const char* data, size_t size )
    {
        // Read-only file mapping: the pages are read again from the file if
        // accessed
        madvise( const_cast< char* >( data ), size, MADV_DONTNEED );
    }
#endif

    /*!
     * Read the whole content of a regular file (UTF-8 path) into memory.
     * @return std::nullopt if the file cannot be opened or is not a regular
     * file.
     * @exception OpenGeodeException (data) if the file cannot be read.
     */
    std::optional< geode::detail::ByteBuffer > read_file_content(
        std::string_view filename )
    {
        const auto path = to_path( filename );
        std::error_code error;
        if( !std::filesystem::is_regular_file( path, error ) )
        {
            return std::nullopt;
        }
        std::ifstream file{ path, std::ifstream::binary | std::ifstream::ate };
        if( !file )
        {
            return std::nullopt;
        }
        const auto end = file.tellg();
        geode::OpenGeodeBasicException::check_exception( end >= 0, nullptr,
            geode::OpenGeodeException::TYPE::data,
            "[read_file_content] Cannot read file: ", filename );
        geode::detail::ByteBuffer buffer{ static_cast< size_t >( end ) };
        file.seekg( 0 );
        file.read(
            buffer.data(), static_cast< std::streamsize >( buffer.size() ) );
        geode::OpenGeodeBasicException::check_exception(
            file.gcount() == static_cast< std::streamsize >( buffer.size() ),
            nullptr, geode::OpenGeodeException::TYPE::data,
            "[read_file_content] Cannot read file: ", filename );
        return buffer;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        std::shared_ptr< const MappedFile > MappedFile::map(
            std::string_view filename )
        {
            const auto bytes = map_bytes( to_path( filename ) );
            if( !bytes.mapped )
            {
                return nullptr;
            }
            return std::shared_ptr< const MappedFile >{ new MappedFile{
                bytes.data, bytes.size } };
        }

        MappedFile::MappedFile( const char* data, size_t size )
            : data_{ data }, size_{ size }
        {
        }

        MappedFile::~MappedFile()
        {
            if( data_ != nullptr )
            {
                unmap_bytes( data_, size_ );
            }
        }

        ByteSpan MappedFile::span() const
        {
            return { data_, size_ };
        }

        void MappedFile::release( ByteSpan bytes ) const
        {
            if( bytes.empty() || data_ == nullptr )
            {
                return;
            }
            static const auto page = page_size();
            const auto begin_offset =
                static_cast< size_t >( bytes.data() - data_ );
            const auto end_offset = begin_offset + bytes.size();
            const auto first_page = ( begin_offset + page - 1 ) / page * page;
            const auto last_page = end_offset / page * page;
            if( first_page >= last_page )
            {
                return;
            }
            release_pages( data_ + first_page, last_page - first_page );
        }

        FileContent::FileContent( ByteBuffer buffer )
            : buffer_{ std::move( buffer ) }, bytes_{ buffer_->span() }
        {
        }

        FileContent::FileContent(
            std::shared_ptr< const MappedFile > mapping, ByteSpan bytes )
            : mapping_{ std::move( mapping ) }, bytes_{ bytes }
        {
        }

        FileContent::FileContent( FileContent&& other ) noexcept
            : mapping_{ std::move( other.mapping_ ) },
              buffer_{ std::move( other.buffer_ ) },
              bytes_{ other.bytes_ }
        {
            other.mapping_.reset();
            other.buffer_.reset();
            other.bytes_ = {};
        }

        FileContent::~FileContent()
        {
            if( mapping_ )
            {
                mapping_->release( bytes_ );
            }
        }

        std::optional< FileContent > map_file_content(
            std::string_view filename )
        {
            if( auto mapping = MappedFile::map( filename ) )
            {
                const auto bytes = mapping->span();
                return FileContent{ std::move( mapping ), bytes };
            }
            Logger::debug( "[map_file_content] Cannot map file ", filename,
                ", reading it" );
            if( auto buffer = read_file_content( filename ) )
            {
                return FileContent{ std::move( *buffer ) };
            }
            return std::nullopt;
        }
    } // namespace detail
} // namespace geode
