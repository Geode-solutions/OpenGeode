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

#include <absl/synchronization/mutex.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class FileLoggerClient::Impl
    {
    public:
        explicit Impl( std::string_view file_path )
        {
            set_file_path( file_path );
        }

        void always_flush()
        {
            absl::MutexLock lock{ mutex_ };
            logger_impl_->flush_on( spdlog::level::level_enum::trace );
            always_flush_ = true;
        }

        void set_file_path( std::string_view file_path )
        {
            std::shared_ptr< spdlog::logger > logger;
            try
            {
                logger = std::make_shared< spdlog::logger >( "file",
                    std::make_shared< spdlog::sinks::basic_file_sink_mt >(
                        std::string( file_path ) ) );
            }
            catch( const spdlog::spdlog_ex &exception )
            {
                throw OpenGeodeBasicException{ nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[FileLoggerClient] Cannot open log file ", file_path, ": ",
                    exception.what() };
            }
            logger->set_level( spdlog::level::level_enum::trace );
            absl::MutexLock lock{ mutex_ };
            logger->flush_on( always_flush_ ? spdlog::level::level_enum::trace
                                            : spdlog::level::level_enum::warn );
            logger_impl_ = std::move( logger );
        }

        void trace( const std::string &message )
        {
            logger()->trace( message );
        }

        void debug( const std::string &message )
        {
            logger()->debug( message );
        }

        void info( const std::string &message )
        {
            logger()->info( message );
        }

        void warning( const std::string &message )
        {
            logger()->warn( message );
        }

        void error( const std::string &message )
        {
            logger()->error( message );
        }

        void critical( const std::string &message )
        {
            logger()->critical( message );
        }

    private:
        std::shared_ptr< spdlog::logger > logger() const
        {
            absl::ReaderMutexLock lock{ mutex_ };
            return logger_impl_;
        }

    private:
        mutable absl::Mutex mutex_;
        std::shared_ptr< spdlog::logger > logger_impl_{ nullptr };
        bool always_flush_{ false };
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

    void FileLoggerClient::set_file_path( std::string_view file_path )
    {
        impl_->set_file_path( file_path );
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

    void FileLoggerClient::warning( const std::string &message )
    {
        impl_->warning( message );
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
