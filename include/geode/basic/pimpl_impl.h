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

namespace geode
{
    template < typename T >
    template < typename... Args >
    PImpl< T >::PImpl( Args&&... args )
        : pimpl_{ new T{ std::forward< Args >( args )... } }
    {
    }

    template < typename T >
    PImpl< T >::PImpl( PImpl< T >&& other )
        : pimpl_{ std::move( other.pimpl_ ) }
    {
    }

    template < typename T >
    PImpl< T >::~PImpl()
    {
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
} // namespace geode
