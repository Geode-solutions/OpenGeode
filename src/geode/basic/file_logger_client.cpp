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

#include <geode/basic/file_logger_client.hpp>

// clang-format off
#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>
// clang-format on

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class FileLoggerClient::Impl
    {
    public:
        Impl( std::string_view file_path )
            : logger_impl_( spdlog::basic_logger_mt(
                  "basic_logger", to_string( file_path ) ) )
        {
            spdlog::set_level( spdlog::level::level_enum::trace );
        }

        void always_flush()
        {
            logger_impl_->flush_on( spdlog::level::level_enum::trace );
        }

        void trace( const std::string &message )
        {
            logger_impl_->trace( message );
        }

        void debug( const std::string &message )
        {
            logger_impl_->debug( message );
        }

        void info( const std::string &message )
        {
            logger_impl_->info( message );
        }

        void warn( const std::string &message )
        {
            logger_impl_->warn( message );
        }

        void error( const std::string &message )
        {
            logger_impl_->error( message );
        }

        void critical( const std::string &message )
        {
            logger_impl_->critical( message );
        }

    private:
        std::shared_ptr< spdlog::logger > logger_impl_;
    };

    FileLoggerClient::FileLoggerClient( std::string_view file_path )
        : impl_{ file_path }
    {
    }

    FileLoggerClient::~FileLoggerClient() = default;

    void FileLoggerClient::always_flush()
    {
        impl_->always_flush();
    }

    void FileLoggerClient::trace( const std::string &message )
    {
        impl_->trace( message );
    }

    void FileLoggerClient::debug( const std::string &message )
    {
        impl_->debug( message );
    }

    void FileLoggerClient::info( const std::string &message )
    {
        impl_->info( message );
    }

    void FileLoggerClient::warn( const std::string &message )
    {
        impl_->warn( message );
    }

    void FileLoggerClient::error( const std::string &message )
    {
        impl_->error( message );
    }

    void FileLoggerClient::critical( const std::string &message )
    {
        impl_->critical( message );
    }
} // namespace geode
