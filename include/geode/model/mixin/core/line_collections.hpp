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

        class opengeode_model_api LineCollectionRangeBase
        {
        public:
            ~LineCollectionRangeBase();

            bool operator!=( const LineCollectionRangeBase& /*unused*/ ) const;

            void operator++();

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

            const LineCollectionRange& begin() const;

            const LineCollectionRange& end() const;

            const LineCollection< dimension >& operator*() const;
        };

    public:
        ~LineCollections();

        index_t nb_line_collections() const;

        const LineCollection< dimension >& line_collection(
            const uuid& id ) const;

        LineCollectionRange line_collections() const;

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

            const ModifiableLineCollectionRange& begin() const;

            const ModifiableLineCollectionRange& end() const;

            LineCollection< dimension >& operator*() const;
        };

    public:
        const uuid& create_line_collection( LineCollectionsBuilderKey key );

        void create_line_collection(
            uuid line_collection_id, LineCollectionsBuilderKey key );

        void delete_line_collection(
            const LineCollection< dimension >& boundary,
            LineCollectionsBuilderKey key );

        void load_line_collections(
            std::string_view directory, LineCollectionsBuilderKey key );

        ModifiableLineCollectionRange modifiable_line_collections(
            LineCollectionsBuilderKey key );

        LineCollection< dimension >& modifiable_line_collection(
            const uuid& id, LineCollectionsBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( LineCollections );
} // namespace geode
