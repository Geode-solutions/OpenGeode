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
    FORWARD_DECLARATION_DIMENSION_CLASS( Corner );
    FORWARD_DECLARATION_DIMENSION_CLASS( CornersBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Storage class providing range loop to iterate on Corners
     */
    template < index_t dimension >
    class opengeode_model_api Corners
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Corners );

    public:
        using Builder = CornersBuilder< dimension >;
        friend Builder;
        /*!
         * Base class for range-based iteration on Corners
         */
        class opengeode_model_api CornerRangeBase
        {
        public:
            ~CornerRangeBase();

            bool operator!=( const CornerRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            CornerRangeBase( const Corners& corners );
            CornerRangeBase( CornerRangeBase&& other ) noexcept;
            CornerRangeBase( const CornerRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        /*!
         * Class for range-based iteration on unmodifiable Corners
         */
        class opengeode_model_api CornerRange : public CornerRangeBase
        {
        public:
            CornerRange( const Corners& corners );

            const CornerRange& begin() const
            {
                return *this;
            }

            const CornerRange& end() const
            {
                return *this;
            }

            const Corner< dimension >& operator*() const;
        };

    public:
        ~Corners();

        index_t nb_corners() const;

        /*!
         * Access to an unmodifiable Corner by its unique index
         */
        const Corner< dimension >& corner( const uuid& id ) const;

        CornerRange corners() const;

        /*!
         * Save each Corner in a file located in the specified directory
         * @return the list of filenames
         */
        std::vector< std::string > save_corners(
            const std::string& directory ) const;

    protected:
        Corners();

    private:
        class ModifiableCornerRange : public CornerRangeBase
        {
        public:
            ModifiableCornerRange( const Corners& corners );

            const ModifiableCornerRange& begin() const
            {
                return *this;
            }

            const ModifiableCornerRange& end() const
            {
                return *this;
            }

            Corner< dimension >& operator*() const;
        };

    private:
        const uuid& create_corner();

        const uuid& create_corner( const MeshType& type );

        void delete_corner( const Corner< dimension >& corner );

        void load_corners( const std::string& directory );

        ModifiableCornerRange modifiable_corners();

        Corner< dimension >& modifiable_corner( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Corners );
} // namespace geode
