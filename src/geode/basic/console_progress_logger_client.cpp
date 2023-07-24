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

#include <geode/basic/console_progress_logger_client.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/timer.h>

namespace geode
{
    class ConsoleProgressLoggerClient::Impl
    {
    public:
        void start( const std::string& message, index_t /*nb_steps */ )
        {
            timer_.reset();
            message_ = message;
            Logger::info( message_, " started" );
        }

        void update( index_t current, index_t nb_steps )
        {
            const auto percent =
                std::floor( static_cast< double >( current ) / nb_steps * 100 );
            Logger::info(
                message_, " ", current, "/", nb_steps, " (", percent, "%)" );
        }

        void completed()
        {
            Logger::info( message_, " completed in ", timer_.duration() );
        }

        void failed()
        {
            Logger::info( message_, " failed in ", timer_.duration() );
        }

    private:
        Timer timer_;
        std::string message_;
    };

    ConsoleProgressLoggerClient::ConsoleProgressLoggerClient() {} // NOLINT

    ConsoleProgressLoggerClient::~ConsoleProgressLoggerClient() {} // NOLINT

    void ConsoleProgressLoggerClient::start(
        const std::string& message, index_t nb_steps )
    {
        impl_->start( message, nb_steps );
    }

    void ConsoleProgressLoggerClient::update(
        index_t current, index_t nb_steps )
    {
        impl_->update( current, nb_steps );
    }

    void ConsoleProgressLoggerClient::completed()
    {
        impl_->completed();
    }

    void ConsoleProgressLoggerClient::failed()
    {
        impl_->failed();
    }
} // namespace geode
