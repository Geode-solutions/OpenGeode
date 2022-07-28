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

#include <geode/basic/console_progress_logger_client.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/timer.h>

namespace geode
{
    class ConsoleProgressLoggerClient::Impl
    {
    public:
        void start( absl::string_view message, index_t /*nb_steps */ )
        {
            Logger::info( message, " started" );
        }

        void update(
            absl::string_view message, index_t current, index_t nb_steps )
        {
            const auto percent = std::floor( current / nb_steps * 100 );
            Logger::info(
                message, " ", current, "/", nb_steps, "(", percent, "%)" );
        }

        void end( absl::string_view message, index_t current, index_t nb_steps )
        {
            const auto status = current == nb_steps ? "completed" : "failed";
            Logger::info( message, " ", status, " in ", timer_.duration() );
        }

    private:
        DEBUG_CONST Timer timer_;
    };

    ConsoleProgressLoggerClient::ConsoleProgressLoggerClient(
        const std::string& message )
        : ProgressLoggerClient{ message }
    {
    }

    ConsoleProgressLoggerClient::~ConsoleProgressLoggerClient() {}

    void ConsoleProgressLoggerClient::start( index_t nb_steps )
    {
        impl_->start( message(), nb_steps );
    }

    void ConsoleProgressLoggerClient::update(
        index_t current, index_t nb_steps )
    {
        impl_->update( message(), current, nb_steps );
    }

    void ConsoleProgressLoggerClient::end( index_t current, index_t nb_steps )
    {
        impl_->end( message(), current, nb_steps );
    }
} // namespace geode
