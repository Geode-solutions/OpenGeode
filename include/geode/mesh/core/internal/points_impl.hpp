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

#include <bitsery/ext/std_smart_ptr.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/coordinate_reference_system_managers_builder.hpp>
#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>
#include <geode/mesh/core/vertex_set.hpp>

namespace geode
{
    namespace internal
    {
        /*!
         * This class implements the mesh points storage.
         * Pre-v13: this class was used in the Mesh::Impl classes
         * Post-v13: the inheritance in the Mesh::Impl is kept for backward file
         * serialization, and it is now used in the CRS implementation.
         */
        template < index_t dimension >
        class PointsImpl
        {
        public:
            static constexpr auto POINTS_NAME = "points";

            [[nodiscard]] const Point< dimension >& get_point(
                index_t vertex_id ) const
            {
                return points_->value( vertex_id );
            }

            void set_point( index_t vertex_id, Point< dimension > point )
            {
                points_->set_value( vertex_id, std::move( point ) );
            }

            [[nodiscard]] index_t nb_points() const
            {
                return points_->size();
            }

            [[nodiscard]] std::string_view attribute_name() const
            {
                return points_->name();
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
                archive.ext( *this,
                    Growable< Archive, PointsImpl >{
                        { []( Archive& a, PointsImpl& impl ) {
                             a.ext( impl.points_, bitsery::ext::StdSmartPtr{} );
                             if( !impl.points_ )
                             {
                                 return;
                             }
                             const auto& old_points_properties =
                                 impl.points_->properties();
                             impl.points_->set_properties(
                                 { old_points_properties.assignable,
                                     old_points_properties.interpolable,
                                     false } );
                         },
                            []( Archive& a, PointsImpl& impl ) {
                                a.ext(
                                    impl.points_, bitsery::ext::StdSmartPtr{} );
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
            explicit PointsImpl( Mesh& mesh )
                : PointsImpl( mesh.vertex_attribute_manager() )
            {
                register_as_active_crs( mesh );
            }

            explicit PointsImpl( AttributeManager& manager )
                : PointsImpl{ manager, POINTS_NAME }
            {
            }

            PointsImpl(
                AttributeManager& manager, std::string_view attribute_name )
                : points_{ manager
                          .template find_or_create_attribute< VariableAttribute,
                              Point< dimension > >( attribute_name,
                              Point< dimension >{},
                              { false, false, false } ) }
            {
            }

        private:
            std::shared_ptr< VariableAttribute< Point< dimension > > > points_;
        };
    } // namespace internal
} // namespace geode
