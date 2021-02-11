/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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
#include <geode/basic/passkey.h>

#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/common.h>
#include <geode/mesh/core/mesh_id.h>

namespace geode
{
    class MeshBuilderFactoryKey
    {
        friend class MeshBuilderFactory;

    private:
        MeshBuilderFactoryKey() {}
    };

    class opengeode_mesh_api MeshBuilderFactory : public Factory< MeshImpl,
                                                      VertexSetBuilder,
                                                      VertexSet&,
                                                      MeshBuilderFactoryKey >
    {
        PASSKEY( MeshBuilderFactory, Key );

    public:
        template < typename MeshBuilder >
        static void register_mesh_builder( MeshImpl key )
        {
            register_creator< MeshBuilder >( std::move( key ) );
        }

        template < typename MeshBuilder, typename Mesh >
        static std::unique_ptr< MeshBuilder > create_mesh_builder( Mesh& mesh )
        {
            auto* builder = dynamic_cast< MeshBuilder* >(
                create( mesh.impl_name(), mesh, {} ).release() );
            OPENGEODE_EXCEPTION( builder,
                "Cannot create mesh builder with key: ",
                mesh.impl_name().get() );
            builder->set_mesh( mesh, MeshBuilderFactoryKey{} );
            return std::unique_ptr< MeshBuilder >{ builder };
        }
    };
} // namespace geode
