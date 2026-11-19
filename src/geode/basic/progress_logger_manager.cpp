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

#include <geode/basic/progress_logger_manager.hpp>

#include <mutex>

#include <geode/basic/console_progress_logger_client.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/progress_logger_client.hpp>

namespace geode
{
    class ProgressLoggerManager::Impl
    {
        struct RegisteredClient
        {
            std::unique_ptr< ProgressLoggerClient > client;
            bool is_console;
        };

    public:
        void register_client( std::unique_ptr< ProgressLoggerClient >&& client )
        {
            const auto is_console =
                dynamic_cast< const ConsoleProgressLoggerClient* >(
                    client.get() )
                != nullptr;
            const std::lock_guard< std::mutex > locking{ lock_ };
            loggers_.emplace_back(
                RegisteredClient{ std::move( client ), is_console } );
        }

        void set_console_output( bool enabled )
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            console_output_ = enabled;
        }

        bool is_console_output_enabled()
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            return console_output_;
        }

        void start( const uuid& progress_logger_id,
            Logger::LEVEL level,
            const std::string& message,
            index_t nb_steps )
        {
            dispatch( [&]( ProgressLoggerClient& logger ) {
                logger.start( progress_logger_id, level, message, nb_steps );
            } );
        }

        void update( const uuid& progress_logger_id,
            Logger::LEVEL level,
            index_t current_step,
            index_t nb_steps )
        {
            dispatch( [&]( ProgressLoggerClient& logger ) {
                logger.update(
                    progress_logger_id, level, current_step, nb_steps );
            } );
        }

        void completed( const uuid& progress_logger_id, Logger::LEVEL level )
        {
            dispatch( [&]( ProgressLoggerClient& logger ) {
                logger.completed( progress_logger_id, level );
            } );
        }

        void failed( const uuid& progress_logger_id, Logger::LEVEL level )
        {
            dispatch( [&]( ProgressLoggerClient& logger ) {
                logger.failed( progress_logger_id, level );
            } );
        }

    private:
        template < typename Function >
        void dispatch( const Function& function )
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            for( auto& logger : loggers_ )
            {
                if( console_output_ || !logger.is_console )
                {
                    function( *logger.client );
                }
            }
        }

    private:
        std::vector< RegisteredClient > loggers_;
        std::mutex lock_;
        bool console_output_{ true };
    };

    ProgressLoggerManager::ProgressLoggerManager() = default;

    ProgressLoggerManager::~ProgressLoggerManager() = default;

    void ProgressLoggerManager::register_client(
        std::unique_ptr< ProgressLoggerClient >&& client )
    {
        instance().impl_->register_client( std::move( client ) );
    }

    void ProgressLoggerManager::set_console_output( bool enabled )
    {
        instance().impl_->set_console_output( enabled );
    }

    bool ProgressLoggerManager::is_console_output_enabled()
    {
        return instance().impl_->is_console_output_enabled();
    }

    void ProgressLoggerManager::start( const uuid& progress_logger_id,
        Logger::LEVEL level,
        const std::string& message,
        index_t nb_steps )
    {
        instance().impl_->start( progress_logger_id, level, message, nb_steps );
    }

    void ProgressLoggerManager::update( const uuid& progress_logger_id,
        Logger::LEVEL level,
        index_t current_step,
        index_t nb_steps )
    {
        instance().impl_->update(
            progress_logger_id, level, current_step, nb_steps );
    }

    void ProgressLoggerManager::completed(
        const uuid& progress_logger_id, Logger::LEVEL level )
    {
        instance().impl_->completed( progress_logger_id, level );
    }

    void ProgressLoggerManager::failed(
        const uuid& progress_logger_id, Logger::LEVEL level )
    {
        instance().impl_->failed( progress_logger_id, level );
    }

    ProgressLoggerManager& ProgressLoggerManager::instance()
    {
        static ProgressLoggerManager manager;
        return manager;
    }
} // namespace geode
