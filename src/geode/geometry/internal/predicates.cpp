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

#include <geode/geometry/internal/predicates.hpp>

#include <algorithm>
#include <cmath>

/*
 *  Copyright (c) 2012-2014, Bruno Levy
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *  * Neither the name of the ALICE Project-Team nor the names of its
 *  contributors may be used to endorse or promote products derived from this
 *  software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Bruno Levy
 *
 *     Bruno.Levy@inria.fr
 *     http://www.loria.fr/~levy
 *
 *     ALICE Project
 *     LORIA, INRIA Lorraine,
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX
 *     FRANCE
 *
 */

/*
 *  This file is a PSM (pluggable software module)
 *   generated from the distribution of Geogram.
 *
 *  See Geogram documentation on:
 *   http://alice.loria.fr/software/geogram/doc/html/index.html
 *
 *  See documentation of the functions bundled in this PSM on:
 *   http://alice.loria.fr/software/geogram/doc/html/namespaceGEO_1_1PCK.html
 */

/******* extracted from ../basic/matrix.h *******/

#ifndef GEOGRAM_BASIC_MATRIX
#    define GEOGRAM_BASIC_MATRIX

namespace GEO
{
    inline double det2x2( double a11, double a12, double a21, double a22 )
    {
        return a11 * a22 - a12 * a21;
    }

    inline double det3x3( double a11,
        double a12,
        double a13,
        double a21,
        double a22,
        double a23,
        double a31,
        double a32,
        double a33 )
    {
        return a11 * det2x2( a22, a23, a32, a33 )
               - a21 * det2x2( a12, a13, a32, a33 )
               + a31 * det2x2( a12, a13, a22, a23 );
    }
} // namespace GEO

#endif

/******* extracted from multi_precision.h *******/

#ifndef GEOGRAM_NUMERICS_MULTI_PRECISION
#    define GEOGRAM_NUMERICS_MULTI_PRECISION

#    include <iostream>
#    include <new>

namespace GEO
{
    extern double expansion_splitter_;
    extern double expansion_epsilon_;

    inline void two_sum( double a, double b, double& x, double& y )
    {
        x = a + b;
        double bvirt = x - a;
        double avirt = x - bvirt;
        double bround = b - bvirt;
        double around = a - avirt;
        y = around + bround;
    }

    inline void two_diff( double a, double b, double& x, double& y )
    {
        x = a - b;
        double bvirt = a - x;
        double avirt = x + bvirt;
        double bround = bvirt - b;
        double around = a - avirt;
        y = around + bround;
    }

    inline void split( double a, double& ahi, double& alo )
    {
        double c = expansion_splitter_ * a;
        double abig = c - a;
        ahi = c - abig;
        alo = a - ahi;
    }

    inline void two_product( double a, double b, double& x, double& y )
    {
#    ifdef FP_FAST_FMA
        // If the target processor supports the FMA (Fused Multiply Add)
        // instruction, then the product of two doubles into a length-2
        // expansion can be implemented as follows. Thanks to Marc Glisse
        // for the information.
        // Note: under gcc, automatic generations of fma() for a*b+c needs
        // to be deactivated, using -ffp-contract=off, else it may break
        // other functions such as fast_expansion_sum_zeroelim().
        x = a * b;
        y = fma( a, b, -x );
#    else
        x = a * b;
        double ahi, alo;
        split( a, ahi, alo );
        double bhi, blo;
        split( b, bhi, blo );
        double err1 = x - ( ahi * bhi );
        double err2 = err1 - ( alo * bhi );
        double err3 = err2 - ( ahi * blo );
        y = ( alo * blo ) - err3;
#    endif
    }

    class expansion
    {
    public:
        index_t length() const
        {
            return length_;
        }

        index_t capacity() const
        {
            return capacity_;
        }

        void set_length( index_t new_length )
        {
            geo_debug_assert( new_length <= capacity() );
            length_ = new_length;
        }

        const double& operator[]( index_t i ) const
        {
            // Note: we allocate capacity+1 storage
            // systematically, since basic functions
            // may access one additional value (without
            // using it)
            geo_debug_assert( i <= capacity_ );
            return x_[i];
        }

        double& operator[]( index_t i )
        {
            // Note: we allocate capacity+1 storage
            // systematically, since basic functions
            // may access one additional value (without
            // using it)
            geo_debug_assert( i <= capacity_ );
            return x_[i];
        }

        double* data()
        {
            return x_;
        }

        const double* data() const
        {
            return x_;
        }

        static size_t bytes( index_t capa )
        {
            // --> 2*sizeof(double) because x_ is declared of size [2]
            // to avoid compiler's warning.
            // --> capa+1 to have an additional 'sentry' at the end
            // because fast_expansion_sum_zeroelim() may access
            // an entry past the end (without using it).
            return sizeof( expansion ) - 2 * sizeof( double )
                   + ( capa + 1 ) * sizeof( double );
        }

        expansion( index_t capa ) : length_( 0 ), capacity_( capa ) {}

#    ifdef CPPCHECK
        // cppcheck does not understand that the result
        // of alloca() is passed to the placement syntax
        // of operator new.
        expansion& new_expansion_on_stack( index_t capa );
#    else
#        define new_expansion_on_stack( capa )                                 \
            ( new( alloca( expansion::bytes( capa ) ) ) expansion( capa ) )
#    endif

        // ========================== Initialization from doubles

        expansion& assign( double a )
        {
            set_length( 1 );
            x_[0] = a;
            return *this;
        }

        static index_t sum_capacity( double a, double b )
        {
            geo_argused( a );
            geo_argused( b );
            return 2;
        }

        expansion& assign_sum( double a, double b )
        {
            set_length( 2 );
            two_sum( a, b, x_[1], x_[0] );
            return *this;
        }

        static index_t diff_capacity( double a, double b )
        {
            geo_argused( a );
            geo_argused( b );
            return 2;
        }

        expansion& assign_diff( double a, double b )
        {
            set_length( 2 );
            two_diff( a, b, x_[1], x_[0] );
            return *this;
        }

        static index_t product_capacity( double a, double b )
        {
            geo_argused( a );
            geo_argused( b );
            return 2;
        }

        expansion& assign_product( double a, double b )
        {
            set_length( 2 );
            two_product( a, b, x_[1], x_[0] );
            return *this;
        }

        // ====== Initialization from expansion and double

        static index_t sum_capacity( const expansion& a, double b )
        {
            geo_argused( b );
            return a.length() + 1;
        }

        expansion& assign_sum( const expansion& a, double b );

        static index_t diff_capacity( const expansion& a, double b )
        {
            geo_argused( b );
            return a.length() + 1;
        }

        expansion& assign_diff( const expansion& a, double b );

        static index_t product_capacity( const expansion& a, double b )
        {
            geo_argused( b );
            // TODO: implement special case where the double argument
            // is a power of two.
            return a.length() * 2;
        }

        // ========================== Initialization from expansions

        static index_t sum_capacity( const expansion& a, const expansion& b )
        {
            return a.length() + b.length();
        }

        expansion& assign_sum( const expansion& a, const expansion& b );

        static index_t sum_capacity(
            const expansion& a, const expansion& b, const expansion& c )
        {
            return a.length() + b.length() + c.length();
        }

        expansion& assign_sum(
            const expansion& a, const expansion& b, const expansion& c );

        static index_t sum_capacity( const expansion& a,
            const expansion& b,
            const expansion& c,
            const expansion& d )
        {
            return a.length() + b.length() + c.length() + d.length();
        }

        expansion& assign_sum( const expansion& a,
            const expansion& b,
            const expansion& c,
            const expansion& d );

        static index_t diff_capacity( const expansion& a, const expansion& b )
        {
            return a.length() + b.length();
        }

        expansion& assign_diff( const expansion& a, const expansion& b );

        static index_t product_capacity(
            const expansion& a, const expansion& b )
        {
            return a.length() * b.length() * 2;
        }

        expansion& assign_product( const expansion& a, const expansion& b );

        static index_t product_capacity(
            const expansion& a, const expansion& b, const expansion& c )
        {
            return a.length() * b.length() * c.length() * 4;
        }

        // ====== Determinants =============================

        static index_t det2x2_capacity( const expansion& a11,
            const expansion& a12,
            const expansion& a21,
            const expansion& a22 )
        {
            return product_capacity( a11, a22 ) + product_capacity( a21, a12 );
        }

        expansion& assign_det2x2( const expansion& a11,
            const expansion& a12,
            const expansion& a21,
            const expansion& a22 );

