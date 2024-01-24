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

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/core/mesh_id.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( CornerCollection );
    FORWARD_DECLARATION_DIMENSION_CLASS( CornerCollectionsBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class CornerCollections
    {
        OPENGEODE_DISABLE_COPY( CornerCollections );
        PASSKEY( CornerCollectionsBuilder< dimension >,
            CornerCollectionsBuilderKey );

    public:
        using Builder = CornerCollectionsBuilder< dimension >;

        class opengeode_model_api CornerCollectionRangeBase
        {
        public:
            ~CornerCollectionRangeBase();

            bool operator!=(
                const CornerCollectionRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            CornerCollectionRangeBase( const CornerCollections& collections );
            CornerCollectionRangeBase(
                CornerCollectionRangeBase&& other ) noexcept;
            CornerCollectionRangeBase( const CornerCollectionRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api CornerCollectionRange
            : public CornerCollectionRangeBase
        {
        public:
            CornerCollectionRange( const CornerCollections& boundaries );
            CornerCollectionRange( const CornerCollectionRange& range );
            ~CornerCollectionRange();

            const CornerCollectionRange& begin() const;

            const CornerCollectionRange& end() const;

            const CornerCollection< dimension >& operator*() const;
        };

    public:
        ~CornerCollections();

        index_t nb_corner_collections() const;

        const CornerCollection< dimension >& corner_collection(
            const uuid& id ) const;

        CornerCollectionRange corner_collections() const;

        void save_corner_collections( absl::string_view directory ) const;

    protected:
        CornerCollections();
        CornerCollections( CornerCollections&& other ) noexcept;
        CornerCollections& operator=( CornerCollections&& other ) noexcept;

    private:
        class ModifiableCornerCollectionRange : public CornerCollectionRangeBase
        {
        public:
            ModifiableCornerCollectionRange(
                const CornerCollections& boundaries );
            ModifiableCornerCollectionRange(
                const ModifiableCornerCollectionRange& range );
            ~ModifiableCornerCollectionRange();

            const ModifiableCornerCollectionRange& begin() const;

            const ModifiableCornerCollectionRange& end() const;

            CornerCollection< dimension >& operator*() const;
        };

    public:
        const uuid& create_corner_collection( CornerCollectionsBuilderKey key );

        void create_corner_collection(
            uuid corner_collection_id, CornerCollectionsBuilderKey key );

        void delete_corner_collection(
            const CornerCollection< dimension >& boundary,
            CornerCollectionsBuilderKey key );

        void load_corner_collections(
            absl::string_view directory, CornerCollectionsBuilderKey key );

        ModifiableCornerCollectionRange modifiable_corner_collections(
            CornerCollectionsBuilderKey key );

        CornerCollection< dimension >& modifiable_corner_collection(
            const uuid& id, CornerCollectionsBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( CornerCollections );
} // namespace geode
