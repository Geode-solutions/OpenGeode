/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/logger.h>

#include <absl/container/flat_hash_map.h>

// clang-format off
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
// clang-format on

#include <geode/basic/mapping.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class Logger::Impl
    {
    public:
        Impl() : logger_impl_( spdlog::stdout_color_mt( "console" ) )
        {
            mapping_.reserve( spdlog::level::level_enum::n_levels );
            mapping_.map( Level::trace, spdlog::level::level_enum::trace );
            mapping_.map( Level::debug, spdlog::level::level_enum::debug );
            mapping_.map( Level::info, spdlog::level::level_enum::info );
            mapping_.map( Level::warn, spdlog::level::level_enum::warn );
            mapping_.map( Level::err, spdlog::level::level_enum::err );
            mapping_.map(
                Level::critical, spdlog::level::level_enum::critical );
            mapping_.map( Level::off, spdlog::level::level_enum::off );
        }

        Level level()
        {
            return mapping_.out2in( spdlog::get_level() );
        }

        void set_level( Level level )
        {
            spdlog::set_level( mapping_.in2out( level ) );
        }

        void log_trace( const std::string &message )
        {
            logger_impl_->trace( message );
        }

        void log_debug( const std::string &message )
        {
            logger_impl_->debug( message );
        }

        void log_info( const std::string &message )
        {
            logger_impl_->info( message );
        }

        void log_warn( const std::string &message )
        {
            logger_impl_->warn( message );
        }

        void log_error( const std::string &message )
        {
            logger_impl_->error( message );
        }

        void log_critical( const std::string &message )
        {
            logger_impl_->critical( message );
        }

    private:
        std::shared_ptr< spdlog::logger > logger_impl_;
        BijectiveMapping< Level, spdlog::level::level_enum > mapping_;
    };

    Logger::Logger()
    {
        impl_->set_level( Level::trace );
    }

    Logger::~Logger() {}

    Logger &Logger::instance()
    {
        static Logger logger;
        return logger;
    }

    Logger::Level Logger::level()
    {
        return instance().impl_->level();
    }

    void Logger::set_level( Level level )
    {
        instance().impl_->set_level( level );
    }

    void Logger::log_trace( const std::string &message )
    {
        instance().impl_->log_trace( message );
    }

    void Logger::log_debug( const std::string &message )
    {
        instance().impl_->log_debug( message );
    }

    void Logger::log_info( const std::string &message )
    {
        instance().impl_->log_info( message );
    }

    void Logger::log_warn( const std::string &message )
    {
        instance().impl_->log_warn( message );
    }

    void Logger::log_error( const std::string &message )
    {
        instance().impl_->log_error( message );
    }

    void Logger::log_critical( const std::string &message )
    {
        instance().impl_->log_critical( message );
    }
} // namespace geode
