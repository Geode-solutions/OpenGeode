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

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/graph.h>

namespace geode
{
    class opengeode_mesh_api OpenGeodeGraph : public Graph
    {
        friend class OpenGeodeGraphBuilder;

    public:
        OpenGeodeGraph();
        ~OpenGeodeGraph();

        static MeshType type_name_static()
        {
            return MeshType{ "OpenGeodeGraph" };
        }

        MeshType type_name() const override
        {
            return type_name_static();
        }

        static std::string native_extension_static()
        {
            return "og_grp";
        }

        std::string native_extension() const override
        {
            return native_extension_static();
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        index_t get_edge_vertex( const EdgeVertex& edge_vertex ) const override;

        void set_edge_vertex(
            const EdgeVertex& edge_vertex, index_t vertex_id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
