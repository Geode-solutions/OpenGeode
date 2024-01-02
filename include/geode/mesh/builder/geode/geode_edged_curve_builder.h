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

#include <vector>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeEdgedCurve );
    class VertexSet;
} // namespace geode

namespace geode
{
    /*!
     * Implementation class for EdgedCurveBuilder using OpenGeode data structure
     */
    template < index_t dimension >
    class OpenGeodeEdgedCurveBuilder : public EdgedCurveBuilder< dimension >
    {
    public:
        static constexpr auto dim = dimension;

        OpenGeodeEdgedCurveBuilder(
            VertexSet& vertex_set, MeshBuilderFactoryKey );

        OpenGeodeEdgedCurveBuilder( OpenGeodeEdgedCurve< dimension >& mesh );

    private:
        void do_create_vertex() final;

        void do_create_vertices( index_t nb ) final;

        void do_delete_graph_vertices( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) final;

        void do_permute_graph_vertices( absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) final;

        void do_set_edge_vertex(
            const EdgeVertex& edge_vertex, index_t vertex_id ) final;

        void do_create_edge() final;

        void do_create_edges( index_t nb ) final;

        void do_delete_edges( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) final;

        void do_permute_edges( absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) final;

        void do_copy_points( const EdgedCurve< dimension >& mesh ) final;

        void do_copy_edges( const Graph& mesh ) final;

    private:
        OpenGeodeEdgedCurve< dimension >& geode_edged_curve_;
    };
    ALIAS_2D_AND_3D( OpenGeodeEdgedCurveBuilder );
} // namespace geode
