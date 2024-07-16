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

#include <geode/mesh/builder/geode/geode_edged_curve_builder.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/mesh_builder_factory.hpp>
#include <geode/mesh/core/geode/geode_edged_curve.hpp>

namespace geode
{
    template < index_t dimension >
    OpenGeodeEdgedCurveBuilder< dimension >::OpenGeodeEdgedCurveBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : OpenGeodeEdgedCurveBuilder< dimension >(
            dynamic_cast< OpenGeodeEdgedCurve< dimension >& >( vertex_set ) )
    {
    }

    template < index_t dimension >
    OpenGeodeEdgedCurveBuilder< dimension >::OpenGeodeEdgedCurveBuilder(
        OpenGeodeEdgedCurve< dimension >& mesh )
        : EdgedCurveBuilder< dimension >( mesh ), geode_edged_curve_( mesh )
    {
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_delete_graph_vertices(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_permute_graph_vertices(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        geode_edged_curve_.set_edge_vertex( edge_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_create_edge()
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_create_edges(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_delete_edges(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_permute_edges(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_copy_points(
        const EdgedCurve< dimension >& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeEdgedCurveBuilder< dimension >::do_copy_edges(
        const Graph& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template class opengeode_mesh_api OpenGeodeEdgedCurveBuilder< 2 >;
    template class opengeode_mesh_api OpenGeodeEdgedCurveBuilder< 3 >;
} // namespace geode
