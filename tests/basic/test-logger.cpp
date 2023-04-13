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

#include <iostream>

#include <absl/memory/memory.h>

#include <geode/basic/library.h>
#include <geode/basic/logger.h>
#include <geode/basic/logger_client.h>
#include <geode/basic/logger_manager.h>

#include <geode/tests/common.h>

class CustomClient : public geode::LoggerClient
{
public:
    void trace( const std::string &message ) override
    {
        std::cout << "Old school logger => " << message << std::endl;
    }

    void debug( const std::string &message ) override
    {
        std::cout << "Old school logger => " << message << std::endl;
    }

    void info( const std::string &message ) override
    {
        std::cout << "Old school logger => " << message << std::endl;
    }

    void warn( const std::string &message ) override
    {
        std::cout << "Old school logger => " << message << std::endl;
    }

    void error( const std::string &message ) override
    {
        std::cout << "Old school logger => " << message << std::endl;
    }

    void critical( const std::string &message ) override
    {
        std::cout << "Old school logger => " << message << std::endl;
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

void test()
{
    geode::OpenGeodeBasicLibrary::initialize();
    geode::LoggerManager::register_client(
        absl::make_unique< CustomClient >() );

    test_logger();
    geode::Logger::set_level( geode::Logger::Level::err );
    test_logger();
}

OPENGEODE_TEST( "logger" )