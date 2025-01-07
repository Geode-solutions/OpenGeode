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

#include <geode/geometry/basic_objects/triangle.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

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
    class GenericMeshAccessor< SurfaceMesh< dimension > >
    {
    public:
        using ElementVertex = PolygonVertex;
        using ElementVertices = PolygonVertices;
        using ElementFacet = PolygonEdge;
        using ElementFacetVertices = std::array< index_t, 2 >;

        explicit GenericMeshAccessor( const SurfaceMesh< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        [[nodiscard]] index_t nb_vertices() const
        {
            return mesh_.nb_vertices();
        }

        [[nodiscard]] index_t nb_elements() const
        {
            return mesh_.nb_polygons();
        }

        [[nodiscard]] index_t nb_element_vertices( index_t polygon_id ) const
        {
            return mesh_.nb_polygon_vertices( polygon_id );
        }

        [[nodiscard]] index_t nb_element_facets( index_t polygon_id ) const
        {
            return mesh_.nb_polygon_edges( polygon_id );
        }

        [[nodiscard]] index_t element_index(
            const ElementFacet& polygon_edge ) const
        {
            return polygon_edge.polygon_id;
        }

        [[nodiscard]] index_t facet_index(
            const ElementFacet& polygon_edge ) const
        {
            return polygon_edge.edge_id;
        }

        [[nodiscard]] index_t element_vertex(
            const ElementVertex& polygon_vertex ) const
        {
            return mesh_.polygon_vertex( polygon_vertex );
        }

        [[nodiscard]] ElementVertices element_vertices(
            index_t polygon_id ) const
        {
            return mesh_.polygon_vertices( polygon_id );
        }

        [[nodiscard]] Point< dimension > element_barycenter(
            index_t polygon_id ) const
        {
            return mesh_.polygon_barycenter( polygon_id );
        }

        [[nodiscard]] ElementFacetVertices element_facet_vertices(
            const ElementFacet& element_facet ) const
        {
            return mesh_.polygon_edge_vertices( element_facet );
        }

        [[nodiscard]] std::optional< index_t > element_adjacent(
            const ElementFacet& element_facet ) const
        {
            return mesh_.polygon_adjacent( element_facet );
        }

        [[nodiscard]] std::optional< ElementFacet > element_adjacent_facet(
            const ElementFacet& element_facet ) const
        {
            return mesh_.polygon_adjacent_edge( element_facet );
        }

        [[nodiscard]] const uuid& id() const
        {
            return mesh_.id();
        }

        [[nodiscard]] const Point< dimension >& point( index_t vertex_id ) const
        {
            return mesh_.point( vertex_id );
        }

        [[nodiscard]] AttributeManager& element_attribute_manager() const
        {
            return mesh_.polygon_attribute_manager();
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    class GenericMeshAccessor< TriangulatedSurface< dimension > >
        : public GenericMeshAccessor< SurfaceMesh< dimension > >
    {
    public:
        using Base = GenericMeshAccessor< SurfaceMesh< dimension > >;
        using Element = Triangle< dimension >;
        using ElementVertex = typename Base::ElementVertex;
        using ElementVertices = typename Base::ElementVertices;
        using ElementFacet = typename Base::ElementFacet;
        using ElementFacetVertices = typename Base::ElementFacetVertices;

        explicit GenericMeshAccessor(
            const TriangulatedSurface< dimension >& mesh )
            : Base{ mesh }, mesh_( mesh )
        {
        }

        [[nodiscard]] Element element( index_t triangle_id ) const
        {
            return mesh_.triangle( triangle_id );
        }

    private:
        const TriangulatedSurface< dimension >& mesh_;
    };

    template < index_t dimension >
    class GenericMeshAccessor< PolygonalSurface< dimension > >
        : public GenericMeshAccessor< SurfaceMesh< dimension > >
    {
    public:
        using Base = GenericMeshAccessor< SurfaceMesh< dimension > >;
        using Base::GenericMeshAccessor;
        using ElementVertex = typename Base::ElementVertex;
        using ElementVertices = typename Base::ElementVertices;
        using ElementFacet = typename Base::ElementFacet;
        using ElementFacetVertices = typename Base::ElementFacetVertices;
    };
} // namespace geode