        static index_t det3x3_capacity( const expansion& a11,
            const expansion& a12,
            const expansion& a13,
            const expansion& a21,
            const expansion& a22,
            const expansion& a23,
            const expansion& a31,
            const expansion& a32,
            const expansion& a33 )
        {
            // Development w.r.t. first row
            index_t c11_capa = det2x2_capacity( a22, a23, a32, a33 );
            index_t c12_capa = det2x2_capacity( a21, a23, a31, a33 );
            index_t c13_capa = det2x2_capacity( a21, a22, a31, a32 );
            return 2
                   * ( a11.length() * c11_capa + a12.length() * c12_capa
                       + a13.length() * c13_capa );
        }

        expansion& assign_det3x3( const expansion& a11,
            const expansion& a12,
            const expansion& a13,
            const expansion& a21,
            const expansion& a22,
            const expansion& a23,
            const expansion& a31,
            const expansion& a32,
            const expansion& a33 );

        static index_t det_111_2x3_capacity( const expansion& a21,
            const expansion& a22,
            const expansion& a23,
            const expansion& a31,
            const expansion& a32,
            const expansion& a33 )
        {
            return det2x2_capacity( a22, a23, a32, a33 )
                   + det2x2_capacity( a23, a21, a33, a31 )
                   + det2x2_capacity( a21, a22, a31, a32 );
        }

        // =============== some general purpose functions =========

        SIGN sign() const
        {
            if( length() == 0 )
            {
                return zero;
            }
            return geo_sgn( x_[length() - 1] );
        }

    protected:
        static index_t sub_product_capacity(
            index_t a_length, index_t b_length )
        {
            return a_length * b_length * 2;
        }

        expansion& assign_sub_product(
            const double* a, index_t a_length, const expansion& b );

#    define expansion_sub_product( a, a_length, b )                            \
        new_expansion_on_stack( sub_product_capacity( a_length, b.length() ) ) \
            ->assign_sub_product( a, a_length, b )

    private:
        expansion( const expansion& rhs );

        expansion& operator=( const expansion& rhs );

    private:
        index_t length_;
        index_t capacity_;
        double x_[2]; // x_ is in fact of size [capacity_]

        friend class expansion_nt;
    };

    // =============== arithmetic operations ===========================

#    define expansion_create( a ) new_expansion_on_stack( 1 )->assign( a )

#    define expansion_sum( a, b )                                              \
        new_expansion_on_stack( expansion::sum_capacity( a, b ) )              \
            ->assign_sum( a, b )

#    define expansion_sum3( a, b, c )                                          \
        new_expansion_on_stack( expansion::sum_capacity( a, b, c ) )           \
            ->assign_sum( a, b, c )

#    define expansion_diff( a, b )                                             \
        new_expansion_on_stack( expansion::diff_capacity( a, b ) )             \
            ->assign_diff( a, b )

#    define expansion_product( a, b )                                          \
        new_expansion_on_stack( expansion::product_capacity( a, b ) )          \
            ->assign_product( a, b )

    // =============== determinants =====================================

#    define expansion_det2x2( a11, a12, a21, a22 )                             \
        new_expansion_on_stack(                                                \
            expansion::det2x2_capacity( a11, a12, a21, a22 ) )                 \
            ->assign_det2x2( a11, a12, a21, a22 )

#    define expansion_det3x3( a11, a12, a13, a21, a22, a23, a31, a32, a33 )    \
        new_expansion_on_stack( expansion::det3x3_capacity( a11, a12, a13,     \
                                    a21, a22, a23, a31, a32, a33 ) )           \
            ->assign_det3x3( a11, a12, a13, a21, a22, a23, a31, a32, a33 )

} // namespace GEO

#endif

/******* extracted from multi_precision.cpp *******/

// This makes sure the compiler will not optimize y = a*x+b
// with fused multiply-add, this would break the exact
// predicates.
#ifdef GEO_COMPILER_MSVC
#    pragma fp_contract( off )
#endif

namespace
{
    using namespace GEO;

    inline void fast_two_sum( double a, double b, double& x, double& y )
    {
        x = a + b;
        double bvirt = x - a;
        y = b - bvirt;
    }

#ifdef REMOVE_ME
    inline void fast_two_diff( double a, double b, double& x, double& y )
    {
        x = a - b;
        double bvirt = a - x;
        y = bvirt - b;
    }
#endif
    inline void two_product_presplit(
        double a, double b, double bhi, double blo, double& x, double& y )
    {
        x = a * b;
        double ahi;
        double alo;
        split( a, ahi, alo );
        double err1 = x - ( ahi * bhi );
        double err2 = err1 - ( alo * bhi );
        double err3 = err2 - ( ahi * blo );
        y = ( alo * blo ) - err3;
    }

    inline void two_product_2presplit( double a,
        double ahi,
        double alo,
        double b,
        double bhi,
        double blo,
        double& x,
        double& y )
    {
        x = a * b;
        double err1 = x - ( ahi * bhi );
        double err2 = err1 - ( alo * bhi );
        double err3 = err2 - ( ahi * blo );
        y = ( alo * blo ) - err3;
    }

    void two_two_product( const double* a, const double* b, double* x )
    {
        double _0, _1, _2;
        double _i, _j, _k, _l, _m, _n;

        // If the target processor supports the FMA (Fused Multiply Add)
        // instruction, then the product of two doubles into a length-2
        // expansion can be implemented as follows. Thanks to Marc Glisse
        // for the information.
        // Note: under gcc, automatic generations of fma() for a*b+c needs
        // to be deactivated, using -ffp-contract=off, else it may break
        // other functions such as fast_expansion_sum_zeroelim().
#ifdef FP_FAST_FMA
        two_product( a[0], b[0], _i, x[0] );
        two_product( a[1], b[0], _j, _0 );
        two_sum( _i, _0, _k, _1 );
        fast_two_sum( _j, _k, _l, _2 );
        two_product( a[0], b[1], _i, _0 );
        two_sum( _1, _0, _k, x[1] );
        two_sum( _2, _k, _j, _1 );
        two_sum( _l, _j, _m, _2 );
        two_product( a[1], b[1], _j, _0 );
        two_sum( _i, _0, _n, _0 );
        two_sum( _1, _0, _i, x[2] );
        two_sum( _2, _i, _k, _1 );
        two_sum( _m, _k, _l, _2 );
        two_sum( _j, _n, _k, _0 );
        two_sum( _1, _0, _j, x[3] );
        two_sum( _2, _j, _i, _1 );
        two_sum( _l, _i, _m, _2 );
        two_sum( _1, _k, _i, x[4] );
        two_sum( _2, _i, _k, x[5] );
        two_sum( _m, _k, x[7], x[6] );
#else
        double a0hi, a0lo;
        split( a[0], a0hi, a0lo );
        double bhi, blo;
        split( b[0], bhi, blo );
        two_product_2presplit( a[0], a0hi, a0lo, b[0], bhi, blo, _i, x[0] );
        double a1hi, a1lo;
        split( a[1], a1hi, a1lo );
        two_product_2presplit( a[1], a1hi, a1lo, b[0], bhi, blo, _j, _0 );
        two_sum( _i, _0, _k, _1 );
        fast_two_sum( _j, _k, _l, _2 );
        split( b[1], bhi, blo );
        two_product_2presplit( a[0], a0hi, a0lo, b[1], bhi, blo, _i, _0 );
        two_sum( _1, _0, _k, x[1] );
        two_sum( _2, _k, _j, _1 );
        two_sum( _l, _j, _m, _2 );
        two_product_2presplit( a[1], a1hi, a1lo, b[1], bhi, blo, _j, _0 );
        two_sum( _i, _0, _n, _0 );
        two_sum( _1, _0, _i, x[2] );
        two_sum( _2, _i, _k, _1 );
        two_sum( _m, _k, _l, _2 );
        two_sum( _j, _n, _k, _0 );
        two_sum( _1, _0, _j, x[3] );
        two_sum( _2, _j, _i, _1 );
        two_sum( _l, _i, _m, _2 );
        two_sum( _1, _k, _i, x[4] );
        two_sum( _2, _i, _k, x[5] );
        two_sum( _m, _k, x[7], x[6] );
#endif
    }

