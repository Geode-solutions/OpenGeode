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

#include <geode/basic/progress_logger.h>

#include <absl/synchronization/mutex.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/timer.h>

namespace
{
    static constexpr absl::Duration SLEEP = absl::Seconds( 1 );
}

namespace geode
{
    class ProgressLogger::Impl
    {
    public:
        Impl( std::string message, index_t nb_steps )
            : message_{ std::move( message ) },
              nb_steps_( nb_steps ),
              current_time_{ absl::Now() }
        {
            Logger::info( message_, " started" );
        }

        ~Impl()
        {
            const auto status = current_ == nb_steps_ ? "completed" : "failed";
            Logger::info( message_, " ", status, " in ", timer_.duration() );
        }

        void increment()
        {
            absl::MutexLock locking{ &lock_ };
            current_++;
            auto now = absl::Now();
            if( now - current_time_ > SLEEP )
            {
                current_time_ = now;
                const auto percent = std::floor( current_ / nb_steps_ * 100 );
                Logger::info( message_, " ", current_, "/", nb_steps_, " (",
                    percent, "%)" );
            }
        }

        void increment_nb_steps()
        {
            absl::MutexLock locking{ &lock_ };
            nb_steps_++;
        }

    private:
        DEBUG_CONST std::string message_;
        double nb_steps_;
        index_t current_{ 0 };
        DEBUG_CONST Timer timer_;
        absl::Time current_time_;
        absl::Mutex lock_;
    };

    ProgressLogger::ProgressLogger( std::string message, index_t nb_steps )
        : impl_( std::move( message ), nb_steps )
    {
    }

    ProgressLogger::~ProgressLogger() {}

    void ProgressLogger::increment()
    {
        impl_->increment();
    }

    void ProgressLogger::increment_nb_steps()
    {
        impl_->increment_nb_steps();
    }
} // namespace geode
