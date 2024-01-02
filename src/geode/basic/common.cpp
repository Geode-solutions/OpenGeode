/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
#include <geode/basic/common.h>

#include <absl/memory/memory.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/console_logger_client.h>
#include <geode/basic/console_progress_logger_client.h>
#include <geode/basic/library.h>
#include <geode/basic/logger_manager.h>
#include <geode/basic/progress_logger_manager.h>

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( OpenGeodeBasic )
    {
        LoggerManager::register_client(
            absl::make_unique< ConsoleLoggerClient >() );
        ProgressLoggerManager::register_client(
            absl::make_unique< ConsoleProgressLoggerClient >() );
        BitseryExtensions::register_functions(
            register_basic_serialize_pcontext,
            register_basic_deserialize_pcontext );
    }
} // namespace geode
