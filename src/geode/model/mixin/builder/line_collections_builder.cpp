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

#include <geode/model/mixin/builder/line_collections_builder.hpp>

#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/line_collections.hpp>

namespace geode
{
    template < index_t dimension >
    const uuid& LineCollectionsBuilder< dimension >::create_line_collection()
    {
        return line_collections_.create_line_collection( {} );
    }

    template < index_t dimension >
    void LineCollectionsBuilder< dimension >::create_line_collection(
        uuid line_collection_id )
    {
        line_collections_.create_line_collection(
            std::move( line_collection_id ), {} );
    }

    template < index_t dimension >
    void LineCollectionsBuilder< dimension >::delete_line_collection(
        const LineCollection< dimension >& collection )
    {
        line_collections_.delete_line_collection( collection, {} );
    }

    template < index_t dimension >
    void LineCollectionsBuilder< dimension >::load_line_collections(
        std::string_view directory )
    {
        return line_collections_.load_line_collections( directory, {} );
    }

    template < index_t dimension >
    void LineCollectionsBuilder< dimension >::set_line_collection_name(
        const uuid& id, std::string_view name )
    {
        line_collections_.modifiable_line_collection( id, {} )
            .set_line_collection_name( name, {} );
    }

    template class opengeode_model_api LineCollectionsBuilder< 2 >;
    template class opengeode_model_api LineCollectionsBuilder< 3 >;
} // namespace geode
