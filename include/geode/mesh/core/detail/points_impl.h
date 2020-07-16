/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class PointsImpl
        {
        public:
            PointsImpl( VertexSet& mesh )
                : points_(
                      mesh.vertex_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              Point< dimension > >(
                              "points", Point< dimension >{} ) )
            {
            }

            const Point< dimension >& get_point( index_t vertex_id ) const
            {
                return points_->value( vertex_id );
            }

            void set_point( index_t vertex_id, const Point< dimension >& point )
            {
                points_->set_value( vertex_id, point );
            }

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, DefaultGrowable< Archive, PointsImpl >{},
                    []( Archive& archive, PointsImpl& impl ) {
                        archive.ext(
                            impl.points_, bitsery::ext::StdSmartPtr{} );
                    } );
            }

        protected:
            PointsImpl() = default;

        private:
            std::shared_ptr< VariableAttribute< Point< dimension > > > points_;
        };
    } // namespace detail
} // namespace geode
