/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <absl/container/flat_hash_set.h>
#include <absl/synchronization/mutex.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/cached_value.hpp>
#include <geode/basic/permutation.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/angle.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/mensuration.hpp>

#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/detail/facet_storage.hpp>
#include <geode/mesh/core/detail/legacy_attributes.hpp>
#include <geode/mesh/core/internal/surface_mesh_impl.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/texture2d.hpp>
#include <geode/mesh/core/texture_storage.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

namespace
{
    template < geode::index_t dimension >
    void check_vertex_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t vertex_id )
    {
        geode_unused( surface );
        geode_unused( vertex_id );
        geode::OpenGeodeMeshException::check_assertion(
            vertex_id < surface.nb_vertices(),
            "[check_vertex_id] Trying to access an invalid vertex" );
    }

    template < geode::index_t dimension >
    void check_polygon_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        geode::OpenGeodeMeshException::check_assertion(
            polygon_id < surface.nb_polygons(),
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
        geode::OpenGeodeMeshException::check_assertion(
            vertex_id < surface.nb_polygon_vertices( polygon_id ),
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
        geode::OpenGeodeMeshException::check_assertion(
            edge_id < surface.nb_polygon_edges( polygon_id ),
            "[check_polygon_edge_id] Trying to access an invalid polygon local "
            "edge" );
    }

    /*!
     * Return the other edge of the polygon sharing the given vertex with the
     * given edge. Works whatever the orientation of the polygon.
     */
    template < geode::index_t dimension >
    geode::PolygonEdge other_polygon_edge_around_vertex(
        const geode::SurfaceMesh< dimension >& mesh,
        const geode::PolygonEdge& edge,
        geode::index_t vertex_id )
    {
        if( mesh.polygon_vertex( geode::PolygonVertex{ edge } ) == vertex_id )
        {
            return mesh.previous_polygon_edge( edge );
        }
        return mesh.next_polygon_edge( edge );
    }

    template < geode::index_t dimension >
    std::optional< geode::PolygonVertex > next_polygon_vertex_around(
        const geode::SurfaceMesh< dimension >& mesh,
        const geode::PolygonVertex& cur_polygon_vertex,
        geode::index_t vertex_id,
        bool& vertex_is_next )
    {
        const auto exit_vertex =
            vertex_is_next ? cur_polygon_vertex
                           : mesh.previous_polygon_vertex( cur_polygon_vertex );
        const auto adj_edge =
            mesh.polygon_adjacent_edge( geode::PolygonEdge{ exit_vertex } );
        if( !adj_edge )
        {
            return std::nullopt;
        }
        // The adjacent edge contains vertex_id either as first or as second
        // vertex, whatever the orientation of the adjacent polygon
        const geode::PolygonVertex adj_vertex{ adj_edge.value() };
        vertex_is_next = mesh.polygon_vertex( adj_vertex ) != vertex_id;
        return vertex_is_next ? mesh.next_polygon_vertex( adj_vertex )
                              : adj_vertex;
    }

    /*!
     * Detects when turning around a vertex visits a PolygonVertex twice,
     * which can only happen with invalid polygon adjacencies.
     * Small fans are not checked (no overhead for usual meshes): a loop
     * makes the fan grow until it is checked.
     */
    class PolygonVertexLoopDetector
    {
    public:
        [[nodiscard]] bool is_new( const geode::PolygonsAroundVertex& visited,
            const geode::PolygonVertex& polygon_vertex )
        {
            if( visited.size() < MIN_FAN_SIZE_TO_CHECK )
            {
                return true;
            }
            if( visited_.empty() )
            {
                visited_.insert( visited.begin(), visited.end() );
            }
            return visited_.insert( polygon_vertex ).second;
        }

    private:
        static constexpr geode::index_t MIN_FAN_SIZE_TO_CHECK{ 64 };
        absl::flat_hash_set< geode::PolygonVertex > visited_;
    };

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
        geode::OpenGeodeMeshException::check_assertion(
            mesh.polygon_vertex( first_polygon.value() ) == vertex_id,
            "[SurfaceMesh::polygons_around_vertex] Wrong polygon "
            "around vertex" );
        const auto throw_loop = [&mesh, vertex_id] {
            throw geode::OpenGeodeMeshException{ mesh.point( vertex_id ),
                geode::OpenGeodeException::TYPE::data,
                "[SurfaceMesh::polygons_around_vertex] Surface ",
                mesh.name().value_or( mesh.id().string() ),
                ": Loop detected while turning around vertex ", vertex_id, " (",
                mesh.point( vertex_id ).string(),
                "). This is probably related to a bug in the polygon "
                "adjacencies." };
        };
        geode::internal::PolygonsAroundVertexImpl result;
        PolygonVertexLoopDetector loop_detector;
        auto cur_polygon_vertex = first_polygon;
        bool vertex_is_next{ false };
        do
        {
            geode::OpenGeodeMeshException::check_assertion(
                mesh.polygon_vertex( cur_polygon_vertex.value() ) == vertex_id,
                "[SurfaceMesh::polygons_around_vertex] Wrong polygon "
                "around vertex ",
                vertex_id, " / ", cur_polygon_vertex->string(), " ",
                mesh.polygon_vertex( cur_polygon_vertex.value() ) );
            result.polygons.push_back( cur_polygon_vertex.value() );
            cur_polygon_vertex = next_polygon_vertex_around(
                mesh, cur_polygon_vertex.value(), vertex_id, vertex_is_next );
            if( !cur_polygon_vertex )
            {
                break;
            }
            if( cur_polygon_vertex != first_polygon
                && !loop_detector.is_new(
                    result.polygons, cur_polygon_vertex.value() ) )
            {
                throw_loop();
            }
        } while( cur_polygon_vertex != first_polygon );

        result.vertex_is_on_border = cur_polygon_vertex != first_polygon;
        if( result.vertex_is_on_border )
        {
            vertex_is_next = true;
            cur_polygon_vertex = next_polygon_vertex_around(
                mesh, first_polygon.value(), vertex_id, vertex_is_next );
        }
        else
        {
            cur_polygon_vertex = std::nullopt;
        }
        while( cur_polygon_vertex )
        {
            geode::OpenGeodeMeshException::check_assertion(
                mesh.polygon_vertex( cur_polygon_vertex.value() ) == vertex_id,
                "[SurfaceMesh::polygons_around_vertex] Wrong polygon "
                "around vertex" );
            if( !loop_detector.is_new(
                    result.polygons, cur_polygon_vertex.value() ) )
            {
                throw_loop();
            }
            result.polygons.push_back( cur_polygon_vertex.value() );
            cur_polygon_vertex = next_polygon_vertex_around(
                mesh, cur_polygon_vertex.value(), vertex_id, vertex_is_next );
        }
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
    void PolygonVertex::serialize( Archive& serializer )
    {
        serializer.ext( *this,
            Growable< Archive, PolygonVertex >{
                { []( Archive& archive, PolygonVertex& polygon_vertex ) {
                     archive.value4b( polygon_vertex.polygon_id );
                     index_t value{ NO_ID };
                     archive.value4b( value );
                     polygon_vertex.vertex_id = value;
                 },
                    []( Archive& archive, PolygonVertex& polygon_vertex ) {
                        archive.value4b( polygon_vertex.polygon_id );
                        archive.value1b( polygon_vertex.vertex_id );
                    } } } );
    }

    template < typename Archive >
    void PolygonEdge::serialize( Archive& serializer )
    {
        serializer.ext(
            *this, Growable< Archive, PolygonEdge >{
                       { []( Archive& archive, PolygonEdge& polygon_edge ) {
                            archive.value4b( polygon_edge.polygon_id );
                            index_t value{ NO_ID };
                            archive.value4b( value );
                            polygon_edge.edge_id = value;
                        },
                           []( Archive& archive, PolygonEdge& polygon_edge ) {
                               archive.value4b( polygon_edge.polygon_id );
                               archive.value1b( polygon_edge.edge_id );
                           } } } );
    }

    template < index_t dimension >
    class SurfaceMesh< dimension >::Impl
    {
        friend class bitsery::Access;
        using CachedPolygons =
            CachedValue< internal::PolygonsAroundVertexImpl >;

    public:
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

        double polygon_minimum_height(
            const SurfaceMesh< dimension >& mesh, index_t polygon_id ) const
        {
            OpenGeodeMeshException::check_exception(
                polygon_id < mesh.nb_polygons(), nullptr,
                OpenGeodeException::TYPE::data,
                "[Impl::polygon_minimum_height] Wrong polygon id" );
            const auto polygon = mesh.polygon( polygon_id );
            return polygon.minimum_height();
        }

        std::optional< PolygonVertex > polygon_around_vertex(
            const index_t vertex_id ) const
        {
            const auto& value = polygon_around_vertex_[vertex_id];
            if( value.polygon_id != NO_ID )
            {
                return value;
            }
            return std::nullopt;
        }

        void reset_polygons_around_vertex( index_t vertex_id )
        {
            polygons_around_vertex_[vertex_id].reset();
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
            polygon_around_vertex_[vertex_id] = polygon_vertex;
        }

        bool are_edges_enabled() const
        {
            return edges_.get() != nullptr;
        }

        void enable_edges( const SurfaceMesh< dimension >& surface ) const
        {
            {
                if( are_edges_enabled() )
                {
                    return;
                }
            }
            edges_.reset( new SurfaceEdges< dimension >{ surface } );
        }

        void copy_edges( const SurfaceMesh< dimension >& from_surface ) const
        {
            OpenGeodeMeshException::check_exception( !are_edges_enabled(),
                nullptr, OpenGeodeException::TYPE::data,
                "[SurfaceMesh] Cannot copy edges into mesh where edges are "
                "already enabled." );
            edges_.reset( new SurfaceEdges< dimension >{} );
            SurfaceEdgesBuilder< dimension > edges_builder{ *edges_ };
            edges_builder.copy( from_surface.edges() );
        }

        void disable_edges() const
        {
            edges_.reset();
        }

        const SurfaceEdges< dimension >& edges() const
        {
            OpenGeodeMeshException::check_exception( are_edges_enabled(),
                nullptr, OpenGeodeException::TYPE::data,
                "[SurfaceMesh] Edges should be enabled before accessing them" );
            return *edges_;
        }

        SurfaceEdges< dimension >& edges()
        {
            OpenGeodeMeshException::check_exception( are_edges_enabled(),
                nullptr, OpenGeodeException::TYPE::data,
                "[SurfaceMesh] Edges should be enabled before accessing them" );
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

        void resize_vertex_storage( index_t nb_vertices )
        {
            polygon_around_vertex_.resize( nb_vertices );
            polygons_around_vertex_.resize( nb_vertices );
        }

        void delete_vertex_storage( const std::vector< bool >& to_delete )
        {
            delete_vector_elements( to_delete, polygon_around_vertex_ );
            delete_vector_elements( to_delete, polygons_around_vertex_ );
        }

        void permute_vertex_storage( absl::Span< const index_t > permutation )
        {
            permute( polygon_around_vertex_, permutation );
            permute( polygons_around_vertex_, permutation );
        }

        void remove_legacy_attributes( AttributeManager& manager )
        {
            legacy_attributes_.remove_from( manager );
        }

        Impl() = default;

    private:
        template < typename Archive >
        void deserialize_legacy_polygon_around_vertex( Archive& archive )
        {
            legacy_attributes_.deserialize( archive, polygon_around_vertex_ );
            polygons_around_vertex_.resize( polygon_around_vertex_.size() );
        }

        template < typename Archive >
        void deserialize_legacy_polygons_around_vertex( Archive& archive )
        {
            legacy_attributes_.skip< CachedPolygons >( archive );
        }

        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                         archive.object( impl.polygon_attribute_manager_ );
                         impl.deserialize_legacy_polygon_around_vertex(
                             archive );
                         archive.ext(
                             impl.edges_, bitsery::ext::StdSmartPtr{} );
                     },
                        []( Archive& archive, Impl& impl ) {
                            archive.object( impl.polygon_attribute_manager_ );
                            impl.deserialize_legacy_polygon_around_vertex(
                                archive );
                            impl.deserialize_legacy_polygons_around_vertex(
                                archive );
                            archive.ext(
                                impl.edges_, bitsery::ext::StdSmartPtr{} );
                        },
                        []( Archive& archive, Impl& impl ) {
                            archive.object( impl.polygon_attribute_manager_ );
                            impl.deserialize_legacy_polygon_around_vertex(
                                archive );
                            impl.deserialize_legacy_polygons_around_vertex(
                                archive );
                            archive.ext(
                                impl.edges_, bitsery::ext::StdSmartPtr{} );
                            archive.object( impl.texture_storage_ );
                        },
                        []( Archive& archive, Impl& impl ) {
                            archive.object( impl.polygon_attribute_manager_ );
                            impl.deserialize_legacy_polygon_around_vertex(
                                archive );
                            impl.deserialize_legacy_polygons_around_vertex(
                                archive );
                            archive.ext(
                                impl.edges_, bitsery::ext::StdSmartPtr{} );
                            archive.object( impl.texture_storage_ );
                        },
                        []( Archive& archive, Impl& impl ) {
                            archive.object( impl.polygon_attribute_manager_ );
                            archive.container( impl.polygon_around_vertex_,
                                impl.polygon_around_vertex_.max_size() );
                            impl.polygons_around_vertex_.resize(
                                impl.polygon_around_vertex_.size() );
                            archive.ext(
                                impl.edges_, bitsery::ext::StdSmartPtr{} );
                            archive.object( impl.texture_storage_ );
                        } } } );
        }

        const internal::PolygonsAroundVertexImpl&
            updated_polygons_around_vertex(
                const SurfaceMesh< dimension >& mesh,
                const index_t vertex_id,
                const std::optional< PolygonVertex >& first_polygon ) const
        {
            const auto& cached = polygons_around_vertex_[vertex_id];
            if( cached.computed() && first_polygon
                && absl::c_contains(
                    cached.value().polygons, first_polygon.value() ) )
            {
                return cached.value();
            }
            cached.reset();
            cached( compute_polygons_around_vertex, mesh, vertex_id,
                first_polygon );
            return cached.value();
        }

    private:
        mutable AttributeManager polygon_attribute_manager_;
        std::vector< PolygonVertex > polygon_around_vertex_;
        mutable std::vector< CachedPolygons > polygons_around_vertex_;
        mutable std::unique_ptr< SurfaceEdges< dimension > > edges_;
        mutable TextureStorage2D texture_storage_;
        detail::LegacyAttributes legacy_attributes_;
    };

    template < index_t dimension >
    SurfaceMesh< dimension >::SurfaceMesh() = default;

    template < index_t dimension >
    SurfaceMesh< dimension >::SurfaceMesh( BITSERY )
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
    double SurfaceMesh< dimension >::polygon_minimum_height(
        index_t polygon_id ) const
    {
        return impl_->polygon_minimum_height( *this, polygon_id );
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
        const PolygonVertex& polygon_vertex,
        index_t vertex_id,
        SurfaceMeshKey /*key*/ )
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
        check_polygon_id( *this, polygon_edge.polygon_id );
        check_polygon_edge_id(
            *this, polygon_edge.polygon_id, polygon_edge.edge_id );
        // Hot path of polygons_around_vertex: unchecked accessors only
        const auto polygon_adj = get_polygon_adjacent( polygon_edge );
        if( !polygon_adj )
        {
            return std::nullopt;
        }
        const auto polygon_adj_id = polygon_adj.value();
        const auto nb_vertices =
            get_nb_polygon_vertices( polygon_edge.polygon_id );
        const auto v0 = get_polygon_vertex( PolygonVertex{ polygon_edge } );
        const local_index_t next_vertex =
            polygon_edge.edge_id + 1 == nb_vertices ? 0
                                                    : polygon_edge.edge_id + 1;
        const auto v1 =
            get_polygon_vertex( { polygon_edge.polygon_id, next_vertex } );
        const auto nb_adj_edges = get_nb_polygon_vertices( polygon_adj_id );
        const auto adj_first_vertex =
            get_polygon_vertex( { polygon_adj_id, 0 } );
        auto adj_vertex = adj_first_vertex;
        absl::InlinedVector< PolygonEdge, 1 > failed_edges;
        for( const auto edge_id : LRange{ nb_adj_edges } )
        {
            const local_index_t enext =
                edge_id + 1 == nb_adj_edges ? 0 : edge_id + 1;
            const auto adj_next_vertex =
                enext == 0 ? adj_first_vertex
                           : get_polygon_vertex( { polygon_adj_id, enext } );
            if( ( v0 == adj_vertex && v1 == adj_next_vertex )
                || ( v1 == adj_vertex && v0 == adj_next_vertex ) )
            {
                const PolygonEdge polygon_adj_edge{ polygon_adj_id, edge_id };
                if( get_polygon_adjacent( polygon_adj_edge )
                    == polygon_edge.polygon_id )
                {
                    return polygon_adj_edge;
                }
                failed_edges.emplace_back( polygon_adj_edge );
            }
            adj_vertex = adj_next_vertex;
        }
        if( failed_edges.empty() )
        {
            throw OpenGeodeMeshException{ edge_barycenter(
                                              std::array{ v0, v1 } ),
                OpenGeodeException::TYPE::data,
                "[SurfaceMesh::polygon_adjacent_edge] Wrong "
                "adjacency with polygons (different vertices): ",
                polygon_edge.string(), " and ", polygon_adj_id,
                " (v0 = ", this->point( v0 ).string(),
                ", v1 = ", this->point( v1 ).string(), ")" };
        }
        auto message = absl::StrCat( "[SurfaceMesh::polygon_adjacent_"
                                     "edge] Wrong adjacency with polygons "
                                     "(bijectivity): ",
            polygon_edge.string() );
        for( const auto& edge : failed_edges )
        {
            absl::StrAppend( &message, " and ", edge.string() );
        }
        throw OpenGeodeMeshException{ edge_barycenter( std::array{ v0, v1 } ),
            OpenGeodeException::TYPE::data, message };
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
        OpenGeodeMeshException::check_exception(
            is_edge_on_border( polygon_edge ), edge_barycenter( polygon_edge ),
            OpenGeodeException::TYPE::data,
            "[SurfaceMesh::next_on_border] Polygon edge should be on border" );
        const auto vertex = polygon_edge_vertex( polygon_edge, 1 );
        auto next_border = next_polygon_edge( polygon_edge );
        while( !is_edge_on_border( next_border ) )
        {
            next_border = other_polygon_edge_around_vertex(
                *this, polygon_adjacent_edge( next_border ).value(), vertex );
        }
        return next_border;
    }

    template < index_t dimension >
    PolygonEdge SurfaceMesh< dimension >::previous_on_border(
        const PolygonEdge& polygon_edge ) const
    {
        OpenGeodeMeshException::check_exception(
            is_edge_on_border( polygon_edge ), edge_barycenter( polygon_edge ),
            OpenGeodeException::TYPE::data,
            "[SurfaceMesh::previous_on_border] Polygon edge should be on "
            "border" );
        const auto vertex = polygon_vertex( PolygonVertex{ polygon_edge } );
        auto previous_border = previous_polygon_edge( polygon_edge );
        while( !is_edge_on_border( previous_border ) )
        {
            previous_border = other_polygon_edge_around_vertex( *this,
                polygon_adjacent_edge( previous_border ).value(), vertex );
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
        OpenGeodeMeshException::check_assertion( vertex_id < 2,
            "[SurfaceMesh::polygon_edge_vertex] vertex_id should be 0 or 1" );
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
                return std::optional< PolygonEdge >{ std::in_place,
                    polygon_vertex };
            }
        }
        return std::nullopt;
    }

    template < index_t dimension >
    Segment< dimension > SurfaceMesh< dimension >::segment(
        const PolygonEdge& polygon_edge ) const
    {
        const auto vertices = polygon_edge_vertices( polygon_edge );
        return { this->point( vertices[0] ), this->point( vertices[1] ) };
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
    SurfaceEdges< dimension >& SurfaceMesh< dimension >::edges(
        SurfaceMeshKey /*key*/ )
    {
        return impl_->edges();
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::copy_edges(
        const SurfaceMesh< dimension >& surface_mesh, SurfaceMeshKey /*key*/ )
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
    void SurfaceMesh< dimension >::serialize( Archive& serializer )
    {
        serializer.ext( *this,
            Growable< Archive, SurfaceMesh >{
                { []( Archive& archive, SurfaceMesh& surface ) {
                     archive.ext(
                         surface, bitsery::ext::BaseClass< VertexSet >{} );
                     archive.object( surface.impl_ );
                     surface.impl_->remove_legacy_attributes(
                         surface.vertex_attribute_manager() );
                 },
                    []( Archive& archive, SurfaceMesh& surface ) {
                        archive.ext(
                            surface, bitsery::ext::BaseClass< VertexSet >{} );
                        archive.object( surface.impl_ );
                        surface.impl_->remove_legacy_attributes(
                            surface.vertex_attribute_manager() );
                    },
                    []( Archive& archive, SurfaceMesh& surface ) {
                        archive.ext(
                            surface, bitsery::ext::BaseClass< VertexSet >{} );
                        archive.ext(
                            surface, bitsery::ext::BaseClass<
                                         CoordinateReferenceSystemManagers<
                                             dimension > >{} );
                        archive.object( surface.impl_ );
                        surface.impl_->remove_legacy_attributes(
                            surface.vertex_attribute_manager() );
                    } } } );
    }

    template < index_t dimension >
    BoundingBox< dimension > SurfaceMesh< dimension >::bounding_box() const
    {
        OpenGeodeMeshException::check_exception( nb_vertices() != 0, nullptr,
            OpenGeodeException::TYPE::data,
            "[SurfaceMesh::bounding_box] Cannot return the bounding_box of an "
            "empty surface mesh." );
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( this->point( p ) );
        }
        return box;
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::resize_vertex_storage(
        index_t nb_vertices, SurfaceMeshKey /*key*/ )
    {
        impl_->resize_vertex_storage( nb_vertices );
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::delete_vertex_storage(
        const std::vector< bool >& to_delete, SurfaceMeshKey /*key*/ )
    {
        impl_->delete_vertex_storage( to_delete );
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::permute_vertex_storage(
        absl::Span< const index_t > permutation, SurfaceMeshKey /*key*/ )
    {
        impl_->permute_vertex_storage( permutation );
    }

    template < index_t dimension >
    void SurfaceMesh< dimension >::reset_polygons_around_vertex(
        index_t vertex_id, SurfaceMeshKey /*key*/ )
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
    Polygon< dimension > SurfaceMesh< dimension >::polygon(
        index_t polygon_id ) const
    {
        check_polygon_id( *this, polygon_id );
        std::vector< std::reference_wrapper< const Point< dimension > > >
            polygon;
        for( const auto vertex : this->polygon_vertices( polygon_id ) )
        {
            polygon.emplace_back( this->point( vertex ) );
        }
        return Polygon< dimension >{ std::move( polygon ) };
    }

    template < index_t dimension >
    template < index_t T >
    std::enable_if_t< T == 3, std::optional< Vector3D > >
        SurfaceMesh< dimension >::polygon_normal( index_t polygon_id ) const
    {
        check_polygon_id( *this, polygon_id );
        const auto polygon = this->polygon( polygon_id );
        return polygon.normal();
    }

    template < index_t dimension >
    template < index_t T >
    std::enable_if_t< T == 3, std::optional< Vector3D > >
        SurfaceMesh< dimension >::polygon_vertex_normal(
            index_t vertex_id ) const
    {
        Vector3D normal;
        for( const auto& polygon : polygons_around_vertex( vertex_id ) )
        {
            const auto polygon_id = polygon.polygon_id;
            if( const auto p_normal = polygon_normal( polygon_id ) )
            {
                const auto previous_vertex =
                    this->previous_polygon_vertex( polygon );
                const auto next_vertex = this->next_polygon_vertex( polygon );
                const Vector3D prev{ this->point( this->polygon_vertex(
                                         previous_vertex ) ),
                    this->point( vertex_id ) };
                const Vector3D next{ this->point(
                                         this->polygon_vertex( next_vertex ) ),
                    this->point( vertex_id ) };
                if( prev.length() <= GLOBAL_EPSILON
                    || next.length() <= GLOBAL_EPSILON )
                {
                    // Coincident points: no angle defined at this polygon
                    continue;
                }
                const auto angle = Angle::create_from_vectors( prev, next );
                normal = normal + p_normal.value() * angle.radians();
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
        check_polygon_id( *this, polygon_id );
        const auto polygon = this->polygon( polygon_id );
        return polygon.is_degenerated();
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

namespace std
{
    size_t hash< geode::PolygonVertex >::operator()(
        const geode::PolygonVertex& polygon_vertex ) const
    {
        return absl::HashOf(
            polygon_vertex.polygon_id, polygon_vertex.vertex_id );
    }

    size_t hash< geode::PolygonEdge >::operator()(
        const geode::PolygonEdge& polygon_edge ) const
    {
        return absl::HashOf( polygon_edge.polygon_id, polygon_edge.edge_id );
    }
} // namespace std
