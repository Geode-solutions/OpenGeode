/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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
    FORWARD_DECLARATION_DIMENSION_CLASS( LineCollection );
    FORWARD_DECLARATION_DIMENSION_CLASS( LineCollectionsBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class LineCollections
    {
        OPENGEODE_DISABLE_COPY( LineCollections );
        PASSKEY(
            LineCollectionsBuilder< dimension >, LineCollectionsBuilderKey );

    public:
        using Builder = LineCollectionsBuilder< dimension >;
        using Type = LineCollection< dimension >;

        class opengeode_model_api LineCollectionRangeBase
        {
        public:
            ~LineCollectionRangeBase();

            [[nodiscard]] bool operator!=(
                const LineCollectionRangeBase& /*unused*/ ) const;

            void operator++();

            void set_active_only();

        protected:
            LineCollectionRangeBase( const LineCollections& collections );
            LineCollectionRangeBase( LineCollectionRangeBase&& other ) noexcept;
            LineCollectionRangeBase( const LineCollectionRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api LineCollectionRange
            : public LineCollectionRangeBase
        {
        public:
            LineCollectionRange( const LineCollections& boundaries );
            LineCollectionRange( const LineCollectionRange& range );
            ~LineCollectionRange();

            [[nodiscard]] const LineCollectionRange& begin() const;

            [[nodiscard]] const LineCollectionRange& end() const;

            [[nodiscard]] const LineCollection< dimension >& operator*() const;
        };

    public:
        ~LineCollections();

        [[nodiscard]] index_t nb_line_collections() const;

        [[nodiscard]] index_t nb_active_line_collections() const;

        [[nodiscard]] const LineCollection< dimension >& line_collection(
            const uuid& id ) const;

        [[nodiscard]] bool has_line_collection( const uuid& id ) const;

        [[nodiscard]] LineCollectionRange line_collections() const;

        [[nodiscard]] LineCollectionRange active_line_collections() const;

        [[nodiscard]] LineCollectionRange components() const
        {
            return line_collections();
        }

        [[nodiscard]] bool has_component( const uuid& id ) const
        {
            return has_line_collection( id );
        }

        [[nodiscard]] const LineCollection< dimension >& component(
            const uuid& id ) const
        {
            return line_collection( id );
        }

        void save_line_collections( std::string_view directory ) const;

    protected:
        LineCollections();
        LineCollections( LineCollections&& other ) noexcept;
        LineCollections& operator=( LineCollections&& other ) noexcept;

    private:
        class ModifiableLineCollectionRange : public LineCollectionRangeBase
        {
        public:
            ModifiableLineCollectionRange( const LineCollections& boundaries );
            ModifiableLineCollectionRange(
                const ModifiableLineCollectionRange& range );
            ~ModifiableLineCollectionRange();

            [[nodiscard]] const ModifiableLineCollectionRange& begin() const;

            [[nodiscard]] const ModifiableLineCollectionRange& end() const;

            [[nodiscard]] LineCollection< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_line_collection(
            LineCollectionsBuilderKey key );

        void create_line_collection(
            uuid line_collection_id, LineCollectionsBuilderKey key );

        void delete_line_collection(
            const LineCollection< dimension >& boundary,
            LineCollectionsBuilderKey key );

        void load_line_collections(
            std::string_view directory, LineCollectionsBuilderKey key );

        [[nodiscard]] ModifiableLineCollectionRange modifiable_line_collections(
            LineCollectionsBuilderKey key );

        [[nodiscard]] LineCollection< dimension >& modifiable_line_collection(
            const uuid& id, LineCollectionsBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( LineCollections );
} // namespace geode
