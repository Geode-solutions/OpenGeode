/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/model/common.h>

#include <geode/mesh/core/mesh_type.h>

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
        OPENGEODE_DISABLE_COPY_AND_MOVE( Lines );

    public:
        using Builder = LinesBuilder< dimension >;
        friend Builder;

        class opengeode_model_api LineRangeBase
        {
        public:
            ~LineRangeBase();

            bool operator!=( const LineRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            LineRangeBase( const Lines& lines );
            LineRangeBase( LineRangeBase&& other ) noexcept;
            LineRangeBase( const LineRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api LineRange : public LineRangeBase, public BeginEnd< LineRange >
        {
        public:
            LineRange( const Lines& lines );

            const Line< dimension >& operator*() const;
        };

    public:
        ~Lines();

        index_t nb_lines() const;

        const Line< dimension >& line( const uuid& id ) const;

        LineRange lines() const;

        std::vector< std::string > save_lines(
            const std::string& directory ) const;

    protected:
        Lines();

    private:
        class ModifiableLineRange : public LineRangeBase, public BeginEnd< ModifiableLineRange >
        {
        public:
            ModifiableLineRange( const Lines& lines );

            Line< dimension >& operator*() const;
        };

    private:
        const uuid& create_line();

        const uuid& create_line( const MeshType& type );

        void delete_line( const Line< dimension >& line );

        void load_lines( const std::string& directory );

        ModifiableLineRange modifiable_lines();

        Line< dimension >& modifiable_line( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Lines );
} // namespace geode
