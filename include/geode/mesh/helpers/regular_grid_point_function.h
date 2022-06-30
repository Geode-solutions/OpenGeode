/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/regular_grid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class RegularGridPointFunction
    {
    public:
        RegularGridPointFunction(
            RegularGridPointFunction< dimension >&& other );
        ~RegularGridPointFunction();

        /*!
         * Create a new object function from a RegularGrid, a name, and a
         * value.
         * Throws an exception if an attribute with the same name exists.
         */
        static RegularGridPointFunction< dimension > create(
            const RegularGrid< dimension >& grid,
            absl::string_view function_name,
            Point< dimension > value );

        /*!
         * Finds an object function that already exists in the given
         * RegularGrid, from its given name.
         * Throws an exception if no attribute with the same name exists.
         */
        static RegularGridPointFunction< dimension > find(
            const RegularGrid< dimension >& grid,
            absl::string_view function_name );

        void set_value( const GridVertexIndices< dimension >& vertex_index,
            Point< dimension > value );

        void set_value( index_t vertex_index, Point< dimension > value );

        const Point< dimension >& value(
            const GridVertexIndices< dimension >& vertex_index ) const;

        const Point< dimension >& value( index_t vertex_index ) const;

        Point< dimension > value( const Point< dimension >& point,
            const GridCellIndices< dimension >& grid_cell_indices ) const;

    private:
        RegularGridPointFunction( const RegularGrid< dimension >& grid,
            absl::string_view function_name );

        RegularGridPointFunction( const RegularGrid< dimension >& grid,
            absl::string_view function_name,
            Point< dimension > value );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( RegularGridPointFunction );
} // namespace geode