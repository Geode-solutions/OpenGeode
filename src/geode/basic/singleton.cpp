/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/singleton.h>

#include <geode/basic/pimpl_impl.h>

#include <map>

namespace geode
{
    class Singleton::Impl
    {
    public:
        void set_instance( const std::type_info &type, Singleton *singleton )
        {
            singletons_[type.name()].reset( singleton );
        }

        Singleton *instance( const std::type_info &type )
        {
            auto iter = singletons_.find( type.name() );
            if( iter == singletons_.end() )
            {
                return nullptr;
            }
            return iter->second.get();
        }

    private:
        std::map< const char *, std::unique_ptr< Singleton > > singletons_;
    };

    Singleton::Singleton() {} // NOLINT

    Singleton::~Singleton() {} // NOLINT

    Singleton &Singleton::instance()
    {
        static Singleton singleton;
        return singleton;
    }

    void Singleton::set_instance(
        const std::type_info &type, Singleton *singleton )
    {
        instance().impl_->set_instance( type, singleton );
    }

    Singleton *Singleton::instance( const std::type_info &type )
    {
        return instance().impl_->instance( type );
    }
} // namespace geode
