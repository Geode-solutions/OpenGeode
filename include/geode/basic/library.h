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

#pragma once

#include <geode/basic/common.h>
#include <geode/basic/pimpl.h>

namespace geode
{
    class opengeode_basic_api Library
    {
    public:
        virtual ~Library();

    protected:
        Library();

        void call_initialize( const char* library_name );

    private:
        virtual void do_initialize() = 0;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode

/*!
 * Use this macro to declare an OpenGeode library in a header file (usually
 * common.h)
 */
#define OPENGEODE_LIBRARY( export_api, library_name )                          \
    class export_api library_name : public geode::Library                      \
    {                                                                          \
    public:                                                                    \
        static void initialize()                                               \
        {                                                                      \
            static library_name library;                                       \
            library.call_initialize( #library_name );                          \
        }                                                                      \
                                                                               \
    private:                                                                   \
        void do_initialize() override;                                         \
    }

/*!
 * Use this macro to implement an OpenGeode library in a cpp file (usually
 * common.cpp) and call all functions that need to be initialized.
 */
#define OPENGEODE_LIBRARY_IMPLEMENTATION( library_name )                       \
    void library_name::do_initialize()

namespace geode
{
    OPENGEODE_LIBRARY( opengeode_basic_api, OpenGeodeBasic );
} // namespace geode