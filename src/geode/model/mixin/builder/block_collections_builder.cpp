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

#include <geode/model/mixin/builder/block_collections_builder.h>

#include <geode/model/mixin/core/block_collection.h>
#include <geode/model/mixin/core/block_collections.h>

namespace geode
{
    template < index_t dimension >
    const uuid& BlockCollectionsBuilder< dimension >::create_block_collection()
    {
        return block_collections_.create_block_collection( {} );
    }

    template < index_t dimension >
    void BlockCollectionsBuilder< dimension >::create_block_collection(
        uuid block_collection_id )
    {
        block_collections_.create_block_collection(
            std::move( block_collection_id ), {} );
    }

    template < index_t dimension >
    void BlockCollectionsBuilder< dimension >::delete_block_collection(
        const BlockCollection< dimension >& collection )
    {
        block_collections_.delete_block_collection( collection, {} );
    }

    template < index_t dimension >
    void BlockCollectionsBuilder< dimension >::load_block_collections(
        absl::string_view directory )
    {
        return block_collections_.load_block_collections( directory, {} );
    }

    template < index_t dimension >
    void BlockCollectionsBuilder< dimension >::set_block_collection_name(
        const uuid& id, absl::string_view name )
    {
        block_collections_.modifiable_block_collection( id, {} )
            .set_block_collection_name( name, {} );
    }

    template class opengeode_model_api BlockCollectionsBuilder< 2 >;
    template class opengeode_model_api BlockCollectionsBuilder< 3 >;
} // namespace geode
