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

#include <geode/basic/progress_logger_manager.h>

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

        void start( const std::string& message, index_t nb_steps )
        {
            for( auto& logger : loggers_ )
            {
                logger->start( message, nb_steps );
            }
        }

        void update( index_t current_step, index_t nb_steps )
        {
            for( auto& logger : loggers_ )
            {
                logger->update( current_step, nb_steps );
            }
        }

        void completed()
        {
            for( auto& logger : loggers_ )
            {
                logger->completed();
            }
        }

        void failed()
        {
            for( auto& logger : loggers_ )
            {
                logger->failed();
            }
        }

    private:
        std::vector< std::unique_ptr< ProgressLoggerClient > > loggers_;
    };

    ProgressLoggerManager::ProgressLoggerManager() {} // NOLINT

    ProgressLoggerManager::~ProgressLoggerManager() {} // NOLINT

    void ProgressLoggerManager::register_client(
        std::unique_ptr< ProgressLoggerClient >&& client )
    {
        instance().impl_->register_client( std::move( client ) );
    }

    void ProgressLoggerManager::start(
        const std::string& message, index_t nb_steps )
    {
        instance().impl_->start( message, nb_steps );
    }

    void ProgressLoggerManager::update( index_t current_step, index_t nb_steps )
    {
        instance().impl_->update( current_step, nb_steps );
    }

    void ProgressLoggerManager::completed()
    {
        instance().impl_->completed();
    }

    void ProgressLoggerManager::failed()
    {
        instance().impl_->failed();
    }

    ProgressLoggerManager& ProgressLoggerManager::instance()
    {
        static ProgressLoggerManager manager;
        return manager;
    }
} // namespace geode
