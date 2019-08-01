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

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <geode/basic/common.h>

#include <geode/basic/logger.h>
#include <geode/basic/singleton.h>

namespace geode
{
    /*!
     * Generic factory
     * Example of use with A the base class and B, C derived classes
     *      // Instantiation
     *      using MyFactory = Factory< std::string, A, int, double >;
     *      // Registration
     *      MyFactory::register_creator< B >( "B" );   // B constructor takes an
     * int and a double
     *      MyFactory::register_creator< C >( "C" );   // C constructor takes an
     * int and a double
     *      // Creation
     *      std::unique_ptr< A > c = MyFactory::create( "C", 2, 8.6 );
     */
    template < typename Key, typename BaseClass, typename... Args >
    class Factory : public Singleton
    {
        static_assert( std::has_virtual_destructor< BaseClass >::value,
            "BaseClass must have a virtual destructor" );

    public:
        template < typename DerivedClass >
        static void register_creator( const Key &key )
        {
            static_assert( std::is_base_of< BaseClass, DerivedClass >::value,
                "DerivedClass is not a subclass of BaseClass" );
            static_assert(
                std::is_constructible< DerivedClass, Args... >::value,
                "DerivedClass is not constructible with Args..." );
            auto &store = get_store();
            if( !store
                     .emplace(
                         key, Creator( create_function_impl< DerivedClass > ) )
                     .second )
            {
                Logger::warn(
                    "Factory: Trying to register twice the same key" );
            }
        }

        static std::unique_ptr< BaseClass > create(
            const Key &key, Args... args )
        {
            auto &store = get_store();
            auto creator = store.find( key );
            OPENGEODE_EXCEPTION( creator != store.end(),
                "Factory does not contain the requested key" );
            return creator->second( std::forward< Args >( args )... );
        }

        static std::vector< Key > list_creators()
        {
            auto &store = get_store();
            std::vector< Key > creators;
            creators.reserve( store.size() );
            for( const auto &creator : store )
            {
                creators.emplace_back( creator.first );
            }
            return creators;
        }

        static bool has_creator( const Key &key )
        {
            auto &store = get_store();
            return store.find( key ) != store.end();
        }

        using Creator = typename std::add_pointer< std::unique_ptr< BaseClass >(
            Args... ) >::type;
        using FactoryStore = std::unordered_map< Key, Creator >;

    private:
        template < typename DerivedClass >
        static std::unique_ptr< BaseClass > create_function_impl( Args... args )
        {
            return std::unique_ptr< BaseClass >{ new DerivedClass{
                std::forward< Args >( args )... } };
        }

        static FactoryStore &get_store()
        {
            return Singleton::instance< Factory >().store_;
        }

    private:
        FactoryStore store_;
    };
} // namespace geode
