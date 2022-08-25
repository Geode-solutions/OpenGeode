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

#include <geode/basic/logger_manager.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class Logger::Impl
    {
    public:
        Level level() const
        {
            return level_;
        }

        void set_level( Level level )
        {
            level_ = level;
        }

        void log_trace( const std::string &message )
        {
            if( level_ <= Level::trace )
            {
                LoggerManager::trace( message );
            }
        }

        void log_debug( const std::string &message )
        {
            if( level_ <= Level::debug )
            {
                LoggerManager::debug( message );
            }
        }

        void log_info( const std::string &message )
        {
            if( level_ <= Level::info )
            {
                LoggerManager::info( message );
            }
        }

        void log_warn( const std::string &message )
        {
            if( level_ <= Level::warn )
            {
                LoggerManager::warn( message );
            }
        }

        void log_error( const std::string &message )
        {
            if( level_ <= Level::err )
            {
                LoggerManager::error( message );
            }
        }

        void log_critical( const std::string &message )
        {
            if( level_ <= Level::critical )
            {
                LoggerManager::critical( message );
            }
        }

    private:
        Level level_{ Level::trace };
    };

    Logger::Logger() {}

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
