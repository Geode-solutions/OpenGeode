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

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/mesh_id.h>

namespace geode
{
    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent set of vertices.
     */
    class opengeode_mesh_api VertexSet
    {
        OPENGEODE_DISABLE_COPY( VertexSet );
        friend class bitsery::Access;

    public:
        /*!
         * Create a new VertexSet
         */
        static std::unique_ptr< VertexSet > create();

        /*!
         * Create a new VertexSet using a specified data structure.
         * @param[in] type Data structure type
         */
        static std::unique_ptr< VertexSet > create( const MeshType& type );

        static MeshKind kind_name_static()
        {
            return MeshKind{ "VertexSet" };
        }

        std::unique_ptr< VertexSet > clone() const;

        virtual ~VertexSet();

        virtual absl::string_view native_extension() const = 0;

        index_t nb_vertices() const;

        /*!
         * Access to the attribute manager.
         * Attributes are associated with vertices.
         */
        AttributeManager& vertex_attribute_manager() const;

        virtual MeshType type_name() const = 0;

        virtual MeshKind kind_name() const = 0;

    protected:
        VertexSet();
        VertexSet( VertexSet&& );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
