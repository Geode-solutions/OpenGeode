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

#include <geode/mesh/core/polyhedral_solid.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/vector.h>

#include <geode/mesh/core/geode_polyhedral_solid.h>

namespace
{
    template < geode::index_t dimension >
    void check_polyhedron_id( const geode::PolyhedralSolid< dimension >& solid,
        geode::index_t polyhedron_id )
    {
        OPENGEODE_EXCEPTION( polyhedron_id < solid.nb_polyhedra(),
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_vertex_id(
        const geode::PolyhedralSolid< dimension >& solid,
        geode::index_t polyhedron_id,
        geode::index_t vertex_id )
    {
        OPENGEODE_EXCEPTION(
            vertex_id < solid.nb_polyhedron_vertices( polyhedron_id ),
            "[check_polyhedron_vertex_id] Trying to access an invalid "
            "polyhedron vertex" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_id(
        const geode::PolyhedralSolid< dimension >& solid,
        geode::index_t polyhedron_id,
        geode::index_t facet_id )
    {
        OPENGEODE_EXCEPTION(
            facet_id < solid.nb_polyhedron_facets( polyhedron_id ),
            "[check_polyhedron_facet_id] Trying to access an invalid "
            "polyhedron facet" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_vertex_id(
        const geode::PolyhedralSolid< dimension >& solid,
        geode::index_t polyhedron_id,
        geode::index_t facet_id,
        geode::index_t vertex_id )
    {
        OPENGEODE_EXCEPTION( vertex_id < solid.nb_polyhedron_facet_vertices(
                                 { polyhedron_id, facet_id } ),
            "[check_polyhedron_facet_vertex_id] Trying to access an invalid "
            "polyhedron facet vertex" );
    }

} // namespace

namespace geode
{
    template < index_t dimension >
    class PolyhedralSolid< dimension >::Impl
    {
    public:
        explicit Impl( PolyhedralSolid& solid )
            : polyhedron_around_vertex_(
                solid.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        PolyhedronVertex >(
                        "polyhedron_around_vertex", PolyhedronVertex{} ) )
        {
        }

        const PolyhedronVertex& polyhedron_around_vertex(
            index_t vertex_id ) const
        {
            return polyhedron_around_vertex_->value( vertex_id );
        }

        void associate_polyhedron_vertex_to_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
        {
            polyhedron_around_vertex_->value( vertex_id ) = polyhedron_vertex;
        }

        AttributeManager& polyhedron_attribute_manager() const
        {
            return polyhedron_attribute_manager_;
        }

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.object( polyhedron_attribute_manager_ );
            archive.ext(
                polyhedron_around_vertex_, bitsery::ext::StdSmartPtr{} );
        }

    private:
        mutable AttributeManager polyhedron_attribute_manager_;
        std::shared_ptr< VariableAttribute< PolyhedronVertex > >
            polyhedron_around_vertex_;
    };

    template < index_t dimension >
    PolyhedralSolid< dimension >::PolyhedralSolid() : impl_( *this )
    {
    }

    template < index_t dimension >
    PolyhedralSolid< dimension >::~PolyhedralSolid() // NOLINT
    {
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::create()
    {
        return std::unique_ptr< PolyhedralSolid< dimension > >{
            new OpenGeodePolyhedralSolid< dimension >
        };
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::create( const MeshType& type )
    {
        try
        {
            return PolyhedralSolidFactory< dimension >::create( type );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException(
                "Could not create PolyhedralSolid data structure: ",
                type.get() );
        }
    }

    template < index_t dimension >
    double PolyhedralSolid< dimension >::polyhedron_volume(
        index_t /*unused*/ ) const
    {
        throw OpenGeodeException( "polyhedron_volume not implemented yet" );
        return 0;
    }

    template < index_t dimension >
    double PolyhedralSolid< dimension >::polyhedron_facet_area(
        const PolyhedronFacet& /*unused*/ ) const
    {
        throw OpenGeodeException( "polyhedron_facet_area not implemented yet" );
        return 0;
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        check_polyhedron_id( *this, polyhedron_vertex.polyhedron_id );
        check_polyhedron_vertex_id( *this, polyhedron_vertex.polyhedron_id,
            polyhedron_vertex.vertex_id );
        return get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    Point< dimension > PolyhedralSolid< dimension >::polyhedron_barycenter(
        index_t polyhedron_id ) const
    {
        Point< dimension > barycenter;
        for( auto v : Range{ nb_polyhedron_vertices( polyhedron_id ) } )
        {
            barycenter =
                barycenter
                + this->point( polyhedron_vertex( { polyhedron_id, v } ) );
        }
        return barycenter / nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    Point< dimension >
        PolyhedralSolid< dimension >::polyhedron_facet_barycenter(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        Point< dimension > barycenter;
        for( auto v :
            Range{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
        {
            barycenter = barycenter
                         + this->point( polyhedron_facet_vertex(
                             { polyhedron_facet, v } ) );
        }
        return barycenter / nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedra() const
    {
        return polyhedron_attribute_manager().nb_elements();
    }

    template < index_t dimension >
    std::vector< PolyhedronVertex >
        PolyhedralSolid< dimension >::polyhedra_around_vertex(
            index_t vertex_id ) const
    {
        std::vector< PolyhedronVertex > polyhedra;
        const auto& first_polyhedron =
            impl_->polyhedron_around_vertex( vertex_id );
        if( first_polyhedron.polyhedron_id == NO_ID )
        {
            return polyhedra;
        }
        std::vector< index_t > polyhedra_visited;
        polyhedra_visited.reserve( 10 );
        std::stack< PolyhedronVertex > S;
        S.push( first_polyhedron );
        while( !S.empty() )
        {
            auto polyhedron_vertex_id = S.top();
            S.pop();
            auto p = polyhedron_vertex_id.polyhedron_id;
            if( contain( polyhedra_visited, p ) )
            {
                continue;
            }
            polyhedra_visited.push_back( p );
            polyhedra.push_back( polyhedron_vertex_id );

            for( auto f : Range{ nb_polyhedron_facets( p ) } )
            {
                PolyhedronFacet polyhedron_facet{ p, f };
                for( auto v :
                    Range{ nb_polyhedron_facet_vertices( polyhedron_facet ) } )
                {
                    if( polyhedron_facet_vertex( { polyhedron_facet, v } )
                        == vertex_id )
                    {
                        if( !is_polyhedron_facet_on_border( polyhedron_facet ) )
                        {
                            auto adj_polyhedron =
                                polyhedron_adjacent( polyhedron_facet );
                            PolyhedronVertex adj_vertex{ adj_polyhedron,
                                NO_ID };
                            for( auto v_adj : Range{ nb_polyhedron_vertices(
                                     adj_polyhedron ) } )
                            {
                                if( polyhedron_vertex(
                                        { adj_polyhedron, v_adj } )
                                    == vertex_id )
                                {
                                    adj_vertex.vertex_id = v_adj;
                                    break;
                                }
                            }
                            S.emplace( adj_vertex );
                        }
                        break;
                    }
                }
            }
        }
        return polyhedra;
    }

    template < index_t dimension >
    const PolyhedronVertex&
        PolyhedralSolid< dimension >::polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        return impl_->polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    void PolyhedralSolid< dimension >::associate_polyhedron_vertex_to_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        impl_->associate_polyhedron_vertex_to_vertex(
            polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        return get_nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedron_facets(
        index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        return get_nb_polyhedron_facets( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_facet_vertex(
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
        return get_polyhedron_facet_vertex( polyhedron_facet_vertex );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::nb_polyhedron_facet_vertices(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    index_t PolyhedralSolid< dimension >::polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        check_polyhedron_id( *this, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id(
            *this, polyhedron_facet.polyhedron_id, polyhedron_facet.facet_id );
        return get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    PolyhedronFacet PolyhedralSolid< dimension >::polyhedron_adjacent_facet(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        if( !is_polyhedron_facet_on_border( polyhedron_facet ) )
        {
            auto polyhedron_adj = polyhedron_adjacent( polyhedron_facet );
            for( auto f : Range{ nb_polyhedron_facets( polyhedron_adj ) } )
            {
                auto polyhedron = polyhedron_adjacent( { polyhedron_adj, f } );
                if( polyhedron == polyhedron_facet.polyhedron_id )
                {
                    return { polyhedron_adj, f };
                }
            }
            throw OpenGeodeException( "Wrong adjacency with polyhedra: ",
                polyhedron_facet.polyhedron_id, " and ", polyhedron_adj );
        }
        return {};
    }

    template < index_t dimension >
    bool PolyhedralSolid< dimension >::is_polyhedron_facet_on_border(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return polyhedron_adjacent( polyhedron_facet ) == NO_ID;
    }

    template < index_t dimension >
    std::vector< PolyhedronFacet >
        PolyhedralSolid< dimension >::polyhedron_facets_on_border(
            index_t polyhedron_id ) const
    {
        check_polyhedron_id( *this, polyhedron_id );
        std::vector< PolyhedronFacet > borders;
        for( auto f : Range{ nb_polyhedron_facets( polyhedron_id ) } )
        {
            PolyhedronFacet facet{ polyhedron_id, f };
            if( is_polyhedron_facet_on_border( facet ) )
            {
                borders.emplace_back( facet );
            }
        }
        return borders;
    }

    template < index_t dimension >
    AttributeManager&
        PolyhedralSolid< dimension >::polyhedron_attribute_manager() const
    {
        return impl_->polyhedron_attribute_manager();
    }

    template < index_t dimension >
    const Point< dimension >& PolyhedralSolid< dimension >::point(
        index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < nb_vertices(),
            "[PolyhedralSolid::point] Trying to access an invalid point" );
        return get_point( vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void PolyhedralSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, bitsery::ext::BaseClass< VertexSet >{} );
        archive.object( impl_ );
    }

    template class opengeode_mesh_api PolyhedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PolyhedralSolid< 3 > );
} // namespace geode
