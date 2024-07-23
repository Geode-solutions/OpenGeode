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

#include <geode/mesh/core/surface_mesh.hpp>

#include <algorithm>
#include <stack>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/cached_value.hpp>
#include <geode/basic/detail/mapping_after_deletion.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/mensuration.hpp>

#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/detail/facet_storage.hpp>
#include <geode/mesh/core/internal/surface_mesh_impl.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/texture2d.hpp>
#include <geode/mesh/core/texture_storage.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

namespace
{
    template < geode::index_t dimension >
    void check_vertex_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t vertex_id )
    {
        geode_unused( surface );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < surface.nb_vertices(),
            "[check_vertex_id] Trying to access an invalid vertex" );
    }

    template < geode::index_t dimension >
    void check_polygon_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        OPENGEODE_ASSERT( polygon_id < surface.nb_polygons(),
            "[check_polygon_id] Trying to access an invalid polygon" );
    }

    template < geode::index_t dimension >
    void check_polygon_vertex_id(
        const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id,
        const geode::local_index_t vertex_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < surface.nb_polygon_vertices( polygon_id ),
            "[check_polygon_vertex_id] Trying to access an invalid polygon "
            "local vertex" );
    }

    template < geode::index_t dimension >
    void check_polygon_edge_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id,
        const geode::local_index_t edge_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        geode_unused( edge_id );
        OPENGEODE_ASSERT( edge_id < surface.nb_polygon_edges( polygon_id ),
            "[check_polygon_edge_id] Trying to access an invalid polygon local "
            "edge" );
    }

    template < geode::index_t dimension >
    void output( const geode::SurfaceMesh< dimension >& mesh )
    {
        auto surf = geode::TriangulatedSurface< dimension >::create();
        auto bui =
            geode::TriangulatedSurfaceBuilder< dimension >::create( *surf );
        for( const auto v : geode::Range{ mesh.nb_vertices() } )
        {
            bui->create_point( mesh.point( v ) );
        }
        for( const auto p : geode::Range{ mesh.nb_polygons() } )
        {
            const auto vertices = mesh.polygon_vertices( p );
            bui->create_triangle( { vertices[0], vertices[1], vertices[2] } );
        }
        bui->delete_isolated_vertices();
        geode::save_triangulated_surface( *surf, "output.og_tsf3d" );
    }

    template < geode::index_t dimension >
    geode::internal::PolygonsAroundVertexImpl compute_polygons_around_vertex(
        const geode::SurfaceMesh< dimension >& mesh,
        const geode::index_t& vertex_id,
        const std::optional< geode::PolygonVertex >& first_polygon )
    {
        if( !first_polygon )
        {
            return {};
        }
        OPENGEODE_ASSERT(
            mesh.polygon_vertex( first_polygon.value() ) == vertex_id,
            "[SurfaceMesh::polygons_around_vertex] Wrong polygon "
            "around vertex" );
        geode::index_t safety_count{ 0 };
        constexpr geode::index_t MAX_SAFETY_COUNT{ 1000 };
        geode::internal::PolygonsAroundVertexImpl result;
        auto cur_polygon_vertex = first_polygon;
        do
        {
            OPENGEODE_ASSERT(
                mesh.polygon_vertex( cur_polygon_vertex.value() ) == vertex_id,
                "[SurfaceMesh::polygons_around_vertex] Wrong polygon "
                "around vertex" );
            result.polygons.push_back( cur_polygon_vertex.value() );
            const auto prev_vertex =
                mesh.previous_polygon_vertex( cur_polygon_vertex.value() );
            auto adj_edge =
                mesh.polygon_adjacent_edge( geode::PolygonEdge{ prev_vertex } );
            safety_count++;
            if( adj_edge )
            {
                cur_polygon_vertex = geode::PolygonVertex{ adj_edge.value() };
            }
            else
            {
                cur_polygon_vertex = std::nullopt;
                break;
            }
        } while( cur_polygon_vertex != first_polygon
                 && safety_count < MAX_SAFETY_COUNT );

        result.vertex_is_on_border = cur_polygon_vertex != first_polygon;
        if( result.vertex_is_on_border )
        {
            auto adj_edge = mesh.polygon_adjacent_edge(
                geode::PolygonEdge{ first_polygon.value() } );
            if( adj_edge )
            {
                cur_polygon_vertex = geode::PolygonVertex{ adj_edge.value() };
            }
            else
            {
                cur_polygon_vertex = std::nullopt;
            }
            while( cur_polygon_vertex && safety_count < MAX_SAFETY_COUNT )
            {
                const geode::PolygonVertex next_vertex{ mesh.next_polygon_edge(
                    geode::PolygonEdge{ cur_polygon_vertex.value() } ) };
                OPENGEODE_ASSERT(
                    mesh.polygon_vertex( next_vertex ) == vertex_id,
                    "[SurfaceMesh::polygons_around_vertex] Wrong polygon "
                    "around vertex" );
                result.polygons.push_back( next_vertex );
                safety_count++;
                adj_edge = mesh.polygon_adjacent_edge(
                    geode::PolygonEdge{ next_vertex } );
                if( adj_edge )
                {
                    cur_polygon_vertex =
                        geode::PolygonVertex{ adj_edge.value() };
                    continue;
                }
                cur_polygon_vertex = std::nullopt;
            }
        }
        if( safety_count >= MAX_SAFETY_COUNT )
        {
            output( mesh );
        }
        OPENGEODE_EXCEPTION( safety_count < MAX_SAFETY_COUNT,
            "[SurfaceMesh::polygons_around_vertex] Too many polygons "
            "around vertex ",
            vertex_id, " (", mesh.point( vertex_id ).string(),
            "). This is probably related to a bug in the polygon "
            "adjacencies." );
        return result;
    }
} // namespace

