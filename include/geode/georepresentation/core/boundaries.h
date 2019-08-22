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

#include <geode/georepresentation/common.h>

#include <geode/mesh/core/mesh_type.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Boundary );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundariesBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_georepresentation_api Boundaries
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Boundaries );
        friend class BoundariesBuilder< dimension >;

    public:
        class opengeode_georepresentation_api BoundaryRangeBase
        {
        public:
            ~BoundaryRangeBase();

            bool operator!=( const BoundaryRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            BoundaryRangeBase( const Boundaries& boundaries );
            BoundaryRangeBase( BoundaryRangeBase&& other ) noexcept;
            BoundaryRangeBase( const BoundaryRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_georepresentation_api BoundaryRange
            : public BoundaryRangeBase
        {
        public:
            BoundaryRange( const Boundaries& boundaries );

            const BoundaryRange& begin() const
            {
                return *this;
            }

            const BoundaryRange& end() const
            {
                return *this;
            }

            const Boundary< dimension >& operator*() const;
        };

    public:
        ~Boundaries();

        index_t nb_boundaries() const;

        const Boundary< dimension >& boundary( const uuid& id ) const;

        BoundaryRange boundaries() const;

        std::vector< std::string > save_boundaries(
            const std::string& directory ) const;

    protected:
        Boundaries();

    private:
        class ModifiableBoundaryRange : public BoundaryRangeBase
        {
        public:
            ModifiableBoundaryRange( const Boundaries& boundaries );

            const ModifiableBoundaryRange& begin() const
            {
                return *this;
            }

            const ModifiableBoundaryRange& end() const
            {
                return *this;
            }

            Boundary< dimension >& operator*() const;
        };

    private:
        const uuid& create_boundary();

        const uuid& create_boundary( const MeshType& type );

        void delete_boundary( const Boundary< dimension >& boundary );

        void load_boundaries( const std::string& directory );

        ModifiableBoundaryRange modifiable_boundaries();

        Boundary< dimension >& modifiable_boundary( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Boundaries );
} // namespace geode
