/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/model/helpers/detail/solid_mesh_validity_fix.hpp>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/helpers/detail/solid_mesh_validity_fix.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace geode
{
    namespace detail
    {
        GenericMapping< index_t > repair_non_manifold_vertices(
            const BRep& model,
            BRepBuilder& model_builder,
            const Block3D& block )
        {
            const auto& mesh = block.mesh();
            auto mesh_builder = model_builder.block_mesh_builder( block.id() );
            const auto vertex_mappings =
                repair_non_manifold_vertices( mesh, *mesh_builder );
            for( const auto& [old_vertex, new_vertices] :
                vertex_mappings.in2out_map() )
            {
                const auto unique_vertex =
                    model.unique_vertex( { block.component_id(), old_vertex } );
                for( const auto new_vertex : new_vertices )
                {
                    model_builder.set_unique_vertex(
                        { block.component_id(), new_vertex }, unique_vertex );
                }
            }
            return vertex_mappings;
        }
    } // namespace detail
} // namespace geode