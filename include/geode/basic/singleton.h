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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#pragma once

#include <iostream>
#include <mutex>
#include <typeinfo>

#include <absl/memory/memory.h>

#include <geode/basic/common.h>
#include <geode/basic/pimpl.h>

namespace geode
{
    /*!
     *  Cross platform singleton implementation
     *  Classic templated singleton cannot be exported on Windows.
     *  To ensure a unique instance of the singleton, we store it and
     *  export methods to retrieve the unique instance.
     *
     *  To use this class, inherit from it and use the protected
     *  method Singleton::instance().
     */
    class opengeode_basic_api Singleton
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Singleton );

    public:
        virtual ~Singleton();

    protected:
        Singleton();

        template < class SingletonType >
        static SingletonType& instance()
        {
            const auto& type = typeid( SingletonType );
            auto* singleton =
                dynamic_cast< SingletonType* >( instance( type ) );
            if( singleton != nullptr )
            {
                return *singleton;
            }
            static std::mutex lock;
            const std::lock_guard< std::mutex > locking{ lock };
            singleton = dynamic_cast< SingletonType* >( instance( type ) );
            if( singleton == nullptr )
            {
                singleton = new SingletonType{};
                set_instance( type, singleton );
            }
            return *singleton;
        }

    private:
        static Singleton& instance();
        static void set_instance(
            const std::type_info& type, Singleton* singleton );
        static Singleton* instance( const std::type_info& type );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
