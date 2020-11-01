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

#include <geode/mesh/core/solid_mesh.h>

#include <absl/container/flat_hash_set.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_facets.h>

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
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
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
        const geode::index_t facet_id,
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
    geode::index_t get_vertex_id_in_polyhedron_facet(
        const geode::SolidMesh< dimension >& solid,
        const geode::PolyhedronFacet& facet,
        geode::index_t vertex_id )
    {
        for( const auto v :
            geode::Range{ solid.nb_polyhedron_facet_vertices( facet ) } )
        {
            if( solid.polyhedron_facet_vertex( { facet, v } ) == vertex_id )
            {
                return v;
            }
        }
        return geode::NO_ID;
    }

    template < geode::index_t dimension >
    bool is_edge_in_polyhedron_facet(
        const geode::SolidMesh< dimension >& solid,
        const geode::PolyhedronFacet& facet,
        const std::array< geode::index_t, 2 >& edge_vertices )
    {
        const auto nb_facet_vertices =
            solid.nb_polyhedron_facet_vertices( facet );
        const auto local_id =
            get_vertex_id_in_polyhedron_facet( solid, facet, edge_vertices[0] );
        if( local_id == geode::NO_ID )
        {
            return false;
        }
        const auto previous = solid.polyhedron_facet_vertex( { facet,
            ( local_id + nb_facet_vertices - 1 ) % nb_facet_vertices } );
        if( previous == edge_vertices[1] )
        {
            return true;
        }
        const auto next = solid.polyhedron_facet_vertex(
            { facet, ( local_id + 1 ) % nb_facet_vertices } );
        return next == edge_vertices[1];
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidMesh< dimension >::Impl
    {
        friend class bitsery::Access;

    public:
        explicit Impl( SolidMesh& solid )
            : polyhedron_around_vertex_(
                solid.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        PolyhedronVertex >(
                        "polyhedron_around_vertex", PolyhedronVertex{} ) )
        {
        }

        absl::optional< PolyhedronVertex > polyhedron_around_vertex(
            const index_t vertex_id ) const
        {
            const auto& value = polyhedron_around_vertex_->value( vertex_id );
            if( value.polyhedron_id != NO_ID )
            {
                return value;
            }
            return absl::nullopt;
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

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.object( impl.polyhedron_attribute_manager_ );
                    archive.ext( impl.polyhedron_around_vertex_,
                        bitsery::ext::StdSmartPtr{} );
                    archive.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                    archive.ext( impl.facets_, bitsery::ext::StdSmartPtr{} );
                } );
        }

    private:
        mutable AttributeManager polyhedron_attribute_manager_;
        std::shared_ptr< VariableAttribute< PolyhedronVertex > >
            polyhedron_around_vertex_;
        mutable std::unique_ptr< SolidEdges< dimension > > edges_;
        mutable std::unique_ptr< SolidFacets< dimension > > facets_;
    };

    template < index_t dimension >
    SolidMesh< dimension >::SolidMesh() : impl_( *this )
    {
    }

    template < index_t dimension >
    SolidMesh< dimension >::~SolidMesh() // NOLINT
    {
    }

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
    absl::optional< index_t > SolidMesh< dimension >::vertex_in_polyhedron(
        index_t polyhedron_id, index_t vertex_id ) const
    {
        for( const auto v : Range{ nb_polyhedron_vertices( polyhedron_id ) } )

        {
            if( polyhedron_vertex( { polyhedron_id, v } ) == vertex_id )
            {
                return v;
            }
        }
        return absl::nullopt;
    }

    template < index_t dimension >
    Point< dimension > SolidMesh< dimension >::polyhedron_barycenter(
        index_t polyhedron_id ) const
    {
        Point< dimension > barycenter;
        for( const auto v : Range{ nb_polyhedron_vertices( polyhedron_id ) } )
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
    PolyhedronFacetVertices SolidMesh< dimension >::polyhedron_facet_vertices(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        PolyhedronFacetVertices vertices;
        for( const auto v :
            Range{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
        {
            vertices.push_back(
                polyhedron_facet_vertex( { polyhedron_facet, v } ) );
        }
        return vertices;
    }

    template < index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, Vector3D >::type
        SolidMesh< dimension >::polyhedron_facet_normal(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        Vector3D normal;
        const auto& p0 = this->point(
            this->polyhedron_facet_vertex( { polyhedron_facet, 0 } ) );
        for( const auto v :
            Range{ 2, nb_polyhedron_facet_vertices( polyhedron_facet ) } )
        {
            const auto& p1 = this->point(
                polyhedron_facet_vertex( { polyhedron_facet, v - 1 } ) );
            const auto& p2 = this->point(
                polyhedron_facet_vertex( { polyhedron_facet, v } ) );
            normal = normal + Vector3D{ p1, p0 }.cross( { p2, p0 } );
        }
        return normal.normalize();
    }

    template < index_t dimension >
    index_t SolidMesh< dimension >::nb_polyhedra() const
    {
        return polyhedron_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::isolated_vertex( index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return !get_polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    absl::optional< PolyhedronFacet >
        SolidMesh< dimension >::polyhedron_facet_from_vertices(
            PolyhedronFacetVertices facet_vertices ) const
    {
        detail::VertexCycle< absl::InlinedVector< index_t, 4 > > vertices{
            std::move( facet_vertices )
        };
        for( const auto& polyhedron_vertex :
            polyhedra_around_vertex( vertices.vertices()[0] ) )
        {
            for( const auto f : Range{
                     nb_polyhedron_facets( polyhedron_vertex.polyhedron_id ) } )
            {
                const PolyhedronFacet facet{ polyhedron_vertex.polyhedron_id,
                    f };
                detail::VertexCycle< absl::InlinedVector< index_t, 4 > >
                    cur_vertices{ polyhedron_facet_vertices( facet ) };
                if( vertices == cur_vertices )
                {
                    return facet;
                }
            }
        }
        return absl::nullopt;
    }

    template < index_t dimension >
    PolyhedraAroundFacet SolidMesh< dimension >::polyhedra_from_facet(
        PolyhedronFacetVertices facet_vertices ) const
    {
        if( const auto facet =
                polyhedron_facet_from_vertices( std::move( facet_vertices ) ) )
        {
            if( const auto adjacent = polyhedron_adjacent( facet.value() ) )
            {
                return { facet->polyhedron_id, adjacent.value() };
            }
            else
            {
                return { facet->polyhedron_id };
            }
        }
        return {};
    }

    template < index_t dimension >
    PolyhedraAroundVertex SolidMesh< dimension >::polyhedra_around_vertex(
        index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        const auto first_polyhedron = polyhedron_around_vertex( vertex_id );
        if( !first_polyhedron )
        {
            return {};
        }
        OPENGEODE_ASSERT(
            polyhedron_vertex( first_polyhedron.value() ) == vertex_id,
            "[SolidMesh::polyhedra_around_vertex] Wrong "
            "polyhedron around vertex" );
        PolyhedraAroundVertex polyhedra;
        absl::flat_hash_set< index_t > polyhedra_visited;
        std::stack< PolyhedronVertex > S;
        S.push( first_polyhedron.value() );
        polyhedra_visited.insert( first_polyhedron->polyhedron_id );
        while( !S.empty() )
        {
            const auto polyhedron_vertex_id = S.top();
            S.pop();
            const auto p = polyhedron_vertex_id.polyhedron_id;
            polyhedra.push_back( polyhedron_vertex_id );

            for( const auto f : Range{ nb_polyhedron_facets( p ) } )
            {
                const PolyhedronFacet polyhedron_facet{ p, f };
                for( const auto v :
                    Range{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
                {
                    if( polyhedron_facet_vertex( { polyhedron_facet, v } )
                        != vertex_id )
                    {
                        continue;
                    }
                    if( !is_polyhedron_facet_on_border( polyhedron_facet ) )
                    {
                        const auto adj_polyhedron =
                            polyhedron_adjacent( polyhedron_facet ).value();
                        if( !polyhedra_visited.insert( adj_polyhedron ).second )
                        {
                            continue;
                        }
                        PolyhedronVertex adj_vertex{ adj_polyhedron, NO_ID };
                        for( const auto v_adj :
                            Range{ nb_polyhedron_vertices( adj_polyhedron ) } )
                        {
                            if( polyhedron_vertex( { adj_polyhedron, v_adj } )
                                == vertex_id )
                            {
                                adj_vertex.vertex_id = v_adj;
                                break;
                            }
                        }
                        OPENGEODE_ASSERT( adj_vertex.vertex_id != NO_ID,
                            "[SolidMesh::polyhedra_around_vertex] "
                            "Adjacency issue detected" );
                        S.emplace( adj_vertex );
                    }
                    break;
                }
            }
        }
        return polyhedra;
    }

    template < index_t dimension >
    PolyhedraAroundEdge SolidMesh< dimension >::polyhedra_around_edge(
        const std::array< index_t, 2 >& vertices ) const
    {
        PolyhedraAroundEdge result;
        const auto& polyhedron_vertices =
            polyhedra_around_vertex( vertices[0] );
        for( const auto& polyhedron_vertex : polyhedron_vertices )
        {
            for( const auto f : Range{
                     nb_polyhedron_facets( polyhedron_vertex.polyhedron_id ) } )
            {
                const PolyhedronFacet facet{ polyhedron_vertex.polyhedron_id,
                    f };
                if( is_edge_in_polyhedron_facet( *this, facet, vertices ) )
                {
                    result.push_back( polyhedron_vertex.polyhedron_id );
                    break;
                }
            }
        }
        return result;
    }

    template < index_t dimension >
    std::vector< std::array< index_t, 2 > >
        SolidMesh< dimension >::polyhedron_edges_vertices(
            index_t polyhedron ) const
    {
        std::vector< std::array< index_t, 2 > > edge_vertices;
        edge_vertices.reserve( 3 * nb_polyhedron_facets( polyhedron ) );
        for( const auto f : Range{ nb_polyhedron_facets( polyhedron ) } )
        {
            const PolyhedronFacet facet{ polyhedron, f };
            for( const auto v : Range{ nb_polyhedron_facet_vertices( facet ) } )
            {
                edge_vertices.emplace_back(
                    polyhedron_facet_edge_vertices( { facet, v } ) );
            }
        }
        return edge_vertices;
    }

    template < index_t dimension >
    std::vector< PolyhedronFacetVertices >
        SolidMesh< dimension >::polyhedron_facets_vertices(
            index_t polyhedron ) const
    {
        std::vector< PolyhedronFacetVertices > facet_vertices;
        facet_vertices.reserve( 3 * nb_polyhedron_facets( polyhedron ) );
        for( const auto f : Range{ nb_polyhedron_facets( polyhedron ) } )
        {
            facet_vertices.emplace_back(
                polyhedron_facet_vertices( { polyhedron, f } ) );
        }
        return facet_vertices;
    }

    template < index_t dimension >
    absl::optional< PolyhedronVertex >
        SolidMesh< dimension >::polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return get_polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    absl::optional< PolyhedronVertex >
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
    index_t SolidMesh< dimension >::nb_polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        return get_nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    index_t SolidMesh< dimension >::nb_polyhedron_facets(
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
        const auto v1 =
            polyhedron_facet_vertex( { polyhedron_facet_edge.polyhedron_facet,
                ( polyhedron_facet_edge.edge_id + 1 )
                    % nb_polyhedron_facet_vertices(
                        polyhedron_facet_edge.polyhedron_facet ) } );
        return { v0, v1 };
    }

    template < index_t dimension >
    index_t SolidMesh< dimension >::nb_polyhedron_facet_vertices(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    absl::optional< index_t > SolidMesh< dimension >::polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    absl::optional< PolyhedronFacet >
        SolidMesh< dimension >::polyhedron_adjacent_facet(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        if( !is_polyhedron_facet_on_border( polyhedron_facet ) )
        {
            absl::FixedArray< index_t > vertices(
                nb_polyhedron_facet_vertices( polyhedron_facet ) );
            for( const auto v : Indices{ vertices } )
            {
                vertices[v] =
                    polyhedron_facet_vertex( { polyhedron_facet, v } );
            }
            const auto polyhedron_adj =
                polyhedron_adjacent( polyhedron_facet ).value();
            for( const auto f :
                Range{ nb_polyhedron_facets( polyhedron_adj ) } )
            {
                const auto polyhedron =
                    polyhedron_adjacent( { polyhedron_adj, f } );
                if( polyhedron == polyhedron_facet.polyhedron_id )
                {
                    bool all_contained{ true };
                    for( const auto v : Range{ nb_polyhedron_facet_vertices(
                             { polyhedron_adj, f } ) } )
                    {
                        if( absl::c_find(
                                vertices, polyhedron_facet_vertex(
                                              { { polyhedron_adj, f }, v } ) )
                            == vertices.end() )
                        {
                            all_contained = false;
                            break;
                        }
                    }
                    if( all_contained )
                    {
                        return PolyhedronFacet{ polyhedron_adj, f };
                    }
                }
            }
            throw OpenGeodeException{ "[SolidMesh::polyhedron_adjacent_"
                                      "facet] Wrong adjacency with polyhedra: ",
                polyhedron_facet.polyhedron_id, " and ", polyhedron_adj };
        }
        return absl::nullopt;
    }

    template < index_t dimension >
    bool SolidMesh< dimension >::is_polyhedron_facet_on_border(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return !polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    PolyhedronFacetsOnBorder
        SolidMesh< dimension >::polyhedron_facets_on_border(
            index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        PolyhedronFacetsOnBorder borders;
        for( const auto f : Range{ nb_polyhedron_facets( polyhedron_id ) } )
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
    const Point< dimension >& SolidMesh< dimension >::point(
        index_t vertex_id ) const
    {
        check_vertex_id( *this, vertex_id );
        return get_point( vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void SolidMesh< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, SolidMesh >{},
            []( Archive& archive, SolidMesh& solid ) {
                archive.ext( solid, bitsery::ext::BaseClass< VertexSet >{} );
                archive.object( solid.impl_ );
            } );
    }

    template < index_t dimension >
    std::unique_ptr< SolidMesh< dimension > >
        SolidMesh< dimension >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder = SolidMeshBuilder< dimension >::create( *clone );
        builder->copy( *this, {} );
        return clone;
    }

    template < index_t dimension >
    BoundingBox< dimension > SolidMesh< dimension >::bounding_box() const
    {
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( point( p ) );
        }
        return box;
    }

    template class opengeode_mesh_api SolidMesh< 3 >;

    template opengeode_mesh_api Vector3D
        SolidMesh< 3 >::polyhedron_facet_normal< 3 >(
            const PolyhedronFacet& ) const;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, SolidMesh< 3 > );
} // namespace geode
