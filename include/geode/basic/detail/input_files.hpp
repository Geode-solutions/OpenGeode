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

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <geode/basic/common.hpp>
#include <geode/basic/detail/file_content.hpp>
#include <geode/basic/pimpl.hpp>

namespace geode::detail
{
    /*!
     * Read-only set of named files making up a saved object (e.g. a model):
     * either the files of a directory or the entries of an archive.
     * All the const functions can be called concurrently.
     */
    class opengeode_basic_api InputFiles
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( InputFiles );

    public:
        virtual ~InputFiles();

        /*!
         * Return true if there is a file of this name.
         */
        [[nodiscard]] virtual bool has( std::string_view name ) const = 0;

        /*!
         * Names of every file (directories excluded), in a deterministic
         * order.
         */
        [[nodiscard]] virtual std::vector< std::string > names() const = 0;

        /*!
         * Bytes of a file, without copy when possible.
         * @exception OpenGeodeException (data) if the file is missing, cannot
         * be read, or is corrupted.
         */
        [[nodiscard]] virtual FileContent content(
            std::string_view name ) const = 0;

        /*!
         * Path of a file on disk, if the files are in a directory.
         * @return std::nullopt for an entry of an archive.
         */
        [[nodiscard]] virtual std::optional< std::string > path(
            std::string_view name ) const = 0;

        /*!
         * Description of a file for messages: its path, or the archive path
         * followed by the entry name.
         */
        [[nodiscard]] virtual std::string location(
            std::string_view name ) const = 0;

    protected:
        InputFiles() = default;
    };

    /*!
     * Files of a directory (not recursive): files are mapped in memory when
     * read.
     */
    class opengeode_basic_api DirectoryInputFiles final : public InputFiles
    {
    public:
        explicit DirectoryInputFiles( std::string_view directory );
        ~DirectoryInputFiles() final;

        [[nodiscard]] bool has( std::string_view name ) const final;

        [[nodiscard]] std::vector< std::string > names() const final;

        [[nodiscard]] FileContent content( std::string_view name ) const final;

        [[nodiscard]] std::optional< std::string > path(
            std::string_view name ) const final;

        [[nodiscard]] std::string location( std::string_view name ) const final;

    private:
        std::string directory_;
    };

    /*!
     * Entries of a zip archive, read in place: the archive is mapped in
     * memory (or read entry by entry if it cannot be mapped), stored entries
     * are returned without copy and deflated entries are decompressed.
     * The CRC-32 of an entry is checked each time it is read.
     */
    class opengeode_basic_api ArchiveInputFiles final : public InputFiles
    {
    public:
        /*!
         * Read the list of entries of a zip archive (UTF-8 path).
         * @exception OpenGeodeException (data) if the file is not a readable
         * zip archive or holds an entry with an unsafe path (absolute or
         * with ".."), encrypted or compressed by another method than DEFLATE.
         */
        explicit ArchiveInputFiles( std::string_view filename );
        ~ArchiveInputFiles() final;

        [[nodiscard]] bool has( std::string_view name ) const final;

        [[nodiscard]] std::vector< std::string > names() const final;

        /*!
         * @exception OpenGeodeException (data) if the entry is missing,
         * cannot be read or decompressed, or does not match its CRC-32.
         */
        [[nodiscard]] FileContent content( std::string_view name ) const final;

        [[nodiscard]] std::optional< std::string > path(
            std::string_view name ) const final;

        [[nodiscard]] std::string location( std::string_view name ) const final;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    /*!
     * A file holding the given bytes, in a new directory of the temporary
     * directory of the system. Both are removed on destruction.
     */
    class opengeode_basic_api TemporaryFile
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( TemporaryFile );

    public:
        /*!
         * @param[in] name Name of the file (without directory).
         * @exception OpenGeodeException (internal) if there is no temporary
         * directory, or if the directory or the file cannot be created.
         */
        TemporaryFile( std::string_view name, ByteSpan content );
        ~TemporaryFile();

        [[nodiscard]] const std::string& path() const;

    private:
        std::string directory_;
        std::string path_;
    };
} // namespace geode::detail
