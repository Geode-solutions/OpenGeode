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
#include <stack>

#include <geode/mesh/core/solid_mesh.hpp>

#include <stack>

#include <absl/container/flat_hash_set.h>
#include <absl/hash/hash.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/cached_value.hpp>
#include <geode/basic/detail/mapping_after_deletion.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/solid_edges_builder.hpp>
#include <geode/mesh/builder/solid_facets_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/internal/solid_mesh_impl.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_facets.hpp>
#include <geode/mesh/core/texture3d.hpp>
#include <geode/mesh/core/texture_storage.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

namespace
{
    template < geode::index_t dimension >
    void check_vertex_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < solid.nb_vertices(),
            "[check_vertex_id] Trying to access an invalid vertex" );
    }

    template < geode::index_t dimension >
    void check_facet_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t facet_id )
    {
        geode_unused( solid );
        geode_unused( facet_id );
        OPENGEODE_ASSERT( facet_id < solid.nb_facets(),
            "[check_facet_id] Trying to access an invalid facet" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        OPENGEODE_ASSERT( polyhedron_id < solid.nb_polyhedra(),
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_vertex_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT(
            vertex_id < solid.nb_polyhedron_vertices( polyhedron_id ),
            "[check_polyhedron_vertex_id] Trying to access an invalid "
            "polyhedron vertex" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t facet_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( facet_id );
        OPENGEODE_ASSERT(
            facet_id < solid.nb_polyhedron_facets( polyhedron_id ),
            "[check_polyhedron_facet_id] Trying to access an invalid "
            "polyhedron facet" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_vertex_id(
        const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::local_index_t facet_id,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( facet_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < solid.nb_polyhedron_facet_vertices(
                              { polyhedron_id, facet_id } ),
            "[check_polyhedron_facet_vertex_id] Trying to access an invalid "
            "polyhedron facet vertex" );
    }

    template < geode::index_t dimension >
    geode::local_index_t get_vertex_id_in_polyhedron_facet(
        const geode::SolidMesh< dimension >& solid,
        const geode::PolyhedronFacet& facet,
        geode::index_t vertex_id )
    {
        for( const auto v :
            geode::LRange{ solid.nb_polyhedron_facet_vertices( facet ) } )
        {
            if( solid.polyhedron_facet_vertex( { facet, v } ) == vertex_id )
            {
                return v;
            }
        }
        return geode::NO_LID;
    }

    template < geode::index_t dimension >
    std::tuple< geode::PolyhedraAroundEdge, bool > propagate_around_edge(
        const geode::SolidMesh< dimension >& solid,
        geode::PolyhedronFacet facet,
        const std::array< geode::index_t, 2 >& edge_vertices )
    {
        geode::PolyhedraAroundEdge result;
        const auto first_polyhedron = facet.polyhedron_id;
        geode::index_t safety_count{ 0 };
        constexpr geode::index_t MAX_SAFETY_COUNT{ 1000 };
        do
        {
            if( const auto adj = solid.polyhedron_adjacent_facet( facet ) )
            {
                const auto adj_facet = adj.value();
                result.push_back( adj_facet.polyhedron_id );
                for( const auto f : geode::LRange{ solid.nb_polyhedron_facets(
                         adj_facet.polyhedron_id ) } )
                {
                    if( adj_facet.facet_id == f )
                    {
                        continue;
                    }
                    if( solid.is_edge_in_polyhedron_facet(
                            { adj_facet.polyhedron_id, f }, edge_vertices ) )
                    {
                        facet = { adj_facet.polyhedron_id, f };
                        break;
                    }
                }
            }
            else
            {
                return std::make_tuple( std::move( result ), false );
            }
        } while( facet.polyhedron_id != first_polyhedron
                 && safety_count < MAX_SAFETY_COUNT );
        OPENGEODE_EXCEPTION( safety_count < MAX_SAFETY_COUNT,
            "[SolidMesh::propagate_around_edge] Too many polyhedra "
            "around edge ",
            edge_vertices[0], " ", edge_vertices[1], " (",
            solid.point( edge_vertices[0] ).string(), " ",
            solid.point( edge_vertices[1] ).string(),
            "). This is probably related to a bug in the polyhedron "
            "adjacencies." );
        return std::make_tuple( std::move( result ), true );
    }

    template < geode::index_t dimension >
    std::optional< geode::index_t > first_polyhedron_around_edge(
        const geode::SolidMesh< dimension >& solid,
        const std::array< geode::index_t, 2 >& vertices )
    {
        for( const auto vertex : vertices )
        {
            for( const auto& polyhedron :
                solid.polyhedra_around_vertex( vertex ) )
            {
                for( const auto& edge_vertices :
                    solid.polyhedron_edges_vertices(
                        polyhedron.polyhedron_id ) )
                {
                    if( vertices == edge_vertices
                        || ( vertices[0] == edge_vertices[1]
                             && vertices[1] == edge_vertices[0] ) )
                    {
                        return polyhedron.polyhedron_id;
                    }
                }
            }
        }
        return std::nullopt;
    }

    template < geode::index_t dimension >
    bool is_edge_on_facet_border( const geode::SolidMesh< dimension >& solid,
        const std::array< geode::index_t, 2 >& vertices,
        geode::index_t polyhedron )
    {
        for( const auto facet :
            solid.polyhedron_facets_on_border( polyhedron ) )
        {
            if( solid.is_edge_in_polyhedron_facet( facet, vertices ) )
            {
                return true;
            }
        }
        return false;
    }

    template < geode::index_t dimension >
    geode::internal::PolyhedraAroundVertexImpl compute_polyhedra_around_vertex(
        const geode::SolidMesh< dimension >& solid,
        const geode::index_t& vertex_id,
        const std::optional< geode::PolyhedronVertex >& first_polyhedron )
    {
        if( !first_polyhedron )
        {
            return {};
        }
        OPENGEODE_ASSERT(
            solid.polyhedron_vertex( first_polyhedron.value() ) == vertex_id,
            "[SolidMesh::compute_polyhedra_around_vertex] Wrong polyhedron "
            "around vertex" );
        geode::index_t safety_count{ 0 };
        constexpr geode::index_t MAX_SAFETY_COUNT{ 40000 };
        geode::internal::PolyhedraAroundVertexImpl result;
        result.vertex_is_on_border = false;
        absl::flat_hash_set< geode::index_t > polyhedra_visited;
        polyhedra_visited.reserve( 20 );
        std::stack< geode::PolyhedronVertex > S;
        S.push( first_polyhedron.value() );
        polyhedra_visited.insert( first_polyhedron->polyhedron_id );
        while( !S.empty() && safety_count < MAX_SAFETY_COUNT )
        {
            safety_count++;
            result.polyhedra.push_back( S.top() );
            const auto& polyhedron_vertex_id = result.polyhedra.back();
            S.pop();

            for( const auto& polyhedron_facet :
                solid.polyhedron_vertex_facets( polyhedron_vertex_id ) )
            {
                const auto adj_polyhedron =
                    solid.polyhedron_adjacent( polyhedron_facet );
                if( !adj_polyhedron )
                {
                    result.vertex_is_on_border = true;
                    continue;
                }
                const auto p_adj = adj_polyhedron.value();
                if( !polyhedra_visited.insert( p_adj ).second )
                {
                    continue;
                }
                if( const auto v_adj =
                        solid.vertex_in_polyhedron( p_adj, vertex_id ) )
                {
                    S.emplace( p_adj, v_adj.value() );
                }
            }
        }
        OPENGEODE_EXCEPTION( safety_count < MAX_SAFETY_COUNT,
            "[SolidMesh::compute_polyhedra_around_vertex] Too many polyhedra "
            "around vertex ",
            vertex_id, " (", solid.point( vertex_id ).string(),
            "). This is probably related to a bug in the polyhedra "
            "adjacencies." );
        return result;
    }

    template < geode::index_t dimension >
    std::optional< geode::PolyhedronFacet > polyhedron_facet_from_vertices(
        const geode::SolidMesh< dimension >& mesh,
        const geode::detail::VertexCycle< geode::PolyhedronFacetVertices >&
            vertices,
        geode::index_t polyhedron_id )
    {
        for( const auto f :
            geode::LRange{ mesh.nb_polyhedron_facets( polyhedron_id ) } )
        {
            std::optional< geode::PolyhedronFacet > facet{ std::in_place,
                polyhedron_id, f };
            geode::detail::VertexCycle< geode::PolyhedronFacetVertices >
                cur_vertices{ mesh.polyhedron_facet_vertices( facet.value() ) };
            if( vertices == cur_vertices )
            {
                return facet;
            }
        }
        return std::nullopt;
    }
} // namespace

namespace geode
{
    PolyhedronVertex::PolyhedronVertex( const PolyhedronFacet& facet )
        : polyhedron_id( facet.polyhedron_id ), vertex_id( facet.facet_id )
    {
    }

    PolyhedronFacet::PolyhedronFacet( const PolyhedronVertex& vertex )
        : polyhedron_id( vertex.polyhedron_id ), facet_id( vertex.vertex_id )
    {
    }

    template < typename Archive >
    void PolyhedronVertex::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, PolyhedronVertex >{
                { []( Archive& a, PolyhedronVertex& polyhedron_vertex ) {
                     a.value4b( polyhedron_vertex.polyhedron_id );
                     index_t value{ NO_ID };
                     a.value4b( value );
                     polyhedron_vertex.vertex_id = value;
                 },
                    []( Archive& a, PolyhedronVertex& polyhedron_vertex ) {
                        a.value4b( polyhedron_vertex.polyhedron_id );
                        a.value1b( polyhedron_vertex.vertex_id );
                    } } } );
    }

    template < typename Archive >
    void PolyhedronFacet::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, PolyhedronFacet >{
                       { []( Archive& a, PolyhedronFacet& polyhedron_facet ) {
                            a.value4b( polyhedron_facet.polyhedron_id );
                            index_t value{ NO_ID };
                            a.value4b( value );
                            polyhedron_facet.facet_id = value;
                        },
                           []( Archive& a, PolyhedronFacet& polyhedron_facet ) {
                               a.value4b( polyhedron_facet.polyhedron_id );
                               a.value1b( polyhedron_facet.facet_id );
                           } } } );
    }

    template < typename Archive >
    void PolyhedronFacetVertex::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, PolyhedronFacetVertex >{
                { []( Archive& a,
                      PolyhedronFacetVertex& polyhedron_facet_vertex ) {
                     a.object( polyhedron_facet_vertex.polyhedron_facet );
                     index_t value{ NO_ID };
                     a.value4b( value );
                     polyhedron_facet_vertex.vertex_id = value;
                 },
                    []( Archive& a,
                        PolyhedronFacetVertex& polyhedron_facet_vertex ) {
                        a.object( polyhedron_facet_vertex.polyhedron_facet );
                        a.value1b( polyhedron_facet_vertex.vertex_id );
                    } } } );
    }

    template < typename Archive >
    void PolyhedronFacetEdge::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, PolyhedronFacetEdge >{
                { []( Archive& a, PolyhedronFacetEdge& polyhedron_facet_edge ) {
                     a.object( polyhedron_facet_edge.polyhedron_facet );
                     index_t value{ NO_ID };
                     a.value4b( value );
                     polyhedron_facet_edge.edge_id = value;
                 },
                    []( Archive& a,
                        PolyhedronFacetEdge& polyhedron_facet_edge ) {
                        a.object( polyhedron_facet_edge.polyhedron_facet );
                        a.value1b( polyhedron_facet_edge.edge_id );
                    } } } );
    }

    template < index_t dimension >
    class SolidMesh< dimension >::Impl
    {
        friend class bitsery::Access;
        using CachedPolyhedra =
            CachedValue< internal::PolyhedraAroundVertexImpl >;
        static constexpr auto POLYHEDRA_AROUND_VERTEX_NAME =

            "polyhedra_around_vertex";

    public:
        explicit Impl( SolidMesh& solid )
            : polyhedron_around_vertex_( solid.vertex_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          PolyhedronVertex >( "polyhedron_around_vertex",
                          PolyhedronVertex{},
                          { false, false, false } ) ),
              polyhedra_around_vertex_( solid.vertex_attribute_manager()
                      .template find_or_create_attribute< VariableAttribute,
                          CachedPolyhedra >( POLYHEDRA_AROUND_VERTEX_NAME,
                          CachedPolyhedra{},
                          { false, false, false } ) )
        {
        }

        VerticesAroundVertex vertices_around_vertex(
            const SolidMesh< dimension >& mesh, index_t vertex_id ) const
        {
            VerticesAroundVertex result;
            for( const auto& poly_vertex :
                mesh.polyhedra_around_vertex( vertex_id ) )
            {
                for( const auto& poly_edge : mesh.polyhedron_edges_vertices(
                         poly_vertex.polyhedron_id ) )
                {
                    if( poly_edge[0] == vertex_id )
                    {
                        if( absl::c_find( result, poly_edge[1] )
                            == result.end() )
                        {
                            result.push_back( poly_edge[1] );
                        }
                    }
                    else if( poly_edge[1] == vertex_id )
                    {
                        if( absl::c_find( result, poly_edge[0] )
                            == result.end() )
                        {
                            result.push_back( poly_edge[0] );
                        }
                    }
                }
            }
            return result;
        }

        std::optional< PolyhedronVertex > polyhedron_around_vertex(
            index_t vertex_id ) const
        {
            const auto& value = polyhedron_around_vertex_->value( vertex_id );
            if( value.polyhedron_id != NO_ID )
            {
                return value;
            }
            return std::nullopt;
        }

        double polyhedron_minimum_height(
            const geode::SolidMesh< dimension >& mesh,
            const index_t polyhedron_id ) const
        {
            double max_area{ 0. };
            local_index_t max_area_facet{ 0 };
            for( const auto f :
                LRange{ mesh.nb_polyhedron_facets( polyhedron_id ) } )
            {
                const auto cur_area =
                    mesh.polyhedron_facet_area( { polyhedron_id, f } );
                if( cur_area > max_area )
                {
                    max_area = cur_area;
                    max_area_facet = f;
                }
            }
            const auto vertices = mesh.polyhedron_vertices( polyhedron_id );
            const auto normal = mesh.polyhedron_facet_normal(
                { polyhedron_id, max_area_facet } );
            if( !normal )
            {
                return true;
            }
            const auto facet_vertices = mesh.polyhedron_facet_vertices(
                { polyhedron_id, max_area_facet } );
            Plane plane{ normal.value(), mesh.point( facet_vertices[0] ) };
            auto opposite_vertex{ 0 };
            for( const auto vertex_id : vertices )
            {
                if( absl::c_contains( facet_vertices, vertex_id ) )
                {
                    continue;
                }
                opposite_vertex = vertex_id;
            }
            return std::get< 0 >(
                point_plane_distance( mesh.point( opposite_vertex ), plane ) );
        }

        void reset_polyhedra_around_vertex( index_t vertex_id )
        {
            polyhedra_around_vertex_->modify_value(
                vertex_id, []( CachedPolyhedra& value ) {
                    value.reset();
                } );
        }

        const PolyhedraAroundVertex& polyhedra_around_vertex(
            const SolidMesh< dimension >& mesh,
            index_t vertex_id,
            const std::optional< PolyhedronVertex >& first_polyhedron ) const
        {
            return updated_polyhedra_around_vertex(
                mesh, vertex_id, first_polyhedron )
                .polyhedra;
        }

        bool is_vertex_on_border( const SolidMesh< dimension >& mesh,
            index_t vertex_id,
            const std::optional< PolyhedronVertex >& first_polyhedron ) const
        {
            return updated_polyhedra_around_vertex(
                mesh, vertex_id, first_polyhedron )
                .vertex_is_on_border;
        }

        void associate_polyhedron_vertex_to_vertex(
            const PolyhedronVertex& polyhedron_vertex, const index_t vertex_id )
        {
            polyhedron_around_vertex_->set_value(
                vertex_id, polyhedron_vertex );
        }

        AttributeManager& polyhedron_attribute_manager() const
        {
            return polyhedron_attribute_manager_;
        }

        TextureManager3D texture_manager() const
        {
            return { polyhedron_attribute_manager_, texture_storage_ };
        }

        bool are_edges_enabled() const
        {
            return edges_.get() != nullptr;
        }

        void enable_edges( const SolidMesh< dimension >& solid ) const
        {
            if( !are_edges_enabled() )
            {
                edges_.reset( new SolidEdges< dimension >{ solid } );
            }
        }

        void copy_edges( const SolidMesh< dimension >& solid )
        {
            OPENGEODE_EXCEPTION( !are_edges_enabled(),
                "[SolidMesh] Cannot copy edges into mesh "
                "where edges are already enabled." );
            edges_.reset( new SolidEdges< dimension >{} );
            SolidEdgesBuilder< dimension > edges_builder{ *edges_ };
            edges_builder.copy( solid.edges() );
        }

        void disable_edges() const
        {
            edges_.reset();
        }

        const SolidEdges< dimension >& edges() const
        {
            OPENGEODE_EXCEPTION( are_edges_enabled(),
                "[SolidMesh::edges] Edges should be "
                "enabled before accessing them" );
            return *edges_;
        }

        SolidEdges< dimension >& edges()
        {
            OPENGEODE_EXCEPTION( are_edges_enabled(),
                "[SolidMesh::edges] Edges should be "
                "enabled before accessing them" );
            return *edges_;
        }

        bool are_facets_enabled() const
        {
            return facets_.get() != nullptr;
        }

        void enable_facets( const SolidMesh< dimension >& solid ) const
        {
            if( !are_facets_enabled() )
            {
                facets_.reset( new SolidFacets< dimension >{ solid } );
            }
        }

        void copy_facets( const SolidMesh< dimension >& solid )
        {
            OPENGEODE_EXCEPTION( !are_facets_enabled(),
                "[SolidMesh] Cannot copy facets into mesh where facets are "
                "already enabled." );
            facets_.reset( new SolidFacets< dimension >{} );
            SolidFacetsBuilder< dimension > facets_builder{ *facets_ };
            facets_builder.copy( solid.facets() );
        }

        void disable_facets() const
        {
            facets_.reset();
        }

        const SolidFacets< dimension >& facets() const
        {
            OPENGEODE_EXCEPTION( are_facets_enabled(),
                "[SolidMesh::facets] Facets should be "
                "enabled before accessing them" );
            return *facets_;
        }

        SolidFacets< dimension >& facets()
        {
            OPENGEODE_EXCEPTION( are_facets_enabled(),
                "[SolidMesh::facets] Facets should be "
                "enabled before accessing them" );
            return *facets_;
        }

        void initialize_polyhedra_around_vertex(
            const SolidMesh< dimension >& solid )
        {
            polyhedra_around_vertex_ =
                solid.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        CachedPolyhedra >(
                        POLYHEDRA_AROUND_VERTEX_NAME, CachedPolyhedra{} );
        }

        const internal::PolyhedraAroundVertexImpl&
            updated_polyhedra_around_vertex( const SolidMesh< dimension >& mesh,
                const index_t vertex_id,
                const std::optional< PolyhedronVertex >& first_polyhedron )
                const
        {
            const auto& cached = polyhedra_around_vertex_->value( vertex_id );
            const auto& polyhedra = cached.value().polyhedra;
            if( !cached.computed()
                || ( first_polyhedron
                     && absl::c_find( polyhedra, first_polyhedron.value() )
                            == polyhedra.end() ) )
            {
                cached( compute_polyhedra_around_vertex, mesh, vertex_id,
                    first_polyhedron );
            }
            return cached.value();
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive,
                    Impl >{ { []( Archive& a, Impl& impl ) {
                                 a.object( impl.polyhedron_attribute_manager_ );
                                 a.ext( impl.polyhedron_around_vertex_,
                                     bitsery::ext::StdSmartPtr{} );
                                 a.ext(
                                     impl.edges_, bitsery::ext::StdSmartPtr{} );
                                 a.ext( impl.facets_,
                                     bitsery::ext::StdSmartPtr{} );
                                 const auto&
                                     old_polyhedron_around_vertex_properties =
                                         impl.polyhedron_around_vertex_
                                             ->properties();
                                 impl.polyhedron_around_vertex_->set_properties(
                                     { old_polyhedron_around_vertex_properties
                                             .assignable,
                                         old_polyhedron_around_vertex_properties
                                             .interpolable,
                                         false } );
                                 const auto&
                                     old_polyhedra_around_vertex_properties =
                                         impl.polyhedra_around_vertex_
                                             ->properties();
                                 impl.polyhedra_around_vertex_->set_properties(
                                     { old_polyhedra_around_vertex_properties
                                             .assignable,
                                         old_polyhedra_around_vertex_properties
                                             .interpolable,
                                         false } );
                             },
                    []( Archive& a, Impl& impl ) {
                        a.object( impl.polyhedron_attribute_manager_ );
                        a.ext( impl.polyhedron_around_vertex_,
                            bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.polyhedra_around_vertex_,
                            bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.facets_, bitsery::ext::StdSmartPtr{} );
                        const auto& old_polyhedron_around_vertex_properties =
                            impl.polyhedron_around_vertex_->properties();
                        impl.polyhedron_around_vertex_->set_properties(
                            { old_polyhedron_around_vertex_properties
                                    .assignable,
                                old_polyhedron_around_vertex_properties
                                    .interpolable,
                                false } );
                        const auto& old_polyhedra_around_vertex_properties =
                            impl.polyhedra_around_vertex_->properties();
                        impl.polyhedra_around_vertex_->set_properties(
                            { old_polyhedra_around_vertex_properties.assignable,
                                old_polyhedra_around_vertex_properties
                                    .interpolable,
                                false } );
                    },
                    []( Archive& a, Impl& impl ) {
                        a.object( impl.polyhedron_attribute_manager_ );
                        a.ext( impl.polyhedron_around_vertex_,
                            bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.polyhedra_around_vertex_,
                            bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.facets_, bitsery::ext::StdSmartPtr{} );
                        a.object( impl.texture_storage_ );
                        const auto& old_polyhedron_around_vertex_properties =
                            impl.polyhedron_around_vertex_->properties();
                        impl.polyhedron_around_vertex_->set_properties(
                            { old_polyhedron_around_vertex_properties
                                    .assignable,
                                old_polyhedron_around_vertex_properties
                                    .interpolable,
                                false } );
                        const auto& old_polyhedra_around_vertex_properties =
                            impl.polyhedra_around_vertex_->properties();
                        impl.polyhedra_around_vertex_->set_properties(
                            { old_polyhedra_around_vertex_properties.assignable,
                                old_polyhedra_around_vertex_properties
                                    .interpolable,
                                false } );
                    },
                    []( Archive& a, Impl& impl ) {
                        a.object( impl.polyhedron_attribute_manager_ );
                        a.ext( impl.polyhedron_around_vertex_,
                            bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.polyhedra_around_vertex_,
                            bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                        a.ext( impl.facets_, bitsery::ext::StdSmartPtr{} );
                        a.object( impl.texture_storage_ );
                    } } } );
        }

    private:
        mutable AttributeManager polyhedron_attribute_manager_;
        std::shared_ptr< VariableAttribute< PolyhedronVertex > >
            polyhedron_around_vertex_;
        mutable std::shared_ptr< VariableAttribute< CachedPolyhedra > >
            polyhedra_around_vertex_;
        mutable std::unique_ptr< SolidEdges< dimension > > edges_;
        mutable std::unique_ptr< SolidFacets< dimension > > facets_;
        mutable TextureStorage3D texture_storage_;
    };

    template < index_t dimension >
    SolidMesh< dimension >::SolidMesh() : impl_( *this )
    {
    }

    template < index_t dimension >
    SolidMesh< dimension >::SolidMesh( SolidMesh&& ) noexcept = default;

    template < index_t dimension >
    SolidMesh< dimension >& SolidMesh< dimension >::operator=(
        SolidMesh&& ) noexcept = default;

    template < index_t dimension >
    SolidMesh< dimension >::~SolidMesh() = default;

    template < index_t dimension >
    std::unique_ptr< SolidMesh< dimension > > SolidMesh< dimension >::create()
    {
        return MeshFactory::create_default_mesh< SolidMesh< dimension > >(
            PolyhedralSolid< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< SolidMesh< dimension > > SolidMesh< dimension >::create(
        const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< SolidMesh< dimension > >( impl );
    }

    template < index_t dimension >
    double SolidMesh< dimension >::edge_length(
        const std::array< index_t, 2 >& edge_vertices ) const
    {
        return Vector< dimension >{ this->point( edge_vertices[0] ),
            this->point( edge_vertices[1] ) }
            .length();
    }

    template < index_t dimension >
    index_t SolidMesh< dimension >::polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        check_polyhedron_id( *this, polyhedron_vertex.polyhedron_id );
        check_polyhedron_vertex_id( *this, polyhedron_vertex.polyhedron_id,
            polyhedron_vertex.vertex_id );
        return get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    PolyhedronVertices SolidMesh< dimension >::polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        const auto nb_vertices = nb_polyhedron_vertices( polyhedron_id );
        PolyhedronVertices vertices( nb_vertices );
        for( const auto v : LRange{ nb_vertices } )
        {
            vertices[v] = polyhedron_vertex( { polyhedron_id, v } );
        }
        return vertices;
    }

    template < index_t dimension >
    std::optional< local_index_t > SolidMesh< dimension >::vertex_in_polyhedron(
        index_t polyhedron_id, index_t vertex_id ) const
    {
        for( const auto v : LRange{ nb_polyhedron_vertices( polyhedron_id ) } )

        {
            if( polyhedron_vertex( { polyhedron_id, v } ) == vertex_id )
            {
                return v;
            }
        }
        return std::nullopt;
    }

    template < index_t dimension >
    Point< dimension > SolidMesh< dimension >::polyhedron_barycenter(
        index_t polyhedron_id ) const
    {
        Point< dimension > barycenter;
        for( const auto v : LRange{ nb_polyhedron_vertices( polyhedron_id ) } )
        {
            barycenter =
                barycenter
                + this->point( polyhedron_vertex( { polyhedron_id, v } ) );
        }
        return barycenter / nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    Point< dimension > SolidMesh< dimension >::facet_barycenter(
        const PolyhedronFacetVertices& facet_vertices ) const
    {
        Point< dimension > barycenter;
        for( const auto v : facet_vertices )
        {
            barycenter = barycenter + this->point( v );
        }
        return barycenter / static_cast< double >( facet_vertices.size() );
    }

    template < index_t dimension >
    Point< dimension > SolidMesh< dimension >::edge_barycenter(
        const std::array< index_t, 2 >& edge_vertices ) const
    {
        return ( this->point( edge_vertices[0] )
                   + this->point( edge_vertices[1] ) )
               / 2.;
    }

    template < index_t dimension >
    PolyhedronVertex SolidMesh< dimension >::polyhedron_facet_vertex_id(
        const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
    }

    template < index_t dimension >
    PolyhedronFacetVertices SolidMesh< dimension >::polyhedron_facet_vertices(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        PolyhedronFacetVertices vertices;
        for( const auto v :
            LRange{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
        {
            vertices.push_back(
                polyhedron_facet_vertex( { polyhedron_facet, v } ) );
        }
        return vertices;
    }

    template < index_t dimension >
    double SolidMesh< dimension >::polyhedron_volume(
        index_t polyhedron_id ) const
    {
        if( nb_polyhedron_vertices( polyhedron_id ) < 4 )
        {
            return 0;
        }
        double volume{ 0 };
        const auto first_pt_index = polyhedron_vertex( { polyhedron_id, 0 } );
        const auto& p0 = this->point( first_pt_index );
        const auto facets_vertices =
            polyhedron_facets_vertices( polyhedron_id );
        for( const auto facet_id : LIndices{ facets_vertices } )
        {
            const auto& facet_vertices = facets_vertices[facet_id];
            if( absl::c_any_of(
                    facet_vertices, [first_pt_index]( index_t vertex_id ) {
                        return vertex_id == first_pt_index;
                    } ) )
            {
                continue;
            }
            for( const auto i : LRange{ 2, facet_vertices.size() } )
            {
                const auto& p1 = this->point( facet_vertices[i - 2] );
                const auto& p2 = this->point( facet_vertices[i - 1] );
                const auto& p3 = this->point( facet_vertices[i] );
                volume += tetrahedron_signed_volume( { p1, p2, p3, p0 } );
            }
        }
        return volume;
    }

    template < index_t dimension >
    double SolidMesh< dimension >::polyhedron_facet_area(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        if( nb_polyhedron_facet_vertices( polyhedron_facet ) < 3 )
        {
            return 0;
        }
        double area{ 0 };
        const auto direction = polyhedron_facet_normal( polyhedron_facet )
                                   .value_or( Vector3D{ { 0, 0, 1 } } );
        const auto vertices = polyhedron_facet_vertices( polyhedron_facet );
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
    std::optional< Vector3D > SolidMesh< dimension >::polyhedron_facet_normal(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        Vector3D normal;
        const auto facet_vertices =
            polyhedron_facet_vertices( polyhedron_facet );
        const auto& p0 = this->point( facet_vertices[0] );
        for( const auto v :
            LRange{ 2, nb_polyhedron_facet_vertices( polyhedron_facet ) } )
        {
            const auto& p1 = this->point( facet_vertices[v - 1] );
            const auto& p2 = this->point( facet_vertices[v] );
            if( const auto triangle_normal = Triangle3D{ p0, p1, p2 }.normal() )
            {
                normal += triangle_normal.value();
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
    bool SolidMesh< dimension >::is_polyhedron_degenerated(
        index_t polyhedron_id ) const
    {
        return polyhedron_minimum_height( polyhedron_id ) < GLOBAL_EPSILON;
    }

    template < index_t dimension >
    index_t SolidMesh< dimension >::nb_polyhedra() const
    {
        return polyhedron_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::is_vertex_isolated( index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return !get_polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    std::optional< PolyhedronFacet >
        SolidMesh< dimension >::polyhedron_facet_from_vertices(
            PolyhedronFacetVertices facet_vertices ) const
    {
        detail::VertexCycle< PolyhedronFacetVertices > vertices{ std::move(
            facet_vertices ) };
        for( const auto& polyhedron_vertex :
            polyhedra_around_vertex( vertices.vertices()[0] ) )
        {
            if( auto result = ::polyhedron_facet_from_vertices(
                    *this, vertices, polyhedron_vertex.polyhedron_id ) )
            {
                return result;
            }
        }
        return std::nullopt;
    }

    template < index_t dimension >
    std::optional< PolyhedronFacet >
        SolidMesh< dimension >::polyhedron_facet_from_vertices(
            PolyhedronFacetVertices facet_vertices,
            index_t polyhedron_id ) const
    {
        detail::VertexCycle< PolyhedronFacetVertices > vertices{ std::move(
            facet_vertices ) };
        return ::polyhedron_facet_from_vertices(
            *this, vertices, polyhedron_id );
    }

    template < index_t dimension >
    std::optional< PolyhedronFacetEdge >
        SolidMesh< dimension >::polyhedron_facet_edge_from_vertices(
            const std::array< index_t, 2 >& edge_vertices,
            index_t polyhedron_id ) const
    {
        const auto facet_vertices = polyhedron_facets_vertices( polyhedron_id );
        for( const auto f : LIndices{ facet_vertices } )
        {
            const auto& vertices = facet_vertices[f];
            for( const auto v : LIndices{ vertices } )
            {
                if( vertices[v] == edge_vertices[0] )
                {
                    const auto next = v == vertices.size() - 1 ? 0 : v + 1;
                    if( vertices[next] == edge_vertices[1] )
                    {
                        return std::optional< PolyhedronFacetEdge >{
                            std::in_place, PolyhedronFacet{ polyhedron_id, f },
                            v
                        };
                    }
                }
                else if( vertices[v] == edge_vertices[1] )
                {
                    const auto next = v == vertices.size() - 1 ? 0 : v + 1;
                    if( vertices[next] == edge_vertices[0] )
                    {
                        return std::optional< PolyhedronFacetEdge >{
                            std::in_place, PolyhedronFacet{ polyhedron_id, f },
                            v
                        };
                    }
                }
            }
        }
        return std::nullopt;
    }

    template < index_t dimension >
    std::optional< PolyhedronFacetEdge >
        SolidMesh< dimension >::polyhedron_facet_edge_from_vertices(
            const std::array< index_t, 2 >& edge_vertices ) const
    {
        for( const auto& polyhedron_vertex :
            polyhedra_around_vertex( edge_vertices[0] ) )
        {
            if( const auto polyhedron_facet_edge =
                    polyhedron_facet_edge_from_vertices(
                        edge_vertices, polyhedron_vertex.polyhedron_id ) )
            {
                return polyhedron_facet_edge;
            }
        }
        return std::nullopt;
    }

    template < index_t dimension >
    PolyhedraAroundFacet SolidMesh< dimension >::polyhedra_from_facet_vertices(
        PolyhedronFacetVertices facet_vertices ) const
    {
        if( const auto facet =
                polyhedron_facet_from_vertices( std::move( facet_vertices ) ) )
        {
            if( const auto adjacent_facet =
                    polyhedron_adjacent_facet( facet.value() ) )
            {
                return { facet.value(), adjacent_facet.value() };
            }
            else
            {
                return { facet.value() };
            }
        }
        return {};
    }

    template < index_t dimension >
    auto SolidMesh< dimension >::vertices_around_vertex(
        index_t vertex_id ) const -> VerticesAroundVertex
    {
        return impl_->vertices_around_vertex( *this, vertex_id );
    }

    template < index_t dimension >
    const PolyhedraAroundVertex&
        SolidMesh< dimension >::polyhedra_around_vertex(
            const PolyhedronVertex& first_polyhedron ) const
    {
        return impl_->polyhedra_around_vertex(
            *this, polyhedron_vertex( first_polyhedron ), first_polyhedron );
    }

    template < index_t dimension >
    const PolyhedraAroundVertex&
        SolidMesh< dimension >::polyhedra_around_vertex(
            index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return impl_->polyhedra_around_vertex(
            *this, vertex_id, polyhedron_around_vertex( vertex_id ) );
    }

    template < geode::index_t dimension >
    bool SolidMesh< dimension >::is_edge_in_polyhedron_facet(
        const PolyhedronFacet& facet,
        const std::array< index_t, 2 >& edge_vertices ) const
    {
        const auto facet_vertices = polyhedron_facet_vertices( facet );
        const auto it = absl::c_find( facet_vertices, edge_vertices[0] );
        if( it == facet_vertices.end() )
        {
            return false;
        }
        const auto it_next = std::next( it );
        if( it_next != facet_vertices.end() )
        {
            if( *it_next == edge_vertices[1] )
            {
                return true;
            }
        }
        else
        {
            if( facet_vertices[0] == edge_vertices[1] )
            {
                return true;
            }
        }

        if( it != facet_vertices.begin() )
        {
            const auto it_prev = std::prev( it );
            if( *it_prev == edge_vertices[1] )
            {
                return true;
            }
        }
        else
        {
            if( facet_vertices[facet_vertices.size() - 1] == edge_vertices[1] )
            {
                return true;
            }
        }
        return false;
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::is_vertex_on_border( index_t vertex_id ) const
    {
        return impl_->is_vertex_on_border(
            *this, vertex_id, polyhedron_around_vertex( vertex_id ) );
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::is_edge_on_border(
        const std::array< index_t, 2 >& vertices ) const
    {
        const auto first_polyhedron =
            first_polyhedron_around_edge( *this, vertices );
        OPENGEODE_EXCEPTION( first_polyhedron,
            "[SolidMesh::is_edge_on_border] Given pair of "
            "vertices does not define a Solid edge" );
        return is_edge_on_border( vertices, first_polyhedron.value() );
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::is_edge_on_border(
        const std::array< index_t, 2 >& vertices,
        index_t first_polyhedron ) const
    {
        for( const auto f : LRange{ nb_polyhedron_facets( first_polyhedron ) } )
        {
            if( !is_edge_in_polyhedron_facet(
                    { first_polyhedron, f }, vertices ) )
            {
                continue;
            }
            return !std::get< 1 >( propagate_around_edge(
                *this, { first_polyhedron, f }, vertices ) );
        }
        OPENGEODE_EXCEPTION( true,
            "[SolidMesh::is_edge_on_border] Incoherence between given vertices "
            "and given first_polyhedron" );
        return false;
    }

    template < index_t dimension >
    PolyhedraAroundEdge SolidMesh< dimension >::polyhedra_around_edge(
        const std::array< index_t, 2 >& vertices ) const
    {
        if( const auto first_polyhedron =
                first_polyhedron_around_edge( *this, vertices ) )
        {
            return polyhedra_around_edge( vertices, first_polyhedron.value() );
        }
        return {};
    }

    template < index_t dimension >
    PolyhedraAroundEdge SolidMesh< dimension >::polyhedra_around_edge(
        const PolyhedronFacetEdge& edge ) const
    {
        return polyhedra_around_edge( polyhedron_facet_edge_vertices( edge ),
            edge.polyhedron_facet.polyhedron_id );
    }

    template < index_t dimension >
    PolyhedraAroundEdge SolidMesh< dimension >::polyhedra_around_edge(
        const std::array< index_t, 2 >& vertices,
        index_t first_polyhedron ) const
    {
        PolyhedraAroundEdge result{ first_polyhedron };
        for( const auto f : LRange{ nb_polyhedron_facets( first_polyhedron ) } )
        {
            if( !is_edge_in_polyhedron_facet(
                    { first_polyhedron, f }, vertices ) )
            {
                continue;
            }
            const auto propagation_result = propagate_around_edge(
                *this, { first_polyhedron, f }, vertices );
            auto polyhedra = std::get< 0 >( propagation_result );
            if( std::get< 1 >( propagation_result ) )
            {
                return polyhedra;
            }
            result.insert( result.end(), polyhedra.begin(), polyhedra.end() );
        }
        return result;
    }

    template < index_t dimension >
    std::optional< index_t > SolidMesh< dimension >::polyhedron_around_edge(
        const std::array< index_t, 2 >& vertices ) const
    {
        return first_polyhedron_around_edge( *this, vertices );
    }

    template < index_t dimension >
    PolyhedronEdgesVertices SolidMesh< dimension >::polyhedron_edges_vertices(
        index_t polyhedron ) const
    {
        PolyhedronEdgesVertices edge_vertices;
        for( const auto f : LRange{ nb_polyhedron_facets( polyhedron ) } )
        {
            const PolyhedronFacet facet{ polyhedron, f };
            for( const auto v :
                LRange{ nb_polyhedron_facet_vertices( facet ) } )
            {
                auto vertices = polyhedron_facet_edge_vertices( { facet, v } );
                if( vertices[0] < vertices[1] )
                {
                    edge_vertices.emplace_back( std::move( vertices ) );
                }
            }
        }
        return edge_vertices;
    }

    template < index_t dimension >
    PolyhedronFacetsVertices SolidMesh< dimension >::polyhedron_facets_vertices(
        index_t polyhedron ) const
    {
        PolyhedronFacetsVertices facets_vertices;
        facets_vertices.reserve( nb_polyhedron_facets( polyhedron ) );
        const auto vertices = polyhedron_vertices( polyhedron );
        for( const auto f : LRange{ nb_polyhedron_facets( polyhedron ) } )
        {
            const PolyhedronFacet facet{ polyhedron, f };
            auto& facet_vertices = facets_vertices.emplace_back();
            for( const auto v :
                LRange{ nb_polyhedron_facet_vertices( facet ) } )
            {
                facet_vertices.push_back(
                    vertices[polyhedron_facet_vertex_id( { facet, v } )
                            .vertex_id] );
            }
        }
        return facets_vertices;
    }

    template < index_t dimension >
    PolyhedronFacets SolidMesh< dimension >::polyhedron_vertex_facets(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        const auto vertex_id = this->polyhedron_vertex( polyhedron_vertex );
        PolyhedronFacets facets;
        for( const auto f :
            LRange{ nb_polyhedron_facets( polyhedron_vertex.polyhedron_id ) } )
        {
            PolyhedronFacet polyhedron_facet{ polyhedron_vertex.polyhedron_id,
                f };
            for( const auto v :
                LRange{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
            {
                if( polyhedron_facet_vertex( { polyhedron_facet, v } )
                    == vertex_id )
                {
                    facets.emplace_back( std::move( polyhedron_facet ) );
                    break;
                }
            }
        }
        return facets;
    }

    template < index_t dimension >
    std::optional< PolyhedronVertex >
        SolidMesh< dimension >::polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return get_polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    double SolidMesh< dimension >::polyhedron_minimum_height(
        const index_t polyhedron_id ) const
    {
        return impl_->polyhedron_minimum_height( *this, polyhedron_id );
    }

    template < index_t dimension >
    std::optional< PolyhedronVertex >
        SolidMesh< dimension >::get_polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        return impl_->polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    void SolidMesh< dimension >::associate_polyhedron_vertex_to_vertex(
        const PolyhedronVertex& polyhedron_vertex,
        index_t vertex_id,
        SolidMeshKey )
    {
        impl_->associate_polyhedron_vertex_to_vertex(
            polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void SolidMesh< dimension >::reset_polyhedra_around_vertex(
        index_t vertex_id, SolidMeshKey )
    {
        impl_->reset_polyhedra_around_vertex( vertex_id );
    }

    template < index_t dimension >
    local_index_t SolidMesh< dimension >::nb_polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        return get_nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    local_index_t SolidMesh< dimension >::nb_polyhedron_facets(
        index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        return get_nb_polyhedron_facets( polyhedron_id );
    }

    template < index_t dimension >
    index_t SolidMesh< dimension >::polyhedron_facet_vertex(
        const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        check_polyhedron_id(
            *this, polyhedron_facet_vertex.polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id( *this,
            polyhedron_facet_vertex.polyhedron_facet.polyhedron_id,
            polyhedron_facet_vertex.polyhedron_facet.facet_id );
        check_polyhedron_facet_vertex_id( *this,
            polyhedron_facet_vertex.polyhedron_facet.polyhedron_id,
            polyhedron_facet_vertex.polyhedron_facet.facet_id,
            polyhedron_facet_vertex.vertex_id );
        return polyhedron_vertex(
            get_polyhedron_facet_vertex_id( polyhedron_facet_vertex ) );
    }

    template < index_t dimension >
    std::array< index_t, 2 >
        SolidMesh< dimension >::polyhedron_facet_edge_vertices(
            const PolyhedronFacetEdge& polyhedron_facet_edge ) const
    {
        check_polyhedron_id(
            *this, polyhedron_facet_edge.polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id( *this,
            polyhedron_facet_edge.polyhedron_facet.polyhedron_id,
            polyhedron_facet_edge.polyhedron_facet.facet_id );
        check_polyhedron_facet_vertex_id( *this,
            polyhedron_facet_edge.polyhedron_facet.polyhedron_id,
            polyhedron_facet_edge.polyhedron_facet.facet_id,
            polyhedron_facet_edge.edge_id );
        const auto v0 =
            polyhedron_facet_vertex( { polyhedron_facet_edge.polyhedron_facet,
                polyhedron_facet_edge.edge_id } );
        const local_index_t next =
            polyhedron_facet_edge.edge_id + 1
                    == nb_polyhedron_facet_vertices(
                        polyhedron_facet_edge.polyhedron_facet )
                ? 0
                : polyhedron_facet_edge.edge_id + 1;
        const auto v1 = polyhedron_facet_vertex(
            { polyhedron_facet_edge.polyhedron_facet, next } );
        return { v0, v1 };
    }

    template < index_t dimension >
    local_index_t SolidMesh< dimension >::nb_polyhedron_facet_vertices(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    std::optional< index_t > SolidMesh< dimension >::polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    std::optional< PolyhedronFacet >
        SolidMesh< dimension >::polyhedron_adjacent_facet(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        const auto opt_polyhedron_adj = polyhedron_adjacent( polyhedron_facet );
        if( !opt_polyhedron_adj )
        {
            return std::nullopt;
        }
        absl::FixedArray< index_t > vertices(
            nb_polyhedron_facet_vertices( polyhedron_facet ) );
        for( const auto v : LIndices{ vertices } )
        {
            vertices[v] = polyhedron_facet_vertex( { polyhedron_facet, v } );
        }
        const auto polyhedron_adj = opt_polyhedron_adj.value();
        for( const auto f : LRange{ nb_polyhedron_facets( polyhedron_adj ) } )
        {
            if( polyhedron_adjacent( { polyhedron_adj, f } )
                != polyhedron_facet.polyhedron_id )
            {
                continue;
            }
            bool all_contained{ true };
            for( const auto v : LRange{
                     nb_polyhedron_facet_vertices( { polyhedron_adj, f } ) } )
            {
                if( absl::c_find( vertices, polyhedron_facet_vertex(
                                                { { polyhedron_adj, f }, v } ) )
                    == vertices.end() )
                {
                    all_contained = false;
                    break;
                }
            }
            if( all_contained )
            {
                return std::optional< PolyhedronFacet >{ std::in_place,
                    polyhedron_adj, f };
            }
        }
        throw OpenGeodeException{ "[SolidMesh::polyhedron_adjacent_"
                                  "facet] Wrong adjacency with polyhedra: ",
            polyhedron_facet.polyhedron_id, " and ", polyhedron_adj };
        return std::nullopt;
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::is_polyhedron_facet_on_border(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return !polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::is_polyhedron_on_border(
        index_t polyhedron_id ) const
    {
        return !polyhedron_facets_on_border( polyhedron_id ).empty();
    }

    template < index_t dimension >
    PolyhedronFacetsOnBorder
        SolidMesh< dimension >::polyhedron_facets_on_border(
            index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        PolyhedronFacetsOnBorder borders;
        for( const auto f : LRange{ nb_polyhedron_facets( polyhedron_id ) } )
        {
            const PolyhedronFacet facet{ polyhedron_id, f };
            if( is_polyhedron_facet_on_border( facet ) )
            {
                borders.emplace_back( facet );
            }
        }
        return borders;
    }

    template < index_t dimension >
    std::array< local_index_t, 2 >
        SolidMesh< dimension >::edge_vertices_in_polyhedron(
            index_t polyhedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const
    {
        std::array< local_index_t, 2 > result{ { NO_LID, NO_LID } };

        const auto vertices = polyhedron_vertices( polyhedron_id );
        for( const auto v : LRange{ nb_polyhedron_vertices( polyhedron_id ) } )
        {
            if( vertices[v] == edge_vertices[0] && result[0] == NO_LID )
            {
                result[0] = v;
            }
            else if( vertices[v] == edge_vertices[1] && result[1] == NO_LID )
            {
                result[1] = v;
            }
        }
        return result;
    }

    template < index_t dimension >
    AttributeManager&
        SolidMesh< dimension >::polyhedron_attribute_manager() const
    {
        return impl_->polyhedron_attribute_manager();
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::are_edges_enabled() const
    {
        return impl_->are_edges_enabled();
    }

    template < index_t dimension >
    void SolidMesh< dimension >::enable_edges() const
    {
        impl_->enable_edges( *this );
    }

    template < index_t dimension >
    void SolidMesh< dimension >::copy_edges(
        const SolidMesh< dimension >& solid, SolidMeshKey )
    {
        impl_->copy_edges( solid );
    }

    template < index_t dimension >
    void SolidMesh< dimension >::disable_edges() const
    {
        impl_->disable_edges();
    }

    template < index_t dimension >
    const SolidEdges< dimension >& SolidMesh< dimension >::edges() const
    {
        return impl_->edges();
    }

    template < index_t dimension >
    SolidEdges< dimension >& SolidMesh< dimension >::edges( SolidMeshKey )
    {
        return impl_->edges();
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::are_facets_enabled() const
    {
        return impl_->are_facets_enabled();
    }

    template < index_t dimension >
    void SolidMesh< dimension >::enable_facets() const
    {
        impl_->enable_facets( *this );
    }

    template < index_t dimension >
    void SolidMesh< dimension >::copy_facets(
        const SolidMesh< dimension >& solid, SolidMeshKey )
    {
        impl_->copy_facets( solid );
    }

    template < index_t dimension >
    void SolidMesh< dimension >::disable_facets() const
    {
        impl_->disable_facets();
    }

    template < index_t dimension >
    const SolidFacets< dimension >& SolidMesh< dimension >::facets() const
    {
        return impl_->facets();
    }

    template < index_t dimension >
    SolidFacets< dimension >& SolidMesh< dimension >::facets( SolidMeshKey )
    {
        return impl_->facets();
    }

    template < index_t dimension >
    template < typename Archive >
    void SolidMesh< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, SolidMesh >{
                { []( Archive& a, SolidMesh& solid ) {
                     a.ext( solid, bitsery::ext::BaseClass< VertexSet >{} );
                     a.object( solid.impl_ );
                     solid.impl_->initialize_polyhedra_around_vertex( solid );
                 },
                    []( Archive& a, SolidMesh& solid ) {
                        a.ext( solid, bitsery::ext::BaseClass< VertexSet >{} );
                        a.object( solid.impl_ );
                    },
                    []( Archive& a, SolidMesh& solid ) {
                        a.ext( solid, bitsery::ext::BaseClass< VertexSet >{} );
                        a.ext( solid, bitsery::ext::BaseClass<
                                          CoordinateReferenceSystemManagers<
                                              dimension > >{} );
                        a.object( solid.impl_ );
                    } } } );
    }

    template < index_t dimension >
    std::unique_ptr< SolidMesh< dimension > >
        SolidMesh< dimension >::clone() const
    {
        auto clone = create( impl_name() );
        auto builder = SolidMeshBuilder< dimension >::create( *clone );
        builder->copy_identifier( *this );
        builder->copy( *this );
        return clone;
    }

    template < index_t dimension >
    BoundingBox< dimension > SolidMesh< dimension >::bounding_box() const
    {
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( this->point( p ) );
        }
        return box;
    }

    template < index_t dimension >
    TextureManager3D SolidMesh< dimension >::texture_manager() const
    {
        return impl_->texture_manager();
    }

    template class opengeode_mesh_api SolidMesh< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolyhedronVertex );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolyhedronFacet );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolyhedronFacetVertex );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolyhedronFacetEdge );

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SolidMesh< 3 > );
} // namespace geode

namespace std
{
    size_t hash< geode::PolyhedronVertex >::operator()(
        const geode::PolyhedronVertex& polyhedron_vertex ) const
    {
        return absl::Hash< geode::index_t >()( polyhedron_vertex.polyhedron_id )
               ^ absl::Hash< geode::index_t >()( polyhedron_vertex.vertex_id );
    }

    size_t hash< geode::PolyhedronFacet >::operator()(
        const geode::PolyhedronFacet& polyhedron_facet ) const
    {
        return absl::Hash< geode::index_t >()( polyhedron_facet.polyhedron_id )
               ^ absl::Hash< geode::index_t >()( polyhedron_facet.facet_id );
    }

    size_t hash< geode::PolyhedronFacetVertex >::operator()(
        const geode::PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return absl::Hash< geode::PolyhedronFacet >()(
                   polyhedron_facet_vertex.polyhedron_facet )
               ^ absl::Hash< geode::index_t >()(
                   polyhedron_facet_vertex.vertex_id );
    }

    size_t hash< geode::PolyhedronFacetEdge >::operator()(
        const geode::PolyhedronFacetEdge& polyhedron_facet_edge ) const
    {
        return absl::Hash< geode::PolyhedronFacet >()(
                   polyhedron_facet_edge.polyhedron_facet )
               ^ absl::Hash< geode::index_t >()(
                   polyhedron_facet_edge.edge_id );
    }
} // namespace std