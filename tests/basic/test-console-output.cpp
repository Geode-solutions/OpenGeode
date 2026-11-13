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

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <geode/basic/console_logger_client.hpp>
#include <geode/basic/console_progress_logger_client.hpp>
#include <geode/basic/library.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/logger_client.hpp>
#include <geode/basic/logger_manager.hpp>
#include <geode/basic/progress_logger.hpp>
#include <geode/basic/progress_logger_manager.hpp>
#include <geode/basic/range.hpp>

#include <geode/tests/common.hpp>

namespace
{
    std::atomic< geode::index_t > nb_console_messages{ 0 };
    std::atomic< geode::index_t > nb_custom_messages{ 0 };
    std::atomic< geode::index_t > nb_console_progress_events{ 0 };
    std::atomic< geode::index_t > nb_progress_events{ 0 };

    class CountingConsoleClient : public geode::ConsoleLoggerClient
    {
        void info( const std::string& /*message*/ ) override
        {
            nb_console_messages++;
        }
    };

    class CountingClient : public geode::LoggerClient
    {
        void trace( const std::string& /*message*/ ) override {}

        void debug( const std::string& /*message*/ ) override {}

        void info( const std::string& /*message*/ ) override
        {
            nb_custom_messages++;
        }

        void warning( const std::string& /*message*/ ) override {}

        void error( const std::string& /*message*/ ) override {}

        void critical( const std::string& /*message*/ ) override {}
    };

    class CountingProgressClient : public geode::ProgressLoggerClient
    {
        void start( const geode::uuid& /*progress_logger_id*/,
            geode::Logger::LEVEL /*level*/,
            const std::string& /*message*/,
            geode::index_t /*nb_steps*/ ) override
        {
            nb_progress_events++;
        }

        void update( const geode::uuid& /*progress_logger_id*/,
            geode::Logger::LEVEL /*level*/,
            geode::index_t /*current_step*/,
            geode::index_t /*nb_steps*/ ) override
        {
        }

        void completed( const geode::uuid& /*progress_logger_id*/,
            geode::Logger::LEVEL /*level*/ ) override
        {
            nb_progress_events++;
        }

        void failed( const geode::uuid& /*progress_logger_id*/,
            geode::Logger::LEVEL /*level*/ ) override
        {
            nb_progress_events++;
        }
    };

    class CountingConsoleProgressClient
        : public geode::ConsoleProgressLoggerClient
    {
        void start( const geode::uuid& /*progress_logger_id*/,
            geode::Logger::LEVEL /*level*/,
            const std::string& /*message*/,
            geode::index_t /*nb_steps*/ ) override
        {
            nb_console_progress_events++;
        }

        void completed( const geode::uuid& /*progress_logger_id*/,
            geode::Logger::LEVEL /*level*/ ) override
        {
            nb_console_progress_events++;
        }
    };

    void test_logger_console_output()
    {
        geode::LoggerManager::register_client(
            std::make_unique< CountingConsoleClient >() );
        geode::LoggerManager::register_client(
            std::make_unique< CountingClient >() );
        geode::OpenGeodeBasicException::test(
            geode::LoggerManager::is_console_output_enabled(),
            "[Test] Console output should be enabled by default" );
        geode::Logger::info( "Displayed in console" );
        geode::OpenGeodeBasicException::test( nb_console_messages == 1,
            "[Test] Console client should receive the message" );

        geode::LoggerManager::set_console_output( false );
        geode::LoggerManager::set_console_output( false );
        geode::OpenGeodeBasicException::test(
            !geode::LoggerManager::is_console_output_enabled(),
            "[Test] Console output should be disabled" );
        geode::Logger::info( "Not displayed in console" );
        geode::OpenGeodeBasicException::test( nb_console_messages == 1,
            "[Test] Console client should not receive the message" );
        geode::OpenGeodeBasicException::test( nb_custom_messages == 2,
            "[Test] Other clients should still receive the messages" );

        geode::LoggerManager::set_console_output( true );
        geode::Logger::info( "Displayed in console again" );
        geode::OpenGeodeBasicException::test( nb_console_messages == 2,
            "[Test] Console client should receive the message again" );
        geode::OpenGeodeBasicException::test( nb_custom_messages == 3,
            "[Test] Custom client should receive all the messages" );
    }