    void scale_expansion_zeroelim( const expansion& e, double b, expansion& h )
    {
        double Q, sum;
        double hh;
        double product1;
        double product0;
        index_t eindex, hindex;

        // If the target processor supports the FMA (Fused Multiply Add)
        // instruction, then the product of two doubles into a length-2
        // expansion can be implemented as follows. Thanks to Marc Glisse
        // for the information.
        // Note: under gcc, automatic generations of fma() for a*b+c needs
        // to be deactivated, using -ffp-contract=off, else it may break
        // other functions such as fast_expansion_sum_zeroelim().
#ifndef FP_FAST_FMA
        double bhi, blo;
#endif
        index_t elen = e.length();

        // Sanity check: e and h cannot be the same.
        geo_debug_assert( &e != &h );

#ifdef FP_FAST_FMA
        two_product( e[0], b, Q, hh );
#else
        split( b, bhi, blo );
        two_product_presplit( e[0], b, bhi, blo, Q, hh );
#endif

        hindex = 0;
        if( hh != 0 )
        {
            h[hindex++] = hh;
        }
        for( eindex = 1; eindex < elen; eindex++ )
        {
            double enow = e[eindex];
#ifdef FP_FAST_FMA
            two_product( enow, b, product1, product0 );
#else
            two_product_presplit( enow, b, bhi, blo, product1, product0 );
#endif
            two_sum( Q, product0, sum, hh );
            if( hh != 0 )
            {
                h[hindex++] = hh;
            }
            fast_two_sum( product1, sum, Q, hh );
            if( hh != 0 )
            {
                h[hindex++] = hh;
            }
        }
        if( ( Q != 0.0 ) || ( hindex == 0 ) )
        {
            h[hindex++] = Q;
        }
        h.set_length( hindex );
    }

    void fast_expansion_sum_zeroelim(
        const expansion& e, const expansion& f, expansion& h )
    {
        double Q;
        double Qnew;
        double hh;
        index_t eindex, findex, hindex;
        double enow, fnow;
        index_t elen = e.length();
        index_t flen = f.length();

        // sanity check: h cannot be e or f
        geo_debug_assert( &h != &e );
        geo_debug_assert( &h != &f );

        enow = e[0];
        fnow = f[0];
        eindex = findex = 0;
        if( ( fnow > enow ) == ( fnow > -enow ) )
        {
            Q = enow;
            enow = e[++eindex];
        }
        else
        {
            Q = fnow;
            fnow = f[++findex];
        }
        hindex = 0;
        if( ( eindex < elen ) && ( findex < flen ) )
        {
            if( ( fnow > enow ) == ( fnow > -enow ) )
            {
                fast_two_sum( enow, Q, Qnew, hh );
                enow = e[++eindex];
            }
            else
            {
                fast_two_sum( fnow, Q, Qnew, hh );
                fnow = f[++findex];
            }
            Q = Qnew;
            if( hh != 0.0 )
            {
                h[hindex++] = hh;
            }
            while( ( eindex < elen ) && ( findex < flen ) )
            {
                if( ( fnow > enow ) == ( fnow > -enow ) )
                {
                    two_sum( Q, enow, Qnew, hh );
                    enow = e[++eindex];
                }
                else
                {
                    two_sum( Q, fnow, Qnew, hh );
                    fnow = f[++findex];
                }
                Q = Qnew;
                if( hh != 0.0 )
                {
                    h[hindex++] = hh;
                }
            }
        }
        while( eindex < elen )
        {
            two_sum( Q, enow, Qnew, hh );
            enow = e[++eindex];
            Q = Qnew;
            if( hh != 0.0 )
            {
                h[hindex++] = hh;
            }
        }
        while( findex < flen )
        {
            two_sum( Q, fnow, Qnew, hh );
            fnow = f[++findex];
            Q = Qnew;
            if( hh != 0.0 )
            {
                h[hindex++] = hh;
            }
        }
        if( ( Q != 0.0 ) || ( hindex == 0 ) )
        {
            h[hindex++] = Q;
        }
        h.set_length( hindex );
    }

    void fast_expansion_diff_zeroelim(
        const expansion& e, const expansion& f, expansion& h )
    {
        double Q;
        double Qnew;
        double hh;
        index_t eindex, findex, hindex;
        double enow, fnow;
        index_t elen = e.length();
        index_t flen = f.length();

        // sanity check: h cannot be e or f
        geo_debug_assert( &h != &e );
        geo_debug_assert( &h != &f );

        enow = e[0];
        fnow = -f[0];
        eindex = findex = 0;
        if( ( fnow > enow ) == ( fnow > -enow ) )
        {
            Q = enow;
            enow = e[++eindex];
        }
        else
        {
            Q = fnow;
            fnow = -f[++findex];
        }
        hindex = 0;
        if( ( eindex < elen ) && ( findex < flen ) )
        {
            if( ( fnow > enow ) == ( fnow > -enow ) )
            {
                fast_two_sum( enow, Q, Qnew, hh );
                enow = e[++eindex];
            }
            else
            {
                fast_two_sum( fnow, Q, Qnew, hh );
                fnow = -f[++findex];
            }
            Q = Qnew;
            if( hh != 0.0 )
            {
                h[hindex++] = hh;
            }
            while( ( eindex < elen ) && ( findex < flen ) )
            {
                if( ( fnow > enow ) == ( fnow > -enow ) )
                {
                    two_sum( Q, enow, Qnew, hh );
                    enow = e[++eindex];
                }
                else
                {
                    two_sum( Q, fnow, Qnew, hh );
                    fnow = -f[++findex];
                }
                Q = Qnew;
                if( hh != 0.0 )
                {
                    h[hindex++] = hh;
                }
            }
        }
        while( eindex < elen )
        {
            two_sum( Q, enow, Qnew, hh );
            enow = e[++eindex];
            Q = Qnew;
            if( hh != 0.0 )
            {
                h[hindex++] = hh;
            }
        }
        while( findex < flen )
        {
            two_sum( Q, fnow, Qnew, hh );
            fnow = -f[++findex];
            Q = Qnew;
            if( hh != 0.0 )
            {
                h[hindex++] = hh;
            }
        }
        if( ( Q != 0.0 ) || ( hindex == 0 ) )
        {
            h[hindex++] = Q;
        }
        h.set_length( hindex );
    }
} // namespace

namespace GEO
{
    // Values for IEEE 754 doubles, as computed by PCK::initialize(): set at
    // compile time for the predicates to be exact even if the library has
    // not been initialized.
    double expansion_splitter_ = 134217729.0; // 2^27 + 1
    double expansion_epsilon_ = 1.1102230246251565404e-16; // 2^-53

    // =============  expansion sum and difference =========================

    expansion& expansion::assign_sum( const expansion& a, const expansion& b )
    {
        geo_debug_assert( capacity() >= sum_capacity( a, b ) );
        fast_expansion_sum_zeroelim( a, b, *this );
        return *this;
    }

    expansion& expansion::assign_sum(
        const expansion& a, const expansion& b, const expansion& c )
    {
        geo_debug_assert( capacity() >= sum_capacity( a, b, c ) );
        expansion& ab = expansion_sum( a, b );
        this->assign_sum( ab, c );
        return *this;
    }

    expansion& expansion::assign_diff( const expansion& a, const expansion& b )
    {
        geo_debug_assert( capacity() >= diff_capacity( a, b ) );
        fast_expansion_diff_zeroelim( a, b, *this );
        return *this;
    }

    // =============  expansion product ==================================

    // Recursive helper function for product implementation
    expansion& expansion::assign_sub_product(
        const double* a, index_t a_length, const expansion& b )
    {
        geo_debug_assert(
            capacity() >= sub_product_capacity( a_length, b.length() ) );
        if( a_length == 1 )
        {
            scale_expansion_zeroelim( b, a[0], *this );
        }
        else
        {
            // "Distillation" (see Shewchuk's paper) is computed recursively,
            // by splitting the list of expansions to sum into two halves.
            const double* a1 = a;
            index_t a1_length = a_length / 2;
            const double* a2 = a1 + a1_length;
            index_t a2_length = a_length - a1_length;
            expansion& a1b = expansion_sub_product( a1, a1_length, b );
            expansion& a2b = expansion_sub_product( a2, a2_length, b );
            this->assign_sum( a1b, a2b );
        }
        return *this;
    }

    expansion& expansion::assign_product(
        const expansion& a, const expansion& b )
    {
        geo_debug_assert( capacity() >= product_capacity( a, b ) );
        if( a.length() == 0 || b.length() == 0 )
        {
            x_[0] = 0.0;
            set_length( 0 );
        }
        else if( a.length() == 1 && b.length() == 1 )
        {
            two_product( a[0], b[0], x_[1], x_[0] );
            set_length( 2 );
        }
        else if( a.length() == 1 )
        {
            scale_expansion_zeroelim( b, a[0], *this );
        }
        else if( b.length() == 1 )
        {
            scale_expansion_zeroelim( a, b[0], *this );
        }
        else if( a.length() == 2 && b.length() == 2 )
        {
            two_two_product( a.data(), b.data(), x_ );
            set_length( 8 );
        }
        else
        {
            // Recursive distillation: the shortest expansion
            // is split into two parts.
            if( a.length() < b.length() )
            {
                const double* a1 = a.data();
                index_t a1_length = a.length() / 2;
                const double* a2 = a1 + a1_length;
                index_t a2_length = a.length() - a1_length;
                expansion& a1b = expansion_sub_product( a1, a1_length, b );
                expansion& a2b = expansion_sub_product( a2, a2_length, b );
                this->assign_sum( a1b, a2b );
            }
            else
            {
                const double* b1 = b.data();
                index_t b1_length = b.length() / 2;
                const double* b2 = b1 + b1_length;
                index_t b2_length = b.length() - b1_length;
                expansion& ab1 = expansion_sub_product( b1, b1_length, a );
                expansion& ab2 = expansion_sub_product( b2, b2_length, a );
                this->assign_sum( ab1, ab2 );
            }
        }
        return *this;
    }

