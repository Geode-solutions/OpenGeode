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

#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>

#include <geode/basic/byte_span.hpp>
#include <geode/basic/common.hpp>

namespace geode::detail
{
    /*!
     * Owned bytes, uninitialized at construction.
     */
    class ByteBuffer
    {
    public:
        explicit ByteBuffer( size_t size )
            : data_{ new char[size] }, size_{ size }
        {
        }

        [[nodiscard]] char* data()
        {
            return data_.get();
        }

        [[nodiscard]] size_t size() const
        {
            return size_;
        }

        [[nodiscard]] ByteSpan span() const
        {
            return { data_.get(), size_ };
        }

    private:
        std::unique_ptr< char[] > data_;
        size_t size_;
    };

    /*!
     * A whole file mapped read-only in memory: its pages are read from the
     * file when first accessed, and belong to the system file cache.
     * The file must not be truncated while it is mapped.
     */
    class opengeode_basic_api MappedFile
    {
    public:
        /*!
         * Map a whole file (UTF-8 path) read-only.
         * @return nullptr if the file cannot be mapped (missing, not a regular
         * file, or mapping not supported).
         */
        [[nodiscard]] static std::shared_ptr< const MappedFile > map(
            std::string_view filename );

        MappedFile( const MappedFile& ) = delete;
        MappedFile& operator=( const MappedFile& ) = delete;
        MappedFile( MappedFile&& ) = delete;
        MappedFile& operator=( MappedFile&& ) = delete;
        ~MappedFile();

        /*!
         * Bytes of the file (empty for an empty file).
         */
        [[nodiscard]] ByteSpan span() const;

        /*!
         * Remove the pages lying entirely within the given bytes of span()
         * from the memory of the process (they stay in the file cache).
         * The bytes stay readable: their pages are mapped again if accessed.
         */
        void release( ByteSpan bytes ) const;

    private:
        MappedFile( const char* data, size_t size );

    private:
        const char* data_;
        size_t size_;
    };

    /*!
     * Bytes of a file: either a slice of a mapped file (kept mapped while
     * this object lives, and released from memory when it is destroyed), or
     * an owned buffer.
     */
    class opengeode_basic_api FileContent
    {
    public:
        explicit FileContent( ByteBuffer buffer );

        /*!
         * @param[in] bytes Bytes inside mapping->span().
         */
        FileContent(
            std::shared_ptr< const MappedFile > mapping, ByteSpan bytes );

        FileContent( const FileContent& ) = delete;
        FileContent& operator=( const FileContent& ) = delete;
        FileContent( FileContent&& other ) noexcept;
        FileContent& operator=( FileContent&& ) = delete;
        ~FileContent();

        [[nodiscard]] ByteSpan span() const
        {
            return bytes_;
        }

    private:
        std::shared_ptr< const MappedFile > mapping_;
        std::optional< ByteBuffer > buffer_;
        ByteSpan bytes_;
    };

    /*!
     * Content of a whole file (UTF-8 path): mapped when possible, otherwise
     * read into memory.
     * @return std::nullopt if the file cannot be opened or is not a regular
     * file.
     * @exception OpenGeodeException (data) if the file cannot be read.
     */
    [[nodiscard]] std::optional< FileContent >
        opengeode_basic_api map_file_content( std::string_view filename );
} // namespace geode::detail
