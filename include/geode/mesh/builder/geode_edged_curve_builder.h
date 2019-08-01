/*
 * Copyright (c) 2019 Geode-solutions
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
        OpenGeodeEdgedCurveBuilder( EdgedCurve< dimension >& edged_curve )
            : EdgedCurveBuilder< dimension >( edged_curve ),
              geode_edged_curve_(
                  dynamic_cast< OpenGeodeEdgedCurve< dimension >& >(
                      edged_curve ) )
        {
        }

    private:
        void do_set_point(
            index_t vertex_id, const Point< dimension >& point ) override;

        void do_create_vertex() override;

        void do_create_vertices( index_t nb ) override;

        void do_delete_curve_vertices(
            const std::vector< bool >& to_delete ) override;

        void do_set_edge_vertex(
            const EdgeVertex& edge_vertex, index_t vertex_id ) override;

        void do_create_edge() override;

        void do_create_edges( index_t nb ) override;

        void do_delete_edges( const std::vector< bool >& to_delete ) override;

    private:
        OpenGeodeEdgedCurve< dimension >& geode_edged_curve_;
    };
    ALIAS_2D_AND_3D( OpenGeodeEdgedCurveBuilder );
} // namespace geode