    // =============  determinants ==========================================

    expansion& expansion::assign_det2x2( const expansion& a11,
        const expansion& a12,
        const expansion& a21,
        const expansion& a22 )
    {
        const expansion& a11a22 = expansion_product( a11, a22 );
        const expansion& a12a21 = expansion_product( a12, a21 );
        return this->assign_diff( a11a22, a12a21 );
    }

    expansion& expansion::assign_det3x3( const expansion& a11,
        const expansion& a12,
        const expansion& a13,
        const expansion& a21,
        const expansion& a22,
        const expansion& a23,
        const expansion& a31,
        const expansion& a32,
        const expansion& a33 )
    {
        // Development w.r.t. first row
        const expansion& c11 = expansion_det2x2( a22, a23, a32, a33 );
        const expansion& c12 = expansion_det2x2( a23, a21, a33, a31 );
        const expansion& c13 = expansion_det2x2( a21, a22, a31, a32 );
        const expansion& a11c11 = expansion_product( a11, c11 );
        const expansion& a12c12 = expansion_product( a12, c12 );
        const expansion& a13c13 = expansion_product( a13, c13 );
        return this->assign_sum( a11c11, a12c12, a13c13 );
    }

    /******* extracted from predicates/orient2d.h *******/

    inline int orient_2d_filter( const geode::Point2D& p0,
        const geode::Point2D& p1,
        const geode::Point2D& p2 )
    {
        double a11;
        a11 = ( p1.value( 0 ) - p0.value( 0 ) );
        double a12;
        a12 = ( p1.value( 1 ) - p0.value( 1 ) );
        double a21;
        a21 = ( p2.value( 0 ) - p0.value( 0 ) );
        double a22;
        a22 = ( p2.value( 1 ) - p0.value( 1 ) );
        double Delta;
        Delta = ( ( a11 * a22 ) - ( a12 * a21 ) );
        int int_tmp_result;
        double eps;
        double max1 = fabs( a11 );
        if( ( max1 < fabs( a12 ) ) )
        {
            max1 = fabs( a12 );
        }
        double max2 = fabs( a21 );
        if( ( max2 < fabs( a22 ) ) )
        {
            max2 = fabs( a22 );
        }
        double lower_bound_1;
        double upper_bound_1;
        lower_bound_1 = max1;
        upper_bound_1 = max1;
        if( ( max2 < lower_bound_1 ) )
        {
            lower_bound_1 = max2;
        }
        else
        {
            if( ( max2 > upper_bound_1 ) )
            {
                upper_bound_1 = max2;
            }
        }
        if( ( lower_bound_1 < 5.00368081960964635413e-147 ) )
        {
            return FPG_UNCERTAIN_VALUE;
        }
        else
        {
            if( ( upper_bound_1 > 1.67597599124282407923e+153 ) )
            {
                return FPG_UNCERTAIN_VALUE;
            }
            eps = ( 8.88720573725927976811e-16 * ( max1 * max2 ) );
            if( ( Delta > eps ) )
            {
                int_tmp_result = 1;
            }
            else
            {
                if( ( Delta < -eps ) )
                {
                    int_tmp_result = -1;
                }
                else
                {
                    return FPG_UNCERTAIN_VALUE;
                }
            }
        }
        return int_tmp_result;
    }

    /******* extracted from predicates/orient3d.h *******/

    inline int orient_3d_filter( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2,
        const geode::Point3D& p3 )
    {
        double a11;
        a11 = ( p1.value( 0 ) - p0.value( 0 ) );
        double a12;
        a12 = ( p1.value( 1 ) - p0.value( 1 ) );
        double a13;
        a13 = ( p1.value( 2 ) - p0.value( 2 ) );
        double a21;
        a21 = ( p2.value( 0 ) - p0.value( 0 ) );
        double a22;
        a22 = ( p2.value( 1 ) - p0.value( 1 ) );
        double a23;
        a23 = ( p2.value( 2 ) - p0.value( 2 ) );
        double a31;
        a31 = ( p3.value( 0 ) - p0.value( 0 ) );
        double a32;
        a32 = ( p3.value( 1 ) - p0.value( 1 ) );
        double a33;
        a33 = ( p3.value( 2 ) - p0.value( 2 ) );
        double Delta;
        Delta = ( ( ( a11 * ( ( a22 * a33 ) - ( a23 * a32 ) ) )
                      - ( a21 * ( ( a12 * a33 ) - ( a13 * a32 ) ) ) )
                  + ( a31 * ( ( a12 * a23 ) - ( a13 * a22 ) ) ) );
        int int_tmp_result;
        double eps;
        double max1 = fabs( a11 );
        if( ( max1 < fabs( a21 ) ) )
        {
            max1 = fabs( a21 );
        }
        if( ( max1 < fabs( a31 ) ) )
        {
            max1 = fabs( a31 );
        }
        double max2 = fabs( a12 );
        if( ( max2 < fabs( a13 ) ) )
        {
            max2 = fabs( a13 );
        }
        if( ( max2 < fabs( a22 ) ) )
        {
            max2 = fabs( a22 );
        }
        if( ( max2 < fabs( a23 ) ) )
        {
            max2 = fabs( a23 );
        }
        double max3 = fabs( a22 );
        if( ( max3 < fabs( a23 ) ) )
        {
            max3 = fabs( a23 );
        }
        if( ( max3 < fabs( a32 ) ) )
        {
            max3 = fabs( a32 );
        }
        if( ( max3 < fabs( a33 ) ) )
        {
            max3 = fabs( a33 );
        }
        double lower_bound_1;
        double upper_bound_1;
        lower_bound_1 = max1;
        upper_bound_1 = max1;
        if( ( max2 < lower_bound_1 ) )
        {
            lower_bound_1 = max2;
        }
        else
        {
            if( ( max2 > upper_bound_1 ) )
            {
                upper_bound_1 = max2;
            }
        }
        if( ( max3 < lower_bound_1 ) )
        {
            lower_bound_1 = max3;
        }
        else
        {
            if( ( max3 > upper_bound_1 ) )
            {
                upper_bound_1 = max3;
            }
        }
        if( ( lower_bound_1 < 1.63288018496748314939e-98 ) )
        {
            return FPG_UNCERTAIN_VALUE;
        }
        else
        {
            if( ( upper_bound_1 > 5.59936185544450928309e+101 ) )
            {
                return FPG_UNCERTAIN_VALUE;
            }
            eps = ( 5.11071278299732992696e-15 * ( ( max2 * max3 ) * max1 ) );
            if( ( Delta > eps ) )
            {
                int_tmp_result = 1;
            }
            else
            {
                if( ( Delta < -eps ) )
                {
                    int_tmp_result = -1;
                }
                else
                {
                    return FPG_UNCERTAIN_VALUE;
                }
            }
        }
        return int_tmp_result;
    }

    /******* extracted from predicates/dot3d.h *******/

