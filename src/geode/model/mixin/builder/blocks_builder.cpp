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

#include <geode/model/mixin/builder/blocks_builder.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/builder/polyhedral_solid_builder.h>
#include <geode/mesh/core/polyhedral_solid.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/blocks.h>

namespace geode
{
    template < index_t dimension >
    const uuid& BlocksBuilder< dimension >::create_block()
    {
        return blocks_.create_block();
    }

    template < index_t dimension >
    const uuid& BlocksBuilder< dimension >::create_block( const MeshImpl& impl )
    {
        return blocks_.create_block( impl );
    }

    template < index_t dimension >
    void BlocksBuilder< dimension >::delete_block(
        const Block< dimension >& block )
    {
        blocks_.delete_block( block );
    }

    template < index_t dimension >
    void BlocksBuilder< dimension >::load_blocks( absl::string_view directory )
    {
        return blocks_.load_blocks( directory );
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolidBuilder< dimension > >
        BlocksBuilder< dimension >::block_mesh_builder( const uuid& id )
    {
        auto& mesh = blocks_.modifiable_block( id ).modifiable_mesh(
            typename Block< dimension >::BlocksBuilderKey{} );
        return MeshBuilderFactory::create_mesh_builder<
            PolyhedralSolidBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void BlocksBuilder< dimension >::set_block_name(
        const uuid& id, absl::string_view name )
    {
        blocks_.modifiable_block( id ).set_block_name( name, {} );
    }

    template < index_t dimension >
    void BlocksBuilder< dimension >::set_block_mesh(
        const uuid& id, std::unique_ptr< PolyhedralSolid< dimension > > mesh )
    {
        blocks_.modifiable_block( id ).set_mesh( std::move( mesh ),
            typename Block< dimension >::BlocksBuilderKey{} );
    }

    template class opengeode_model_api BlocksBuilder< 3 >;
} // namespace geode
