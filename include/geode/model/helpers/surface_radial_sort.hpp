/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <absl/container/fixed_array.h>

#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/common.hpp>

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

    struct opengeode_model_api SortedSurfaces
    {
        explicit SortedSurfaces( index_t nb_surfaces );

        absl::InlinedVector< index_t, 1 > find(
            const SidedSurface& surface ) const;

        index_t opposite( index_t position ) const;

        index_t next( index_t position ) const;

        absl::FixedArray< SidedSurface > surfaces;
    };

    SortedSurfaces opengeode_model_api surface_radial_sort(
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
