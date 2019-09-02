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

#include <geode/basic/common.h>

#include <geode/basic/pimpl.h>

namespace geode
{
    class opengeode_basic_api ZipFile
    {
    public:
        ZipFile(
            const std::string& file, const std::string& archive_temp_filename );
        ~ZipFile();

        void archive_files( const std::vector< std::string >& files ) const;

        void archive_file( const std::string& file ) const;

        std::string directory() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    class opengeode_basic_api UnzipFile
    {
    public:
        UnzipFile(
            const std::string& file, std::string unarchive_temp_filename );
        ~UnzipFile();

        void extract_all() const;

        std::string directory() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

} // namespace geode