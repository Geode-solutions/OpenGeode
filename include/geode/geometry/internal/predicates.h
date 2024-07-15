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

#pragma once

#include <geode/geometry/common.h>

#define GEOGRAM_PSM
#ifndef GEO_STATIC_LIBS
#    define GEO_DYNAMIC_LIBS
#endif

/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Bruno Levy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  Website: https://github.com/BrunoLevy/geogram
 *
 */

/******* extracted from ../api/defs.h *******/

#ifndef GEOGRAM_API_DEFS
#    define GEOGRAM_API_DEFS

/*
 * Deactivate warnings about documentation
 * We do that, because CLANG's doxygen parser does not know
 * some doxygen commands that we use (retval, copydoc) and
 * generates many warnings for them...
 */
#    if defined( __clang__ )
#        pragma clang diagnostic ignored "-Wunknown-pragmas"
#        pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#    endif

#    if defined( GEO_DYNAMIC_LIBS )
#        if defined( _MSC_VER )
#            define GEO_IMPORT __declspec( dllimport )
#            define GEO_EXPORT __declspec( dllexport )
#        elif defined( __GNUC__ )
#            define GEO_IMPORT
#            define GEO_EXPORT __attribute__( ( visibility( "default" ) ) )
#        else
#            define GEO_IMPORT
#            define GEO_EXPORT
#        endif
#    else
#        define GEO_IMPORT
#        define GEO_EXPORT
#    endif

#    ifdef geogram_EXPORTS
#        define GEOGRAM_API GEO_EXPORT
#    else
#        define GEOGRAM_API GEO_IMPORT
#    endif

#    define NO_GEOGRAM_API

typedef int GeoMesh;

typedef unsigned char geo_coord_index_t;

/*
 * If GARGANTUA is defined, then geogram is compiled
 * with 64 bit indices.
 */
#    ifdef GARGANTUA

#        include <stdint.h>

typedef uint64_t geo_index_t;

typedef int64_t geo_signed_index_t;

#    else

typedef unsigned int geo_index_t;

typedef int geo_signed_index_t;

#    endif

typedef double geo_coord_t;

typedef int geo_boolean;

enum
{
    GEO_FALSE = 0,
    GEO_TRUE = 1
};

#endif

/******* extracted from ../basic/common.h *******/

#ifndef GEOGRAM_BASIC_COMMON
#    define GEOGRAM_BASIC_COMMON

// iostream should be included before anything else,
// otherwise 'cin', 'cout' and 'cerr' will be uninitialized.
#    include <iostream>

#    if( defined( NDEBUG ) || defined( GEOGRAM_PSM ) )                         \
        && !defined( GEOGRAM_PSM_DEBUG )
#        undef GEO_DEBUG
#        undef GEO_PARANOID
#    else
#        define GEO_DEBUG
#        define GEO_PARANOID
#    endif

// =============================== LINUX defines ===========================

#    if defined( __ANDROID__ )
#        define GEO_OS_ANDROID
#    endif

#    if defined( __linux__ )

#        define GEO_OS_LINUX
#        define GEO_OS_UNIX

#        ifndef GEO_OS_ANDROID
#            define GEO_OS_X11
#        endif

#        if defined( _OPENMP )
#            define GEO_OPENMP
#        endif

#        if defined( __INTEL_COMPILER )
#            define GEO_COMPILER_INTEL
#        elif defined( __clang__ )
#            define GEO_COMPILER_CLANG
#        elif defined( __GNUC__ )
#            define GEO_COMPILER_GCC
#        else
#            error "Unsupported compiler"
#        endif

// The following works on GCC and ICC
#        if defined( __x86_64 )
#            define GEO_ARCH_64
#        else
#            define GEO_ARCH_32
#        endif

// =============================== WINDOWS defines =========================

#    elif defined( _WIN32 ) || defined( _WIN64 )

#        define GEO_OS_WINDOWS

#        if defined( _OPENMP )
#            define GEO_OPENMP
#        endif

#        if defined( _MSC_VER )
#            define GEO_COMPILER_MSVC
#        elif defined( __MINGW32__ ) || defined( __MINGW64__ )
#            define GEO_COMPILER_MINGW
#        endif

#        if defined( _WIN64 )
#            define GEO_ARCH_64
#        else
#            define GEO_ARCH_32
#        endif

// =============================== APPLE defines ===========================

#    elif defined( __APPLE__ )

#        define GEO_OS_APPLE
#        define GEO_OS_UNIX

#        if defined( _OPENMP )
#            define GEO_OPENMP
#        endif

