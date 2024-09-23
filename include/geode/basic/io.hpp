/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/common.hpp>
#include <geode/basic/filename.hpp>
#include <geode/basic/logger.hpp>

namespace geode
{
    class IOFile
    {
    public:
        virtual ~IOFile() = default;

        [[nodiscard]] std::string_view filename() const
        {
            return filename_;
        }

    protected:
        explicit IOFile( std::string_view filename )
            : filename_( expand_predefined_folders( filename ) )
        {
        }

    private:
        std::string filename_;
    };

    template < typename Factory >
    void print_available_extensions( std::string_view type )
    {
        auto message = absl::StrCat( "Available ", type, " extensions:" );
        for( const auto& extension : Factory::list_creators() )
        {
            absl::StrAppend( &message, " ", extension );
        }
        Logger::info( message );
    }
} // namespace geode
