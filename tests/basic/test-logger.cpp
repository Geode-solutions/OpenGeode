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

#include <iostream>
#include <memory>

#include <geode/basic/file_logger_client.hpp>
#include <geode/basic/library.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/logger_client.hpp>
#include <geode/basic/logger_manager.hpp>
#include <geode/basic/range.hpp>

#include <geode/tests/common.hpp>
namespace
{

    class CustomClient : public geode::LoggerClient
    {
    public:
        void trace( const std::string &message ) override
        {
            std::cout << "Old school logger => " << message << '\n';
        }

        void debug( const std::string &message ) override
        {
            std::cout << "Old school logger => " << message << '\n';
        }

        void info( const std::string &message ) override
        {
            std::cout << "Old school logger => " << message << '\n';
        }

        void warn( const std::string &message ) override
        {
            std::cout << "Old school logger => " << message << '\n';
        }

        void error( const std::string &message ) override
        {
            std::cout << "Old school logger => " << message << '\n';
        }

        void critical( const std::string &message ) override
        {
            std::cout << "Old school logger => " << message << '\n';
        }
    };

    void test_logger()
    {
        geode::Logger::trace( "test ", "trace" );
        geode::Logger::debug( "test ", "debug" );
        geode::Logger::info( "test ", "info" );
        geode::Logger::warn( "test ", "warn" );
        geode::Logger::error( "test ", "error" );
        geode::Logger::critical( "test ", "critial" );
    }

    std::string test_huge_message()
    {
        std::string huge_message;
        const geode::index_t MSG_SIZE{ 100000 };
        huge_message.reserve( MSG_SIZE );
        for( const auto count : geode::Range{ MSG_SIZE } )
        {
            huge_message.push_back( 'A' + ( count % 26 ) );
        }
        geode::Logger::info( "Huge message size = ", huge_message.size() );
        geode::Logger::info( huge_message );

        geode::Logger::info(
            "Huge message begin = ", huge_message.substr( 0, 50 ) );
        geode::Logger::info( "Huge message end = ",
            huge_message.substr( huge_message.size() - 50 ) );
        return huge_message;
    }

    void test_change_log_file( const std::string &huge_msg )
    {
        geode::Logger::info( "==============================" );
        geode::Logger::info( "TEST CHANGE LOG FILE" );
        geode::Logger::info( "==============================" );

        static constexpr auto FILENAME1 = "first.log";
        auto file_logger =
            std::make_unique< geode::FileLoggerClient >( FILENAME1 );
        auto &registered_file_logger =
            dynamic_cast< geode::FileLoggerClient & >(
                geode::LoggerManager::register_client(
                    std::move( file_logger ) ) );
        geode::Logger::info(
            absl::StrCat( "Message written in first.log", "\n", huge_msg ) );
        static constexpr auto FILENAME2 = "second.log";
        registered_file_logger.set_file_path( FILENAME2 );
        geode::Logger::info(
            absl::StrCat( "Message written in second.log", "\n", huge_msg ) );
    }

    void test()
    {
        geode::OpenGeodeBasicLibrary::initialize();
        geode::LoggerManager::register_client(
            std::make_unique< CustomClient >() );
        geode::LoggerManager::register_client(
            std::make_unique< geode::FileLoggerClient >( "geode.log" ) );

        test_logger();
        const auto &huge_msg = test_huge_message();
        test_change_log_file( huge_msg );

        geode::Logger::set_level( geode::Logger::LEVEL::err );
        test_logger();
    }
} // namespace
OPENGEODE_TEST( "logger" )