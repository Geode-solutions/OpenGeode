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

#if defined( _WIN32 )
#    define OPENGEODE_WINDOWS
#elif defined( __linux__ )
#    define OPENGEODE_LINUX
#elif defined( __APPLE__ )
#    define OPENGEODE_APPLE
#endif

#ifndef NDEBUG
#    define OPENGEODE_DEBUG
#endif

#ifdef OPENGEODE_WINDOWS
#    pragma warning( disable : 4267 ) // conversion between long unsigned int
                                      // and unsigned int
#    pragma warning( disable : 4251 ) // dll interface warnings
#    pragma warning( disable : 4661 ) // template alias before implementation
#endif

#define OPENGEODE_DISABLE_COPY( Class )                                        \
public:                                                                        \
    Class( const Class & ) = delete;                                           \
    Class &operator=( const Class & ) = delete

#define OPENGEODE_DISABLE_MOVE( Class )                                        \
public:                                                                        \
    Class( Class && ) = delete;                                                \
    Class &operator=( Class && ) = delete

#define OPENGEODE_DISABLE_COPY_AND_MOVE( Class )                               \
    OPENGEODE_DISABLE_COPY( Class );                                           \
    OPENGEODE_DISABLE_MOVE( Class )

#define OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( type )                             \
    static_assert(                                                             \
        ( type ) == 2 || type == 3, #type " template should be 2 or 3" )

#define OPENGEODE_TEMPLATE_ASSERT_3D( type )                                   \
    static_assert( ( type ) == 3, #type " template should be 3" )

#define ALIAS_2D( Class ) using Class##2D = Class< 2 >

#define ALIAS_3D( Class ) using Class##3D = Class< 3 >

#define ALIAS_2D_AND_3D( Class )                                               \
    ALIAS_2D( Class );                                                         \
    ALIAS_3D( Class )

#define FORWARD_DECLARATION_DIMENSION_CLASS( Class )                           \
    template < index_t >                                                       \
    class Class

#define FORWARD_DECLARATION_DIMENSION_STRUCT( Struct )                         \
    template < index_t >                                                       \
    struct Struct

// To avoid unused argument warning in function definition
template < typename T >
void geode_unused( const T & /*unused*/ )
{
}

/*!
 * Use this macro to initialize an OpenGeode library and execute some functions
 * at library loading.
 */
#define OPENGEODE_LIBRARY_INITIALIZE( library_name )                           \
    class lib_library_name                                                     \
    {                                                                          \
    public:                                                                    \
        lib_library_name()                                                     \
        {                                                                      \
            initialize();                                                      \
        }                                                                      \
                                                                               \
    private:                                                                   \
        void initialize();                                                     \
    };                                                                         \
    lib_library_name lib_library_name_instance; /* NOLINT */                   \
    void lib_library_name::initialize()

#include <geode/basic/assert.h>
#include <geode/basic/opengeode_basic_export.h>
#include <geode/basic/types.h>
