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

#include <geode/model/mixin/builder/surface_collections_builder.h>

#include <geode/model/mixin/core/surface_collection.h>
#include <geode/model/mixin/core/surface_collections.h>

namespace geode
{
    template < index_t dimension >
    const uuid&
        SurfaceCollectionsBuilder< dimension >::create_surface_collection()
    {
        return surface_collections_.create_surface_collection();
    }

    template < index_t dimension >
    void SurfaceCollectionsBuilder< dimension >::create_surface_collection(
        uuid surface_collection_id )
    {
        surface_collections_.create_surface_collection(
            std::move( surface_collection_id ) );
    }

    template < index_t dimension >
    void SurfaceCollectionsBuilder< dimension >::delete_surface_collection(
        const SurfaceCollection< dimension >& collection )
    {
        surface_collections_.delete_surface_collection( collection );
    }

    template < index_t dimension >
    void SurfaceCollectionsBuilder< dimension >::load_surface_collections(
        absl::string_view directory )
    {
        return surface_collections_.load_surface_collections( directory );
    }

    template < index_t dimension >
    void SurfaceCollectionsBuilder< dimension >::set_surface_collection_name(
        const uuid& id, absl::string_view name )
    {
        surface_collections_.modifiable_surface_collection( id )
            .set_surface_collection_name( name, {} );
    }

    template class opengeode_model_api SurfaceCollectionsBuilder< 2 >;
    template class opengeode_model_api SurfaceCollectionsBuilder< 3 >;
} // namespace geode
