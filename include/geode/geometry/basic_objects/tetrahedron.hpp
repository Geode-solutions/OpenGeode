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

#include <geode/geometry/common.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_3D( BoundingBox );
    ALIAS_3D( Point );
    class Tetrahedron;
    class OwnerTetrahedron;

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{
    template < typename PointType >
    class GenericTetrahedron
    {
    public:
        static constexpr inline std::array< std::array< index_t, 3 >, 4 >
            tetrahedron_facet_vertex{ { { { 1, 3, 2 } }, { { 0, 2, 3 } },
                { { 3, 1, 0 } }, { { 0, 1, 2 } } } };

        GenericTetrahedron( PointType point0,
            PointType point1,
            PointType point2,
            PointType point3 ) noexcept;

        GenericTetrahedron( const GenericTetrahedron& other ) noexcept;
        GenericTetrahedron& operator=(
            const GenericTetrahedron& other ) noexcept;
        GenericTetrahedron( GenericTetrahedron&& other ) noexcept;
        GenericTetrahedron& operator=( GenericTetrahedron&& other ) noexcept;

        [[nodiscard]] Point3D barycenter() const;
        void set_point( local_index_t vertex, PointType point );
        [[nodiscard]] const std::array< PointType, 4 >& vertices() const;
        [[nodiscard]] BoundingBox3D bounding_box() const;
        [[nodiscard]] bool is_degenerated() const;
        [[nodiscard]] std::string string() const;

    private:
        std::array< PointType, 4 > vertices_;
    };

    class opengeode_geometry_api OwnerTetrahedron
        : public GenericTetrahedron< Point3D >
    {
        using Base = GenericTetrahedron< Point3D >;

    public:
        explicit OwnerTetrahedron( Point3D point0,
            Point3D point1,
            Point3D point2,
            Point3D point3 ) noexcept;

        OwnerTetrahedron( const Tetrahedron& other ) noexcept;
        OwnerTetrahedron( const OwnerTetrahedron& other ) noexcept;
        OwnerTetrahedron& operator=( const OwnerTetrahedron& other ) noexcept;
        OwnerTetrahedron( OwnerTetrahedron&& other ) noexcept;
        OwnerTetrahedron& operator=( OwnerTetrahedron&& other ) noexcept;
    };

    class opengeode_geometry_api Tetrahedron
        : public GenericTetrahedron< RefPoint3D >
    {
        using Base = GenericTetrahedron< RefPoint3D >;

    public:
        Tetrahedron( const Point3D& point0,
            const Point3D& point1,
            const Point3D& point2,
            const Point3D& point3 ) noexcept;

        Tetrahedron( const Tetrahedron& other ) noexcept;
        Tetrahedron( const OwnerTetrahedron& other ) noexcept;
        Tetrahedron& operator=( const Tetrahedron& other ) noexcept;
        Tetrahedron( Tetrahedron&& other ) noexcept;
        Tetrahedron& operator=( Tetrahedron&& other ) noexcept;
    };

    class OpenGeodeTetrahedronException : public OpenGeodeException
    {
    public:
        template < typename... Args >
        explicit OpenGeodeTetrahedronException(
            const Tetrahedron& tetrahedron_in, const Args&... message )
            : OpenGeodeException{ absl::StrCat(
                  message..., " at ", tetrahedron_in.string() ) },
              tetrahedron{ tetrahedron_in }
        {
        }

        OwnerTetrahedron tetrahedron;
    };
} // namespace geode

// NOLINTNEXTLINE
#define OPENGEODE_TETRAHEDRON_EXCEPTION(                                       \
    dimension, condition, tetrahedron, ... )                                   \
    if( ABSL_PREDICT_FALSE( !( condition ) ) )                                 \
        throw geode::OpenGeodeTetrahedronException< dimension >                \
        {                                                                      \
            tetrahedron, __VA_ARGS__                                           \
        }
