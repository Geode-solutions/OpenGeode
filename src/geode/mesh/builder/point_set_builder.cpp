/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/builder/point_set_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/point_set.h>

namespace geode
{
    template < index_t dimension >
    PointSetBuilder< dimension >::PointSetBuilder( VertexSet& vertex_set )
        : VertexSetBuilder( vertex_set )
    {
    }

    template < index_t dimension >
    std::unique_ptr< PointSetBuilder< dimension > >
        PointSetBuilder< dimension >::create( PointSet< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            PointSetBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void PointSetBuilder< dimension >::set_mesh(
        PointSet< dimension >& mesh, MeshBuilderFactoryKey key )
    {
        point_set_ = &mesh;
        VertexSetBuilder::set_mesh( mesh, key );
    }

    template < index_t dimension >
    void PointSetBuilder< dimension >::set_point(
        index_t vertex_id, Point< dimension > point )
    {
        OPENGEODE_ASSERT( vertex_id < point_set_->nb_vertices(),
            "[PointSetBuilder::set_point] Accessing a vertex that does not "
            "exist" );
        do_set_point( vertex_id, std::move( point ) );
    }

    template < index_t dimension >
    index_t PointSetBuilder< dimension >::create_point(
        Point< dimension > point )
    {
        const auto added_vertex = point_set_->nb_vertices();
        create_vertex();
        set_point( added_vertex, std::move( point ) );
        return added_vertex;
    }

    template < index_t dimension >
    void PointSetBuilder< dimension >::copy(
        const PointSet< dimension >& point_set )
    {
        VertexSetBuilder::copy( point_set );
        for( const auto p : Range{ point_set.nb_vertices() } )
        {
            set_point( p, point_set.point( p ) );
        }
    }

    template class opengeode_mesh_api PointSetBuilder< 2 >;
    template class opengeode_mesh_api PointSetBuilder< 3 >;
} // namespace geode
