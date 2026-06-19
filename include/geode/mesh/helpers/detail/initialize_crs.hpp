/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/mesh/core/internal/points_impl.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/coordinate_reference_system_managers_builder.hpp>
#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>

namespace geode
{
    namespace detail
    {
        template < typename Mesh >
        void initialize_crs( Mesh &mesh )
        {
            CoordinateReferenceSystemManagersBuilder< Mesh::dim >{ mesh }
                .main_coordinate_reference_system_manager_builder()
                .delete_coordinate_reference_system(
                    internal::PointsImpl< Mesh::dim >::POINTS_NAME );
            auto crs_manager_builder =
                CoordinateReferenceSystemManagersBuilder< Mesh::dim >{ mesh }
                    .main_coordinate_reference_system_manager_builder();
            crs_manager_builder.register_coordinate_reference_system(
                internal::PointsImpl< Mesh::dim >::POINTS_NAME,
                std::shared_ptr< CoordinateReferenceSystem< Mesh::dim > >{
                    std::make_shared<
                        AttributeCoordinateReferenceSystem< Mesh::dim > >(
                        mesh.vertex_attribute_manager(),
                        internal::PointsImpl< Mesh::dim >::POINTS_NAME ) } );
            crs_manager_builder.set_active_coordinate_reference_system(
                internal::PointsImpl< Mesh::dim >::POINTS_NAME );
        }

        template < typename Mesh >
        void initialize_crs( Mesh &mesh, const geode::uuid &attribute_id )
        {
            CoordinateReferenceSystemManagersBuilder< Mesh::dim >{ mesh }
                .main_coordinate_reference_system_manager_builder()
                .delete_coordinate_reference_system(
                    internal::PointsImpl< Mesh::dim >::POINTS_NAME );
            auto crs_manager_builder =
                CoordinateReferenceSystemManagersBuilder< Mesh::dim >{ mesh }
                    .main_coordinate_reference_system_manager_builder();
            crs_manager_builder.register_coordinate_reference_system(
                internal::PointsImpl< Mesh::dim >::POINTS_NAME,
                std::shared_ptr< CoordinateReferenceSystem< Mesh::dim > >{
                    std::make_shared<
                        AttributeCoordinateReferenceSystem< Mesh::dim > >(
                        mesh.vertex_attribute_manager(), attribute_id ) } );
            crs_manager_builder.set_active_coordinate_reference_system(
                internal::PointsImpl< Mesh::dim >::POINTS_NAME );
        }
    } // namespace detail
} // namespace geode
