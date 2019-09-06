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

#include <geode/model/common.h>
#include <geode/model/mixin/core/add_components.h>
#include <geode/model/mixin/core/blocks.h>
#include <geode/model/mixin/core/corners.h>
#include <geode/model/mixin/core/lines.h>
#include <geode/model/mixin/core/surfaces.h>
#include <geode/model/mixin/core/topology.h>

namespace geode
{
    ALIAS_2D( Corner );
    ALIAS_2D( Line );
    ALIAS_2D( Surface );
} // namespace geode

namespace geode
{
    /*!
     * A Section is a 2D model composed of
     * Corners, Lines, and Surfaces.
     * This class provides classes for range-based iteration on Component
     * boundaries and incidences.
     */
    class opengeode_model_api Section
        : public Topology,
          public AddComponents< 2, Corners, Lines, Surfaces >
    {
    public:
        class opengeode_model_api LineBoundaryRange
            : public Relationships::BoundaryRange
        {
        public:
            LineBoundaryRange( const Section& section,
                const Line2D& line );

            const LineBoundaryRange& begin() const
            {
                return *this;
            }

            const LineBoundaryRange& end() const
            {
                return *this;
            }

            const Corner2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api SurfaceBoundaryRange
            : public Relationships::BoundaryRange
        {
        public:
            SurfaceBoundaryRange( const Section& section,
                const Surface2D& surface );

            const SurfaceBoundaryRange& begin() const
            {
                return *this;
            }

            const SurfaceBoundaryRange& end() const
            {
                return *this;
            }

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

    public:
        class opengeode_model_api CornerIncidenceRange
            : public Relationships::IncidenceRange
        {
        public:
            CornerIncidenceRange( const Section& section,
                const Corner2D& corner );

            const CornerIncidenceRange& begin() const
            {
                return *this;
            }

            const CornerIncidenceRange& end() const
            {
                return *this;
            }

            const Line2D& operator*() const;

        private:
            const Section& section_;
        };

        class opengeode_model_api LineIncidenceRange
            : public Relationships::IncidenceRange
        {
        public:
            LineIncidenceRange( const Section& section,
                const Line2D& line );

            const LineIncidenceRange& begin() const
            {
                return *this;
            }

            const LineIncidenceRange& end() const
            {
                return *this;
            }

            const Surface2D& operator*() const;

        private:
            const Section& section_;
        };

    public:
        LineBoundaryRange boundaries( const Line2D& line ) const;

        SurfaceBoundaryRange boundaries( const Surface2D& surface ) const;

        CornerIncidenceRange incidences( const Corner2D& corner ) const;

        LineIncidenceRange incidences( const Line2D& line ) const;

        static std::string native_extension_static()
        {
            return "og_sctn";
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }

    private:
        void compute_epsilon( double& epsilon ) const override;
    };
} // namespace geode