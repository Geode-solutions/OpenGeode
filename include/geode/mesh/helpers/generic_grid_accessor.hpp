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

#include <geode/geometry/basic_objects/tetrahedron.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/grid.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>

namespace geode
{
    template < typename T >
    class GenericMeshAccessor;
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class GenericMeshAccessor< Grid< dimension > >
    {
    public:
        explicit GenericMeshAccessor( const Grid< dimension >& grid )
            : grid_( grid )
        {
        }

        [[nodiscard]] index_t nb_vertices() const
        {
            return grid_.nb_grid_vertices();
        }

        [[nodiscard]] index_t nb_elements() const
        {
            return grid_.nb_cells();
        }

        [[nodiscard]] index_t nb_element_vertices( index_t /*unused*/ ) const
        {
            return grid_.nb_cell_vertices();
        }

        [[nodiscard]] index_t nb_element_facets( index_t /*unused*/ ) const
        {
            return grid_.nb_cell_neighbors();
        }

        [[nodiscard]] Point< dimension > element_barycenter(
            index_t cell_id ) const
        {
            return grid_.cell_barycenter( cell_id );
        }

        [[nodiscard]] const uuid& id() const
        {
            return grid_.id();
        }

        [[nodiscard]] Point< dimension > point( index_t vertex_id ) const
        {
            return grid_.grid_point( grid_.vertex_indices( vertex_id ) );
        }

        [[nodiscard]] AttributeManager& vertex_attribute_manager() const
        {
            return grid_.grid_vertex_attribute_manager();
        }

        [[nodiscard]] AttributeManager& element_attribute_manager() const
        {
            return grid_.cell_attribute_manager();
        }

    private:
        const Grid< dimension >& grid_;
    };

    template < index_t dimension >
    class GenericMeshAccessor< LightRegularGrid< dimension > >
        : public GenericMeshAccessor< Grid< dimension > >
    {
    public:
        using Base = GenericMeshAccessor< Grid< dimension > >;
        using VerticesAroundVertex =
            typename LightRegularGrid< dimension >::VerticesAroundVertex;

        explicit GenericMeshAccessor(
            const LightRegularGrid< dimension >& grid )
            : Base{ grid }, grid_( grid )
        {
        }

        [[nodiscard]] VerticesAroundVertex vertices_around_vertex(
            index_t vertex_id ) const
        {
            return grid_.vertices_around_vertex( vertex_id );
        }

    private:
        const LightRegularGrid< dimension >& grid_;
    };

    template < index_t dimension >
    class GenericMeshAccessor< RegularGrid< dimension > >
        : public GenericMeshAccessor< Grid< dimension > >
    {
    public:
        using Base = GenericMeshAccessor< Grid< dimension > >;
        using VerticesAroundVertex =
            typename RegularGrid< dimension >::VerticesAroundVertex;

        explicit GenericMeshAccessor( const RegularGrid< dimension >& grid )
            : Base{ grid }, grid_( grid )
        {
        }

        [[nodiscard]] VerticesAroundVertex vertices_around_vertex(
            index_t vertex_id ) const
        {
            return grid_.vertices_around_vertex( vertex_id );
        }

    private:
        const RegularGrid< dimension >& grid_;
    };
} // namespace geode