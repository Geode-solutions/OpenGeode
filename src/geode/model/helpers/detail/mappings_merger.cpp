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

#include <geode/model/helpers/detail/mappings_merger.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>

namespace
{
    class GenericandCopyMappingsMerger
    {
    public:
        GenericandCopyMappingsMerger(
            const geode::ModelGenericMapping& mappings1,
            const geode::ModelCopyMapping& mappings2 )
            : mappings1_( mappings1 ), mappings2_( mappings2 )
        {
        }

        geode::ModelGenericMapping merge()
        {
            geode::ModelGenericMapping result;
            for( const auto& component_mapping :
                mappings1_.components_mappings() )
            {
                if( mappings2_.has_mapping_type( component_mapping.first ) )
                {
                    result.emplace( component_mapping.first,
                        merge_mappings( component_mapping.second,
                            mappings2_.at( component_mapping.first ) ) );
                }
            }
            return result;
        }

    private:
        geode::ModelGenericMapping::Mapping merge_mappings(
            const geode::ModelGenericMapping::Mapping& mappings1,
            const geode::ModelCopyMapping::Mapping& mappings2 )
        {
            geode::ModelGenericMapping::Mapping result;
            for( const auto& in2between : mappings1.in2out_map() )
            {
                for( const auto& between_id : in2between.second )
                {
                    if( !mappings2.has_mapping_input( between_id ) )
                    {
                        continue;
                    }
                    result.map(
                        in2between.first, mappings2.in2out( between_id ) );
                }
            }
            return result;
        }

    private:
        const geode::ModelGenericMapping& mappings1_;
        const geode::ModelCopyMapping& mappings2_;
    };

    class GenericMappingsMerger
    {
    public:
        GenericMappingsMerger( const geode::ModelGenericMapping& mappings1,
            const geode::ModelGenericMapping& mappings2 )
            : mappings1_( mappings1 ), mappings2_( mappings2 )
        {
        }

        geode::ModelGenericMapping merge()
        {
            geode::ModelGenericMapping result;
            for( const auto& component_mapping :
                mappings1_.components_mappings() )
            {
                if( mappings2_.has_mapping_type( component_mapping.first ) )
                {
                    result.emplace( component_mapping.first,
                        merge_mappings( component_mapping.second,
                            mappings2_.at( component_mapping.first ) ) );
                }
            }
            return result;
        }

    private:
        geode::ModelGenericMapping::Mapping merge_mappings(
            const geode::ModelGenericMapping::Mapping& mappings1,
            const geode::ModelGenericMapping::Mapping& mappings2 )
        {
            geode::ModelGenericMapping::Mapping result;
            for( const auto& in2between : mappings1.in2out_map() )
            {
                for( const auto& between_id : in2between.second )
                {
                    if( !mappings2.has_mapping_input( between_id ) )
                    {
                        continue;
                    }
                    for( const auto& out_id : mappings2.in2out( between_id ) )
                    {
                        result.map( in2between.first, out_id );
                    }
                }
            }
            return result;
        }

    private:
        const geode::ModelGenericMapping& mappings1_;
        const geode::ModelGenericMapping& mappings2_;
    };

    class CopyToGenericMappings
    {
    public:
        CopyToGenericMappings( const geode::ModelCopyMapping& mappings )
            : mappings_( mappings )
        {
        }

        geode::ModelGenericMapping transfer()
        {
            for( const auto& component_mapping :
                mappings_.components_mappings() )
            {
                result_.emplace( component_mapping.first,
                    transfer_mappings( component_mapping.second ) );
            }
            return std::move( result_ );
        }

    private:
        geode::ModelGenericMapping::Mapping transfer_mappings(
            const geode::ModelCopyMapping::Mapping& copy_mappings )
        {
            geode::ModelGenericMapping::Mapping result;
            for( const auto& in2out : copy_mappings.in2out_map() )
            {
                result.map( in2out.first, in2out.second );
            }
            return result;
        }

    private:
        const geode::ModelCopyMapping& mappings_;
        geode::ModelGenericMapping result_;
    };

    geode::MeshElementMapping merge_element_mappings(
        const geode::MeshElementMapping& mappings1,
        const geode::MeshElementMapping& mappings2 )
    {
        geode::MeshElementMapping result;
        for( const auto& first_mapping : mappings1.in2out_map() )
        {
            for( const auto& intermediate_element : first_mapping.second )
            {
                if( !mappings2.has_mapping_input( intermediate_element ) )
                {
                    continue;
                }
                for( const auto& out_element :
                    mappings2.in2out( intermediate_element ) )
                {
                    result.map( first_mapping.first, out_element );
                }
            }
        }
        return result;
    }

    geode::MeshVertexMapping merge_vertex_mappings(
        const geode::MeshVertexMapping& mappings1,
        const geode::MeshVertexMapping& mappings2 )
    {
        geode::MeshVertexMapping result;
        for( const auto& first_mapping : mappings1.in2out_map() )
        {
            for( const auto& intermediate_vertex : first_mapping.second )
            {
                if( !mappings2.has_mapping_input( intermediate_vertex ) )
                {
                    continue;
                }
                for( const auto& out_vertex :
                    mappings2.in2out( intermediate_vertex ) )
                {
                    result.map( first_mapping.first, out_vertex );
                }
            }
        }
        return result;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        ModelGenericMapping merge_mappings(
            const ModelGenericMapping& mappings1,
            const ModelCopyMapping& mappings2 )
        {
            GenericandCopyMappingsMerger merger{ mappings1, mappings2 };
            return merger.merge();
        }

        ModelGenericMapping merge_mappings(
            const ModelGenericMapping& mappings1,
            const ModelGenericMapping& mappings2 )
        {
            GenericMappingsMerger merger{ mappings1, mappings2 };
            return merger.merge();
        }

        ModelGenericMapping copy_to_generic_mappings(
            const ModelCopyMapping& mappings )
        {
            CopyToGenericMappings transferer{ mappings };
            return transferer.transfer();
        }

        ModelMappings merge_mappings(
            const ModelMappings& mappings1, const ModelMappings& mappings2 )
        {
            ModelMappings result;
            result.component_mapping = merge_mappings(
                mappings1.component_mapping, mappings2.component_mapping );
            result.mesh_element_mapping.corners =
                merge_element_mappings( mappings1.mesh_element_mapping.corners,
                    mappings2.mesh_element_mapping.corners );
            result.mesh_element_mapping.lines =
                merge_element_mappings( mappings1.mesh_element_mapping.lines,
                    mappings2.mesh_element_mapping.lines );
            result.mesh_element_mapping.surfaces =
                merge_element_mappings( mappings1.mesh_element_mapping.surfaces,
                    mappings2.mesh_element_mapping.surfaces );
            result.mesh_vertices_mapping.corners =
                merge_vertex_mappings( mappings1.mesh_vertices_mapping.corners,
                    mappings2.mesh_vertices_mapping.corners );
            result.mesh_vertices_mapping.lines =
                merge_vertex_mappings( mappings1.mesh_vertices_mapping.lines,
                    mappings2.mesh_vertices_mapping.lines );
            result.mesh_vertices_mapping.surfaces =
                merge_vertex_mappings( mappings1.mesh_vertices_mapping.surfaces,
                    mappings2.mesh_vertices_mapping.surfaces );
            return result;
        }
    } // namespace detail
} // namespace geode
