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

#include <geode/geometry/private/predicates.h>

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

        Sign sign() const
        {
            if( length() == 0 )
            {
                return ZERO;
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
    double expansion_splitter_;
    double expansion_epsilon_;

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

    /******* extracted from predicates/aligned3d.h *******/

    inline int aligned_3d_filter( const geode::Point3D& p0,
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
        double delta1;
        delta1 = ( ( a12 * a23 ) - ( a22 * a13 ) );
        double delta2;
        delta2 = ( ( a13 * a21 ) - ( a23 * a11 ) );
        double delta3;
        delta3 = ( ( a11 * a22 ) - ( a21 * a12 ) );
        int int_tmp_result;
        double eps;
        int int_tmp_result_FFWKCAA;
        int int_tmp_result_k60Ocge;
        double max1 = fabs( a12 );
        if( ( max1 < fabs( a22 ) ) )
        {
            max1 = fabs( a22 );
        }
        double max2 = fabs( a13 );
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
            if( ( delta1 > eps ) )
            {
                int_tmp_result = 1;
            }
            else
            {
                if( ( delta1 < -eps ) )
                {
                    int_tmp_result = -1;
                }
                else
                {
                    return FPG_UNCERTAIN_VALUE;
                }
            }
        }
        double max3 = fabs( a11 );
        if( ( max3 < fabs( a21 ) ) )
        {
            max3 = fabs( a21 );
        }
        lower_bound_1 = max3;
        upper_bound_1 = max3;
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
            eps = ( 8.88720573725927976811e-16 * ( max2 * max3 ) );
            if( ( delta2 > eps ) )
            {
                int_tmp_result_FFWKCAA = 1;
            }
            else
            {
                if( ( delta2 < -eps ) )
                {
                    int_tmp_result_FFWKCAA = -1;
                }
                else
                {
                    return FPG_UNCERTAIN_VALUE;
                }
            }
        }
        lower_bound_1 = max1;
        upper_bound_1 = max1;
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
            eps = ( 8.88720573725927976811e-16 * ( max3 * max1 ) );
            if( ( delta3 > eps ) )
            {
                int_tmp_result_k60Ocge = 1;
            }
            else
            {
                if( ( delta3 < -eps ) )
                {
                    int_tmp_result_k60Ocge = -1;
                }
                else
                {
                    return FPG_UNCERTAIN_VALUE;
                }
            }
        }
        return (
            ( ( ( int_tmp_result == 0 ) && ( int_tmp_result_FFWKCAA == 0 ) )
                && ( int_tmp_result_k60Ocge == 0 ) )
                ? 0
                : 1 );
    }
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

    Sign orient_2d_exact( const geode::Point2D& p0,
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

    Sign orient_3d_exact( const geode::Point3D& p0,
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

    Sign det_3d_exact( const geode::Vector3D& p0,
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

    Sign dot_3d_exact( const geode::Point3D& p0,
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

    Sign dot_2d_exact( const geode::Point2D& p0,
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
        Sign orient_2d( const geode::Point2D& p0,
            const geode::Point2D& p1,
            const geode::Point2D& p2 )
        {
            Sign result = Sign( orient_2d_filter( p0, p1, p2 ) );
            if( result == 0 )
            {
                result = orient_2d_exact( p0, p1, p2 );
            }
            return result;
        }

        Sign orient_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2,
            const geode::Point3D& p3 )
        {
            Sign result = Sign( orient_3d_filter( p0, p1, p2, p3 ) );
            if( result == 0 )
            {
                result = orient_3d_exact( p0, p1, p2, p3 );
            }
            return result;
        }

        Sign det_3d( const geode::Vector3D& p0,
            const geode::Vector3D& p1,
            const geode::Vector3D& p2 )
        {
            Sign result = Sign( det_3d_filter( p0, p1, p2 ) );
            if( result == 0 )
            {
                result = det_3d_exact( p0, p1, p2 );
            }
            return result;
        }

        bool aligned_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2 )
        {
            /*
            Sign result = Sign(
            aligned_3d_filter(p0,p1,p2)
            );
            if(result != 0) {
            return false;
            }
            */
            return aligned_3d_exact( p0, p1, p2 );
        }

        Sign dot_3d( const geode::Point3D& p0,
            const geode::Point3D& p1,
            const geode::Point3D& p2 )
        {
            Sign result = Sign( dot_3d_filter( p0, p1, p2 ) );
            if( result == 0 )
            {
                result = dot_3d_exact( p0, p1, p2 );
            }
            return result;
        }

        Sign dot_2d( const geode::Point2D& p0,
            const geode::Point2D& p1,
            const geode::Point2D& p2 )
        {
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
