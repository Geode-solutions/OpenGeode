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
    FORWARD_DECLARATION_DIMENSION_CLASS( Block );
    FORWARD_DECLARATION_DIMENSION_CLASS( BlocksBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Storage class providing range loop to iterate on Blocks
     */
    template < index_t dimension >
    class opengeode_model_api Blocks
    {
        OPENGEODE_DISABLE_COPY( Blocks );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        PASSKEY( BlocksBuilder< dimension >, BlocksBuilderKey );

    public:
        using Builder = BlocksBuilder< dimension >;

        /*!
         * Base class for range-based iteration on Blocks
         */
        class opengeode_model_api BlockRangeBase
        {
        public:
            ~BlockRangeBase();

            bool operator!=( const BlockRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            BlockRangeBase( const Blocks& blocks );
            BlockRangeBase( BlockRangeBase&& other ) noexcept;
            BlockRangeBase( const BlockRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        /*!
         * Class for range-based iteration on unmodifiable Blocks
         */
        class opengeode_model_api BlockRange : public BlockRangeBase
        {
        public:
            BlockRange( const Blocks& blocks );
            BlockRange( const BlockRange& range );
            ~BlockRange();

            const BlockRange& begin() const;

            const BlockRange& end() const;

            const Block< dimension >& operator*() const;
        };

    public:
        ~Blocks();

        index_t nb_blocks() const;

        bool has_block( const uuid& id ) const;

        /*!
         * Access to an unmodifiable Block by its unique index
         */
        const Block< dimension >& block( const uuid& id ) const;

        BlockRange blocks() const;

        /*!
         * Save each Block in a file located in the specified directory
         */
        void save_blocks( std::string_view directory ) const;

    protected:
        Blocks();
        Blocks( Blocks&& other ) noexcept;

        Blocks& operator=( Blocks&& other ) noexcept;

    private:
        class ModifiableBlockRange : public BlockRangeBase
        {
        public:
            ModifiableBlockRange( const Blocks& blocks );
            ModifiableBlockRange( const ModifiableBlockRange& range );
            ~ModifiableBlockRange();

            const ModifiableBlockRange& begin() const;

            const ModifiableBlockRange& end() const;

            Block< dimension >& operator*() const;
        };

    public:
        const uuid& create_block( BlocksBuilderKey key );

        const uuid& create_block( const MeshImpl& impl, BlocksBuilderKey key );

        void create_block( uuid block_id, BlocksBuilderKey key );

        void create_block(
            uuid block_id, const MeshImpl& impl, BlocksBuilderKey key );

        void delete_block(
            const Block< dimension >& block, BlocksBuilderKey key );

        void load_blocks( std::string_view directory, BlocksBuilderKey key );

        ModifiableBlockRange modifiable_blocks( BlocksBuilderKey key );

        Block< dimension >& modifiable_block(
            const uuid& id, BlocksBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( Blocks );
} // namespace geode
