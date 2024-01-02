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

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( CoordinateReferenceSystemManagers );
    FORWARD_DECLARATION_DIMENSION_CLASS(
        CoordinateReferenceSystemManagerBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_1D_AND_2D_AND_3D( CoordinateReferenceSystemManagerBuilder );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class CoordinateReferenceSystemManagersBuilder
    {
    public:
        CoordinateReferenceSystemManagersBuilder(
            CoordinateReferenceSystemManagers< dimension >& crs_managers )
            : crs_managers_( crs_managers )
        {
        }

        CoordinateReferenceSystemManagerBuilder1D
            coordinate_reference_system_manager_builder1D();

        CoordinateReferenceSystemManagerBuilder2D
            coordinate_reference_system_manager_builder2D();

        CoordinateReferenceSystemManagerBuilder3D
            coordinate_reference_system_manager_builder3D();

        CoordinateReferenceSystemManagerBuilder< dimension >
            main_coordinate_reference_system_manager_builder();

        /*!
         * Set coordinates to a vertex. This vertex should be created before.
         * It will be set in the active CRS.
         * @param[in] vertex_id The vertex, in [0, nb_vertices()-1].
         * @param[in] point The vertex coordinates
         */
        void set_point( index_t vertex, Point< dimension > point );

    private:
        CoordinateReferenceSystemManagers< dimension >& crs_managers_;
    };
    ALIAS_2D_AND_3D( CoordinateReferenceSystemManagersBuilder );
} // namespace geode