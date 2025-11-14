/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/basic/common.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/percentage.hpp>

namespace geode
{
    struct AdditionalFile
    {
        AdditionalFile( std::string filename_in, bool is_missing_in )
            : filename{ std::move( filename_in ) }, is_missing{ is_missing_in }
        {
        }

        std::string filename;
        bool is_missing;
    };

    struct AdditionalFiles
    {
        [[nodiscard]] bool has_additional_files() const
        {
            const auto check_missing = []( const auto& file ) {
                return file.is_missing;
            };
            return absl::c_find_if( optional_files, check_missing )
                       != optional_files.end()
                   || absl::c_find_if( mandatory_files, check_missing )
                          != mandatory_files.end();
        }

        std::vector< AdditionalFile > optional_files;
        std::vector< AdditionalFile > mandatory_files;
    };

    template < typename Object, typename... Args >
    class Input : public IOFile
    {
    public:
        using InputData = Object;

        [[nodiscard]] virtual AdditionalFiles additional_files() const = 0;

        [[nodiscard]] virtual index_t object_priority() const = 0;

        [[nodiscard]] virtual Percentage is_loadable() const = 0;

        [[nodiscard]] virtual Object read( const Args&... args ) = 0;

        ~Input()
        {
            if( inspect_required_ )
            {
                geode::Logger::warn(
                    "[Input] The file loader notified INCONSISTENCIES in the "
                    "given data file. In consequence, the loaded structure is "
                    "likely BROKEN, and there is NO GUARANTEE that any further "
                    "operation will work on it without repairing it first. We "
                    "highly recommend inspecting the data to make sure these "
                    "inconsistencies do not impact your following work. To do "
                    "so, you can for example use the Open-Source "
                    "OpenGeode-Inspector or the online free tool: "
                    "https://geode-solutions.com/tools/validity_checker" );
            }
        }

    protected:
        explicit Input( std::string_view filename ) : IOFile( filename ) {}

        void need_to_inspect_result()
        {
            inspect_required_ = true;
        }

    private:
        bool inspect_required_{ false };
    };
} // namespace geode
