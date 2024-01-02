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

#include <geode/basic/attribute_manager.h>

#include <geode/geometry/basic_objects/segment.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/edged_curve.h>

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
    class GenericMeshAccessor< EdgedCurve< dimension > >
    {
    public:
        using ElementVertex = EdgeVertex;
        using ElementVertices = std::array< index_t, 2 >;

        GenericMeshAccessor( const EdgedCurve< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        index_t nb_elements() const
        {
            return mesh_.nb_edges();
        }

        index_t nb_element_vertices( index_t /*unused*/ ) const
        {
            return 2;
        }

        index_t nb_element_facets( index_t /*unused*/ ) const
        {
            return 2;
        }

        index_t element_vertex( const ElementVertex& edge_vertex ) const
        {
            return mesh_.edge_vertex( edge_vertex );
        }

        ElementVertices element_vertices( index_t edge_id ) const
        {
            return mesh_.edge_vertices( edge_id );
        }

        Point< dimension > element_barycenter( index_t edge_id ) const
        {
            return mesh_.edge_barycenter( edge_id );
        }

        const uuid& id() const
        {
            return mesh_.id();
        }

        const Point< dimension >& point( index_t vertex_id ) const
        {
            return mesh_.point( vertex_id );
        }

        AttributeManager& element_attribute_manager() const
        {
            return mesh_.edge_attribute_manager();
        }

    private:
        const EdgedCurve< dimension >& mesh_;
    };
} // namespace geode