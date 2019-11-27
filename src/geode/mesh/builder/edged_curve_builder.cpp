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

#include <geode/mesh/builder/edged_curve_builder.h>

#include <geode/mesh/core/edged_curve.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< EdgedCurveBuilder< dimension > >
        EdgedCurveBuilder< dimension >::create( EdgedCurve< dimension >& mesh )
    {
        try
        {
            return EdgedCurveBuilderFactory< dimension >::create(
                mesh.type_name(), mesh );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Could not create EdgedCurve builder of data structure: ",
                mesh.type_name().get()
            };
        }
    }

    template < index_t dimension >
    void EdgedCurveBuilder< dimension >::set_point(
        index_t vertex_id, const Point< dimension >& point )
    {
        OPENGEODE_ASSERT( vertex_id < edged_curve_.nb_vertices(),
            "[EdgedCurveBuilder::set_point] Accessing a vertex that does not "
            "exist" );
        do_set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t EdgedCurveBuilder< dimension >::create_point(
        const Point< dimension >& point )
    {
        const auto added_vertex = edged_curve_.nb_vertices();
        create_vertex();
        set_point( added_vertex, point );
        return added_vertex;
    }

    template < index_t dimension >
    void EdgedCurveBuilder< dimension >::copy(
        const EdgedCurve< dimension >& edged_curve )
    {
        VertexSetBuilder::copy( edged_curve );
        GraphBuilder::copy( edged_curve );
        for( const auto p : Range{ edged_curve.nb_vertices() } )
        {
            set_point( p, edged_curve.point( p ) );
        }
    }

    template class opengeode_mesh_api EdgedCurveBuilder< 2 >;
    template class opengeode_mesh_api EdgedCurveBuilder< 3 >;
} // namespace geode