#        if defined( __clang__ )
#            define GEO_COMPILER_CLANG
#        elif defined( __GNUC__ )
#            define GEO_COMPILER_GCC
#        else
#            error "Unsupported compiler"
#        endif

#        if defined( __x86_64 ) || defined( __ppc64__ )
#            define GEO_ARCH_64
#        else
#            define GEO_ARCH_32
#        endif

// =============================== Emscripten defines ======================

#    elif defined( __EMSCRIPTEN__ )

#        define GEO_OS_UNIX
#        define GEO_OS_LINUX
#        define GEO_OS_EMSCRIPTEN
#        define GEO_ARCH_64
#        define GEO_COMPILER_EMSCRIPTEN

// =============================== Unsupported =============================
#    else
#        error "Unsupported operating system"
#    endif

#    if defined( GEO_COMPILER_GCC ) || defined( GEO_COMPILER_CLANG )           \
        || defined( GEO_COMPILER_MINGW ) || defined( GEO_COMPILER_EMSCRIPTEN )
#        define GEO_COMPILER_GCC_FAMILY
#    endif

#    ifdef DOXYGEN_ONLY
// Keep doxygen happy
#        define GEO_OS_WINDOWS
#        define GEO_OS_APPLE
#        define GEO_OS_ANDROID
#        define GEO_ARCH_32
#        define GEO_COMPILER_INTEL
#        define GEO_COMPILER_MSVC
#    endif

#    define CPP_CONCAT_( A, B ) A##B

#    define CPP_CONCAT( A, B ) CPP_CONCAT_( A, B )

#    if defined( GOMGEN )
#        define GEO_NORETURN
#    elif defined( GEO_COMPILER_GCC_FAMILY ) || defined( GEO_COMPILER_INTEL )
#        define GEO_NORETURN __attribute__( ( noreturn ) )
#    else
#        define GEO_NORETURN
#    endif

#    if defined( GOMGEN )
#        define GEO_NORETURN_DECL
#    elif defined( GEO_COMPILER_MSVC )
#        define GEO_NORETURN_DECL __declspec( noreturn )
#    else
#        define GEO_NORETURN_DECL
#    endif

#    if defined( GEO_COMPILER_CLANG ) || defined( GEO_COMPILER_EMSCRIPTEN )
#        if __has_feature( cxx_noexcept )
#            define GEO_NOEXCEPT noexcept
#        endif
#    endif

#    ifndef GEO_NOEXCEPT
#        define GEO_NOEXCEPT throw()
#    endif

#    define FOR( I, UPPERBND )                                                 \
        for( index_t I = 0; I < index_t( UPPERBND ); ++I )

#endif

/******* extracted from ../basic/argused.h *******/

#ifndef GEOGRAM_BASIC_ARGUSED
#    define GEOGRAM_BASIC_ARGUSED

namespace GEO
{
    template < class T >
    inline void geo_argused( const T& )
    {
    }
} // namespace GEO

#endif

/******* extracted from ../basic/numeric.h *******/

#ifndef GEOGRAM_BASIC_NUMERIC
#    define GEOGRAM_BASIC_NUMERIC

#    include <algorithm> // for std::min / std::max
#    define _USE_MATH_DEFINES
#    include <cmath>
#    include <float.h>
#    include <limits.h>

// Visual C++ ver. < 2010 does not have C99 stdint.h,
// using a fallback portable one.
#    if defined( GEO_OS_WINDOWS ) && ( _MSC_VER < 1600 )
#    else
#        include <stdint.h>
#    endif

#    include <limits>

namespace GEO
{
    namespace Numeric
    {
        typedef void* pointer;

        typedef int8_t int8;

        typedef int16_t int16;

        typedef int32_t int32;

        typedef int64_t int64;

        typedef uint8_t uint8;

        typedef uint16_t uint16;

        typedef uint32_t uint32;

        typedef uint64_t uint64;

        typedef float float32;

        typedef double float64;

        inline float32 max_float32()
        {
            return std::numeric_limits< float32 >::max();
        }

        inline float32 min_float32()
        {
            // Note: numeric_limits<>::min() is not
            // what we want (it returns the smallest
            // positive non-denormal).
            return -max_float32();
        }

        inline float64 max_float64()
        {
            return std::numeric_limits< float64 >::max();
        }

        inline float64 min_float64()
        {
            // Note: numeric_limits<>::min() is not
            // what we want (it returns the smallest
            // positive non-denormal).
            return -max_float64();
        }

        bool is_nan( float32 x );

        bool is_nan( float64 x );

        void random_reset();

        int32 random_int32();

        float32 random_float32();

        float64 random_float64();

        template < class T, bool is_numeric >
        struct LimitsHelper : std::numeric_limits< T >
        {
        };

