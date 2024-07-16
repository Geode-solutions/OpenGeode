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

#include <geode/basic/factory.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/mesh_id.hpp>
#include <geode/mesh/core/vertex_set.hpp>

namespace geode
{
    class opengeode_mesh_api MeshFactory : public Factory< MeshImpl, VertexSet >
    {
    public:
        /*!
         * Register a mesh implementation to a given mesh type
         */
        template < typename Mesh >
        static void register_mesh( MeshType type, MeshImpl key )
        {
            register_type( std::move( type ), key );
            register_creator< Mesh >( std::move( key ) );
        }

        /*!
         * Register the default mesh implementation to a given mesh type.
         * This implementation will be used when no specific implementation is
         * required.
         */
        template < typename Mesh >
        static void register_default_mesh( MeshType type, MeshImpl key )
        {
            register_mesh< Mesh >( type, key );
            register_default( type, key );
        }

        static const MeshType& type( const MeshImpl& key );

        static const MeshImpl& default_impl( const MeshType& type );

        template < typename Mesh >
        static std::unique_ptr< Mesh > create_mesh( const MeshImpl& key )
        {
            auto* mesh = dynamic_cast< Mesh* >( create( key ).release() );
            OPENGEODE_EXCEPTION(
                mesh, "Cannot create mesh with key: ", key.get() );
            return std::unique_ptr< Mesh >{ mesh };
        }

        template < typename Mesh >
        static std::unique_ptr< Mesh > create_default_mesh(
            const MeshType& type )
        {
            return create_mesh< Mesh >( default_impl( type ) );
        }

    private:
        MeshFactory();
        ~MeshFactory();

        static MeshFactory& instance();

        static void register_type( MeshType type, MeshImpl key );

        static void register_default( MeshType type, MeshImpl key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
