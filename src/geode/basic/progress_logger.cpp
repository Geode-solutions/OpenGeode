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

#include <geode/basic/progress_logger.hpp>

#include <atomic>
#include <chrono>
#include <exception>

#include <absl/time/time.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/progress_logger_manager.hpp>
#include <geode/basic/uuid.hpp>

namespace geode
{
    class ProgressLogger::Impl
    {
        using Clock = std::chrono::steady_clock;

    public:
        Impl(
            Logger::LEVEL level, const std::string& message, index_t nb_steps )
            : nb_steps_( nb_steps ),
              next_refresh_{ now() + refresh_interval_.load() },
              level_( level ),
              nb_uncaught_exceptions_{ std::uncaught_exceptions() }
        {
            ProgressLoggerManager::start( id_, level, message, nb_steps );
        }

        ~Impl()
        {
            try
            {
                if( current_ == nb_steps_
                    && std::uncaught_exceptions() <= nb_uncaught_exceptions_ )
                {
                    ProgressLoggerManager::completed( id_, level_ );
                }
                else
                {
                    ProgressLoggerManager::failed( id_, level_ );
                }
            }
            catch( ... )
            {
                try
                {
                    geode_lippincott();
                }
                catch( ... )
                {
                }
            }
        }

        index_t increment( index_t nb_increments )
        {
            const auto current =
                current_.fetch_add( nb_increments, std::memory_order_relaxed )
                + nb_increments;
            const auto current_time = now();
            auto next_refresh = next_refresh_.load( std::memory_order_relaxed );
            if( current_time >= next_refresh
                && next_refresh_.compare_exchange_strong( next_refresh,
                    current_time
                        + refresh_interval_.load( std::memory_order_relaxed ),
                    std::memory_order_relaxed ) )
            {
                ProgressLoggerManager::update( id_, level_, current,
                    nb_steps_.load( std::memory_order_relaxed ) );
            }
            return current;
        }

        index_t increment_nb_steps( index_t nb_steps )
        {
            return nb_steps_.fetch_add( nb_steps, std::memory_order_relaxed )
                   + nb_steps;
        }

        void set_refresh_interval( absl::Duration refresh_interval )
        {
            refresh_interval_.store(
                absl::ToInt64Nanoseconds( refresh_interval ),
                std::memory_order_relaxed );
        }

    private:
        static std::int64_t now()
        {
            return std::chrono::duration_cast< std::chrono::nanoseconds >(
                Clock::now().time_since_epoch() )
                .count();
        }

    private:
        uuid id_;
        std::atomic< index_t > nb_steps_;
        std::atomic< index_t > current_{ 0 };
        std::atomic< std::int64_t > refresh_interval_{ absl::ToInt64Nanoseconds(
            absl::Seconds( 1 ) ) };
        std::atomic< std::int64_t > next_refresh_;
        Logger::LEVEL level_;
        int nb_uncaught_exceptions_;
    };

    ProgressLogger::ProgressLogger(
        Logger::LEVEL level, const std::string& message, index_t nb_steps )
        : impl_( level, message, nb_steps )
    {
    }

    ProgressLogger::~ProgressLogger() = default;

    index_t ProgressLogger::increment()
    {
        return impl_->increment( 1 );
    }

    index_t ProgressLogger::increment( index_t nb_increments )
    {
        return impl_->increment( nb_increments );
    }

    index_t ProgressLogger::increment_nb_steps()
    {
        return impl_->increment_nb_steps( 1 );
    }

    index_t ProgressLogger::increment_nb_steps( index_t nb_steps )
    {
        return impl_->increment_nb_steps( nb_steps );
    }

    void ProgressLogger::set_refresh_interval( absl::Duration refresh_interval )
    {
        impl_->set_refresh_interval( std::move( refresh_interval ) );
    }
} // namespace geode
