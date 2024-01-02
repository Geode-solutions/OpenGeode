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

#include <geode/basic/progress_logger_manager.h>

#include <mutex>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/progress_logger_client.h>

namespace geode
{
    class ProgressLoggerManager::Impl
    {
    public:
        void register_client( std::unique_ptr< ProgressLoggerClient >&& client )
        {
            loggers_.emplace_back( std::move( client ) );
        }

        void start( const uuid& progress_logger_id,
            const std::string& message,
            index_t nb_steps )
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            for( auto& logger : loggers_ )
            {
                logger->start( progress_logger_id, message, nb_steps );
            }
        }

        void update( const uuid& progress_logger_id,
            index_t current_step,
            index_t nb_steps )
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            for( auto& logger : loggers_ )
            {
                logger->update( progress_logger_id, current_step, nb_steps );
            }
        }

        void completed( const uuid& progress_logger_id )
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            for( auto& logger : loggers_ )
            {
                logger->completed( progress_logger_id );
            }
        }

        void failed( const uuid& progress_logger_id )
        {
            const std::lock_guard< std::mutex > locking{ lock_ };
            for( auto& logger : loggers_ )
            {
                logger->failed( progress_logger_id );
            }
        }

    private:
        std::vector< std::unique_ptr< ProgressLoggerClient > > loggers_;
        std::mutex lock_;
    };

    ProgressLoggerManager::ProgressLoggerManager() = default;

    ProgressLoggerManager::~ProgressLoggerManager() = default;

    void ProgressLoggerManager::register_client(
        std::unique_ptr< ProgressLoggerClient >&& client )
    {
        instance().impl_->register_client( std::move( client ) );
    }

    void ProgressLoggerManager::start( const uuid& progress_logger_id,
        const std::string& message,
        index_t nb_steps )
    {
        instance().impl_->start( progress_logger_id, message, nb_steps );
    }

    void ProgressLoggerManager::update(
        const uuid& progress_logger_id, index_t current_step, index_t nb_steps )
    {
        instance().impl_->update( progress_logger_id, current_step, nb_steps );
    }

    void ProgressLoggerManager::completed( const uuid& progress_logger_id )
    {
        instance().impl_->completed( progress_logger_id );
    }

    void ProgressLoggerManager::failed( const uuid& progress_logger_id )
    {
        instance().impl_->failed( progress_logger_id );
    }

    ProgressLoggerManager& ProgressLoggerManager::instance()
    {
        static ProgressLoggerManager manager;
        return manager;
    }
} // namespace geode