    inline int dot_3d_filter( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2 )
    {
        double a11;
        a11 = ( p1.value( 0 ) - p0.value( 0 ) );
        double a12;
        a12 = ( p1.value( 1 ) - p0.value( 1 ) );
        double a13;
        a13 = ( p1.value( 2 ) - p0.value( 2 ) );
        double a21;
        a21 = ( p2.value( 0 ) - p0.value( 0 ) );
        double a22;
        a22 = ( p2.value( 1 ) - p0.value( 1 ) );
        double a23;
        a23 = ( p2.value( 2 ) - p0.value( 2 ) );
        double Delta;
        Delta = ( ( ( a11 * a21 ) + ( a12 * a22 ) ) + ( a13 * a23 ) );
        int int_tmp_result;
        double eps;
        double max1 = fabs( a11 );
        if( ( max1 < fabs( a12 ) ) )
        {
            max1 = fabs( a12 );
        }
        if( ( max1 < fabs( a13 ) ) )
        {
            max1 = fabs( a13 );
        }
        double max2 = fabs( a21 );
        if( ( max2 < fabs( a22 ) ) )
        {
            max2 = fabs( a22 );
        }
        if( ( max2 < fabs( a23 ) ) )
        {
            max2 = fabs( a23 );
        }
        double lower_bound_1;
        double upper_bound_1;
        lower_bound_1 = max1;
        upper_bound_1 = max1;
        if( ( max2 < lower_bound_1 ) )
        {
            lower_bound_1 = max2;
        }
        else
        {
            if( ( max2 > upper_bound_1 ) )
            {
                upper_bound_1 = max2;
            }
        }
        if( ( lower_bound_1 < 3.78232824369468524638e-147 ) )
        {
            return FPG_UNCERTAIN_VALUE;
        }
        else
        {
            if( ( upper_bound_1 > 1.67597599124282407923e+153 ) )
            {
                return FPG_UNCERTAIN_VALUE;
            }
            eps = ( 1.55534235888797977480e-15 * ( max1 * max2 ) );
            if( ( Delta > eps ) )
            {
                int_tmp_result = 1;
            }
            else
            {
                if( ( Delta < -eps ) )
                {
                    int_tmp_result = -1;
                }
                else
                {
                    return FPG_UNCERTAIN_VALUE;
                }
            }
        }
        return int_tmp_result;
    }

    /******* extracted from predicates/det3d.h *******/

    inline int det_3d_filter( const geode::Vector3D& p0,
        const geode::Vector3D& p1,
        const geode::Vector3D& p2 )
    {
        double Delta;
        Delta = ( ( ( p0.value( 0 )
                        * ( ( p1.value( 1 ) * p2.value( 2 ) )
                            - ( p1.value( 2 ) * p2.value( 1 ) ) ) )
                      - ( p1.value( 0 )
                          * ( ( p0.value( 1 ) * p2.value( 2 ) )
                              - ( p0.value( 2 ) * p2.value( 1 ) ) ) ) )
                  + ( p2.value( 0 )
                      * ( ( p0.value( 1 ) * p1.value( 2 ) )
                          - ( p0.value( 2 ) * p1.value( 1 ) ) ) ) );
        int int_tmp_result;
        double eps;
        double max1 = fabs( p0.value( 0 ) );
        if( ( max1 < fabs( p1.value( 0 ) ) ) )
        {
            max1 = fabs( p1.value( 0 ) );
        }
        if( ( max1 < fabs( p2.value( 0 ) ) ) )
        {
            max1 = fabs( p2.value( 0 ) );
        }
        double max2 = fabs( p0.value( 1 ) );
        if( ( max2 < fabs( p0.value( 2 ) ) ) )
        {
            max2 = fabs( p0.value( 2 ) );
        }
        if( ( max2 < fabs( p1.value( 1 ) ) ) )
        {
            max2 = fabs( p1.value( 1 ) );
        }
        if( ( max2 < fabs( p1.value( 2 ) ) ) )
        {
            max2 = fabs( p1.value( 2 ) );
        }
        double max3 = fabs( p1.value( 1 ) );
        if( ( max3 < fabs( p1.value( 2 ) ) ) )
        {
            max3 = fabs( p1.value( 2 ) );
        }
        if( ( max3 < fabs( p2.value( 1 ) ) ) )
        {
            max3 = fabs( p2.value( 1 ) );
        }
        if( ( max3 < fabs( p2.value( 2 ) ) ) )
        {
            max3 = fabs( p2.value( 2 ) );
        }
        double lower_bound_1;
        double upper_bound_1;
        lower_bound_1 = max1;
        upper_bound_1 = max1;
        if( ( max2 < lower_bound_1 ) )
        {
            lower_bound_1 = max2;
        }
        else
        {
            if( ( max2 > upper_bound_1 ) )
            {
                upper_bound_1 = max2;
            }
        }
        if( ( max3 < lower_bound_1 ) )
        {
            lower_bound_1 = max3;
        }
        else
        {
            if( ( max3 > upper_bound_1 ) )
            {
                upper_bound_1 = max3;
            }
        }
        if( ( lower_bound_1 < 1.92663387981871579179e-98 ) )
        {
            return FPG_UNCERTAIN_VALUE;
        }
        else
        {
            if( ( upper_bound_1 > 1.11987237108890185662e+102 ) )
            {
                return FPG_UNCERTAIN_VALUE;
            }
            eps = ( 3.11133555671680765034e-15 * ( ( max2 * max3 ) * max1 ) );
            if( ( Delta > eps ) )
            {
                int_tmp_result = 1;
            }
            else
            {
                if( ( Delta < -eps ) )
                {
                    int_tmp_result = -1;
                }
                else
                {
                    return FPG_UNCERTAIN_VALUE;
                }
            }
        }
        return int_tmp_result;
    }

    /******* dot_2d filter (not generated by FPG) *******/

    // Same error bound as orient_2d_filter: two products of differences of
    // the input coordinates, summed instead of subtracted.
    inline int dot_2d_filter( const geode::Point2D& p0,
        const geode::Point2D& p1,
        const geode::Point2D& p2 )
    {
        const double a11 = p1.value( 0 ) - p0.value( 0 );
        const double a12 = p1.value( 1 ) - p0.value( 1 );
        const double a21 = p2.value( 0 ) - p0.value( 0 );
        const double a22 = p2.value( 1 ) - p0.value( 1 );
        const double Delta = ( a11 * a21 ) + ( a12 * a22 );
        const double max1 = std::max( std::fabs( a11 ), std::fabs( a12 ) );
        const double max2 = std::max( std::fabs( a21 ), std::fabs( a22 ) );
        const double lower_bound = std::min( max1, max2 );
        const double upper_bound = std::max( max1, max2 );
        if( lower_bound < 5.00368081960964635413e-147
            || upper_bound > 1.67597599124282407923e+153 )
        {
            return FPG_UNCERTAIN_VALUE;
        }
        const double eps = 8.88720573725927976811e-16 * ( max1 * max2 );
        if( Delta > eps )
        {
            return 1;
        }
        if( Delta < -eps )
        {
            return -1;
        }
        return FPG_UNCERTAIN_VALUE;
    }

    /******* aligned_3d filter (not generated by FPG) *******/

    // Certifies that a 2x2 determinant of coordinate differences is not zero,
    // max1 and max2 bounding the absolute values of the factors of each of
    // its two products (same error bound as orient_2d_filter).
    inline bool is_det_2d_certainly_non_zero(
        double delta, double max1, double max2 )
    {
        const double lower_bound = std::min( max1, max2 );
        const double upper_bound = std::max( max1, max2 );
        if( lower_bound < 5.00368081960964635413e-147
            || upper_bound > 1.67597599124282407923e+153 )
        {
            return false;
        }
        return std::fabs( delta )
               > 8.88720573725927976811e-16 * ( max1 * max2 );
    }

    // Returns 1 if the points are certainly not aligned, FPG_UNCERTAIN_VALUE
    // otherwise. The points are aligned if and only if the three components
    // of (p1-p0)x(p2-p0) are zero: one component certified non-zero is
    // enough to conclude.
    inline int aligned_3d_filter( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2 )
    {
        const double a11 = p1.value( 0 ) - p0.value( 0 );
        const double a12 = p1.value( 1 ) - p0.value( 1 );
        const double a13 = p1.value( 2 ) - p0.value( 2 );
        const double a21 = p2.value( 0 ) - p0.value( 0 );
        const double a22 = p2.value( 1 ) - p0.value( 1 );
        const double a23 = p2.value( 2 ) - p0.value( 2 );
        const double max_x = std::max( std::fabs( a11 ), std::fabs( a21 ) );
        const double max_y = std::max( std::fabs( a12 ), std::fabs( a22 ) );
        const double max_z = std::max( std::fabs( a13 ), std::fabs( a23 ) );
        if( is_det_2d_certainly_non_zero(
                ( a12 * a23 ) - ( a22 * a13 ), max_y, max_z )
            || is_det_2d_certainly_non_zero(
                ( a13 * a21 ) - ( a23 * a11 ), max_z, max_x )
            || is_det_2d_certainly_non_zero(
                ( a11 * a22 ) - ( a21 * a12 ), max_x, max_y ) )
        {
            return 1;
        }
        return FPG_UNCERTAIN_VALUE;
    }

    /******* zero-filters *******/

    // Zero-filters (T. Bartels, V. Fisikopoulos, M. Weiser, "Fast
    // Floating-Point Filters for Robust Predicates", 2022, Definition 19).
    // They certify that a polynomial is exactly zero when each of its
    // products has an exactly zero factor. The difference of two floating
    // point numbers is zero only if they are equal, so degenerate inputs
    // sharing coordinates (duplicated points, grid aligned points, points
    // on an axis aligned plane or line) are decided without exact
    // arithmetic. The floating-point filters cannot decide such cases.

