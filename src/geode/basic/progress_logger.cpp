/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <absl/time/clock.h>
#include <absl/time/time.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

namespace
{
    static constexpr absl::Duration SLEEP = absl::Seconds( 1 );
}

namespace geode
{
    class ProgressLogger::Impl
    {
    public:
        Impl( std::string message, index_t max_number )
            : message_{ std::move( message ) },
              max_number_( max_number ),
              start_{ absl::Now() }
        {
        }

        ~Impl()
        {
            Logger::info( message_, " complete" );
        }

        void increment()
        {
            current_++;
            auto now = absl::Now();
            if( now - start_ > SLEEP )
            {
                start_ = now;
                const auto percent = std::floor( current_ / max_number_ * 100 );
                Logger::info( message_, " ", current_, "/", max_number_, " (",
                    percent, "%)" );
            }
        }

    private:
        std::string message_;
        double max_number_;
        index_t current_{ 0 };
        absl::Time start_;
    };

    ProgressLogger::ProgressLogger( std::string message, index_t max_number )
        : impl_( std::move( message ), max_number )
    {
    }

    ProgressLogger::~ProgressLogger() {}

    void ProgressLogger::increment()
    {
        impl_->increment();
    }
} // namespace geode
