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

#pragma once

#include <absl/algorithm/container.h>
#include <absl/types/span.h>

#include <geode/basic/uuid.hpp>

#include <geode/mesh/common.hpp>

namespace geode
{
    struct MeshElement
    {
        MeshElement() = default;

        MeshElement( uuid mesh_id_in, index_t element_id_in )
            : mesh_id( std::move( mesh_id_in ) ), element_id( element_id_in )
        {
        }

        [[nodiscard]] bool operator==( const MeshElement& other ) const
        {
            return mesh_id == other.mesh_id && element_id == other.element_id;
        }

        [[nodiscard]] bool operator!=( const MeshElement& other ) const
        {
            return !operator==( other );
        }

        [[nodiscard]] bool operator<( const MeshElement& other ) const
        {
            if( mesh_id != other.mesh_id )
            {
                return mesh_id < other.mesh_id;
            }
            return element_id < other.element_id;
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext(
                *this, Growable< Archive, MeshElement >{
                           { []( Archive& a, MeshElement& mesh_element ) {
                               a.object( mesh_element.mesh_id );
                               a.value4b( mesh_element.element_id );
                           } } } );
        }

        [[nodiscard]] std::string string() const
        {
            return absl::StrCat( "[", mesh_id.string(), " ", element_id, "]" );
        }

        uuid mesh_id;
        index_t element_id{ geode::NO_ID };
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

    template < typename MeshElementType >
    struct MeshElementsInclusion
    {
        MeshElementsInclusion() = default;

        absl::Span< const MeshElementType > query;
        absl::Span< const MeshElementType > container;
    };

    struct MeshVerticesInclusion : public MeshElementsInclusion< MeshVertex >
    {
        using MeshElementsInclusion< MeshVertex >::MeshElementsInclusion;
    };

    struct MeshEdgesInclusion : public MeshElementsInclusion< MeshEdge >
    {
        using MeshElementsInclusion< MeshEdge >::MeshElementsInclusion;
    };

    struct MeshPolygonsInclusion : public MeshElementsInclusion< MeshPolygon >
    {
        using MeshElementsInclusion< MeshPolygon >::MeshElementsInclusion;
    };

    template < typename MeshElementType >
    [[nodiscard]] bool are_mesh_elements_included(
        const MeshElementsInclusion< MeshElementType >& inclusion )
    {
        for( const auto& q : inclusion.query )
        {
            if( absl::c_find( inclusion.container, q )
                == inclusion.container.end() )
            {
                return false;
            }
        }
        return true;
    }

    template < typename SkipMeshElement, typename MeshElementType >
    [[nodiscard]] bool are_mesh_elements_included(
        const MeshElementsInclusion< MeshElementType >& inclusion,
        const SkipMeshElement& skip )
    {
        for( const auto& q : inclusion.query )
        {
            if( skip( q ) )
            {
                continue;
            }
            if( absl::c_find( inclusion.container, q )
                == inclusion.container.end() )
            {
                return false;
            }
        }
        return true;
    }
} // namespace geode

namespace std
{
    template <>
    struct opengeode_mesh_api hash< geode::MeshElement >
    {
        size_t operator()( const geode::MeshElement& mesh_element ) const;
    };

    template <>
    struct opengeode_mesh_api hash< geode::MeshVertex >
    {
        size_t operator()( const geode::MeshVertex& mesh_vertex ) const;
    };

    template <>
    struct opengeode_mesh_api hash< geode::MeshEdge >
    {
        size_t operator()( const geode::MeshEdge& mesh_edge ) const;
    };

    template <>
    struct opengeode_mesh_api hash< geode::MeshPolygon >
    {
        size_t operator()( const geode::MeshPolygon& mesh_polygon ) const;
    };
} // namespace std