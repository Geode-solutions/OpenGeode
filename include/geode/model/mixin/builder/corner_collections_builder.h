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
    FORWARD_DECLARATION_DIMENSION_CLASS( CornerCollection );
    FORWARD_DECLARATION_DIMENSION_CLASS( CornerCollections );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class CornerCollectionsBuilder
    {
    public:
        void load_corner_collections( absl::string_view directory );

        void set_corner_collection_name(
            const uuid& id, absl::string_view name );

    protected:
        explicit CornerCollectionsBuilder(
            CornerCollections< dimension >& collections )
            : corner_collections_( collections )
        {
        }

        const uuid& create_corner_collection();

        void create_corner_collection( uuid corner_collection_id );

        void delete_corner_collection(
            const CornerCollection< dimension >& collection );

    private:
        CornerCollections< dimension >& corner_collections_;
    };
    ALIAS_2D_AND_3D( CornerCollectionsBuilder );
} // namespace geode
