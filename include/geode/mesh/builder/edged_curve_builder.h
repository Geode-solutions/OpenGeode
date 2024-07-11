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

#include <geode/mesh/builder/coordinate_reference_system_managers_builder.h>
#include <geode/mesh/builder/graph_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a EdgedCurve
     */
    template < index_t dimension >
    class EdgedCurveBuilder
        : public GraphBuilder,
          public CoordinateReferenceSystemManagersBuilder< dimension >
    {
    public:
        static constexpr auto dim = dimension;

        /*!
         * Create the builder associated with an EdgedCurve.
         * @param[in] mesh The EdgedCurve to build/modify
         */
        static std::unique_ptr< EdgedCurveBuilder< dimension > > create(
            EdgedCurve< dimension >& mesh );

        /*!
         * Create a new point with associated coordinates.
         * @param[in] point The point to create
         * @return the index of the created point
         */
        index_t create_point( Point< dimension > point );

        void copy( const EdgedCurve< dimension >& edged_curve );

    protected:
        explicit EdgedCurveBuilder( EdgedCurve< dimension >& mesh );

    private:
        virtual void do_copy_points( const EdgedCurve< dimension >& mesh ) = 0;

    private:
        EdgedCurve< dimension >& edged_curve_;
    };
    ALIAS_2D_AND_3D( EdgedCurveBuilder );
} // namespace geode
