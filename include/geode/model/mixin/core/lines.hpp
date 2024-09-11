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
    FORWARD_DECLARATION_DIMENSION_CLASS( Line );
    FORWARD_DECLARATION_DIMENSION_CLASS( LinesBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_model_api Lines
    {
        OPENGEODE_DISABLE_COPY( Lines );
        PASSKEY( LinesBuilder< dimension >, LinesBuilderKey );

    public:
        using Builder = LinesBuilder< dimension >;

        class opengeode_model_api LineRangeBase
        {
        public:
            ~LineRangeBase();

            [[nodiscard]] bool operator!=(
                const LineRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            LineRangeBase( const Lines& lines );
            LineRangeBase( LineRangeBase&& other ) noexcept;
            LineRangeBase( const LineRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api LineRange : public LineRangeBase
        {
        public:
            LineRange( const Lines& lines );
            LineRange( const LineRange& range );
            ~LineRange();

            [[nodiscard]] const LineRange& begin() const;

            [[nodiscard]] const LineRange& end() const;

            [[nodiscard]] const Line< dimension >& operator*() const;
        };

    public:
        ~Lines();

        [[nodiscard]] index_t nb_lines() const;

        [[nodiscard]] bool has_line( const uuid& id ) const;

        [[nodiscard]] const Line< dimension >& line( const uuid& id ) const;

        [[nodiscard]] LineRange lines() const;

        void save_lines( std::string_view directory ) const;

    protected:
        Lines();
        Lines( Lines&& other ) noexcept;

        Lines& operator=( Lines&& other ) noexcept;

    private:
        class ModifiableLineRange : public LineRangeBase
        {
        public:
            ModifiableLineRange( const Lines& lines );
            ModifiableLineRange( const ModifiableLineRange& range );
            ~ModifiableLineRange();

            [[nodiscard]] const ModifiableLineRange& begin() const;

            [[nodiscard]] const ModifiableLineRange& end() const;

            [[nodiscard]] Line< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_line( LinesBuilderKey key );

        [[nodiscard]] const uuid& create_line(
            const MeshImpl& impl, LinesBuilderKey key );

        void create_line( uuid line_id, LinesBuilderKey key );

        void create_line(
            uuid line_id, const MeshImpl& impl, LinesBuilderKey key );

        void delete_line( const Line< dimension >& line, LinesBuilderKey key );

        void load_lines( std::string_view directory, LinesBuilderKey key );

        [[nodiscard]] ModifiableLineRange modifiable_lines(
            LinesBuilderKey key );

        [[nodiscard]] Line< dimension >& modifiable_line(
            const uuid& id, LinesBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Lines );
} // namespace geode
