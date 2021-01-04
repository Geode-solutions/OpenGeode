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

#include <memory>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/mesh_id.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Block );
    FORWARD_DECLARATION_DIMENSION_CLASS( Blocks );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMeshBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class BlocksBuilder
    {
    public:
        void load_blocks( absl::string_view directory );

        /*!
         * Get a pointer to the builder of a Block mesh
         * @param[in] id Unique index of the Block
         */
        template < typename Mesh = SolidMesh< dimension > >
        std::unique_ptr< typename Mesh::Builder > block_mesh_builder(
            const uuid& id )
        {
            auto& mesh = blocks_.modifiable_block( id ).modifiable_mesh(
                typename Block< dimension >::BlocksBuilderKey{} );
            return MeshBuilderFactory::create_mesh_builder<
                typename Mesh::Builder >( dynamic_cast< Mesh& >( mesh ) );
        }

        void set_block_name( const uuid& id, absl::string_view name );

    protected:
        BlocksBuilder( Blocks< dimension >& blocks ) : blocks_( blocks ) {}

        const uuid& create_block();

        const uuid& create_block( const MeshImpl& impl );

        void delete_block( const Block< dimension >& block );

        void set_block_mesh(
            const uuid& id, std::unique_ptr< SolidMesh< dimension > > mesh );

    private:
        Blocks< dimension >& blocks_;
    };
    ALIAS_2D_AND_3D( BlocksBuilder );
} // namespace geode