        template < class T >
        struct LimitsHelper< T, true > : std::numeric_limits< T >
        {
            static const size_t size = sizeof( T );

            static const size_t numbits = 8 * sizeof( T );
        };

        template < class T >
        struct Limits
            : LimitsHelper< T, std::numeric_limits< T >::is_specialized >
        {
        };
    } // namespace Numeric

    enum Sign
    {

        NEGATIVE = -1,

        ZERO = 0,

        POSITIVE = 1
    };

    template < class T >
    inline Sign geo_sgn( const T& x )
    {
        return ( x > 0 ) ? POSITIVE : ( ( x < 0 ) ? NEGATIVE : ZERO );
    }

    template < class T >
    inline T geo_sqr( T x )
    {
        return x * x;
    }

    template < class T >
    inline void geo_clamp( T& x, T min, T max )
    {
        if( x < min )
        {
            x = min;
        }
        else if( x > max )
        {
            x = max;
        }
    }

    typedef geo_index_t index_t;

    inline index_t max_index_t()
    {
        return std::numeric_limits< index_t >::max();
    }

    typedef geo_signed_index_t signed_index_t;

    inline signed_index_t max_signed_index_t()
    {
        return std::numeric_limits< signed_index_t >::max();
    }

    inline signed_index_t min_signed_index_t()
    {
        return std::numeric_limits< signed_index_t >::min();
    }

    typedef geo_coord_index_t coord_index_t;

    inline double round( double x )
    {
        return ( ( x - floor( x ) ) > 0.5 ? ceil( x ) : floor( x ) );
    }
} // namespace GEO

#endif

/******* extracted from ../basic/psm.h *******/

#ifndef GEOGRAM_BASIC_PSM
#    define GEOGRAM_BASIC_PSM

#    include <assert.h>
#    include <iostream>
#    include <string>

#    ifndef GEOGRAM_PSM
#        define GEOGRAM_PSM
#    endif

#    ifndef GEOGRAM_BASIC_ASSERT

#        define geo_assert( x ) assert( x )
#        define geo_range_assert( x, min_val, max_val )                        \
            assert( ( x ) >= ( min_val ) && ( x ) <= ( max_val ) )
#        define geo_assert_not_reached assert( 0 )

#        ifdef GEO_DEBUG
#            define geo_debug_assert( x ) assert( x )
#            define geo_debug_range_assert( x, min_val, max_val )              \
                assert( ( x ) >= ( min_val ) && ( x ) <= ( max_val ) )
#        else
#            define geo_debug_assert( x )
#            define geo_debug_range_assert( x, min_val, max_val )
#        endif

#        ifdef GEO_PARANOID
#            define geo_parano_assert( x ) geo_assert( x )
#            define geo_parano_range_assert( x, min_val, max_val )             \
                geo_range_assert( x, min_val, max_val )
#        else
#            define geo_parano_assert( x )
#            define geo_parano_range_assert( x, min_val, max_val )
#        endif

#    endif

#    ifndef geo_cite
#        define geo_cite( x )
#    endif

#    ifndef geo_cite_with_info
#        define geo_cite_with_info( x, y )
#    endif

#    ifndef FPG_UNCERTAIN_VALUE
#        define FPG_UNCERTAIN_VALUE 0
#    endif

#    define GEOGRAM_WITH_PDEL

#endif

/******* extracted from predicates.h *******/

#include <geode/geometry/information.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace GEO
{
    namespace PCK
    {
        Sign orient_2d( const geode::Point2D& p0,
            const geode::Point2D& p1,
            const geode::Point2D& p2 );

        Sign orient_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2,
            const geode::Point3D& p3 );

        Sign det_3d( const geode::Vector3D& p0,
            const geode::Vector3D& p1,
            const geode::Vector3D& p2 );

        bool aligned_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2 );

        Sign dot_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2 );

        Sign dot_2d( const geode::Point2D& p0,
            const geode::Point2D& p1,
            const geode::Point2D& p2 );

        void initialize();
    } // namespace PCK
} // namespace GEO

namespace geode
{
    namespace internal
    {
        inline Side side( const GEO::Sign& sign )
        {
            if( sign == GEO::POSITIVE )
            {
                return Side::positive;
            }
            if( sign == GEO::NEGATIVE )
            {
                return Side::negative;
            }
            return Side::zero;
        }

        inline Side opposite_side( const GEO::Sign& sign )
        {
            if( sign == GEO::POSITIVE )
            {
                return Side::negative;
            }
            if( sign == GEO::NEGATIVE )
            {
                return Side::positive;
            }
            return Side::zero;
        }
    } // namespace internal
} // namespace geode