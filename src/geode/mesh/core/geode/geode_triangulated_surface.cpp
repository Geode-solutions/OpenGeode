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

#include <geode/mesh/core/geode/geode_triangulated_surface.hpp>

#include <array>
#include <fstream>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/internal/points_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeTriangulatedSurface< dimension >::Impl
        : public internal::PointsImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        explicit Impl( OpenGeodeTriangulatedSurface< dimension >& mesh )
            : internal::PointsImpl< dimension >( mesh ),
              triangle_vertices_( mesh.polygon_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          std::array< index_t, 3 > >( "triangle_vertices",
                          std::array< index_t, 3 >{ NO_ID, NO_ID, NO_ID },
                          { false, false, false } ) ),
              triangle_adjacents_( mesh.polygon_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          std::array< index_t, 3 > >( "triangle_adjacents",
                          std::array< index_t, 3 >{ NO_ID, NO_ID, NO_ID },
                          { false, false, false } ) )
        {
        }

        index_t get_polygon_vertex( const PolygonVertex& polygon_vertex ) const
        {
            return triangle_vertices_->value( polygon_vertex.polygon_id )
                .at( polygon_vertex.vertex_id );
        }

        std::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
        {
            const auto adj =
                triangle_adjacents_->value( polygon_edge.polygon_id )
                    .at( polygon_edge.edge_id );
            if( adj == NO_ID )
            {
                return std::nullopt;
            }
            return adj;
        }

        void set_polygon_vertex(
            const PolygonVertex& polygon_vertex, const index_t vertex_id )
        {
            triangle_vertices_->modify_value( polygon_vertex.polygon_id,
                [&polygon_vertex, vertex_id](
                    std::array< index_t, 3 >& array ) {
                    array.at( polygon_vertex.vertex_id ) = vertex_id;
                } );
        }

        void set_polygon_adjacent(
            const PolygonEdge& polygon_edge, const index_t adjacent_id )
        {
            triangle_adjacents_->modify_value(
                polygon_edge.polygon_id, [&polygon_edge, adjacent_id](
                                             std::array< index_t, 3 >& array ) {
                    array.at( polygon_edge.edge_id ) = adjacent_id;
                } );
        }

        void add_triangle(
            const OpenGeodeTriangulatedSurface< dimension >& surface,
            const std::array< index_t, 3 >& vertices )
        {
            triangle_vertices_->set_value(
                surface.nb_polygons() - 1, vertices );
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& a, Impl& impl ) {
                         a.ext(
                             impl, bitsery::ext::BaseClass<
                                       internal::PointsImpl< dimension > >{} );
                         a.ext( impl.triangle_vertices_,
                             bitsery::ext::StdSmartPtr{} );
                         a.ext( impl.triangle_adjacents_,
                             bitsery::ext::StdSmartPtr{} );
                         const auto& old_triangle_vertices_properties =
                             impl.triangle_vertices_->properties();
                         impl.triangle_vertices_->set_properties(
                             { old_triangle_vertices_properties.assignable,
                                 old_triangle_vertices_properties.interpolable,
                                 false } );
                         const auto& old_triangle_adjacents_properties =
                             impl.triangle_adjacents_->properties();
                         impl.triangle_adjacents_->set_properties(
                             { old_triangle_adjacents_properties.assignable,
                                 old_triangle_adjacents_properties.interpolable,
                                 false } );
                     },
                        []( Archive& a, Impl& impl ) {
                            a.ext( impl,
                                bitsery::ext::BaseClass<
                                    internal::PointsImpl< dimension > >{} );
                            a.ext( impl.triangle_vertices_,
                                bitsery::ext::StdSmartPtr{} );
                            a.ext( impl.triangle_adjacents_,
                                bitsery::ext::StdSmartPtr{} );
                        } } } );
        }

    private:
        std::shared_ptr< VariableAttribute< std::array< index_t, 3 > > >
            triangle_vertices_;
        std::shared_ptr< VariableAttribute< std::array< index_t, 3 > > >
            triangle_adjacents_;
    };

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >::OpenGeodeTriangulatedSurface()
        : impl_( *this )
    {
    }

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >::OpenGeodeTriangulatedSurface(
        OpenGeodeTriangulatedSurface&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >&
        OpenGeodeTriangulatedSurface< dimension >::operator=(
            OpenGeodeTriangulatedSurface&& ) noexcept = default;

    template < index_t dimension >
    OpenGeodeTriangulatedSurface< dimension >::~OpenGeodeTriangulatedSurface() =
        default;

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::set_vertex(
        index_t vertex_id, Point< dimension > point, OGTriangulatedSurfaceKey )
    {
        impl_->set_point( vertex_id, std::move( point ) );
    }

    template < index_t dimension >
    index_t OpenGeodeTriangulatedSurface< dimension >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    std::optional< index_t >
        OpenGeodeTriangulatedSurface< dimension >::get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
    {
        return impl_->get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    template < typename Archive >
    void OpenGeodeTriangulatedSurface< dimension >::serialize(
        Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OpenGeodeTriangulatedSurface >{
                { []( Archive& a, OpenGeodeTriangulatedSurface& surface ) {
                     a.ext( surface, bitsery::ext::BaseClass<
                                         TriangulatedSurface< dimension > >{} );
                     a.object( surface.impl_ );
                     surface.impl_->initialize_crs( surface );
                 },
                    []( Archive& a, OpenGeodeTriangulatedSurface& surface ) {
                        a.ext(
                            surface, bitsery::ext::BaseClass<
                                         TriangulatedSurface< dimension > >{} );
                        a.object( surface.impl_ );
                    } } } );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::set_polygon_vertex(
        const PolygonVertex& polygon_vertex,
        index_t vertex_id,
        OGTriangulatedSurfaceKey )
    {
        impl_->set_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::add_triangle(
        const std::array< index_t, 3 >& vertices, OGTriangulatedSurfaceKey )
    {
        impl_->add_triangle( *this, vertices );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurface< dimension >::set_polygon_adjacent(
        const PolygonEdge& polygon_edge,
        index_t adjacent_id,
        OGTriangulatedSurfaceKey )
    {
        impl_->set_polygon_adjacent( polygon_edge, adjacent_id );
    }

    template class opengeode_mesh_api OpenGeodeTriangulatedSurface< 2 >;
    template class opengeode_mesh_api OpenGeodeTriangulatedSurface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodeTriangulatedSurface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_mesh_api, OpenGeodeTriangulatedSurface< 3 > );
} // namespace geode
