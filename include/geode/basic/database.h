/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/common.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl.h>

#include <geode/basic/identifier.h>
#include <geode/basic/uuid.h>

namespace geode
{
    class Identifier;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Stores any classes inherited from Identifier. It owns every data
     * registered. Data is also stored on disk and offload from the memory after
     * some unused time to save memory and performances.
     */
    class opengeode_basic_api Database
    {
        OPENGEODE_DISABLE_COPY( Database );
        struct Storage;

    public:
        using serializer_function = std::function< void( PContext& ) >;

        /*!
         * Class holding a const reference of data.
         * @warning Do not destroy this Data class if the const reference
         * obtained using its get() method is still in used.
         */
        class opengeode_basic_api Data
        {
        public:
            Data( std::shared_ptr< Storage > storage );
            ~Data();
            Data( Data&& other );

            template < typename DataType >
            const DataType& get()
            {
                const auto* typed_data =
                    dynamic_cast< const DataType* >( &data() );
                OPENGEODE_EXCEPTION(
                    typed_data, "[Data::get] Cannot cast data into DataType" );
                return *typed_data;
            }

        private:
            const Identifier& data() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

    public:
        Database( absl::string_view directory );
        ~Database();

        index_t nb_data() const;

        template < typename DataType >
        uuid register_new_data( DataType&& data )
        {
            static_assert( std::is_base_of< Identifier, DataType >::value,
                "[Database::register_data] Data is not a subclass of "
                "Identifier" );
            uuid new_id;
            register_unique_data(
                new_id, absl::make_unique< DataType >( std::move( data ) ) );
            return new_id;
        }

        template < typename DataType >
        uuid register_new_data( std::unique_ptr< DataType >&& data )
        {
            static_assert( std::is_base_of< Identifier, DataType >::value,
                "[Database::register_data] Data is not a subclass of "
                "Identifier" );
            uuid new_id;
            register_unique_data( new_id, std::move( data ) );
            return new_id;
        }

        template < typename DataType >
        void update_data( const uuid& id, DataType&& data )
        {
            static_assert( std::is_base_of< Identifier, DataType >::value,
                "[Database::register_data] Data is not a subclass of "
                "Identifier" );
            return register_unique_data(
                id, absl::make_unique< DataType >( std::move( data ) ) );
        }

        template < typename DataType >
        void update_data( const uuid& id, std::unique_ptr< DataType >&& data )
        {
            static_assert( std::is_base_of< Identifier, DataType >::value,
                "[Database::register_data] Data is not a subclass of "
                "Identifier" );
            return register_unique_data( id, std::move( data ) );
        }

        void delete_data( const uuid& id );

        /*!
         * Retrieve a read only reference to the data corresponding to the given
         * uuid.
         */
        Data get_data( const uuid& id ) const;

        template < typename DataType >
        std::unique_ptr< DataType > take_data( const uuid& id )
        {
            get_data( id ).get< DataType >();
            auto* data =
                dynamic_cast< DataType* >( steal_data( id ).release() );
            return std::unique_ptr< DataType >{ data };
        }

        /*!
         * Use this method to register custom serializer functions to allow
         * saving any custom Object on disk
         */
        void register_serializer_functions(
            serializer_function serializer, serializer_function deserializer );

    private:
        void register_unique_data(
            const uuid& id, std::unique_ptr< Identifier >&& data );

        std::unique_ptr< Identifier > steal_data( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode