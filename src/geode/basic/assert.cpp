/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/assert.h>

#include <cassert>

#include <geode/basic/logger.h>

namespace geode
{
    void geode_assertion_failed( const std::string &condition,
        const std::string &message,
        const std::string &file,
        int line )
    {
        Logger::error( "File: ", file );
        Logger::error( "Line: ", line );
        Logger::error( "Info: ", message );
        throw OpenGeodeException{ "Assertion failed: ", condition };
    }

    int geode_lippincott()
    {
        try
        {
            throw;
        }
        catch( const OpenGeodeException &e )
        {
            Logger::critical( "OpenGeodeException: ", e.what() );
        }
        catch( const std::exception &e )
        {
            Logger::critical( "std::exception: ", e.what() );
        }
        catch( ... )
        {
            Logger::critical( "Unknown exception" );
        }
        return 1;
    }
} // namespace geode
