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

#include <bitsery/ext/std_smart_ptr.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl.h>

namespace geode
{
    template < typename T >
    template < typename... Args >
    PImpl< T >::PImpl( Args&&... args )
        : pimpl_{ new T{ std::forward< Args >( args )... } }
    {
    }

    template < typename T >
    PImpl< T >::PImpl( PImpl< T >&& ) noexcept = default;

    template < typename T >
    PImpl< T >::~PImpl() = default;

    template < typename T >
    PImpl< T >& PImpl< T >::operator=( PImpl< T >&& ) noexcept = default;

    template < typename T >
    void PImpl< T >::reset()
    {
        return pimpl_.reset();
    }

    template < typename T >
    const T* PImpl< T >::operator->() const
    {
        return pimpl_.get();
    }

    template < typename T >
    T* PImpl< T >::operator->()
    {
        return pimpl_.get();
    }

    template < typename T >
    T& PImpl< T >::operator*()
    {
        return *pimpl_.get();
    }

    template < typename T >
    const T& PImpl< T >::operator*() const
    {
        return *pimpl_.get();
    }

    template < typename T >
    template < typename Archive >
    void PImpl< T >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, PImpl >{ { []( Archive& archive2, PImpl& impl ) {
                archive2.ext( impl.pimpl_, bitsery::ext::StdSmartPtr{} );
            } } } );
    }
} // namespace geode
