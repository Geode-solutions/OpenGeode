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

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/common.hpp>

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
        using Type = Block< dimension >;

        /*!
         * Base class for range-based iteration on Blocks
         */
        class opengeode_model_api BlockRangeBase
        {
        public:
            ~BlockRangeBase();

            [[nodiscard]] bool operator!=(
                const BlockRangeBase& /*unused*/ ) const;

            void operator++();

            void set_active_only();

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

            [[nodiscard]] const BlockRange& begin() const;

            [[nodiscard]] const BlockRange& end() const;

            [[nodiscard]] const Block< dimension >& operator*() const;
        };

    public:
        ~Blocks();

        [[nodiscard]] index_t nb_blocks() const;

        [[nodiscard]] index_t nb_active_blocks() const;

        [[nodiscard]] bool has_block( const uuid& id ) const;

        /*!
         * Access to an unmodifiable Block by its unique index
         */
        [[nodiscard]] const Block< dimension >& block( const uuid& id ) const;

        [[nodiscard]] BlockRange blocks() const;

        [[nodiscard]] BlockRange active_blocks() const;

        [[nodiscard]] BlockRange components() const
        {
            return blocks();
        }

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

            [[nodiscard]] const ModifiableBlockRange& begin() const;

            [[nodiscard]] const ModifiableBlockRange& end() const;

            [[nodiscard]] Block< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_block( BlocksBuilderKey key );

        [[nodiscard]] const uuid& create_block(
            const MeshImpl& impl, BlocksBuilderKey key );

        void create_block( uuid block_id, BlocksBuilderKey key );

        void create_block(
            uuid block_id, const MeshImpl& impl, BlocksBuilderKey key );

        void delete_block(
            const Block< dimension >& block, BlocksBuilderKey key );

        void load_blocks( std::string_view directory, BlocksBuilderKey key );

        [[nodiscard]] ModifiableBlockRange modifiable_blocks(
            BlocksBuilderKey key );

        [[nodiscard]] Block< dimension >& modifiable_block(
            const uuid& id, BlocksBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( Blocks );
} // namespace geode
