/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/builder/triangulated_surface_builder.h>

#include <algorithm>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TriangulatedSurfaceBuilder< dimension > >
        TriangulatedSurfaceBuilder< dimension >::create(
            TriangulatedSurface< dimension >& mesh )
    {
        try
        {
            return TriangulatedSurfaceBuilderFactory< dimension >::create(
                mesh.impl_name(), mesh );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Could not create TriangulatedSurface "
                                      "builder of data structure: ",
                mesh.impl_name().get() };
        }
    }

    template < index_t dimension >
    void TriangulatedSurfaceBuilder< dimension >::do_create_polygon(
        absl::Span< const index_t > vertices )
    {
        OPENGEODE_ASSERT( vertices.size() == 3, "[TriangulatedSurfaceBuilder"
                                                "::do_create_polygon] Only "
                                                "triangles are handled" );
        std::array< index_t, 3 > triangle_vertices;
        absl::c_copy_n( vertices, 3, triangle_vertices.begin() );
        do_create_triangle( triangle_vertices );
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceBuilder< dimension >::create_triangle(
        const std::array< index_t, 3 >& vertices )
    {
        const auto added_triangle = triangulated_surface_.nb_polygons();
        triangulated_surface_.polygon_attribute_manager().resize(
            added_triangle + 1 );
        index_t vertex_id{ 0 };
        for( const auto& vertex : vertices )
        {
            this->associate_polygon_vertex_to_vertex(
                { added_triangle, vertex_id++ }, vertex );
        }
        for( const auto e : Range{ vertices.size() - 1 } )
        {
            this->find_or_create_edge( { vertices[e], vertices[e + 1] } );
        }
        this->find_or_create_edge( { vertices.back(), vertices.front() } );
        do_create_triangle( vertices );
        return added_triangle;
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceBuilder< dimension >::create_triangles(
        index_t nb )
    {
        const auto added_triangle = triangulated_surface_.nb_polygons();
        triangulated_surface_.polygon_attribute_manager().resize(
            added_triangle + nb );
        do_create_triangles( nb );
        return added_triangle;
    }

    template < index_t dimension >
    void TriangulatedSurfaceBuilder< dimension >::reserve_triangles(
        index_t nb )
    {
        const auto nb_triangles = triangulated_surface_.nb_polygons();
        triangulated_surface_.polygon_attribute_manager().reserve(
            nb_triangles + nb );
    }

    template < index_t dimension >
    void TriangulatedSurfaceBuilder< dimension >::copy(
        const TriangulatedSurface< dimension >& triangulated_surface )
    {
        PolygonalSurfaceBuilder< dimension >::copy( triangulated_surface );
    }

    template class opengeode_mesh_api TriangulatedSurfaceBuilder< 2 >;
    template class opengeode_mesh_api TriangulatedSurfaceBuilder< 3 >;
} // namespace geode
