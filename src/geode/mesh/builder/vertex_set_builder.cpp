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

#include <geode/mesh/builder/vertex_set_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    VertexSetBuilder::VertexSetBuilder( VertexSet& mesh )
        : IdentifierBuilder( mesh ), vertex_set_( mesh )
    {
    }

    std::unique_ptr< VertexSetBuilder > VertexSetBuilder::create(
        VertexSet& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder< VertexSetBuilder >(
            mesh );
    }

    void VertexSetBuilder::copy( const VertexSet& vertex_set )
    {
        OPENGEODE_EXCEPTION( vertex_set_.nb_vertices() == 0,
            "[VertexSetBuilder::copy] Cannot copy a mesh into an already "
            "initialized mesh." );
        set_name( vertex_set.name() );
        create_vertices( vertex_set.nb_vertices() );
        vertex_set_.vertex_attribute_manager().copy(
            vertex_set.vertex_attribute_manager() );
    }

    index_t VertexSetBuilder::create_vertex()
    {
        const auto added_vertex = vertex_set_.nb_vertices();
        vertex_set_.vertex_attribute_manager().resize( added_vertex + 1 );
        do_create_vertex();
        return added_vertex;
    }

    index_t VertexSetBuilder::create_vertices( index_t nb )
    {
        const auto first_added_vertex = vertex_set_.nb_vertices();
        vertex_set_.vertex_attribute_manager().resize(
            first_added_vertex + nb );
        do_create_vertices( nb );
        return first_added_vertex;
    }

    std::vector< index_t > VertexSetBuilder::delete_vertices(
        const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        if( absl::c_find( to_delete, true ) == to_delete.end() )
        {
            return old2new;
        }
        vertex_set_.vertex_attribute_manager().delete_elements( to_delete );
        do_delete_vertices( to_delete, old2new );
        return old2new;
    }

    std::vector< index_t > VertexSetBuilder::permute_vertices(
        absl::Span< const index_t > permutation )
    {
        const auto old2new = old2new_permutation( permutation );
        vertex_set_.vertex_attribute_manager().permute_elements( permutation );
        do_permute_vertices( permutation, old2new );
        return old2new;
    }
} // namespace geode
