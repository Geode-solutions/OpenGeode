/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <absl/container/fixed_array.h>

#include <geode/basic/uuid.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Line );
    ALIAS_3D( Line );
    class BRep;
    struct ComponentMeshVertex;
} // namespace geode

namespace geode
{
    struct SidedSurface
    {
        static constexpr local_index_t POSITIVE{ 0 };
        static constexpr local_index_t NEGATIVE{ 1 };

        SidedSurface() = default;
        SidedSurface( uuid id_in, local_index_t side_in, PolygonEdge edge_in )
            : id{ std::move( id_in ) },
              side{ side_in },
              edge{ std::move( edge_in ) }
        {
        }
        SidedSurface( uuid id_in, local_index_t side_in )
            : SidedSurface( std::move( id_in ), side_in, {} )
        {
        }

        bool operator==( const SidedSurface& other ) const
        {
            return id == other.id && side == other.side;
        }

        bool operator!=( const SidedSurface& other ) const
        {
            return !( *this == other );
        }

        uuid id;
        local_index_t side{ 0 };
        PolygonEdge edge;
    };

    struct opengeode_mesh_api SortedSurfaces
    {
        SortedSurfaces( index_t nb_surfaces );

        absl::InlinedVector< index_t, 1 > find(
            const SidedSurface& surface ) const;

        index_t opposite( index_t position ) const;

        index_t next( index_t position ) const;

        absl::FixedArray< SidedSurface > surfaces;
    };

    SortedSurfaces opengeode_mesh_api surface_radial_sort(
        const BRep& brep, const Line3D& line );
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::SidedSurface >
    {
    public:
        size_t operator()( const geode::SidedSurface& surface ) const
        {
            return absl::Hash< geode::uuid >()( surface.id );
        }
    };
} // namespace std
