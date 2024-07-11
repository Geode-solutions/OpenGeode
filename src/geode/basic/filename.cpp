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

#include <geode/basic/filename.h>

#include <string_view>

#include <ghc/filesystem.hpp>

namespace geode
{
    std::string filename_with_extension( std::string_view path )
    {
        const ghc::filesystem::path filepath{ to_string( path ) };
        return filepath.filename().string();
    }

    std::string filename_without_extension( std::string_view path )
    {
        const ghc::filesystem::path filepath{ to_string( path ) };
        return filepath.filename().replace_extension( "" ).string();
    }

    std::string filepath_without_extension( std::string_view path )
    {
        ghc::filesystem::path filepath{ to_string( path ) };
        return filepath.replace_extension( "" ).string();
    }

    std::string filepath_without_filename( std::string_view path )
    {
        ghc::filesystem::path filepath{ to_string( path ) };
        return filepath.replace_filename( "" ).string();
    }

    std::string_view extension_from_filename( std::string_view filename )
    {
        return filename.substr( filename.find_last_of( '.' ) + 1 );
    }

    std::string expand_predefined_folders( std::string_view path )
    {
        if( path.empty() )
        {
            return to_string( path );
        }
        if( path[0] == '~' )
        {
            return absl::StrCat( std::getenv( "HOME" ), path.substr( 1 ) );
        }
        return to_string( path );
    }
} // namespace geode
