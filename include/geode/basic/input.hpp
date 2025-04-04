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

namespace geode
{
    template < typename Object, typename... Args >
    class Input : public IOFile
    {
    public:
        using InputData = Object;

        struct MissingFiles
        {
            [[nodiscard]] bool has_missing_files() const
            {
                return !additional_files.empty() || !mandatory_files.empty();
            }

            operator bool() const
            {
                return has_missing_files();
            }

            std::vector< std::string > additional_files;
            std::vector< std::string > mandatory_files;
        };

        [[nodiscard]] virtual MissingFiles check_missing_files() const
        {
            return {};
        }

        [[nodiscard]] virtual bool is_loadable() const
        {
            return true;
        }

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
