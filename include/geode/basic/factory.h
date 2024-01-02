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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#pragma once

#include <memory>
#include <vector>

#include <absl/container/flat_hash_map.h>

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
     *      // where: - std::string is the Key type
     *      //        - int and double are the constructor arguments required by
     *      //        the derived classes B and C
     *
     *      // Registration
     *      MyFactory::register_creator< B >( "key_value_for_B" );
     *      MyFactory::register_creator< C >( "key_value_for_C" );
     *      // B and C constructors take an int and a double
     *
     *      // Creation
     *      auto c = MyFactory::create( "key_value_for_C", 2, 8.6 );
     *      // where c is a std::unique_ptr< A >
     */
    template < typename Key, typename BaseClassType, typename... Args >
    class Factory : public Singleton
    {
    public:
        using BaseClass = BaseClassType;
        using Creator = typename std::add_pointer< std::unique_ptr< BaseClass >(
            Args... ) >::type;
        using FactoryStore = absl::flat_hash_map< Key, Creator >;
        static_assert( std::has_virtual_destructor< BaseClass >::value,
            "BaseClass must have a virtual destructor" );

        template < typename DerivedClass >
        static inline void register_creator( Key key )
        {
            static_assert( std::is_base_of< BaseClass, DerivedClass >::value,
                "DerivedClass is not a subclass of BaseClass" );
            static_assert(
                std::is_constructible< DerivedClass, Args... >::value,
                "DerivedClass is not constructible with Args..." );
            auto &store = get_store();
            if( !store
                     .emplace( std::move( key ),
                         Creator( create_function_impl< DerivedClass > ) )
                     .second )
            {
                Logger::warn(
                    "Factory: Trying to register twice the same key" );
            }
        }

        static inline std::unique_ptr< BaseClass > create(
            const Key &key, Args... args )
        {
            const auto &store = get_store();
            const auto creator = store.find( key );
            OPENGEODE_EXCEPTION( creator != store.end(),
                "[Factory::create] Factory does not "
                "contain the requested key" );
            return creator->second( std::forward< Args >( args )... );
        }

        static inline absl::FixedArray< Key > list_creators()
        {
            const auto &store = get_store();
            absl::FixedArray< Key > creators( store.size() );
            index_t count{ 0 };
            for( const auto &creator : store )
            {
                creators[count++] = creator.first;
            }
            return creators;
        }

        static inline bool has_creator( const Key &key )
        {
            const auto &store = get_store();
            return store.find( key ) != store.end();
        }

    private:
        template < typename DerivedClass >
        static inline std::unique_ptr< BaseClass > create_function_impl(
            Args... args )
        {
            return std::unique_ptr< BaseClass >{ new DerivedClass{
                std::forward< Args >( args )... } };
        }

        static inline FactoryStore &get_store()
        {
            return Singleton::instance< Factory >().store_;
        }

    private:
        FactoryStore store_;
    };
} // namespace geode
