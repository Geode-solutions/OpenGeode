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

namespace geode
{
    /*!
     * Generic factory
     * Example of use with A the base class and B, C derived classes
     *      // Instantiation
     *      using MyFactory = Factory< std::string, A, int, double >;
     *      inline MyFactory my_factory{};
     *      // where: - std::string is the Key type
     *      //        - int and double are the constructor arguments required by
     *      //        the derived classes B and C
     *
     *      // Registration
     *      my_factory.register_creator< B >( "key_value_for_B" );
     *      my_factory.register_creator< C >( "key_value_for_C" );
     *      // B and C constructors take an int and a double
     *
     *      // Creation
     *      auto c = my_factory.create( "key_value_for_C", 2, 8.6 );
     *      // where c is a std::unique_ptr< A >
     */
    template < typename Key, typename BaseClassType, typename... Args >
    class Factory
    {
    public:
        using BaseClass = BaseClassType;
        using Creator = typename std::add_pointer< std::unique_ptr< BaseClass >(
            Args... ) >::type;
        static_assert( std::has_virtual_destructor< BaseClass >::value,
            "BaseClass must have a virtual destructor" );

    public:
        template < typename DerivedClass >
        inline void register_creator( Key key )
        {
            static_assert( std::is_base_of< BaseClass, DerivedClass >::value,
                "DerivedClass is not a subclass of BaseClass" );
            static_assert(
                std::is_constructible< DerivedClass, Args... >::value,
                "DerivedClass is not constructible with Args..." );
            if( !store_
                     .emplace( std::move( key ),
                         Creator( create_function_impl< DerivedClass > ) )
                     .second )
            {
                Logger::warn(
                    "Factory: Trying to register twice the same key" );
            }
        }

        inline std::unique_ptr< BaseClass > create(
            const Key& key, Args... args ) const
        {
            const auto creator = store_.find( key );
            OPENGEODE_EXCEPTION( creator != store_.end(),
                "[Factory::create] Factory does not "
                "contain the requested key" );
            return creator->second( std::forward< Args >( args )... );
        }

        inline absl::FixedArray< Key > list_creators() const
        {
            absl::FixedArray< Key > creators( store_.size() );
            index_t count{ 0 };
            for( const auto& creator : store_ )
            {
                creators[count++] = creator.first;
            }
            return creators;
        }

        inline bool has_creator( const Key& key ) const
        {
            return store_.find( key ) != store_.end();
        }

    private:
        template < typename DerivedClass >
        static inline std::unique_ptr< BaseClass > create_function_impl(
            Args... args )
        {
            return std::unique_ptr< BaseClass >{ new DerivedClass{
                std::forward< Args >( args )... } };
        }

    private:
        absl::flat_hash_map< Key, Creator > store_;
    };
} // namespace geode

#define FACTORY( Type, Name )                                                  \
    template < geode::index_t dimension >                                      \
    constexpr const Type< dimension >& Name()

#define FACTORY_2D( Type, Name )                                               \
    FACTORY( Type, Name );                                                     \
    inline Type< 2 > Name##_2d{};                                              \
    template <>                                                                \
    constexpr const Type< 2 >& Name< 2 >()                                     \
    {                                                                          \
        return Name##_2d;                                                      \
    }

#define FACTORY_3D( Type, Name )                                               \
    FACTORY( Type, Name );                                                     \
    inline Type< 3 > Name##_3d{};                                              \
    template <>                                                                \
    constexpr const Type< 3 >& Name< 3 >()                                     \
    {                                                                          \
        return Name##_3d;                                                      \
    }

#define FACTORY_2D_AND_3D( Type, Name )                                        \
    FACTORY_2D( Type, Name );                                                  \
    FACTORY_3D( Type, Name )
