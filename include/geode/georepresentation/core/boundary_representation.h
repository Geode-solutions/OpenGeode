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

#include <geode/georepresentation/common.h>
#include <geode/georepresentation/core/blocks.h>
#include <geode/georepresentation/core/boundaries.h>
#include <geode/georepresentation/core/corners.h>
#include <geode/georepresentation/core/georepresentation.h>
#include <geode/georepresentation/core/lines.h>
#include <geode/georepresentation/core/surfaces.h>

namespace geode
{
    ALIAS_3D( Block );
    ALIAS_3D( Boundary );
    ALIAS_3D( Corner );
    ALIAS_3D( Line );
    ALIAS_3D( Surface );
} // namespace geode

namespace geode
{
    /*!
     * A Boundary Representation is a GeoRepresentation composed of
     * Corners, Lines, Surfaces and Blocks. BRep is also defined by
     * collections of Surfaces called Boundaries defining the limit between the
     * interior and the exterior of the model. This class provides classes for
     * range-based iteration on Component boundaries and incidences and
     * Collection items.
     */
    class opengeode_georepresentation_api BRep
        : public GeoRepresentation< 3, Corners, Lines, Surfaces, Blocks >,
          public Boundaries< 3 >
    {
    public:
        class opengeode_georepresentation_api LineBoundaryRange
            : public RelationshipManager::BoundaryRange
        {
        public:
            LineBoundaryRange( const BRep& brep,
                const RelationshipManager& manager,
                const Line3D& line );

            const LineBoundaryRange& begin() const
            {
                return *this;
            }

            const LineBoundaryRange& end() const
            {
                return *this;
            }

            const Corner3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_georepresentation_api SurfaceBoundaryRange
            : public RelationshipManager::BoundaryRange
        {
        public:
            SurfaceBoundaryRange( const BRep& brep,
                const RelationshipManager& manager,
                const Surface3D& surface );

            const SurfaceBoundaryRange& begin() const
            {
                return *this;
            }

            const SurfaceBoundaryRange& end() const
            {
                return *this;
            }

            const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_georepresentation_api BlockBoundaryRange
            : public RelationshipManager::BoundaryRange
        {
        public:
            BlockBoundaryRange( const BRep& brep,
                const RelationshipManager& manager,
                const Block3D& block );

            const BlockBoundaryRange& begin() const
            {
                return *this;
            }

            const BlockBoundaryRange& end() const
            {
                return *this;
            }

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

    public:
        class opengeode_georepresentation_api CornerIncidenceRange
            : public RelationshipManager::IncidenceRange
        {
        public:
            CornerIncidenceRange( const BRep& brep,
                const RelationshipManager& manager,
                const Corner3D& corner );

            const CornerIncidenceRange& begin() const
            {
                return *this;
            }

            const CornerIncidenceRange& end() const
            {
                return *this;
            }

            const Line3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_georepresentation_api LineIncidenceRange
            : public RelationshipManager::IncidenceRange
        {
        public:
            LineIncidenceRange( const BRep& brep,
                const RelationshipManager& manager,
                const Line3D& line );

            const LineIncidenceRange& begin() const
            {
                return *this;
            }

            const LineIncidenceRange& end() const
            {
                return *this;
            }

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_georepresentation_api SurfaceIncidenceRange
            : public RelationshipManager::IncidenceRange
        {
        public:
            SurfaceIncidenceRange( const BRep& brep,
                const RelationshipManager& manager,
                const Surface3D& surface );

            const SurfaceIncidenceRange& begin() const
            {
                return *this;
            }

            const SurfaceIncidenceRange& end() const
            {
                return *this;
            }

            const Block3D& operator*() const;

        private:
            const BRep& brep_;
        };

        class opengeode_georepresentation_api BoundaryItemRange
            : public RelationshipManager::ItemRange
        {
        public:
            BoundaryItemRange( const BRep& brep,
                const RelationshipManager& manager,
                const Boundary3D& boundary );

            const BoundaryItemRange& begin() const
            {
                return *this;
            }

            const BoundaryItemRange& end() const
            {
                return *this;
            }

            const Surface3D& operator*() const;

        private:
            const BRep& brep_;
        };

    public:
        LineBoundaryRange boundaries( const Line3D& line ) const;

        SurfaceBoundaryRange boundaries( const Surface3D& surface ) const;

        BlockBoundaryRange boundaries( const Block3D& block ) const;

        CornerIncidenceRange incidences( const Corner3D& corner ) const;

        LineIncidenceRange incidences( const Line3D& line ) const;

        SurfaceIncidenceRange incidences( const Surface3D& surface ) const;

        BoundaryItemRange items( const Boundary3D& boundary ) const;

        static std::string native_extension_static()
        {
            return "og_brep";
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }

    private:
        void compute_epsilon( double& epsilon ) const override;
    };
} // namespace geode
