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

#include <geode/mesh/builder/coordinate_reference_system_managers_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>
#include <geode/mesh/core/coordinate_reference_system_managers.h>

namespace geode
{
    template < index_t dimension >
    CoordinateReferenceSystemManagerBuilder1D
        CoordinateReferenceSystemManagersBuilder<
            dimension >::coordinate_reference_system_manager_builder1D()
    {
        return CoordinateReferenceSystemManagerBuilder1D{
            crs_managers_.coordinate_reference_system_manager1D( {} )
        };
    }

    template < index_t dimension >
    CoordinateReferenceSystemManagerBuilder2D
        CoordinateReferenceSystemManagersBuilder<
            dimension >::coordinate_reference_system_manager_builder2D()
    {
        return CoordinateReferenceSystemManagerBuilder2D{
            crs_managers_.coordinate_reference_system_manager2D( {} )
        };
    }

    template < index_t dimension >
    CoordinateReferenceSystemManagerBuilder3D
        CoordinateReferenceSystemManagersBuilder<
            dimension >::coordinate_reference_system_manager_builder3D()
    {
        return CoordinateReferenceSystemManagerBuilder3D{
            crs_managers_.coordinate_reference_system_manager3D( {} )
        };
    }

    template < index_t dimension >
    CoordinateReferenceSystemManagerBuilder< dimension >
        CoordinateReferenceSystemManagersBuilder<
            dimension >::main_coordinate_reference_system_manager_builder()
    {
        return CoordinateReferenceSystemManagerBuilder< dimension >{
            crs_managers_.main_coordinate_reference_system_manager( {} )
        };
    }

    template < index_t dimension >
    void CoordinateReferenceSystemManagersBuilder< dimension >::set_point(
        index_t vertex, Point< dimension > point )
    {
        crs_managers_.set_point( vertex, std::move( point ), {} );
    }

    template class opengeode_mesh_api
        CoordinateReferenceSystemManagersBuilder< 2 >;
    template class opengeode_mesh_api
        CoordinateReferenceSystemManagersBuilder< 3 >;
} // namespace geode