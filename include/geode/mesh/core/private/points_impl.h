/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>
#include <geode/mesh/builder/coordinate_reference_system_managers_builder.h>
#include <geode/mesh/core/attribute_coordinate_reference_system.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class PointsImpl
        {
        public:
            static constexpr auto POINTS_NAME = "points";

            const Point< dimension >& get_point( index_t vertex_id ) const
            {
                return points_->value( vertex_id );
            }

            void set_point( index_t vertex_id, Point< dimension > point )
            {
                points_->set_value( vertex_id, std::move( point ) );
            }

            index_t nb_points() const
            {
                return points_->size();
            }

            template < typename Mesh >
            void initialize_crs( Mesh& mesh )
            {
                CoordinateReferenceSystemManagersBuilder< dimension >{ mesh }
                    .main_coordinate_reference_system_manager_builder()
                    .delete_coordinate_reference_system( POINTS_NAME );
                register_as_active_crs( mesh );
                points_.reset();
            }

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, Growable< Archive, PointsImpl >{
                                        { []( Archive& a, PointsImpl& impl ) {
                                            a.ext( impl.points_,
                                                bitsery::ext::StdSmartPtr{} );
                                        } } } );
            }

            template < typename Mesh >
            void register_as_active_crs( Mesh& mesh )
            {
                auto crs_manager_builder =
                    CoordinateReferenceSystemManagersBuilder< dimension >{
                        mesh
                    }
                        .main_coordinate_reference_system_manager_builder();
                crs_manager_builder.register_coordinate_reference_system(
                    POINTS_NAME,
                    std::shared_ptr< CoordinateReferenceSystem< dimension > >{
                        std::make_shared<
                            AttributeCoordinateReferenceSystem< dimension > >(
                            mesh.vertex_attribute_manager() ) } );
                crs_manager_builder.set_active_coordinate_reference_system(
                    POINTS_NAME );
            }

        protected:
            PointsImpl() = default;

            template < typename Mesh >
            PointsImpl( Mesh& mesh )
                : PointsImpl( mesh.vertex_attribute_manager() )
            {
                register_as_active_crs( mesh );
            }

            PointsImpl( AttributeManager& manager )
                : PointsImpl{ manager, POINTS_NAME }
            {
            }

            PointsImpl(
                AttributeManager& manager, absl::string_view attribute_name )
                : points_{
                      manager
                          .template find_or_create_attribute< VariableAttribute,
                              Point< dimension > >(
                              attribute_name, Point< dimension >{} )
                  }
            {
            }

        private:
            std::shared_ptr< VariableAttribute< Point< dimension > > > points_;
        };
    } // namespace detail
} // namespace geode
