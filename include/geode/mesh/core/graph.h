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

#pragma once

#include <vector>

#include <geode/basic/attribute.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/factory.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * This struct represents a graph edge endpoint
     */
    struct opengeode_mesh_api EdgeVertex
    {
        EdgeVertex() = default;
        EdgeVertex( index_t edge_id, index_t vertex_id )
            : edge_id( edge_id ), vertex_id( vertex_id )
        {
        }
        bool operator==( const EdgeVertex& other ) const
        {
            return edge_id == other.edge_id && vertex_id == other.vertex_id;
        }
        bool operator!=( const EdgeVertex& other ) const
        {
            return !( *this == other );
        }
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, EdgeVertex >{},
                []( Archive& archive, EdgeVertex& edge_vertex ) {
                    archive.value4b( edge_vertex.edge_id );
                    archive.value4b( edge_vertex.vertex_id );
                } );
        }
        /*!
         * Index of the edge
         */
        index_t edge_id{ NO_ID };
        /*!
         * Identification of the edge endpoint (0 or 1)
         */
        index_t vertex_id{ NO_ID };
    };

    /*!
     * Interface class to represent a Graph.
     * A graph is a set of vertices linked by edges
     */
    class opengeode_mesh_api Graph : public VertexSet
    {
        friend class GraphBuilder;

    public:
        /*!
         * Create a new Graph using default data structure.
         */
        static std::unique_ptr< Graph > create();

        /*!
         * Create a new Graph using a specified data structure.
         * @param[in] type Data structure type
         */
        static std::unique_ptr< Graph > create( const MeshType& type );

        ~Graph();

        /*!
         * Return the vertex index corresponding to an edge endpoint
         * @param[in] edge_vertex Edge endpoint
         */
        index_t edge_vertex( const EdgeVertex& edge_vertex ) const;

        index_t nb_edges() const;

        /*!
         * Access to the manager of attributes associated with edges.
         */
        AttributeManager& edge_attribute_manager() const;

        /*!
         * Get all edge endpoints corresponding to a given vertex
         * @param[in] vertex_id Index of the vertex
         */
        const std::vector< EdgeVertex >& edges_around_vertex(
            index_t vertex_id ) const;

    protected:
        Graph();

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        std::vector< EdgeVertex >& get_edges_around_vertex( index_t vertex_id );

        void associate_edge_vertex_to_vertex(
            const EdgeVertex& edge_vertex, index_t vertex_id );

        virtual index_t get_edge_vertex(
            const EdgeVertex& edge_vertex ) const = 0;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    using GraphFactory = Factory< MeshType, Graph >;
} // namespace geode
