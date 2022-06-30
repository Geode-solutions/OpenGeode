/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class TriangulatedSurfacePointFunction
    {
    public:
        TriangulatedSurfacePointFunction(
            TriangulatedSurfacePointFunction< dimension >&& other );
        ~TriangulatedSurfacePointFunction();

        /*!
         * Create a new object function from a TriangulatedSurface, a name, and
         * a value. Throws an exception if an attribute with the same name
         * exists.
         */
        static TriangulatedSurfacePointFunction< dimension > create(
            const TriangulatedSurface< dimension >& solid,
            absl::string_view function_name,
            Point< dimension > value );

        /*!
         * Finds an object function that already exists in the given
         * TriangulatedSurface, from its given name.
         * Throws an exception if no attribute with the same name exists.
         */
        static TriangulatedSurfacePointFunction< dimension > find(
            const TriangulatedSurface< dimension >& solid,
            absl::string_view function_name );

        void set_value( index_t vertex_index, Point< dimension > value );

        const Point< dimension >& value( index_t vertex_index ) const;

        Point< dimension > value(
            const Point< dimension >& point, index_t tetrahedron_id ) const;

    private:
        TriangulatedSurfacePointFunction(
            const TriangulatedSurface< dimension >& solid,
            absl::string_view function_name );

        TriangulatedSurfacePointFunction(
            const TriangulatedSurface< dimension >& solid,
            absl::string_view function_name,
            Point< dimension > value );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( TriangulatedSurfacePointFunction );
} // namespace geode