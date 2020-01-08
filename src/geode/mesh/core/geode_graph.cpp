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

#include <geode/mesh/core/geode_graph.h>

#include <fstream>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/detail/edges_impl.h>

namespace geode
{
    class OpenGeodeGraph::Impl : public detail::EdgesImpl
    {
    public:
        explicit Impl( OpenGeodeGraph& mesh ) : detail::EdgesImpl( mesh ) {}

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.ext(
                        impl, bitsery::ext::BaseClass< detail::EdgesImpl >{} );
                } );
        }
    };

    OpenGeodeGraph::OpenGeodeGraph() : impl_( *this ) {}

    OpenGeodeGraph::~OpenGeodeGraph() {} // NOLINT

    index_t OpenGeodeGraph::get_edge_vertex(
        const EdgeVertex& edge_vertex ) const
    {
        return impl_->get_edge_vertex( edge_vertex );
    }

    void OpenGeodeGraph::set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        impl_->set_edge_vertex( edge_vertex, vertex_id );
    }

    template < typename Archive >
    void OpenGeodeGraph::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, OpenGeodeGraph >{},
            []( Archive& archive, OpenGeodeGraph& graph ) {
                archive.ext( graph, bitsery::ext::BaseClass< Graph >{} );
                archive.object( graph.impl_ );
            } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, OpenGeodeGraph );
} // namespace geode
