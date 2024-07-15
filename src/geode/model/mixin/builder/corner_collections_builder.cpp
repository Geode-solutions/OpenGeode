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

#include <geode/model/mixin/builder/corner_collections_builder.h>

#include <geode/model/mixin/core/corner_collection.h>
#include <geode/model/mixin/core/corner_collections.h>

namespace geode
{
    template < index_t dimension >
    const uuid&
        CornerCollectionsBuilder< dimension >::create_corner_collection()
    {
        return corner_collections_.create_corner_collection( {} );
    }

    template < index_t dimension >
    void CornerCollectionsBuilder< dimension >::create_corner_collection(
        uuid corner_collection_id )
    {
        corner_collections_.create_corner_collection(
            std::move( corner_collection_id ), {} );
    }

    template < index_t dimension >
    void CornerCollectionsBuilder< dimension >::delete_corner_collection(
        const CornerCollection< dimension >& collection )
    {
        corner_collections_.delete_corner_collection( collection, {} );
    }

    template < index_t dimension >
    void CornerCollectionsBuilder< dimension >::load_corner_collections(
        std::string_view directory )
    {
        return corner_collections_.load_corner_collections( directory, {} );
    }

    template < index_t dimension >
    void CornerCollectionsBuilder< dimension >::set_corner_collection_name(
        const uuid& id, std::string_view name )
    {
        corner_collections_.modifiable_corner_collection( id, {} )
            .set_corner_collection_name( name, {} );
    }

    template class opengeode_model_api CornerCollectionsBuilder< 2 >;
    template class opengeode_model_api CornerCollectionsBuilder< 3 >;
} // namespace geode
