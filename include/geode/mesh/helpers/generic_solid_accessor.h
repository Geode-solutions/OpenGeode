/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/basic_objects/tetrahedron.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    template < typename T >
    class GenericMeshAccessor;
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

        GenericMeshAccessor( const SolidMesh< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        index_t nb_elements() const
        {
            return mesh_.nb_polyhedra();
        }

        index_t nb_element_vertices( index_t polyhedron_id ) const
        {
            return mesh_.nb_polyhedron_vertices( polyhedron_id );
        }

        index_t nb_element_facets( index_t polyhedron_id ) const
        {
            return mesh_.nb_polyhedron_facets( polyhedron_id );
        }

        index_t element_vertex( const ElementVertex& polyhedron_vertex ) const
        {
            return mesh_.polyhedron_vertex( polyhedron_vertex );
        }

        ElementVertices element_vertices( index_t polyhedron_id ) const
        {
            return mesh_.polyhedron_vertices( polyhedron_id );
        }

        ElementFacetVertices element_facet_vertices(
            const ElementFacet& polyhedron_facet ) const
        {
            return mesh_.polyhedron_facet_vertices( polyhedron_facet );
        }

        absl::optional< index_t > element_adjacent(
            const ElementFacet& polyhedron_facet ) const
        {
            return mesh_.polyhedron_adjacent( polyhedron_facet );
        }

        absl::optional< ElementFacet > element_adjacent_facet(
            const ElementFacet& polyhedron_facet ) const
        {
            return mesh_.polyhedron_adjacent_facet( polyhedron_facet );
        }

    protected:
        const SolidMesh< dimension >& mesh() const
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

        GenericMeshAccessor( const TetrahedralSolid< dimension >& mesh )
            : Base{ mesh }, mesh_( mesh )
        {
        }

        Element element( index_t tetrahedron_id ) const
        {
            return this->mesh().tetrahedron( tetrahedron_id );
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