namespace geode
{
    PolygonVertex::PolygonVertex( const PolygonEdge& polygon_edge )
        : polygon_id( polygon_edge.polygon_id ),
          vertex_id( polygon_edge.edge_id )
    {
    }

    PolygonEdge::PolygonEdge( const PolygonVertex& polygon_vertex )
        : polygon_id( polygon_vertex.polygon_id ),
          edge_id( polygon_vertex.vertex_id )
    {
    }

    template < typename Archive >
    void PolygonVertex::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, PolygonVertex >{
                       { []( Archive& a, PolygonVertex& polygon_vertex ) {
                            a.value4b( polygon_vertex.polygon_id );
                            index_t value{ NO_ID };
                            a.value4b( value );
                            polygon_vertex.vertex_id = value;
                        },
                           []( Archive& a, PolygonVertex& polygon_vertex ) {
                               a.value4b( polygon_vertex.polygon_id );
                               a.value1b( polygon_vertex.vertex_id );
                           } } } );
    }

    template < typename Archive >
    void PolygonEdge::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, PolygonEdge >{
                       { []( Archive& a, PolygonEdge& polygon_edge ) {
                            a.value4b( polygon_edge.polygon_id );
                            index_t value{ NO_ID };
                            a.value4b( value );
                            polygon_edge.edge_id = value;
                        },
                           []( Archive& a, PolygonEdge& polygon_edge ) {
                               a.value4b( polygon_edge.polygon_id );
                               a.value1b( polygon_edge.edge_id );
                           } } } );
    }

    template < index_t dimension >
    class SurfaceMesh< dimension >::Impl
    {
        friend class bitsery::Access;
        using CachedPolygons =
            CachedValue< internal::PolygonsAroundVertexImpl >;
        static constexpr auto POLYGONS_AROUND_VERTEX_NAME =

            "polygons_around_vertex";

    public:
        Impl( SurfaceMesh& surface )
            : polygon_around_vertex_(
                surface.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        PolygonVertex >(
                        "polygon_around_vertex", PolygonVertex{} ) ),
              polygons_around_vertex_(
                  surface.vertex_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          CachedPolygons >(
                          POLYGONS_AROUND_VERTEX_NAME, CachedPolygons{} ) )
        {
        }

        VerticesAroundVertex vertices_around_vertex(
            const SurfaceMesh< dimension >& mesh, index_t vertex_id ) const
        {
            VerticesAroundVertex result;
            for( const auto& poly_vertex :
                mesh.polygons_around_vertex( vertex_id ) )
            {
                const auto next_candidate = mesh.polygon_vertex(
                    mesh.next_polygon_vertex( poly_vertex ) );
                if( absl::c_find( result, next_candidate ) == result.end() )
                {
                    result.push_back( next_candidate );
                }
                const auto previous_candidate = mesh.polygon_vertex(
                    mesh.previous_polygon_vertex( poly_vertex ) );
                if( absl::c_find( result, previous_candidate ) == result.end() )
                {
                    result.push_back( previous_candidate );
                }
            }
            return result;
        }

        std::optional< PolygonVertex > polygon_around_vertex(
            const index_t vertex_id ) const
        {
            const auto& value = polygon_around_vertex_->value( vertex_id );
            if( value.polygon_id != NO_ID )
            {
                return value;
            }
            return std::nullopt;
        }

        void reset_polygons_around_vertex( index_t vertex_id )
        {
            polygons_around_vertex_->modify_value(
                vertex_id, []( CachedPolygons& value ) {
                    value.reset();
                } );
        }

        const PolygonsAroundVertex& polygons_around_vertex(
            const SurfaceMesh< dimension >& mesh,
            index_t vertex_id,
            const std::optional< PolygonVertex >& first_polygon ) const
        {
            return updated_polygons_around_vertex(
                mesh, vertex_id, first_polygon )
                .polygons;
        }

        bool is_vertex_on_border( const SurfaceMesh< dimension >& mesh,
            index_t vertex_id,
            const std::optional< PolygonVertex >& first_polygon ) const
        {
            return updated_polygons_around_vertex(
                mesh, vertex_id, first_polygon )
                .vertex_is_on_border;
        }

        void associate_polygon_vertex_to_vertex(
            const PolygonVertex& polygon_vertex, const index_t vertex_id )
        {
            polygon_around_vertex_->set_value( vertex_id, polygon_vertex );
        }

        bool are_edges_enabled() const
        {
            return edges_.get() != nullptr;
        }

        void enable_edges( const SurfaceMesh< dimension >& surface ) const
        {
            if( !are_edges_enabled() )
            {
                edges_.reset( new SurfaceEdges< dimension >{ surface } );
            }
        }

        void copy_edges( const SurfaceMesh< dimension >& surface ) const
        {
            OPENGEODE_EXCEPTION( !are_edges_enabled(),
                "[SurfaceMesh] Cannot copy edges into "
                "mesh where edges are already enabled." );
            edges_.reset( new SurfaceEdges< dimension >{} );
            SurfaceEdgesBuilder< dimension > edges_builder{ *edges_ };
            edges_builder.copy( surface.edges() );
        }

        void disable_edges() const
        {
            edges_.reset();
        }

        const SurfaceEdges< dimension >& edges() const
        {
            OPENGEODE_EXCEPTION( are_edges_enabled(),
                "[SurfaceMesh] Edges should be "
                "enabled before accessing them" );
            return *edges_;
        }

        SurfaceEdges< dimension >& edges()
        {
            OPENGEODE_EXCEPTION( are_edges_enabled(),
                "[SurfaceMesh] Edges should be "
                "enabled before accessing them" );
            return *edges_;
        }

        AttributeManager& polygon_attribute_manager() const
        {
            return polygon_attribute_manager_;
        }

        TextureManager2D texture_manager() const
        {
            return { polygon_attribute_manager_, texture_storage_ };
        }

        void initialize_polygons_around_vertex(
            const SurfaceMesh< dimension >& surface )
        {
            polygons_around_vertex_ =
                surface.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        CachedPolygons >(
                        POLYGONS_AROUND_VERTEX_NAME, CachedPolygons{} );
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& a, Impl& impl ) {
                         a.object( impl.polygon_attribute_manager_ );
                         a.ext( impl.polygon_around_vertex_,
                             bitsery::ext::StdSmartPtr{} );
                         a.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                     },
                        []( Archive& a, Impl& impl ) {
                            a.object( impl.polygon_attribute_manager_ );
                            a.ext( impl.polygon_around_vertex_,
                                bitsery::ext::StdSmartPtr{} );
                            a.ext( impl.polygons_around_vertex_,
                                bitsery::ext::StdSmartPtr{} );
                            a.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                        },
                        []( Archive& a, Impl& impl ) {
                            a.object( impl.polygon_attribute_manager_ );
                            a.ext( impl.polygon_around_vertex_,
                                bitsery::ext::StdSmartPtr{} );
                            a.ext( impl.polygons_around_vertex_,
                                bitsery::ext::StdSmartPtr{} );
                            a.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                            a.object( impl.texture_storage_ );
                        } } } );
        }

        const internal::PolygonsAroundVertexImpl&
            updated_polygons_around_vertex(
                const SurfaceMesh< dimension >& mesh,
                const index_t vertex_id,
                const std::optional< PolygonVertex >& first_polygon ) const
        {
            const auto& cached = polygons_around_vertex_->value( vertex_id );
            const auto& polygons = cached.value().polygons;
            if( !cached.computed()
                || ( first_polygon
                     && absl::c_find( polygons, first_polygon.value() )
                            == polygons.end() ) )
            {
                cached( compute_polygons_around_vertex, mesh, vertex_id,
                    first_polygon );
            }
            return cached.value();
        }

    private:
        mutable AttributeManager polygon_attribute_manager_;
        std::shared_ptr< VariableAttribute< PolygonVertex > >
            polygon_around_vertex_;
        mutable std::shared_ptr< VariableAttribute< CachedPolygons > >
            polygons_around_vertex_;
        mutable std::unique_ptr< SurfaceEdges< dimension > > edges_;
        mutable TextureStorage2D texture_storage_;
    };

    template < index_t dimension >
    SurfaceMesh< dimension >::SurfaceMesh() : impl_( *this )
    {
    }

    template < index_t dimension >
    SurfaceMesh< dimension >::SurfaceMesh( SurfaceMesh&& ) noexcept = default;

    template < index_t dimension >
    SurfaceMesh< dimension >& SurfaceMesh< dimension >::operator=(
        SurfaceMesh&& ) noexcept = default;

    template < index_t dimension >
    SurfaceMesh< dimension >::~SurfaceMesh() = default;

    template < index_t dimension >
    std::unique_ptr< SurfaceMesh< dimension > >
        SurfaceMesh< dimension >::create()
    {
        return MeshFactory::create_default_mesh< SurfaceMesh< dimension > >(
            PolygonalSurface< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< SurfaceMesh< dimension > >
        SurfaceMesh< dimension >::create( const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< SurfaceMesh< dimension > >( impl );
    }

    template < index_t dimension >
    index_t SurfaceMesh< dimension >::polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        check_polygon_id( *this, polygon_vertex.polygon_id );
        check_polygon_vertex_id(
            *this, polygon_vertex.polygon_id, polygon_vertex.vertex_id );
        return get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    PolygonVertices SurfaceMesh< dimension >::polygon_vertices(
        index_t polygon_id ) const
    {
        const auto nb_vertices = nb_polygon_vertices( polygon_id );
        PolygonVertices vertices( nb_vertices );
        for( const auto v : LRange{ nb_vertices } )
        {
            vertices[v] = polygon_vertex( { polygon_id, v } );
        }
        return vertices;
    }

    template < index_t dimension >
    std::optional< local_index_t > SurfaceMesh< dimension >::vertex_in_polygon(
        index_t polygon_id, index_t vertex_id ) const
    {
        for( const auto v : LRange{ nb_polygon_vertices( polygon_id ) } )

        {
            if( polygon_vertex( { polygon_id, v } ) == vertex_id )
            {
                return v;
            }
        }
        return std::nullopt;
    }

    template < index_t dimension >
    auto SurfaceMesh< dimension >::vertices_around_vertex(
        index_t vertex_id ) const -> VerticesAroundVertex
    {
        return impl_->vertices_around_vertex( *this, vertex_id );
    }

    template < index_t dimension >
    std::optional< PolygonVertex >
        SurfaceMesh< dimension >::polygon_around_vertex(
            index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return get_polygon_around_vertex( vertex_id );
    }

    template < index_t dimension >
    std::optional< PolygonVertex >
        SurfaceMesh< dimension >::get_polygon_around_vertex(
            index_t vertex_id ) const
    {
        return impl_->polygon_around_vertex( vertex_id );
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::associate_polygon_vertex_to_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id, SurfaceMeshKey )
    {
        impl_->associate_polygon_vertex_to_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t SurfaceMesh< dimension >::nb_polygons() const
    {
        return polygon_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    bool SurfaceMesh< dimension >::is_vertex_isolated( index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return !get_polygon_around_vertex( vertex_id );
    }

    template < index_t dimension >
    local_index_t SurfaceMesh< dimension >::nb_polygon_vertices(
        index_t polygon_id ) const
    {
        check_polygon_id( *this, polygon_id );
        return get_nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    local_index_t SurfaceMesh< dimension >::nb_polygon_edges(
        index_t polygon_id ) const
    {
        return nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    PolygonVertex SurfaceMesh< dimension >::next_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        const auto vertex = polygon_vertex.vertex_id;
        const auto polygon = polygon_vertex.polygon_id;
        const auto nb_vertices = nb_polygon_vertices( polygon );
        const local_index_t next = vertex + 1 == nb_vertices ? 0 : vertex + 1;
        return { polygon, next };
    }

    template < index_t dimension >
    PolygonVertex SurfaceMesh< dimension >::previous_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        const auto vertex = polygon_vertex.vertex_id;
        const auto polygon = polygon_vertex.polygon_id;
        const auto nb_vertices = nb_polygon_vertices( polygon );
        const local_index_t prev = vertex == 0 ? nb_vertices - 1 : vertex - 1;
        return { polygon, prev };
    }

    template < index_t dimension >
    PolygonEdge SurfaceMesh< dimension >::next_polygon_edge(
        const PolygonEdge& polygon_edge ) const
    {
        const auto edge = polygon_edge.edge_id;
        const auto polygon = polygon_edge.polygon_id;
        const auto nb_vertices = nb_polygon_edges( polygon );
        const local_index_t next = edge + 1 == nb_vertices ? 0 : edge + 1;
        return { polygon, next };
    }

    template < index_t dimension >
    PolygonEdge SurfaceMesh< dimension >::previous_polygon_edge(
        const PolygonEdge& polygon_edge ) const
    {
        const auto edge = polygon_edge.edge_id;
        const auto polygon = polygon_edge.polygon_id;
        const auto nb_vertices = nb_polygon_edges( polygon );
        const local_index_t prev = edge == 0 ? nb_vertices - 1 : edge - 1;
        return { polygon, prev };
    }

    template < index_t dimension >
    std::optional< index_t > SurfaceMesh< dimension >::polygon_adjacent(
        const PolygonEdge& polygon_edge ) const
    {
        check_polygon_id( *this, polygon_edge.polygon_id );
        check_polygon_edge_id(
            *this, polygon_edge.polygon_id, polygon_edge.edge_id );
        return get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    std::optional< PolygonEdge >
        SurfaceMesh< dimension >::polygon_adjacent_edge(
            const PolygonEdge& polygon_edge ) const
    {
        const auto polygon_adj = polygon_adjacent( polygon_edge );
        if( !polygon_adj )
        {
            return std::nullopt;
        }
        const auto polygon_adj_id = polygon_adj.value();
        const auto v0 = polygon_vertex( PolygonVertex{ polygon_edge } );
        const auto v1 = polygon_edge_vertex( polygon_edge, 1 );
        const auto adj_vertices = polygon_vertices( polygon_adj_id );
        const auto nb_edges = adj_vertices.size();
        std::vector< PolygonEdge > failed_edges;
        for( const auto e : LRange{ nb_edges } )
        {
            const PolygonEdge polygon_adj_edge{ polygon_adj_id, e };
            if( v0 == adj_vertices[e] )
            {
                const auto enext = e == nb_edges - 1
                                       ? 0u
                                       : static_cast< local_index_t >( e + 1 );
                if( v1 == adj_vertices[enext] )
                {
                    if( polygon_adjacent( polygon_adj_edge )
                        == polygon_edge.polygon_id )
                    {
                        return polygon_adj_edge;
                    }
                    failed_edges.emplace_back( polygon_adj_edge );
                }
            }
            else if( v1 == adj_vertices[e] )
            {
                const auto enext = e == nb_edges - 1
                                       ? 0u
                                       : static_cast< local_index_t >( e + 1 );
                if( v0 == adj_vertices[enext] )
                {
                    if( polygon_adjacent( polygon_adj_edge )
                        == polygon_edge.polygon_id )
                    {
                        return polygon_adj_edge;
                    }
                    failed_edges.emplace_back( polygon_adj_edge );
                }
            }
        }
        if( failed_edges.empty() )
        {
            throw OpenGeodeException{
                "[SurfaceMesh::polygon_adjacent_edge] Wrong "
                "adjacency with polygons (different vertices): ",
                polygon_edge.string(), " and ", polygon_adj_id,
                " (v0 = ", this->point( v0 ).string(),
                ", v1 = ", this->point( v1 ).string(), ")"
            };
        }
        auto message = absl::StrCat( "[SurfaceMesh::polygon_adjacent_"
                                     "edge] Wrong adjacency with polygons "
                                     "(bijectivity): ",
            polygon_edge.string() );
        for( const auto& edge : failed_edges )
        {
            absl::StrAppend( &message, " and ", edge.string() );
        }
        throw OpenGeodeException{ message };
    }

    template < index_t dimension >
    bool SurfaceMesh< dimension >::is_edge_on_border(
        const PolygonEdge& polygon_edge ) const
    {
        return !polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    bool SurfaceMesh< dimension >::is_polygon_on_border(
        index_t polygon_id ) const
    {
        return !polygon_edges_on_border( polygon_id ).empty();
    }

    template < index_t dimension >
    PolygonEdgesOnBorder SurfaceMesh< dimension >::polygon_edges_on_border(
        index_t polygon_id ) const
    {
        PolygonEdgesOnBorder borders;
        for( const auto e : LRange{ nb_polygon_edges( polygon_id ) } )
        {
            PolygonEdge edge{ polygon_id, e };
            if( is_edge_on_border( edge ) )
            {
                borders.emplace_back( std::move( edge ) );
            }
        }
        return borders;
    }

    template < index_t dimension >
    PolygonEdge SurfaceMesh< dimension >::next_on_border(
        const PolygonEdge& polygon_edge ) const
    {
        OPENGEODE_EXCEPTION( is_edge_on_border( polygon_edge ),
            "[SurfaceMesh::next_on_border] Polygon edge should be on "
            "border" );
        auto next_border = next_polygon_edge( polygon_edge );
        while( !is_edge_on_border( next_border ) )
        {
            next_border = next_polygon_edge(
                polygon_adjacent_edge( next_border ).value() );
        }
        return next_border;
    }

    template < index_t dimension >
    PolygonEdge SurfaceMesh< dimension >::previous_on_border(
        const PolygonEdge& polygon_edge ) const
    {
        OPENGEODE_EXCEPTION( is_edge_on_border( polygon_edge ),
            "[SurfaceMesh::previous_on_border] Polygon edge should be "
            "on border" );
        auto previous_border = previous_polygon_edge( polygon_edge );
        while( !is_edge_on_border( previous_border ) )
        {
            previous_border = previous_polygon_edge(
                polygon_adjacent_edge( previous_border ).value() );
        }
        return previous_border;
    }

    template < index_t dimension >
    double SurfaceMesh< dimension >::edge_length(
        const PolygonEdge& polygon_edge ) const
    {
        return edge_length( polygon_edge_vertices( polygon_edge ) );
    }

    template < index_t dimension >
    double SurfaceMesh< dimension >::edge_length(
        const std::array< index_t, 2 >& polygon_edge_vertices ) const
    {
        return point_point_distance( this->point( polygon_edge_vertices[0] ),
            this->point( polygon_edge_vertices[1] ) );
    }

    template < index_t dimension >
    Point< dimension > SurfaceMesh< dimension >::edge_barycenter(
        const PolygonEdge& polygon_edge ) const
    {
        return edge_barycenter( polygon_edge_vertices( polygon_edge ) );
    }

    template < index_t dimension >
    Point< dimension > SurfaceMesh< dimension >::edge_barycenter(
        const std::array< index_t, 2 >& polygon_edge_vertices ) const
    {
        return ( this->point( polygon_edge_vertices[0] )
                   + this->point( polygon_edge_vertices[1] ) )
               / 2.;
    }

    template < index_t dimension >
    index_t SurfaceMesh< dimension >::polygon_edge_vertex(
        const PolygonEdge& polygon_edge, local_index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < 2, "[SurfaceMesh::polygon_"
                                         "edge_vertex] vertex_id should be "
                                         "0 or 1" );
        const auto edge = polygon_edge.edge_id;
        const auto polygon = polygon_edge.polygon_id;
        const auto nb_vertices = nb_polygon_vertices( polygon );
        const local_index_t vertex =
            edge + vertex_id == nb_vertices ? 0 : edge + vertex_id;
        return polygon_vertex( PolygonVertex{ polygon, vertex } );
    }

    template < index_t dimension >
    std::array< index_t, 2 > SurfaceMesh< dimension >::polygon_edge_vertices(
        const PolygonEdge& polygon_edge ) const
    {
        return { polygon_vertex( PolygonVertex{ polygon_edge } ),
            polygon_edge_vertex( polygon_edge, 1 ) };
    }

    template < index_t dimension >
    Point< dimension > SurfaceMesh< dimension >::polygon_barycenter(
        index_t polygon_id ) const
    {
        Point< dimension > barycenter;
        for( const auto v : LRange{ nb_polygon_vertices( polygon_id ) } )
        {
            barycenter =
                barycenter + this->point( polygon_vertex( { polygon_id, v } ) );
        }
        return barycenter / nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    const PolygonsAroundVertex&
        SurfaceMesh< dimension >::polygons_around_vertex(
            index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return impl_->polygons_around_vertex(
            *this, vertex_id, get_polygon_around_vertex( vertex_id ) );
    }

    template < index_t dimension >
    const PolygonsAroundVertex&
        SurfaceMesh< dimension >::polygons_around_vertex(
            const PolygonVertex& polygon_vertex ) const
    {
        check_polygon_vertex_id(
            *this, polygon_vertex.polygon_id, polygon_vertex.vertex_id );
        return impl_->polygons_around_vertex(
            *this, this->polygon_vertex( polygon_vertex ), polygon_vertex );
    }

    template < index_t dimension >
    bool SurfaceMesh< dimension >::is_vertex_on_border(
        index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return impl_->is_vertex_on_border(
            *this, vertex_id, get_polygon_around_vertex( vertex_id ) );
    }

    template < index_t dimension >
    std::optional< PolygonEdge >
        SurfaceMesh< dimension >::polygon_edge_from_vertices(
            index_t from_vertex_id, index_t to_vertex_id ) const
    {
        for( const auto& polygon_vertex :
            polygons_around_vertex( from_vertex_id ) )
        {
            const auto next_vertex = next_polygon_vertex( polygon_vertex );
            if( this->polygon_vertex( next_vertex ) == to_vertex_id )
            {
                return PolygonEdge{ polygon_vertex };
            }
        }
        return std::nullopt;
    }

    template < index_t dimension >
    PolygonsAroundEdge SurfaceMesh< dimension >::polygons_from_edge_vertices(
        absl::Span< const index_t > edge_vertices ) const
    {
        PolygonsAroundEdge polygons_around_edge;
        for( auto&& polygon_vertex :
            polygons_around_vertex( edge_vertices[0] ) )
        {
            const auto next_vertex = next_polygon_vertex( polygon_vertex );
            if( this->polygon_vertex( next_vertex ) == edge_vertices[1] )
            {
                polygons_around_edge.emplace_back(
                    std::move( polygon_vertex ) );
                continue;
            }
            auto previous_vertex = previous_polygon_vertex( polygon_vertex );
            if( this->polygon_vertex( previous_vertex ) == edge_vertices[1] )
            {
                polygons_around_edge.emplace_back(
                    std::move( previous_vertex ) );
            }
        }
        return polygons_around_edge;
    }

    template < index_t dimension >
    bool SurfaceMesh< dimension >::are_edges_enabled() const
    {
        return impl_->are_edges_enabled();
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::enable_edges() const
    {
        impl_->enable_edges( *this );
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::disable_edges() const
    {
        impl_->disable_edges();
    }

    template < index_t dimension >
    const SurfaceEdges< dimension >& SurfaceMesh< dimension >::edges() const
    {
        return impl_->edges();
    }

    template < index_t dimension >
    SurfaceEdges< dimension >& SurfaceMesh< dimension >::edges( SurfaceMeshKey )
    {
        return impl_->edges();
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::copy_edges(
        const SurfaceMesh< dimension >& surface_mesh, SurfaceMeshKey )
    {
        return impl_->copy_edges( surface_mesh );
    }

    template < index_t dimension >
    AttributeManager&
        SurfaceMesh< dimension >::polygon_attribute_manager() const
    {
        return impl_->polygon_attribute_manager();
    }

    template < index_t dimension >
    template < typename Archive >
    void SurfaceMesh< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, SurfaceMesh >{
                { []( Archive& a, SurfaceMesh& surface ) {
                     a.ext( surface, bitsery::ext::BaseClass< VertexSet >{} );
                     a.object( surface.impl_ );
                     surface.impl_->initialize_polygons_around_vertex(
                         surface );
                 },
                    []( Archive& a, SurfaceMesh& surface ) {
                        a.ext(
                            surface, bitsery::ext::BaseClass< VertexSet >{} );
                        a.object( surface.impl_ );
                    },
                    []( Archive& a, SurfaceMesh& surface ) {
                        a.ext(
                            surface, bitsery::ext::BaseClass< VertexSet >{} );
                        a.ext( surface, bitsery::ext::BaseClass<
                                            CoordinateReferenceSystemManagers<
                                                dimension > >{} );
                        a.object( surface.impl_ );
                    } } } );
    }

    template < index_t dimension >
    BoundingBox< dimension > SurfaceMesh< dimension >::bounding_box() const
    {
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( this->point( p ) );
        }
        return box;
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::reset_polygons_around_vertex(
        index_t vertex_id, SurfaceMeshKey )
    {
        impl_->reset_polygons_around_vertex( vertex_id );
    }

    template <>
    double SurfaceMesh< 2 >::polygon_area( index_t polygon_id ) const
    {
        if( nb_polygon_vertices( polygon_id ) < 3 )
        {
            return 0;
        }
        double area{ 0 };
        const auto vertices = polygon_vertices( polygon_id );
        const auto& p1 = this->point( vertices[0] );
        for( const auto i : LRange{ 1, vertices.size() - 1 } )
        {
            const auto& p2 = this->point( vertices[i] );
            const auto& p3 = this->point( vertices[i + 1] );
            area += triangle_signed_area( { p1, p2, p3 } );
        }
        return area;
    }

    template <>
    double SurfaceMesh< 3 >::polygon_area( index_t polygon_id ) const
    {
        if( nb_polygon_vertices( polygon_id ) < 3 )
        {
            return 0;
        }
        double area{ 0 };
        const auto direction =
            polygon_normal( polygon_id ).value_or( Vector3D{ { 0, 0, 1 } } );
        const auto vertices = polygon_vertices( polygon_id );
        const auto& p1 = this->point( vertices[0] );
        for( const auto i : LRange{ 1, vertices.size() - 1 } )
        {
            const auto& p2 = this->point( vertices[i] );
            const auto& p3 = this->point( vertices[i + 1] );
            area += triangle_signed_area( { p1, p2, p3 }, direction );
        }
        return area;
    }

    template < index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, std::optional< Vector3D > >::type
        SurfaceMesh< dimension >::polygon_normal( index_t polygon_id ) const
    {
        check_polygon_id( *this, polygon_id );
        Vector3D normal;
        const auto vertices = polygon_vertices( polygon_id );
        const auto& p0 = this->point( vertices[0] );
        for( const auto v : LRange{ 2, nb_polygon_vertices( polygon_id ) } )
        {
            const auto& p1 = this->point( vertices[v - 1] );
            const auto& p2 = this->point( vertices[v] );
            if( const auto triangle_normal =
                    Triangle< T >{ p0, p1, p2 }.normal() )
            {
                normal = normal + triangle_normal.value();
            }
        }
        try
        {
            return normal.normalize();
        }
        catch( const OpenGeodeException& /*unused*/ )
        {
            return std::nullopt;
        }
    }

    template < index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, std::optional< Vector3D > >::type
        SurfaceMesh< dimension >::polygon_vertex_normal(
            index_t vertex_id ) const
    {
        Vector3D normal;
        for( const auto& polygon : polygons_around_vertex( vertex_id ) )
        {
            if( const auto p_normal = polygon_normal( polygon.polygon_id ) )
            {
                normal = normal + p_normal.value();
            }
        }
        try
        {
            return normal.normalize();
        }
        catch( const OpenGeodeException& /*unused*/ )
        {
            return std::nullopt;
        }
    }

    template < index_t dimension >
    bool SurfaceMesh< dimension >::is_polygon_degenerated(
        index_t polygon_id ) const
    {
        double max_length{ 0. };
        local_index_t max_length_edge{ 0 };
        for( const auto e : LRange{ nb_polygon_edges( polygon_id ) } )
        {
            const auto cur_length = edge_length( PolygonEdge{ polygon_id, e } );
            if( cur_length > max_length )
            {
                max_length = cur_length;
                max_length_edge = e;
            }
        }
        if( max_length < GLOBAL_EPSILON )
        {
            return true;
        }
        const auto vertices = polygon_vertices( polygon_id );
        const auto next =
            max_length_edge + 1 == nb_polygon_vertices( polygon_id )
                ? 0
                : max_length_edge + 1;
        InfiniteLine< dimension > line{ Segment< dimension >{
            this->point( vertices[max_length_edge] ),
            this->point( vertices[next] ) } };
        for( const auto v : LIndices{ vertices } )
        {
            if( v == max_length_edge || v == next )
            {
                continue;
            }
            if( point_line_distance( this->point( vertices[v] ), line )
                > GLOBAL_EPSILON )
            {
                return false;
            }
        }
        return true;
    }

    template < index_t dimension >
    std::unique_ptr< SurfaceMesh< dimension > >
        SurfaceMesh< dimension >::clone() const
    {
        auto clone = create( impl_name() );
        auto builder = SurfaceMeshBuilder< dimension >::create( *clone );
        builder->copy_identifier( *this );
        builder->copy( *this );
        return clone;
    }

    template < index_t dimension >
    TextureManager2D SurfaceMesh< dimension >::texture_manager() const
    {
        return impl_->texture_manager();
    }

    template class opengeode_mesh_api SurfaceMesh< 2 >;
    template class opengeode_mesh_api SurfaceMesh< 3 >;

    template opengeode_mesh_api std::optional< Vector3D >
        SurfaceMesh< 3 >::polygon_normal< 3 >( index_t ) const;
    template opengeode_mesh_api std::optional< Vector3D >
        SurfaceMesh< 3 >::polygon_vertex_normal< 3 >( index_t ) const;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolygonVertex );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolygonEdge );

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SurfaceMesh< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SurfaceMesh< 3 > );
} // namespace geode
