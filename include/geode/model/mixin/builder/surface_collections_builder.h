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

#include <memory>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceCollection );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceCollections );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class SurfaceCollectionsBuilder
    {
    public:
        void load_surface_collections( absl::string_view directory );

        void set_surface_collection_name(
            const uuid& id, absl::string_view name );

    protected:
        SurfaceCollectionsBuilder(
            SurfaceCollections< dimension >& collections )
            : surface_collections_( collections )
        {
        }

        const uuid& create_surface_collection();

        void create_surface_collection( uuid surface_collection_id );

        void delete_surface_collection(
            const SurfaceCollection< dimension >& collection );

    private:
        SurfaceCollections< dimension >& surface_collections_;
    };
    ALIAS_2D_AND_3D( SurfaceCollectionsBuilder );
} // namespace geode
