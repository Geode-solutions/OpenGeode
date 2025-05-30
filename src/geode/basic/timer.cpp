/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/basic/timer.hpp>

#include <absl/time/clock.h>

#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class Timer::Impl
    {
    public:
        Impl()
        {
            reset();
        }

        absl::Duration raw_duration() const
        {
            return absl::Now() - start_time_;
        }

        std::string duration() const
        {
            return absl::FormatDuration( raw_duration() );
        }

        void reset()
        {
            start_time_ = absl::Now();
        }

    private:
        absl::Time start_time_;
    };

    Timer::Timer() = default;

    Timer::Timer( Timer&& ) noexcept = default;

    Timer::~Timer() = default;

    absl::Duration Timer::raw_duration() const
    {
        return impl_->raw_duration();
    }

    std::string Timer::duration() const
    {
        return impl_->duration();
    }

    void Timer::reset()
    {
        return impl_->reset();
    }
} // namespace geode