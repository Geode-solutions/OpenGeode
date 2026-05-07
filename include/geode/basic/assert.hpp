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

#include <any>
#include <array>
#include <optional>
#include <stdexcept>
#include <string>

#include <absl/base/optimization.h>
#include <absl/debugging/stacktrace.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/opengeode_basic_export.hpp>

namespace geode
{
    /*!
     * OpenGeode exception class.
     * Example:
     *       throw OpenGeodeException{ "Error while loading the BRep" };
     *
     *       try {
     *          ...
     *       }
     *       catch( ... )
     *       {
     *           return geode_lippincott();
     *       }
     */
    class opengeode_basic_api OpenGeodeException : public std::runtime_error
    {
        static constexpr int MAX_STACK_DEPTH = 10;
        static constexpr int NB_SKIPPED_STACKS = 1;
        static constexpr int SYMBOL_SIZE = 1024;

    public:
        enum struct TYPE : std::uint8_t
        {
            data,
            internal,
            result
        };

        OpenGeodeException( OpenGeodeException&& ) = default;
        OpenGeodeException( const OpenGeodeException& ) = delete;
        OpenGeodeException& operator=( const OpenGeodeException& ) = delete;
        OpenGeodeException& operator=( OpenGeodeException&& ) = default;

        ~OpenGeodeException() noexcept override;

        [[nodiscard]] TYPE type() const
        {
            return type_;
        }

        [[nodiscard]] std::string_view type_name() const;

        [[nodiscard]] std::string_view project() const
        {
            return project_;
        }

        [[nodiscard]] std::string_view library() const
        {
            return library_;
        }

        [[nodiscard]] const std::any& data() const
        {
            return data_;
        }

        [[nodiscard]] std::string stack_trace() const;

        [[nodiscard]] bool has_parent() const
        {
            return parent_ != nullptr;
        }

        [[nodiscard]] const OpenGeodeException& parent() const
        {
            return *parent_;
        }

        void set_parent( OpenGeodeException&& parent )
        {
            parent_ =
                std::make_unique< OpenGeodeException >( std::move( parent ) );
        }

        [[nodiscard]] std::string string() const;

    protected:
        template < typename... Args >
        explicit OpenGeodeException( std::string project,
            std::string library,
            std::any data,
            TYPE type,
            const Args&... message )
            : std::runtime_error{ absl::StrCat( message... ) },
              type_{ type },
              project_{ std::move( project ) },
              library_{ std::move( library ) },
              data_{ std::move( data ) }
        {
            stack_.fill( nullptr );
#ifndef NDEBUG
            stack_size_ = absl::GetStackTrace(
                stack_.data(), MAX_STACK_DEPTH, NB_SKIPPED_STACKS );
#endif
        }

    private:
        TYPE type_;
        std::string project_;
        std::string library_;
        std::any data_;
        std::array< void*, MAX_STACK_DEPTH > stack_;
        int stack_size_{ 0 };
        std::unique_ptr< OpenGeodeException > parent_;
    };

    /*!
     * Try to catch several exception types.
     * Always return 1.
     */
    int opengeode_basic_api geode_lippincott();

    /*!
     * Catch all exceptions and rethrow an OpenGeodeException
     */
    void opengeode_basic_api throw_lippincott();
} // namespace geode
