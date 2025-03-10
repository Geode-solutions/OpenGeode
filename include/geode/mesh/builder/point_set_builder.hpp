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

#include <vector>

#include <geode/mesh/builder/coordinate_reference_system_managers_builder.hpp>
#include <geode/mesh/builder/vertex_set_builder.hpp>
#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a PointSet
     */
    template < index_t dimension >
    class PointSetBuilder
        : public VertexSetBuilder,
          public CoordinateReferenceSystemManagersBuilder< dimension >
    {
    public:
        static constexpr auto dim = dimension;

        PointSetBuilder( PointSetBuilder&& ) noexcept = default;

        /*!
         * Create the builder associated with a PointSet.
         * @param[in] mesh The PointSet to build/modify
         */
        [[nodiscard]] static std::unique_ptr< PointSetBuilder< dimension > >
            create( PointSet< dimension >& mesh );

        /*!
         * Create a new point with associated coordinates.
         * @param[in] point The point to create
         * @return the index of the created point
         */
        index_t create_point( Point< dimension > point );

        void copy( const PointSet< dimension >& point_set );

    protected:
        explicit PointSetBuilder( PointSet< dimension >& mesh );

    private:
        virtual void do_copy_points( const PointSet< dimension >& mesh ) = 0;

    private:
        PointSet< dimension >& point_set_;
    };
    ALIAS_1D_AND_2D_AND_3D( PointSetBuilder );
} // namespace geode
