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

#include <async++.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/range.h>

#include <geode/geometry/point.h>

#include <geode/mesh/common.h>

namespace geode
{
    namespace internal
    {
        template < typename MeshFrom, typename Builder >
        void copy_points3d_into_2d(
            const MeshFrom& from, Builder& builder, index_t axis_to_remove )
        {
            OPENGEODE_EXCEPTION( axis_to_remove < 3,
                "[copy_points3d_into_2d] Invalid axis to remove." );
            builder.create_vertices( from.nb_vertices() );
            async::parallel_for(
                async::irange( index_t{ 0 }, from.nb_vertices() ),
                [&from, &builder, axis_to_remove]( index_t v ) {
                    const auto& pt_3d = from.point( v );
                    builder.set_point( v,
                        Point2D{ { pt_3d.value( axis_to_remove == 0 ? 1 : 0 ),
                            pt_3d.value( axis_to_remove == 2 ? 1 : 2 ) } } );
                } );
        }

        template < typename MeshFrom, typename Builder >
        void copy_points2d_into_3d( const MeshFrom& from,
            Builder& builder,
            index_t axis_to_add,
            double axis_coordinate )
        {
            OPENGEODE_EXCEPTION( axis_to_add < 3,
                "[copy_points2d_into_3d] Invalid axis to add." );
            builder.create_vertices( from.nb_vertices() );
            for( const auto v : Range{ from.nb_vertices() } )
            {
                const auto& pt_2d = from.point( v );
                builder.set_point( v,
                    Point3D{
                        { axis_to_add == 0 ? axis_coordinate : pt_2d.value( 0 ),
                            axis_to_add == 1 ? axis_coordinate
                                             : pt_2d.value( axis_to_add / 2 ),
                            axis_to_add == 2 ? axis_coordinate
                                             : pt_2d.value( 1 ) } } );
            }
        }

        template < typename MeshFrom, typename Builder >
        void copy_points( const MeshFrom& from, Builder& builder )
        {
            builder.create_vertices( from.nb_vertices() );
            for( const auto v : Range{ from.nb_vertices() } )
            {
                builder.set_point( v, from.point( v ) );
            }
        }

        template < typename MeshFrom, typename Builder >
        void copy_meta_info( const MeshFrom& from, Builder& builder )
        {
            builder.set_name( from.name() );
        }

        void copy_attributes(
            const AttributeManager& manager_in, AttributeManager& manager_out );
    } // namespace internal
} // namespace geode
