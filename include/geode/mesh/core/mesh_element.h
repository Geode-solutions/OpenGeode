/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/uuid.h>

#include <geode/mesh/common.h>

namespace geode
{
    struct MeshElement
    {
        MeshElement( uuid mesh_id_in, index_t element_id_in )
            : mesh_id( std::move( mesh_id_in ) ), element_id( element_id_in )
        {
        }

        bool operator==( const MeshElement& other ) const
        {
            return mesh_id == other.mesh_id && element_id == other.element_id;
        }

        bool operator!=( const MeshElement& other ) const
        {
            return mesh_id != other.mesh_id || element_id != other.element_id;
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, MeshElement >{},
                []( Archive& a, MeshElement& mesh_element ) {
                    a.object( mesh_element.mesh_id );
                    a.value4b( mesh_element.element_id );
                } );
        }

        uuid mesh_id;
        index_t element_id;

    private:
        friend class bitsery::Access;
        MeshElement() = default;
    };

    struct MeshVertex : public MeshElement
    {
        using MeshElement::MeshElement;
    };

    struct MeshEdge : public MeshElement
    {
        using MeshElement::MeshElement;
    };

    struct MeshPolygon : public MeshElement
    {
        using MeshElement::MeshElement;
    };
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::MeshElement >
    {
    public:
        size_t operator()( const geode::MeshElement& mesh_element ) const
        {
            return absl::Hash< geode::uuid >()( mesh_element.mesh_id )
                   ^ absl::Hash< geode::index_t >()( mesh_element.element_id );
        }
    };

    template <>
    struct hash< geode::MeshVertex >
    {
    public:
        size_t operator()( const geode::MeshVertex& mesh_vertex ) const
        {
            return std::hash< geode::MeshElement >()( mesh_vertex );
        }
    };

    template <>
    struct hash< geode::MeshEdge >
    {
    public:
        size_t operator()( const geode::MeshEdge& mesh_edge ) const
        {
            return std::hash< geode::MeshElement >()( mesh_edge );
        }
    };

    template <>
    struct hash< geode::MeshPolygon >
    {
    public:
        size_t operator()( const geode::MeshPolygon& mesh_polygon ) const
        {
            return std::hash< geode::MeshElement >()( mesh_polygon );
        }
    };
} // namespace std