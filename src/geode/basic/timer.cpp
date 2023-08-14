/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/timer.h>

#include <absl/time/clock.h>

#include <geode/basic/pimpl_impl.h>

namespace geode
{
    class Timer::Impl
    {
    public:
        Impl()
        {
            reset();
        }

        std::string duration() const
        {
            return absl::FormatDuration( absl::Now() - start_time_ );
        }

        void reset()
        {
            start_time_ = absl::Now();
        }

    private:
        absl::Time start_time_;
    };

    Timer::Timer() {} // NOLINT

    Timer::Timer( Timer&& other ) noexcept : impl_{ std::move( other.impl_ ) }
    {
    }

    Timer::~Timer() {} // NOLINT

    std::string Timer::duration() const
    {
        return impl_->duration();
    }

    void Timer::reset()
    {
        return impl_->reset();
    }
} // namespace geode