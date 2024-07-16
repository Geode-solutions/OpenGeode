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

#include <geode/basic/console_progress_logger_client.hpp>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/timer.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    class ConsoleProgressLoggerClient::Impl
    {
        struct Info
        {
            explicit Info( std::string message_in )
                : message{ std::move( message_in ) }
            {
            }
            Info( Info&& ) = default;

            Timer timer;
            std::string message;
        };

    public:
        void start( const uuid& progress_logger_id,
            const std::string& message,
            index_t /*nb_steps */ )
        {
            info_.emplace( progress_logger_id, message );
            Logger::info( message, " started" );
        }

        void update(
            const uuid& progress_logger_id, index_t current, index_t nb_steps )
        {
            const auto percent =
                std::floor( static_cast< double >( current ) / nb_steps * 100 );
            Logger::info( info_.at( progress_logger_id ).message, " ", current,
                "/", nb_steps, " (", percent, "%)" );
        }

        void completed( const uuid& progress_logger_id )
        {
            const auto& info = info_.at( progress_logger_id );
            Logger::info(
                info.message, " completed in ", info.timer.duration() );
            info_.erase( progress_logger_id );
        }

        void failed( const uuid& progress_logger_id )
        {
            const auto& info = info_.at( progress_logger_id );
            Logger::info( info.message, " failed in ", info.timer.duration() );
            info_.erase( progress_logger_id );
        }

    private:
        absl::flat_hash_map< uuid, Info > info_;
    };

    ConsoleProgressLoggerClient::ConsoleProgressLoggerClient() = default;

    ConsoleProgressLoggerClient::~ConsoleProgressLoggerClient() = default;

    void ConsoleProgressLoggerClient::start( const uuid& progress_logger_id,
        const std::string& message,
        index_t nb_steps )
    {
        impl_->start( progress_logger_id, message, nb_steps );
    }

    void ConsoleProgressLoggerClient::update(
        const uuid& progress_logger_id, index_t current, index_t nb_steps )
    {
        impl_->update( progress_logger_id, current, nb_steps );
    }

    void ConsoleProgressLoggerClient::completed(
        const uuid& progress_logger_id )
    {
        impl_->completed( progress_logger_id );
    }

    void ConsoleProgressLoggerClient::failed( const uuid& progress_logger_id )
    {
        impl_->failed( progress_logger_id );
    }
} // namespace geode
