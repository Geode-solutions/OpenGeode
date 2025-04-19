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

#include <absl/container/flat_hash_map.h>

#include <geode/basic/mapping.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/mesh_element.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component_mesh_element.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>

namespace geode
{
    template < typename MappingType >
    class ModelMapping
    {
        OPENGEODE_DISABLE_COPY( ModelMapping );

    public:
        using Mapping = MappingType;

        ModelMapping() = default;
        ModelMapping( ModelMapping&& other ) = default;
        ModelMapping& operator=( ModelMapping&& other ) = default;

        [[nodiscard]] MappingType& at( const ComponentType& type )
        {
            return mappings.at( type );
        }

        [[nodiscard]] const MappingType& at( const ComponentType& type ) const
        {
            return mappings.at( type );
        }

        [[nodiscard]] bool has_mapping_type( const ComponentType& type ) const
        {
            return mappings.contains( type );
        }

        void emplace( const ComponentType& type, MappingType mapping )
        {
            const auto result =
                mappings.try_emplace( type, std::move( mapping ) );
            OPENGEODE_EXCEPTION( result.second,
                "[ModelMapping::emplace] A mapping with the given "
                "ComponentType is already present, cannot emplace another." );
        }

        void remove( const ComponentType& type )
        {
            mappings.erase( type );
        }

        [[nodiscard]] MappingType& operator[]( const ComponentType& type )
        {
            return mappings[type];
        }

        [[nodiscard]] const absl::flat_hash_map< ComponentType, Mapping >&
            components_mappings() const
        {
            return mappings;
        }

    private:
        absl::flat_hash_map< ComponentType, Mapping > mappings;
    };

    using ModelCopyMapping = ModelMapping< BijectiveMapping< uuid > >;

    using ModelGenericMapping = ModelMapping< GenericMapping< uuid > >;

    using ModelAddedComponentMapping = ModelMapping< std::vector< uuid > >;

    using ModelUnchangedComponentMapping = ModelMapping< std::vector< uuid > >;

    using MeshElementMapping = GenericMapping< MeshElement, MeshElement >;

    using MeshVertexMapping = GenericMapping< MeshVertex, MeshVertex >;

    using ComponentMeshElementMapping =
        GenericMapping< MeshElement, ComponentMeshElement >;

    using ComponentMeshVertexMapping =
        GenericMapping< MeshVertex, ComponentMeshVertex >;

    struct ModelMeshesElementMapping
    {
        MeshElementMapping corners;
        MeshElementMapping lines;
        MeshElementMapping surfaces;
    };

    struct SectionMeshesElementMapping : public ModelMeshesElementMapping
    {
    };

    struct BRepMeshesElementMapping : public ModelMeshesElementMapping
    {
        MeshElementMapping blocks;
    };

    struct ModelMeshesVertexMapping
    {
        MeshVertexMapping corners;
        MeshVertexMapping lines;
        MeshVertexMapping surfaces;
    };

    struct SectionMeshesVertexMapping : public ModelMeshesVertexMapping
    {
    };

    struct BRepMeshesVertexMapping : public ModelMeshesVertexMapping
    {
        MeshVertexMapping blocks;
    };

    struct ModelMappings
    {
        ModelGenericMapping component_mapping;
        ModelAddedComponentMapping added_components;
        ModelUnchangedComponentMapping unchanged_components;
        ModelMeshesElementMapping mesh_element_mapping;
        ModelMeshesVertexMapping mesh_vertices_mapping;
    };

    struct BRepMappings
    {
        ModelGenericMapping component_mapping;
        BRepMeshesElementMapping mesh_element_mapping;
        BRepMeshesVertexMapping mesh_vertices_mapping;
    };
} // namespace geode