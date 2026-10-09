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
#include <atomic>
#include <geode/basic/logger.hpp>
#include <iostream>

#include <geode/basic/logger_manager.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class Logger::Impl
    {
    public:
        LEVEL level() const
        {
            return level_.load( std::memory_order_relaxed );
        }

        void set_level( LEVEL level )
        {
            level_.store( level, std::memory_order_relaxed );
        }

        void log( LEVEL level, const std::string &message )
        {
            switch( level )
            {
                case LEVEL::trace:
                    log_trace( message );
                    return;
                case LEVEL::debug:
                    log_debug( message );
                    return;
                case LEVEL::info:
                    log_info( message );
                    return;
                case LEVEL::warning:
                    log_warn( message );
                    return;
                case LEVEL::error:
                    log_error( message );
                    return;
                case LEVEL::critical:
                    log_critical( message );
                    return;
                case LEVEL::off:
                    return;
            }
        }

        void log_trace( const std::string &message )
        {
            if( level() <= LEVEL::trace )
            {
                LoggerManager::trace( message );
            }
        }

        void log_debug( const std::string &message )
        {
            if( level() <= LEVEL::debug )
            {
                LoggerManager::debug( message );
            }
        }

        void log_info( const std::string &message )
        {
            if( level() <= LEVEL::info )
            {
                LoggerManager::info( message );
            }
        }

        void log_warn( const std::string &message )
        {
            if( level() <= LEVEL::warning )
            {
                LoggerManager::warning( message );
            }
        }

        void log_error( const std::string &message )
        {
            if( level() <= LEVEL::error )
            {
                LoggerManager::error( message );
            }
        }

        void log_critical( const std::string &message )
        {
            if( level() <= LEVEL::critical )
            {
                LoggerManager::critical( message );
            }
        }

    private:
        std::atomic< LEVEL > level_{ LEVEL::info };
    };

    Logger::Logger() = default;

    Logger::~Logger() = default;

    Logger &Logger::instance()
    {
        static Logger logger;
        return logger;
    }

    Logger::LEVEL Logger::level()
    {
        return instance().impl_->level();
    }

    void Logger::set_level( LEVEL level )
    {
        instance().impl_->set_level( level );
    }

    bool Logger::is_enabled( LEVEL level )
    {
        return level != LEVEL::off && level >= instance().impl_->level();
    }

    void Logger::log_message( LEVEL level, const std::string &message )
    {
        instance().impl_->log( level, message );
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