    void test_progress_console_output()
    {
        geode::ProgressLoggerManager::register_client(
            std::make_unique< CountingConsoleProgressClient >() );
        geode::OpenGeodeBasicException::test(
            geode::ProgressLoggerManager::is_console_output_enabled(),
            "[Test] Console progress output should be enabled by default" );
        {
            geode::ProgressLogger progress{ geode::Logger::LEVEL::info,
                "Displayed progress", 1 };
            progress.increment();
        }
        geode::OpenGeodeBasicException::test( nb_console_progress_events == 2,
            "[Test] Console progress client should receive the events" );

        geode::ProgressLoggerManager::set_console_output( false );
        geode::OpenGeodeBasicException::test(
            !geode::ProgressLoggerManager::is_console_output_enabled(),
            "[Test] Console progress output should be disabled" );
        {
            geode::ProgressLogger progress{ geode::Logger::LEVEL::info,
                "Hidden progress", 1 };
            progress.increment();
        }
        geode::OpenGeodeBasicException::test( nb_console_progress_events == 2,
            "[Test] Console progress client should not receive the events" );

        geode::ProgressLoggerManager::set_console_output( true );
        {
            geode::ProgressLogger progress{ geode::Logger::LEVEL::info,
                "Displayed progress again", 1 };
            progress.increment();
        }
        geode::OpenGeodeBasicException::test( nb_console_progress_events == 4,
            "[Test] Console progress client should receive the events again" );
    }

    void test_concurrent_console_output()
    {
        geode::LoggerManager::set_console_output( false );
        const auto initial_custom_messages = nb_custom_messages.load();
        static constexpr geode::index_t NB_THREADS{ 4 };
        static constexpr geode::index_t NB_MESSAGES{ 1000 };
        std::vector< std::thread > threads;
        for( const auto thread_id : geode::Range{ NB_THREADS } )
        {
            threads.emplace_back( [thread_id] {
                for( const auto message_id : geode::Range{ NB_MESSAGES } )
                {
                    geode::Logger::info(
                        "Thread ", thread_id, " message ", message_id );
                }
            } );
        }
        threads.emplace_back( [] {
            for( const auto count : geode::Range{ NB_MESSAGES } )
            {
                geode::LoggerManager::set_console_output( count % 2 == 0 );
                geode::ProgressLoggerManager::set_console_output(
                    count % 2 == 0 );
            }
            geode::LoggerManager::set_console_output( false );
            geode::ProgressLoggerManager::set_console_output( true );
        } );
        threads.emplace_back( [] {
            for( const auto count : geode::Range{ 10 } )
            {
                geode::Logger::info( "Registering client ", count );
                geode::LoggerManager::register_client(
                    std::make_unique< CountingConsoleClient >() );
            }
        } );
        for( auto& thread : threads )
        {
            thread.join();
        }
        geode::OpenGeodeBasicException::test(
            nb_custom_messages - initial_custom_messages
                == NB_THREADS * NB_MESSAGES + 10,
            "[Test] Custom client should receive every concurrent message" );
        const auto nb_console = nb_console_messages.load();
        geode::Logger::info( "Not displayed in console" );
        geode::OpenGeodeBasicException::test( nb_console_messages == nb_console,
            "[Test] Console clients should stay disabled" );
        geode::LoggerManager::set_console_output( true );
    }

    void test_concurrent_progress_console_output()
    {
        geode::ProgressLoggerManager::register_client(
            std::make_unique< CountingProgressClient >() );
        geode::LoggerManager::set_console_output( false );
        static constexpr geode::index_t NB_THREADS{ 4 };
        static constexpr geode::index_t NB_PROGRESSES{ 200 };
        std::vector< std::thread > threads;
        for( const auto thread_id : geode::Range{ NB_THREADS } )
        {
            threads.emplace_back( [thread_id] {
                for( const auto progress_id : geode::Range{ NB_PROGRESSES } )
                {
                    geode::ProgressLogger progress{ geode::Logger::LEVEL::info,
                        absl::StrCat(
                            "Thread ", thread_id, " progress ", progress_id ),
                        1 };
                    progress.increment();
                }
            } );
        }
        threads.emplace_back( [] {
            for( const auto count : geode::Range{ NB_PROGRESSES } )
            {
                geode::ProgressLoggerManager::set_console_output(
                    count % 2 == 0 );
            }
            geode::ProgressLoggerManager::set_console_output( false );
        } );
        for( auto& thread : threads )
        {
            thread.join();
        }
        geode::OpenGeodeBasicException::test(
            nb_progress_events == 2 * NB_THREADS * NB_PROGRESSES,
            "[Test] Progress client should receive every concurrent event" );
        const auto nb_console = nb_console_progress_events.load();
        {
            geode::ProgressLogger progress{ geode::Logger::LEVEL::info,
                "Hidden progress", 1 };
            progress.increment();
        }
        geode::OpenGeodeBasicException::test(
            nb_console_progress_events == nb_console,
            "[Test] Console progress clients should stay disabled" );
        geode::ProgressLoggerManager::set_console_output( true );
        geode::LoggerManager::set_console_output( true );
    }

    void test()
    {
        geode::OpenGeodeBasicLibrary::initialize();
        test_logger_console_output();
        test_progress_console_output();
        test_concurrent_console_output();
        test_concurrent_progress_console_output();
    }
} // namespace

OPENGEODE_TEST( "console-output" )
