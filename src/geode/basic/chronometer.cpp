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

#include <geode/basic/chronometer.hpp>

#include <absl/time/clock.h>

#include <geode/basic/pimpl_impl.hpp>

namespace geode
{
    class Chronometer::Impl
    {
    public:
        void start()
        {
            start_time_ = absl::Now();
        }

        void stop()
        {
            duration_ += absl::Now() - start_time_;
        }

        [[nodiscard]] absl::Duration raw_duration() const
        {
            return duration_;
        }

        [[nodiscard]] std::string duration() const
        {
            return absl::FormatDuration( raw_duration() );
        }

        void reset()
        {
            duration_ = absl::ZeroDuration();
        }

    private:
        absl::Time start_time_;
        absl::Duration duration_;
    };

    Chronometer::Chronometer() = default;
    Chronometer::Chronometer( Chronometer&& other ) noexcept = default;
    Chronometer::~Chronometer() = default;

    void Chronometer::start()
    {
        impl_->start();
    }

    void Chronometer::stop()
    {
        impl_->stop();
    }

    [[nodiscard]] absl::Duration Chronometer::raw_duration() const
    {
        return impl_->raw_duration();
    }

    [[nodiscard]] std::string Chronometer::duration() const
    {
        return impl_->duration();
    }

    void Chronometer::reset()
    {
        impl_->reset();
    }
} // namespace geode