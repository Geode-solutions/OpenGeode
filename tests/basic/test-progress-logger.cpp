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

#include <absl/memory/memory.h>

#include <geode/basic/library.h>
#include <geode/basic/logger.h>
#include <geode/basic/progress_logger.h>
#include <geode/basic/progress_logger_client.h>
#include <geode/basic/progress_logger_manager.h>
#include <geode/basic/range.h>

#include <geode/tests/common.h>

class CustomClient : public geode::ProgressLoggerClient
{
public:
    CustomClient( std::string private_message )
        : private_message_{ std::move( private_message ) }
    {
    }

    void start( const std::string& message, geode::index_t nb_steps ) override
    {
        geode::Logger::info(
            private_message_, ": ", message, " => ", nb_steps );
    }

    void update( geode::index_t current_step, geode::index_t nb_steps ) override
    {
        geode::Logger::info(
            private_message_, ": ", current_step, " => ", nb_steps );
    }

    void completed() override
    {
        geode::Logger::info( private_message_, ": DONE" );
    }

    void failed() override
    {
        geode::Logger::info( private_message_, ": TRY AGAIN" );
    }

private:
    std::string private_message_;
};

void test()
{
    geode::OpenGeodeBasic::initialize();
    geode::ProgressLoggerManager::register_client(
        absl::make_unique< CustomClient >( "Custom logger" ) );

    geode::index_t nb{ 30000 };
    geode::ProgressLogger logger{ "Cool message", nb };
    for( const auto i : geode::Range{ nb } )
    {
        auto test = 0;
        for( const auto ii : geode::Range{ i, nb } )
        {
            test += ii;
        }
        logger.increment();
        logger.increment_nb_steps();
    }
    for( const auto i : geode::Range{ nb } )
    {
        auto test = 0;
        for( const auto ii : geode::Range{ i, nb } )
        {
            test += ii;
        }
        logger.increment();
    }
}

OPENGEODE_TEST( "progress-logger" )