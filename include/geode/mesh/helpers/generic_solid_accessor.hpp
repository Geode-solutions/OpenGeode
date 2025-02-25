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
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

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
    class GenericMeshAccessor< SolidMesh< dimension > >
    {
    public:
        using ElementVertex = PolyhedronVertex;
        using ElementVertices = PolyhedronVertices;
        using ElementFacet = PolyhedronFacet;
        using ElementFacetVertices = PolyhedronFacetVertices;

        explicit GenericMeshAccessor( const SolidMesh< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        [[nodiscard]] index_t nb_vertices() const
        {
            return mesh_.nb_vertices();
        }

        [[nodiscard]] index_t nb_elements() const
        {
            return mesh_.nb_polyhedra();
        }

        [[nodiscard]] index_t nb_element_vertices( index_t polyhedron_id ) const
        {
            return mesh_.nb_polyhedron_vertices( polyhedron_id );
        }

        [[nodiscard]] index_t nb_element_facets( index_t polyhedron_id ) const
        {
            return mesh_.nb_polyhedron_facets( polyhedron_id );
        }

        [[nodiscard]] index_t element_index(
            const ElementFacet& polyhedron_facet ) const
        {
            return polyhedron_facet.polyhedron_id;
        }

        [[nodiscard]] index_t facet_index(
            const ElementFacet& polyhedron_facet ) const
        {
            return polyhedron_facet.facet_id;
        }

        [[nodiscard]] index_t element_vertex(
            const ElementVertex& polyhedron_vertex ) const
        {
            return mesh_.polyhedron_vertex( polyhedron_vertex );
        }

        [[nodiscard]] ElementVertices element_vertices(
            index_t polyhedron_id ) const
        {
            return mesh_.polyhedron_vertices( polyhedron_id );
        }

        [[nodiscard]] Point< dimension > element_barycenter(
            index_t polyhedron_id ) const
        {
            return mesh_.polyhedron_barycenter( polyhedron_id );
        }

        [[nodiscard]] ElementFacetVertices element_facet_vertices(
            const ElementFacet& element_facet ) const
        {
            return mesh_.polyhedron_facet_vertices( element_facet );
        }

        [[nodiscard]] std::optional< index_t > element_adjacent(
            const ElementFacet& element_facet ) const
        {
            return mesh_.polyhedron_adjacent( element_facet );
        }

        [[nodiscard]] std::optional< ElementFacet > element_adjacent_facet(
            ElementFacet element_facet ) const
        {
            return mesh_.polyhedron_adjacent_facet( element_facet );
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
            return mesh_.polyhedron_attribute_manager();
        }

    protected:
        [[nodiscard]] const SolidMesh< dimension >& mesh() const
        {
            return mesh_;
        }

    private:
        const SolidMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    class GenericMeshAccessor< TetrahedralSolid< dimension > >
        : public GenericMeshAccessor< SolidMesh< dimension > >
    {
    public:
        using Base = GenericMeshAccessor< SolidMesh< dimension > >;
        using Element = Tetrahedron;
        using ElementVertex = typename Base::ElementVertex;
        using ElementVertices = typename Base::ElementVertices;
        using ElementFacet = typename Base::ElementFacet;
        using ElementFacetVertices = typename Base::ElementFacetVertices;

        explicit GenericMeshAccessor(
            const TetrahedralSolid< dimension >& mesh )
            : Base{ mesh }, mesh_( mesh )
        {
        }

        [[nodiscard]] Element element( index_t tetrahedron_id ) const
        {
            return mesh_.tetrahedron( tetrahedron_id );
        }

    private:
        const TetrahedralSolid< dimension >& mesh_;
    };

    template < index_t dimension >
    class GenericMeshAccessor< PolyhedralSolid< dimension > >
        : public GenericMeshAccessor< SolidMesh< dimension > >
    {
    public:
        using Base = GenericMeshAccessor< SolidMesh< dimension > >;
        using Base::GenericMeshAccessor;
        using ElementVertex = typename Base::ElementVertex;
        using ElementVertices = typename Base::ElementVertices;
        using ElementFacet = typename Base::ElementFacet;
        using ElementFacetVertices = typename Base::ElementFacetVertices;
    };
} // namespace geode