    inline bool is_product_zero( double a, double b )
    {
        return a == 0. || b == 0.;
    }

    inline bool is_det_2d_zero( double a11, double a12, double a21, double a22 )
    {
        return is_product_zero( a11, a22 ) && is_product_zero( a12, a21 );
    }

    inline bool is_det_3d_zero( double a11,
        double a12,
        double a13,
        double a21,
        double a22,
        double a23,
        double a31,
        double a32,
        double a33 )
    {
        return ( a11 == 0. || is_det_2d_zero( a22, a23, a32, a33 ) )
               && ( a21 == 0. || is_det_2d_zero( a12, a13, a32, a33 ) )
               && ( a31 == 0. || is_det_2d_zero( a12, a13, a22, a23 ) );
    }

    inline bool is_orient_2d_zero( const geode::Point2D& p0,
        const geode::Point2D& p1,
        const geode::Point2D& p2 )
    {
        return is_det_2d_zero( p1.value( 0 ) - p0.value( 0 ),
            p1.value( 1 ) - p0.value( 1 ), p2.value( 0 ) - p0.value( 0 ),
            p2.value( 1 ) - p0.value( 1 ) );
    }

    inline bool is_orient_3d_zero( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2,
        const geode::Point3D& p3 )
    {
        return is_det_3d_zero( p1.value( 0 ) - p0.value( 0 ),
            p1.value( 1 ) - p0.value( 1 ), p1.value( 2 ) - p0.value( 2 ),
            p2.value( 0 ) - p0.value( 0 ), p2.value( 1 ) - p0.value( 1 ),
            p2.value( 2 ) - p0.value( 2 ), p3.value( 0 ) - p0.value( 0 ),
            p3.value( 1 ) - p0.value( 1 ), p3.value( 2 ) - p0.value( 2 ) );
    }

    inline bool is_det_3d_zero( const geode::Vector3D& p0,
        const geode::Vector3D& p1,
        const geode::Vector3D& p2 )
    {
        return is_det_3d_zero( p0.value( 0 ), p0.value( 1 ), p0.value( 2 ),
            p1.value( 0 ), p1.value( 1 ), p1.value( 2 ), p2.value( 0 ),
            p2.value( 1 ), p2.value( 2 ) );
    }

    inline bool is_aligned_3d_zero( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2 )
    {
        const double a11 = p1.value( 0 ) - p0.value( 0 );
        const double a12 = p1.value( 1 ) - p0.value( 1 );
        const double a13 = p1.value( 2 ) - p0.value( 2 );
        const double a21 = p2.value( 0 ) - p0.value( 0 );
        const double a22 = p2.value( 1 ) - p0.value( 1 );
        const double a23 = p2.value( 2 ) - p0.value( 2 );
        return is_det_2d_zero( a12, a13, a22, a23 )
               && is_det_2d_zero( a13, a11, a23, a21 )
               && is_det_2d_zero( a11, a12, a21, a22 );
    }

    inline bool is_dot_3d_zero( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2 )
    {
        return is_product_zero( p1.value( 0 ) - p0.value( 0 ),
                   p2.value( 0 ) - p0.value( 0 ) )
               && is_product_zero( p1.value( 1 ) - p0.value( 1 ),
                   p2.value( 1 ) - p0.value( 1 ) )
               && is_product_zero( p1.value( 2 ) - p0.value( 2 ),
                   p2.value( 2 ) - p0.value( 2 ) );
    }

    inline bool is_dot_2d_zero( const geode::Point2D& p0,
        const geode::Point2D& p1,
        const geode::Point2D& p2 )
    {
        return is_product_zero( p1.value( 0 ) - p0.value( 0 ),
                   p2.value( 0 ) - p0.value( 0 ) )
               && is_product_zero( p1.value( 1 ) - p0.value( 1 ),
                   p2.value( 1 ) - p0.value( 1 ) );
    }

    /******* adaptive stages *******/

    // Stages B and C of the adaptive predicates of J. R. Shewchuk, "Adaptive
    // Precision Floating-Point Arithmetic and Fast Robust Geometric
    // Predicates", 1997 (predicates.c, public domain). They are evaluated
    // when the floating-point filter fails: stage B computes exactly the
    // determinant of the rounded coordinate differences, with short
    // expansions. Stage C checks whether these differences were exact, or
    // adds the first order corrections of their roundoff errors. If still
    // uncertain, the full exact evaluation is required.
    namespace adaptive
    {
        constexpr double EPSILON = 1.1102230246251565404e-16; // 2^-53
        constexpr double RESULT_ERRBOUND = ( 3.0 + 8.0 * EPSILON ) * EPSILON;
        constexpr double CCW_ERRBOUND_B = ( 2.0 + 12.0 * EPSILON ) * EPSILON;
        constexpr double CCW_ERRBOUND_C =
            ( 9.0 + 64.0 * EPSILON ) * EPSILON * EPSILON;
        constexpr double O3D_ERRBOUND_B = ( 3.0 + 28.0 * EPSILON ) * EPSILON;
        constexpr double O3D_ERRBOUND_C =
            ( 26.0 + 288.0 * EPSILON ) * EPSILON * EPSILON;

        inline void fast_two_sum( double a, double b, double& x, double& y )
        {
            x = a + b;
            const double bvirt = x - a;
            y = b - bvirt;
        }

        inline double two_diff_tail( double a, double b )
        {
            double x;
            double y;
            two_diff( a, b, x, y );
            return y;
        }

        // Exact a1 + a0 - b, as x2 + x1 + x0
        inline void two_one_diff(
            double a1, double a0, double b, double& x2, double& x1, double& x0 )
        {
            double i;
            two_diff( a0, b, i, x0 );
            two_sum( a1, i, x2, x1 );
        }

        // Exact a*b - c*d as a 4 components expansion
        inline void two_two_product_diff(
            double a, double b, double c, double d, double x[4] )
        {
            double ab1;
            double ab0;
            two_product( a, b, ab1, ab0 );
            double cd1;
            double cd0;
            two_product( c, d, cd1, cd0 );
            double j;
            double r0;
            two_one_diff( ab1, ab0, cd0, j, r0, x[0] );
            two_one_diff( j, r0, cd1, x[3], x[2], x[1] );
        }

        // h cannot be e or f.
        inline int fast_expansion_sum_zeroelim(
            int elen, const double* e, int flen, const double* f, double* h )
        {
            int eindex = 0;
            int findex = 0;
            int hindex = 0;
            double enow = e[0];
            double fnow = f[0];
            const auto next_e = [&] {
                eindex++;
                enow = eindex < elen ? e[eindex] : 0.;
            };
            const auto next_f = [&] {
                findex++;
                fnow = findex < flen ? f[findex] : 0.;
            };
            double Q;
            double Qnew;
            double hh;
            if( ( fnow > enow ) == ( fnow > -enow ) )
            {
                Q = enow;
                next_e();
            }
            else
            {
                Q = fnow;
                next_f();
            }
            if( eindex < elen && findex < flen )
            {
                if( ( fnow > enow ) == ( fnow > -enow ) )
                {
                    fast_two_sum( enow, Q, Qnew, hh );
                    next_e();
                }
                else
                {
                    fast_two_sum( fnow, Q, Qnew, hh );
                    next_f();
                }
                Q = Qnew;
                if( hh != 0. )
                {
                    h[hindex++] = hh;
                }
                while( eindex < elen && findex < flen )
                {
                    if( ( fnow > enow ) == ( fnow > -enow ) )
                    {
                        two_sum( Q, enow, Qnew, hh );
                        next_e();
                    }
                    else
                    {
                        two_sum( Q, fnow, Qnew, hh );
                        next_f();
                    }
                    Q = Qnew;
                    if( hh != 0. )
                    {
                        h[hindex++] = hh;
                    }
                }
            }
            while( eindex < elen )
            {
                two_sum( Q, enow, Qnew, hh );
                next_e();
                Q = Qnew;
                if( hh != 0. )
                {
                    h[hindex++] = hh;
                }
            }
            while( findex < flen )
            {
                two_sum( Q, fnow, Qnew, hh );
                next_f();
                Q = Qnew;
                if( hh != 0. )
                {
                    h[hindex++] = hh;
                }
            }
            if( Q != 0. || hindex == 0 )
            {
                h[hindex++] = Q;
            }
            return hindex;
        }

