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

#include <geode/mesh/core/geode/geode_graph.hpp>

#include <fstream>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/internal/edges_impl.hpp>

namespace geode
{
    class OpenGeodeGraph::Impl : public internal::EdgesImpl
    {
        friend class bitsery::Access;

    public:
        Impl() = default;

    private:
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                        archive.ext( impl,
                            bitsery::ext::BaseClass< internal::EdgesImpl >{} );
                    } } } );
        }
    };

    OpenGeodeGraph::OpenGeodeGraph() = default;

    OpenGeodeGraph::OpenGeodeGraph( BITSERY bitsery ) : Graph{ bitsery } {}

    OpenGeodeGraph::OpenGeodeGraph( OpenGeodeGraph&& ) noexcept = default;

    OpenGeodeGraph& OpenGeodeGraph::operator=(
        OpenGeodeGraph&& ) noexcept = default;

    OpenGeodeGraph::~OpenGeodeGraph() = default;

    index_t OpenGeodeGraph::get_edge_vertex(
        const EdgeVertex& edge_vertex ) const
    {
        return impl_->get_edge_vertex( edge_vertex );
    }

    void OpenGeodeGraph::set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id, OGGraphKey /*key*/ )
    {
        impl_->set_edge_vertex( edge_vertex, vertex_id );
    }

    void OpenGeodeGraph::add_edge( OGGraphKey /*key*/ )
    {
        impl_->add_edge();
    }

    void OpenGeodeGraph::add_edges( index_t nb, OGGraphKey /*key*/ )
    {
        impl_->add_edges( nb );
    }

    void OpenGeodeGraph::remove_edges(
        const std::vector< bool >& to_delete, OGGraphKey /*key*/ )
    {
        impl_->remove_edges( to_delete );
    }

    void OpenGeodeGraph::permute_edges(
        absl::Span< const index_t > permutation, OGGraphKey /*key*/ )
    {
        impl_->permute_edges( permutation );
    }

    void OpenGeodeGraph::copy_edges(
        const OpenGeodeGraph& graph, OGGraphKey /*key*/ )
    {
        impl_->copy_edges( *graph.impl_ );
    }

    template < typename Archive >
    void OpenGeodeGraph::serialize( Archive& serializer )
    {
        serializer.ext( *this,
            Growable< Archive, OpenGeodeGraph >{
                { []( Archive& archive, OpenGeodeGraph& graph ) {
                    archive.ext( graph, bitsery::ext::BaseClass< Graph >{} );
                    archive.object( graph.impl_ );
                    graph.impl_->check_edges_number( graph.nb_edges() );
                    graph.impl_->remove_legacy_attributes(
                        graph.edge_attribute_manager() );
                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeGraph );
} // namespace geode
