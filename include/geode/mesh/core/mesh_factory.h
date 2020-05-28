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

#include <geode/basic/factory.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/mesh_id.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    class opengeode_mesh_api MeshFactory : public Factory< MeshType, VertexSet >
    {
    public:
        template < typename Mesh >
        static void register_mesh( MeshKind kind, MeshType key )
        {
            register_kind( std::move( kind ), key );
            register_creator< Mesh >( std::move( key ) );
        }

        template < typename Mesh >
        static void register_default_mesh( MeshKind kind, MeshType key )
        {
            register_mesh< Mesh >( kind, key );
            register_default( kind, key );
        }

        static const MeshKind& kind( const MeshType& key );

        static const MeshType& default_mesh( const MeshKind& kind );

        template < typename Mesh >
        static std::unique_ptr< Mesh > create_mesh( const MeshType& key )
        {
            auto* mesh = dynamic_cast< Mesh* >( create( key ).release() );
            OPENGEODE_EXCEPTION(
                mesh, "Cannot create mesh with key: ", key.get() );
            return std::unique_ptr< Mesh >{ mesh };
        }

        template < typename Mesh >
        static std::unique_ptr< Mesh > create_default_mesh(
            const MeshKind& kind )
        {
            return create_mesh< Mesh >( default_mesh( kind ) );
        }

    private:
        MeshFactory();
        ~MeshFactory();

        static MeshFactory& instance();

        static void register_kind( MeshKind kind, MeshType key );

        static void register_default( MeshKind kind, MeshType key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
