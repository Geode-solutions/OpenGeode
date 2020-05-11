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

#pragma once

#include <memory>

#include <bitsery/bitsery.h>
#include <bitsery/ext/std_smart_ptr.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/common.h>

namespace geode
{
    template < typename T >
    class PImpl
    {
        OPENGEODE_DISABLE_COPY( PImpl );

    public:
        template < typename... Args >
        explicit PImpl( Args &&... );
        PImpl( PImpl< T > && );
        ~PImpl();
        T *operator->();
        const T *operator->() const;
        T &operator*();
        const T &operator*() const;

        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, PImpl >{},
                []( Archive &archive, PImpl &impl ) {
                    archive.ext( impl.pimpl_, bitsery::ext::StdSmartPtr{} );
                } );
        }

    private:
        std::unique_ptr< T > pimpl_;
    };

#define IMPLEMENTATION_MEMBER( impl )                                          \
    class Impl;                                                                \
    geode::PImpl< Impl > impl

} // namespace geode
