/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/console_logger_client.h>

// clang-format off
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
// clang-format on

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class ConsoleLoggerClient::Impl
    {
    public:
        Impl() : logger_impl_( spdlog::stdout_color_mt( "console" ) )
        {
            spdlog::set_level( spdlog::level::level_enum::trace );
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

    ConsoleLoggerClient::ConsoleLoggerClient() {}

    ConsoleLoggerClient::~ConsoleLoggerClient() {}

    void ConsoleLoggerClient::trace( const std::string &message )
    {
        impl_->trace( message );
    }

    void ConsoleLoggerClient::debug( const std::string &message )
    {
        impl_->debug( message );
    }

    void ConsoleLoggerClient::info( const std::string &message )
    {
        impl_->info( message );
    }

    void ConsoleLoggerClient::warn( const std::string &message )
    {
        impl_->warn( message );
    }

    void ConsoleLoggerClient::error( const std::string &message )
    {
        impl_->error( message );
    }

    void ConsoleLoggerClient::critical( const std::string &message )
    {
        impl_->critical( message );
    }
} // namespace geode
