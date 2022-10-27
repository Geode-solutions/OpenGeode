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

#include <absl/container/flat_hash_map.h>

#include <geode/basic/mapping.h>
#include <geode/basic/uuid.h>

#include <geode/mesh/core/mesh_element.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component_mesh_element.h>
#include <geode/model/mixin/core/vertex_identifier.h>

namespace geode
{
    template < typename MappingType >
    class ModelMapping
    {
    public:
        using Mapping = MappingType;

        MappingType& at( const ComponentType& type )
        {
            return mappings.at( type );
        }

        const MappingType& at( const ComponentType& type ) const
        {
            return mappings.at( type );
        }

        bool has_mapping_type( const ComponentType& type ) const
        {
            return mappings.contains( type );
        }

        void emplace( const ComponentType& type, MappingType mapping )
        {
            mappings.emplace( type, std::move( mapping ) );
        }

    private:
        absl::flat_hash_map< ComponentType, Mapping > mappings;
    };

    using ModelCopyMapping = ModelMapping< BijectiveMapping< uuid > >;

    using ModelGenericMapping = ModelMapping< GenericMapping< uuid > >;

    using ComponentMeshElementMapping =
        GenericMapping< MeshElement, ComponentMeshElement >;

    using ComponentMeshVertexMapping =
        GenericMapping< MeshVertex, ComponentMeshVertex >;

} // namespace geode