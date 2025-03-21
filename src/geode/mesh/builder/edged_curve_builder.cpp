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

#include <geode/mesh/builder/edged_curve_builder.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/mesh_builder_factory.hpp>
#include <geode/mesh/core/edged_curve.hpp>

namespace geode
{
    template < index_t dimension >
    EdgedCurveBuilder< dimension >::EdgedCurveBuilder(
        EdgedCurve< dimension >& mesh )
        : GraphBuilder( mesh ),
          CoordinateReferenceSystemManagersBuilder< dimension >( mesh ),
          edged_curve_( mesh )
    {
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurveBuilder< dimension > >
        EdgedCurveBuilder< dimension >::create( EdgedCurve< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            EdgedCurveBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    index_t EdgedCurveBuilder< dimension >::create_point(
        Point< dimension > point )
    {
        const auto added_vertex = edged_curve_.nb_vertices();
        create_vertex();
        this->set_point( added_vertex, std::move( point ) );
        return added_vertex;
    }

    template < index_t dimension >
    void EdgedCurveBuilder< dimension >::copy(
        const EdgedCurve< dimension >& edged_curve )
    {
        OPENGEODE_EXCEPTION(
            edged_curve_.nb_vertices() == 0 && edged_curve_.nb_edges() == 0,
            "[EdgedCurveBuilder::copy] Cannot copy a mesh into an already "
            "initialized mesh." );
        GraphBuilder::copy( edged_curve );
        if( edged_curve_.impl_name() == edged_curve.impl_name() )
        {
            do_copy_points( edged_curve );
        }
        else
        {
            for( const auto p : Range{ edged_curve.nb_vertices() } )
            {
                this->set_point( p, edged_curve.point( p ) );
            }
        }
    }

    template class opengeode_mesh_api EdgedCurveBuilder< 1 >;
    template class opengeode_mesh_api EdgedCurveBuilder< 2 >;
    template class opengeode_mesh_api EdgedCurveBuilder< 3 >;
} // namespace geode
