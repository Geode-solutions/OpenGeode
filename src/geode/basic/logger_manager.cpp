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

#include <geode/basic/logger_manager.hpp>

#include <geode/basic/logger_client.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class LoggerManager::Impl
    {
    public:
        void register_client( std::unique_ptr< LoggerClient > &&client )
        {
            loggers_.emplace_back( std::move( client ) );
        }

        void trace( const std::string &message )
        {
            for( auto &logger : loggers_ )
            {
                logger->trace( message );
            }
        }

        void debug( const std::string &message )
        {
            for( auto &logger : loggers_ )
            {
                logger->debug( message );
            }
        }

        void info( const std::string &message )
        {
            for( auto &logger : loggers_ )
            {
                logger->info( message );
            }
        }

        void warn( const std::string &message )
        {
            for( auto &logger : loggers_ )
            {
                logger->warn( message );
            }
        }

        void error( const std::string &message )
        {
            for( auto &logger : loggers_ )
            {
                logger->error( message );
            }
        }

        void critical( const std::string &message )
        {
            for( auto &logger : loggers_ )
            {
                logger->critical( message );
            }
        }

    private:
        std::vector< std::unique_ptr< LoggerClient > > loggers_;
    };

    LoggerManager::LoggerManager() = default;

    LoggerManager::~LoggerManager() = default;

    void LoggerManager::register_client(
        std::unique_ptr< LoggerClient > &&client )
    {
        instance().impl_->register_client( std::move( client ) );
    }

    void LoggerManager::trace( const std::string &message )
    {
        instance().impl_->trace( message );
    }

    void LoggerManager::debug( const std::string &message )
    {
        instance().impl_->debug( message );
    }

    void LoggerManager::info( const std::string &message )
    {
        instance().impl_->info( message );
    }

    void LoggerManager::warn( const std::string &message )
    {
        instance().impl_->warn( message );
    }

    void LoggerManager::error( const std::string &message )
    {
        instance().impl_->error( message );
    }

    void LoggerManager::critical( const std::string &message )
    {
        instance().impl_->critical( message );
    }

    LoggerManager &LoggerManager::instance()
    {
        static LoggerManager manager;
        return manager;
    }

} // namespace geode
