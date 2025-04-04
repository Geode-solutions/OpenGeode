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

#pragma once

#include <string>

#include <geode/basic/common.hpp>
#include <geode/basic/pimpl.hpp>
#include <geode/basic/progress_logger_client.hpp>

namespace geode
{
    class opengeode_basic_api ConsoleProgressLoggerClient
        : public ProgressLoggerClient
    {
    public:
        ConsoleProgressLoggerClient();
        ~ConsoleProgressLoggerClient();

    private:
        void start( const uuid& progress_logger_id,
            Logger::LEVEL level,
            const std::string& message,
            index_t nb_steps ) override;

        void update( const uuid& progress_logger_id,
            Logger::LEVEL level,
            index_t current_step,
            index_t nb_steps ) override;

        void completed(
            const uuid& progress_logger_id, Logger::LEVEL level ) override;

        void failed(
            const uuid& progress_logger_id, Logger::LEVEL level ) override;

        [[deprecated]] void start( const uuid& progress_logger_id,
            const std::string& message,
            index_t nb_steps ) override;

        [[deprecated]] void update( const uuid& progress_logger_id,
            index_t current_step,
            index_t nb_steps ) override;

        [[deprecated]] void completed(
            const uuid& progress_logger_id ) override;

        [[deprecated]] void failed( const uuid& progress_logger_id ) override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
