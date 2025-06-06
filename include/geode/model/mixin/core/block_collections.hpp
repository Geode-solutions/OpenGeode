/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/model/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BlockCollection );
    FORWARD_DECLARATION_DIMENSION_CLASS( BlockCollectionsBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class BlockCollections
    {
        OPENGEODE_DISABLE_COPY( BlockCollections );
        PASSKEY(
            BlockCollectionsBuilder< dimension >, BlockCollectionsBuilderKey );

    public:
        using Builder = BlockCollectionsBuilder< dimension >;
        using Type = BlockCollection< dimension >;

        class opengeode_model_api BlockCollectionRangeBase
        {
        public:
            ~BlockCollectionRangeBase();

            [[nodiscard]] bool operator!=(
                const BlockCollectionRangeBase& /*unused*/ ) const;

            void operator++();

            void set_active_only();

        protected:
            BlockCollectionRangeBase( const BlockCollections& collections );
            BlockCollectionRangeBase(
                BlockCollectionRangeBase&& other ) noexcept;
            BlockCollectionRangeBase( const BlockCollectionRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api BlockCollectionRange
            : public BlockCollectionRangeBase
        {
        public:
            BlockCollectionRange( const BlockCollections& boundaries );
            BlockCollectionRange( const BlockCollectionRange& range );
            ~BlockCollectionRange();

            [[nodiscard]] const BlockCollectionRange& begin() const;

            [[nodiscard]] const BlockCollectionRange& end() const;

            [[nodiscard]] const BlockCollection< dimension >& operator*() const;
        };

    public:
        ~BlockCollections();

        [[nodiscard]] index_t nb_block_collections() const;

        [[nodiscard]] index_t nb_active_block_collections() const;

        [[nodiscard]] const BlockCollection< dimension >& block_collection(
            const uuid& id ) const;

        [[nodiscard]] BlockCollectionRange block_collections() const;

        [[nodiscard]] BlockCollectionRange active_block_collections() const;

        [[nodiscard]] BlockCollectionRange components() const
        {
            return block_collections();
        }

        void save_block_collections( std::string_view directory ) const;

    protected:
        BlockCollections();
        BlockCollections( BlockCollections&& other ) noexcept;
        BlockCollections& operator=( BlockCollections&& other ) noexcept;

    private:
        class ModifiableBlockCollectionRange : public BlockCollectionRangeBase
        {
        public:
            ModifiableBlockCollectionRange(
                const BlockCollections& boundaries );
            ModifiableBlockCollectionRange(
                const ModifiableBlockCollectionRange& range );
            ~ModifiableBlockCollectionRange();

            [[nodiscard]] const ModifiableBlockCollectionRange& begin() const;

            [[nodiscard]] const ModifiableBlockCollectionRange& end() const;

            [[nodiscard]] BlockCollection< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_block_collection(
            BlockCollectionsBuilderKey key );

        void create_block_collection(
            uuid block_collection_id, BlockCollectionsBuilderKey key );

        void delete_block_collection(
            const BlockCollection< dimension >& boundary,
            BlockCollectionsBuilderKey key );

        void load_block_collections(
            std::string_view directory, BlockCollectionsBuilderKey key );

        [[nodiscard]] ModifiableBlockCollectionRange
            modifiable_block_collections( BlockCollectionsBuilderKey key );

        [[nodiscard]] BlockCollection< dimension >& modifiable_block_collection(
            const uuid& id, BlockCollectionsBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( BlockCollections );
} // namespace geode