        // e and h cannot be the same.
        inline int scale_expansion_zeroelim(
            int elen, const double* e, double b, double* h )
        {
            int hindex = 0;
            double Q;
            double hh;
            two_product( e[0], b, Q, hh );
            if( hh != 0. )
            {
                h[hindex++] = hh;
            }
            for( int eindex = 1; eindex < elen; eindex++ )
            {
                double product1;
                double product0;
                two_product( e[eindex], b, product1, product0 );
                double sum;
                two_sum( Q, product0, sum, hh );
                if( hh != 0. )
                {
                    h[hindex++] = hh;
                }
                fast_two_sum( product1, sum, Q, hh );
                if( hh != 0. )
                {
                    h[hindex++] = hh;
                }
            }
            if( Q != 0. || hindex == 0 )
            {
                h[hindex++] = Q;
            }
            return hindex;
        }

        inline double estimate( int elen, const double* e )
        {
            double Q = e[0];
            for( int eindex = 1; eindex < elen; eindex++ )
            {
                Q += e[eindex];
            }
            return Q;
        }

        // Components are non-overlapping and sorted by increasing magnitude:
        // the sign is the one of the largest non-zero component.
        inline SIGN expansion_sign( int elen, const double* e )
        {
            for( int eindex = elen - 1; eindex >= 0; eindex-- )
            {
                if( e[eindex] > 0. )
                {
                    return positive;
                }
                if( e[eindex] < 0. )
                {
                    return negative;
                }
            }
            return zero;
        }

        inline SIGN sign_of( double value )
        {
            return value > 0. ? positive : negative;
        }

        // Returns true if the sign of (p1-p0)x(p2-p0) has been determined
        inline bool orient_2d( const geode::Point2D& p0,
            const geode::Point2D& p1,
            const geode::Point2D& p2,
            SIGN& result )
        {
            const double acx = p1.value( 0 ) - p0.value( 0 );
            const double bcx = p2.value( 0 ) - p0.value( 0 );
            const double acy = p1.value( 1 ) - p0.value( 1 );
            const double bcy = p2.value( 1 ) - p0.value( 1 );
            const double detsum =
                std::fabs( acx * bcy ) + std::fabs( acy * bcx );

            double B[4];
            two_two_product_diff( acx, bcy, acy, bcx, B );
            double det = estimate( 4, B );
            double errbound = CCW_ERRBOUND_B * detsum;
            if( det >= errbound || -det >= errbound )
            {
                result = sign_of( det );
                return true;
            }

            const double acxtail =
                two_diff_tail( p1.value( 0 ), p0.value( 0 ) );
            const double bcxtail =
                two_diff_tail( p2.value( 0 ), p0.value( 0 ) );
            const double acytail =
                two_diff_tail( p1.value( 1 ), p0.value( 1 ) );
            const double bcytail =
                two_diff_tail( p2.value( 1 ), p0.value( 1 ) );
            if( acxtail == 0. && acytail == 0. && bcxtail == 0.
                && bcytail == 0. )
            {
                result = expansion_sign( 4, B );
                return true;
            }

            errbound =
                CCW_ERRBOUND_C * detsum + RESULT_ERRBOUND * std::fabs( det );
            det += ( acx * bcytail + bcy * acxtail )
                   - ( acy * bcxtail + bcx * acytail );
            if( det >= errbound || -det >= errbound )
            {
                result = sign_of( det );
                return true;
            }
            return false;
        }

        // Returns true if the sign of det(p1-p0, p2-p0, p3-p0) has been
        // determined
        inline bool orient_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2,
            const geode::Point3D& p3,
            SIGN& result )
        {
            const double adx = p1.value( 0 ) - p0.value( 0 );
            const double bdx = p2.value( 0 ) - p0.value( 0 );
            const double cdx = p3.value( 0 ) - p0.value( 0 );
            const double ady = p1.value( 1 ) - p0.value( 1 );
            const double bdy = p2.value( 1 ) - p0.value( 1 );
            const double cdy = p3.value( 1 ) - p0.value( 1 );
            const double adz = p1.value( 2 ) - p0.value( 2 );
            const double bdz = p2.value( 2 ) - p0.value( 2 );
            const double cdz = p3.value( 2 ) - p0.value( 2 );
            const double permanent =
                ( std::fabs( bdx * cdy ) + std::fabs( cdx * bdy ) )
                    * std::fabs( adz )
                + ( std::fabs( cdx * ady ) + std::fabs( adx * cdy ) )
                      * std::fabs( bdz )
                + ( std::fabs( adx * bdy ) + std::fabs( bdx * ady ) )
                      * std::fabs( cdz );

            double bc[4];
            two_two_product_diff( bdx, cdy, cdx, bdy, bc );
            double adet[8];
            const auto alen = scale_expansion_zeroelim( 4, bc, adz, adet );
            double ca[4];
            two_two_product_diff( cdx, ady, adx, cdy, ca );
            double bdet[8];
            const auto blen = scale_expansion_zeroelim( 4, ca, bdz, bdet );
            double ab[4];
            two_two_product_diff( adx, bdy, bdx, ady, ab );
            double cdet[8];
            const auto clen = scale_expansion_zeroelim( 4, ab, cdz, cdet );
            double abdet[16];
            const auto ablen =
                fast_expansion_sum_zeroelim( alen, adet, blen, bdet, abdet );
            double fin[24];
            const auto finlength =
                fast_expansion_sum_zeroelim( ablen, abdet, clen, cdet, fin );

            double det = estimate( finlength, fin );
            double errbound = O3D_ERRBOUND_B * permanent;
            if( det >= errbound || -det >= errbound )
            {
                result = sign_of( det );
                return true;
            }

            const double adxtail =
                two_diff_tail( p1.value( 0 ), p0.value( 0 ) );
            const double bdxtail =
                two_diff_tail( p2.value( 0 ), p0.value( 0 ) );
            const double cdxtail =
                two_diff_tail( p3.value( 0 ), p0.value( 0 ) );
            const double adytail =
                two_diff_tail( p1.value( 1 ), p0.value( 1 ) );
            const double bdytail =
                two_diff_tail( p2.value( 1 ), p0.value( 1 ) );
            const double cdytail =
                two_diff_tail( p3.value( 1 ), p0.value( 1 ) );
            const double adztail =
                two_diff_tail( p1.value( 2 ), p0.value( 2 ) );
            const double bdztail =
                two_diff_tail( p2.value( 2 ), p0.value( 2 ) );
            const double cdztail =
                two_diff_tail( p3.value( 2 ), p0.value( 2 ) );
            if( adxtail == 0. && bdxtail == 0. && cdxtail == 0. && adytail == 0.
                && bdytail == 0. && cdytail == 0. && adztail == 0.
                && bdztail == 0. && cdztail == 0. )
            {
                result = expansion_sign( finlength, fin );
                return true;
            }

            errbound =
                O3D_ERRBOUND_C * permanent + RESULT_ERRBOUND * std::fabs( det );
            det += ( adz
                           * ( ( bdx * cdytail + cdy * bdxtail )
                               - ( bdy * cdxtail + cdx * bdytail ) )
                       + adztail * ( bdx * cdy - bdy * cdx ) )
                   + ( bdz
                           * ( ( cdx * adytail + ady * cdxtail )
                               - ( cdy * adxtail + adx * cdytail ) )
                       + bdztail * ( cdx * ady - cdy * adx ) )
                   + ( cdz
                           * ( ( adx * bdytail + bdy * adxtail )
                               - ( ady * bdxtail + bdx * adytail ) )
                       + cdztail * ( adx * bdy - ady * bdx ) );
            if( det >= errbound || -det >= errbound )
            {
                result = sign_of( det );
                return true;
            }
            return false;
        }
    } // namespace adaptive
} // namespace GEO

/******* extracted from predicates.cpp *******/

// This makes sure the compiler will not optimize y = a*x+b
// with fused multiply-add, this would break the exact
// predicates.
#ifdef GEO_COMPILER_MSVC
#    pragma fp_contract( off )
#endif

#include <algorithm>

#define FPG_UNCERTAIN_VALUE 0

#ifdef __SSE2__
#    include <emmintrin.h>
#endif

#ifdef __AVX2__
#    include <immintrin.h>
#endif

namespace GEO
{
    // ============ orient2d ==============================================

    SIGN orient_2d_exact( const geode::Point2D& p0,
        const geode::Point2D& p1,
        const geode::Point2D& p2 )
    {
        const expansion& a11 = expansion_diff( p1.value( 0 ), p0.value( 0 ) );
        const expansion& a12 = expansion_diff( p1.value( 1 ), p0.value( 1 ) );

        const expansion& a21 = expansion_diff( p2.value( 0 ), p0.value( 0 ) );
        const expansion& a22 = expansion_diff( p2.value( 1 ), p0.value( 1 ) );

        const expansion& Delta = expansion_det2x2( a11, a12, a21, a22 );

        return Delta.sign();
    }

    // ============ orient3d ==============================================

