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
    /*!
     * This can be used to restrict which class can call methods
     * Example:
     *     class A
     *     {
     *         // Add a key only B can create
     *         PASSKEY( B, KeyForB );
     *
     *     public:
     *         void restrictive_method( KeyForB )
     *         {
     *             // do something
     *         }
     *     };
     *
     *     Now, B class is the only class that can call A::restrictive_method
     *     class B
     *     {
     *     public:
     *          void run( A& a )
     *          {
     *              // the {} will implicitly create a KeyForB
     *              a.restrictive_method( {} );
     *          }
     *     };
     */
    template < typename T >
    class PassKey
    {
        friend T;

    private:
        PassKey(){};
    };

#define PASSKEY( Friend, Key ) using Key = geode::PassKey< Friend >

} // namespace geode
