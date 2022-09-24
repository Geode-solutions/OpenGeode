/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/uuid.h>

#include <geode/mesh/common.h>

namespace geode
{
    struct MeshElement
    {
        MeshElement( geode::uuid mesh_id_in, geode::index_t element_id_in )
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

        geode::uuid mesh_id;
        geode::index_t element_id;

    private:
        friend class bitsery::Access;
        MeshElement() = default;
    };

    using ComponentVertex = MeshElement;
    using ComponentEdge = MeshElement;
    using ComponentPolygon = MeshElement;
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
} // namespace std