    SIGN orient_3d_exact( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2,
        const geode::Point3D& p3 )
    {
        const expansion& a11 = expansion_diff( p1.value( 0 ), p0.value( 0 ) );
        const expansion& a12 = expansion_diff( p1.value( 1 ), p0.value( 1 ) );
        const expansion& a13 = expansion_diff( p1.value( 2 ), p0.value( 2 ) );

        const expansion& a21 = expansion_diff( p2.value( 0 ), p0.value( 0 ) );
        const expansion& a22 = expansion_diff( p2.value( 1 ), p0.value( 1 ) );
        const expansion& a23 = expansion_diff( p2.value( 2 ), p0.value( 2 ) );

        const expansion& a31 = expansion_diff( p3.value( 0 ), p0.value( 0 ) );
        const expansion& a32 = expansion_diff( p3.value( 1 ), p0.value( 1 ) );
        const expansion& a33 = expansion_diff( p3.value( 2 ), p0.value( 2 ) );

        const expansion& Delta =
            expansion_det3x3( a11, a12, a13, a21, a22, a23, a31, a32, a33 );

        return Delta.sign();
    }

    // ================================ det and dot =======================

    SIGN det_3d_exact( const geode::Vector3D& p0,
        const geode::Vector3D& p1,
        const geode::Vector3D& p2 )
    {
        const expansion& p0_0 = expansion_create( p0.value( 0 ) );
        const expansion& p0_1 = expansion_create( p0.value( 1 ) );
        const expansion& p0_2 = expansion_create( p0.value( 2 ) );

        const expansion& p1_0 = expansion_create( p1.value( 0 ) );
        const expansion& p1_1 = expansion_create( p1.value( 1 ) );
        const expansion& p1_2 = expansion_create( p1.value( 2 ) );

        const expansion& p2_0 = expansion_create( p2.value( 0 ) );
        const expansion& p2_1 = expansion_create( p2.value( 1 ) );
        const expansion& p2_2 = expansion_create( p2.value( 2 ) );

        const expansion& Delta = expansion_det3x3(
            p0_0, p0_1, p0_2, p1_0, p1_1, p1_2, p2_0, p2_1, p2_2 );
        return Delta.sign();
    }

    bool aligned_3d_exact( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2 )
    {
        const expansion& U_0 = expansion_diff( p1.value( 0 ), p0.value( 0 ) );
        const expansion& U_1 = expansion_diff( p1.value( 1 ), p0.value( 1 ) );
        const expansion& U_2 = expansion_diff( p1.value( 2 ), p0.value( 2 ) );

        const expansion& V_0 = expansion_diff( p2.value( 0 ), p0.value( 0 ) );
        const expansion& V_1 = expansion_diff( p2.value( 1 ), p0.value( 1 ) );
        const expansion& V_2 = expansion_diff( p2.value( 2 ), p0.value( 2 ) );

        const expansion& N_0 = expansion_det2x2( U_1, V_1, U_2, V_2 );
        const expansion& N_1 = expansion_det2x2( U_2, V_2, U_0, V_0 );
        const expansion& N_2 = expansion_det2x2( U_0, V_0, U_1, V_1 );

        return ( N_0.sign() == 0 && N_1.sign() == 0 && N_2.sign() == 0 );
    }

    SIGN dot_3d_exact( const geode::Point3D& p0,
        const geode::Point3D& p1,
        const geode::Point3D& p2 )
    {
        const expansion& U_0 = expansion_diff( p1.value( 0 ), p0.value( 0 ) );
        const expansion& U_1 = expansion_diff( p1.value( 1 ), p0.value( 1 ) );
        const expansion& U_2 = expansion_diff( p1.value( 2 ), p0.value( 2 ) );

        const expansion& V_0 = expansion_diff( p2.value( 0 ), p0.value( 0 ) );
        const expansion& V_1 = expansion_diff( p2.value( 1 ), p0.value( 1 ) );
        const expansion& V_2 = expansion_diff( p2.value( 2 ), p0.value( 2 ) );

        const expansion& UV_0 = expansion_product( U_0, V_0 );
        const expansion& UV_1 = expansion_product( U_1, V_1 );
        const expansion& UV_2 = expansion_product( U_2, V_2 );

        const expansion& Delta = expansion_sum3( UV_0, UV_1, UV_2 );

        return Delta.sign();
    }

    SIGN dot_2d_exact( const geode::Point2D& p0,
        const geode::Point2D& p1,
        const geode::Point2D& p2 )
    {
        const expansion& U_0 = expansion_diff( p1.value( 0 ), p0.value( 0 ) );
        const expansion& U_1 = expansion_diff( p1.value( 1 ), p0.value( 1 ) );

        const expansion& V_0 = expansion_diff( p2.value( 0 ), p0.value( 0 ) );
        const expansion& V_1 = expansion_diff( p2.value( 1 ), p0.value( 1 ) );

        const expansion& UV_0 = expansion_product( U_0, V_0 );
        const expansion& UV_1 = expansion_product( U_1, V_1 );

        const expansion& Delta = expansion_sum( UV_0, UV_1 );

        return Delta.sign();
    }
} // namespace GEO

namespace GEO
{
    namespace PCK
    {
        SIGN orient_2d( const geode::Point2D& p0,
            const geode::Point2D& p1,
            const geode::Point2D& p2 )
        {
            const auto result = SIGN( orient_2d_filter( p0, p1, p2 ) );
            if( result != zero )
            {
                return result;
            }
            if( is_orient_2d_zero( p0, p1, p2 ) )
            {
                return zero;
            }
            SIGN adaptive_result;
            if( adaptive::orient_2d( p0, p1, p2, adaptive_result ) )
            {
                return adaptive_result;
            }
            return orient_2d_exact( p0, p1, p2 );
        }

        SIGN orient_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2,
            const geode::Point3D& p3 )
        {
            const auto result = SIGN( orient_3d_filter( p0, p1, p2, p3 ) );
            if( result != zero )
            {
                return result;
            }
            if( is_orient_3d_zero( p0, p1, p2, p3 ) )
            {
                return zero;
            }
            SIGN adaptive_result;
            if( adaptive::orient_3d( p0, p1, p2, p3, adaptive_result ) )
            {
                return adaptive_result;
            }
            return orient_3d_exact( p0, p1, p2, p3 );
        }

        SIGN det_3d( const geode::Vector3D& p0,
            const geode::Vector3D& p1,
            const geode::Vector3D& p2 )
        {
            const auto result = SIGN( det_3d_filter( p0, p1, p2 ) );
            if( result != zero )
            {
                return result;
            }
            if( is_det_3d_zero( p0, p1, p2 ) )
            {
                return zero;
            }
            return det_3d_exact( p0, p1, p2 );
        }

        bool aligned_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2 )
        {
            if( aligned_3d_filter( p0, p1, p2 ) != FPG_UNCERTAIN_VALUE )
            {
                return false;
            }
            if( is_aligned_3d_zero( p0, p1, p2 ) )
            {
                return true;
            }
            return aligned_3d_exact( p0, p1, p2 );
        }

        SIGN dot_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2 )
        {
            const auto result = SIGN( dot_3d_filter( p0, p1, p2 ) );
            if( result != zero )
            {
                return result;
            }
            if( is_dot_3d_zero( p0, p1, p2 ) )
            {
                return zero;
            }
            return dot_3d_exact( p0, p1, p2 );
        }

        SIGN dot_2d( const geode::Point2D& p0,
            const geode::Point2D& p1,
            const geode::Point2D& p2 )
        {
            const auto result = SIGN( dot_2d_filter( p0, p1, p2 ) );
            if( result != zero )
            {
                return result;
            }
            if( is_dot_2d_zero( p0, p1, p2 ) )
            {
                return zero;
            }
            return dot_2d_exact( p0, p1, p2 );
        }

        void initialize()
        {
            // Taken from Jonathan Shewchuk's exactinit.
            double half;
            double check, lastcheck;
            int every_other;

            every_other = 1;
            half = 0.5;
            expansion_epsilon_ = 1.0;
            expansion_splitter_ = 1.0;
            check = 1.0;
            // Repeatedly divide `epsilon' by two until it is too small to add
            // to one without causing roundoff.  (Also check if the sum is equal
            // to the previous sum, for machines that round up instead of using
            // exact rounding.  Not that this library will work on such machines
            // anyway.
            do
            {
                lastcheck = check;
                expansion_epsilon_ *= half;
                if( every_other )
                {
                    expansion_splitter_ *= 2.0;
                }
                every_other = !every_other;
                check = 1.0 + expansion_epsilon_;
            } while( ( check != 1.0 ) && ( check != lastcheck ) );
            expansion_splitter_ += 1.0;
        }
    } // namespace PCK
} // namespace GEO
