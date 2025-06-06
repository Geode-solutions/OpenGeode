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

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/bitsery_attribute.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    namespace detail
    {

        template < typename Type, typename Serializer >
        void register_inlinedvector( PContext& context, std::string_view type )
        {
            register_attribute_type< absl::InlinedVector< Type, 1 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_1" ) );
            register_attribute_type< absl::InlinedVector< Type, 2 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_2" ) );
            register_attribute_type< absl::InlinedVector< Type, 3 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_3" ) );
            register_attribute_type< absl::InlinedVector< Type, 4 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_4" ) );
            register_attribute_type< absl::InlinedVector< Type, 5 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_5" ) );
            register_attribute_type< absl::InlinedVector< Type, 6 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_6" ) );
            register_attribute_type< absl::InlinedVector< Type, 7 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_7" ) );
            register_attribute_type< absl::InlinedVector< Type, 8 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_8" ) );
            register_attribute_type< absl::InlinedVector< Type, 9 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_9" ) );
            register_attribute_type< absl::InlinedVector< Type, 10 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_", type, "_10" ) );
        }

        template < typename Serializer >
        void register_geometry_pcontext( PContext& context )
        {
            register_attribute_type< Point1D, Serializer >(
                context, "Point1D" );
            register_attribute_type< Point2D, Serializer >(
                context, "Point2D" );
            register_attribute_type< Point3D, Serializer >(
                context, "Point3D" );
            register_attribute_type< Vector1D, Serializer >(
                context, "Vector1D" );
            register_attribute_type< Vector2D, Serializer >(
                context, "Vector2D" );
            register_attribute_type< Vector3D, Serializer >(
                context, "Vector3D" );
            register_attribute_type< absl::InlinedVector< Point1D, 2 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_Point1D_2" ) );
            register_attribute_type< absl::InlinedVector< Point2D, 3 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_Point2D_3" ) );
            register_attribute_type< absl::InlinedVector< Point3D, 4 >,
                Serializer >(
                context, absl::StrCat( "InlinedVector_Point3D_4" ) );
        }
    } // namespace detail
} // namespace geode
